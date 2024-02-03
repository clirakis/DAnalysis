/**
 ******************************************************************
 *
 * Module Name : Analysis.cpp
 *
 * Author/Date : C.B. Lirakis / 05-Mar-19
 *
 * Description : Lassen control entry points. 
 *
 * Restrictions/Limitations : none
 *
 * Change Descriptions : 
 * 28-Jan-24   CBL Might as well create the NTuple as well. 
 *
 * Classification : Unclassified
 *
 * References : 
 *
 *
 *******************************************************************
 */  
// System includes.
#include <iostream>
using namespace std;

#include <string>
#include <cmath>
#include <cstdlib>
#include <libconfig.h++>
using namespace libconfig;

/// Root includes
#include <TROOT.h>
#include <TFile.h>
#include <TGraph.h>
#include <TMultiGraph.h>
#include <TLegend.h>
#include <TString.h>
#include <TObjString.h>
#include <TNtupleD.h>
#include <TProfile.h>
#include <TH2D.h>

/// Local Includes.
#include "Analysis.hh"
#include "CLogger.hh"
#include "tools.h"
#include "UTC2Sec.hh"
#include "debug.h"
#include "SFilter.hh"

Analysis* Analysis::fAnalysis = NULL;

/**
 ******************************************************************
 *
 * Function Name : Analysis constructor
 *
 * Description : initialize CObject variables
 *
 * Inputs : currently none. 
 *
 * Returns : none
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
Analysis::Analysis(const char* ConfigFile) : CObject()
{
    CLogger *Logger = CLogger::GetThis();

    /* Store the this pointer. */
    fAnalysis = this;
    SetName("Analysis");
    SetError(); // No error.

    fRun           = true;
    fInputFileList = NULL;
    fInputFileName = strdup("Default.txt");
    fRootFile      = NULL;
    fFilter        = NULL;
    ftmg           = NULL;
    fLegend        = NULL;
    fNtuple        = NULL;
    fGraph         = NULL;
    fProfile       = NULL;
    f2D            = NULL;

    if(!ConfigFile)
    {
	SetError(ENO_FILE,__LINE__);
	return;
    }

    fConfigFileName = strdup(ConfigFile);
    if(!ReadConfiguration())
    {
	SetError(ECONFIG_READ_FAIL,__LINE__);
	return;
    }

    /* USER POST CONFIGURATION STUFF. */

    f5InputFile = NULL;

    Logger->Log("# Analysis constructed.\n");

    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name : Analysis Destructor
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
Analysis::~Analysis(void)
{
    SET_DEBUG_STACK;
    CLogger *Logger = CLogger::GetThis();

    // Do some other stuff as well. 
    if(!WriteConfiguration())
    {
	SetError(ECONFIG_WRITE_FAIL,__LINE__);
	Logger->LogError(__FILE__,__LINE__, 'W', 
			 "Failed to write config file.\n");
    }
    free(fConfigFileName);

    if(fInputFileList) 
    {
	fInputFileList->close();
	delete fInputFileList;
    }

    /* Clean up */
    delete f5InputFile;
    f5InputFile = NULL;
    if (ftmg)
    {
	ftmg->Write("IMUData");
    }
    else
    {
	fGraph->Write("IMUData");     // flush this. 
    }
    fLegend->Write("IMULegend");


    /* close root file. */
    fRootFile->Write();
    fRootFile->Close();
    delete fRootFile;
    fRootFile = NULL;

    delete fFilter;

    // Make sure all file streams are closed
    Logger->Log("# Analysis closed.\n");
    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name : OpenOutputFile
 *
 * Description : 
 *     make a root filename based on input filename
 *     open a TFILE 
 *     create an ntuple to fill. 
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool Analysis::OpenOutputFile(const char *Filename)
{
    SET_DEBUG_STACK;
    CLogger *Logger = CLogger::GetThis();
    bool     rc     = true;

    /*
     * Initialize Root package.
     * We don't really need to track the return pointer. 
     * We just need to initialize it. 
     */
    ::new TROOT("HDF5","HDF5 Data analysis");

    /* Create disk file */
    fRootFile = new TFile( Filename, "RECREATE","generic data analysis");
    fRootFile->cd();
    Logger->LogTime(" Output file %s opened.\n", Filename);

    CreateNTuple();

    SET_DEBUG_STACK;
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : CreateNTuple
 *
 * Description : 
 *     create an NTuple in the TFile
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool Analysis::CreateNTuple(void)
{
    SET_DEBUG_STACK;
    const char *Names="Time:AX:AY:AZ:GX:GY:GZ:MX:MY:MZ:Temp:Lat:Lon:Z:UTC:JD";
    fNtuple = new TNtupleD("IMUTuple", "Raspberry Pi DA", Names);

    /*
     *  Create 2D plot as well. 
     * First count number of files. Axes will be 
     * Day and time, fill with the absmag of the value per bin
     * and then normalize. 
     */
    uint32_t count = CountFiles();
    f2D = new TH2D("ABSMAG2D","Day by Day ABSMAG", 
		   count, 0.0, (Double_t) count-1,   // Day is X
		   kNTimeBin, 0.0, (double) kSecPerDay);         // Time is Y

    SET_DEBUG_STACK;
    return true;
}

/**
 ******************************************************************
 *
 * Function Name : Do
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void Analysis::Do(void)
{
    SET_DEBUG_STACK;
    char     Filename[256];
    TString  Name, Result, ProfName;        // stripped down name
    char     tmp[32];
    uint32_t count = 0;                     // file count
    Ssiz_t   n1, n2;

    fRun = true;

    // Create initial TGraph for the data
    fGraph = new TGraph();
    fGraph->SetTitle("IMU Data");

    fProfile = new TProfile("ABSMAG", "Absolute Magnitude", 
			    kNTimeBin, 0.0, (double)kSecPerDay, 80.0, 90.0);

    fLegend = new TLegend(0.1, 0.1, 0.5, 0.4);

    // Loop over input file name until there are no more. 
    while(fRun)
    {
	fInputFileList->getline( Filename, sizeof(Filename),'\n');
	cout << "Input file name: " << Filename << " count: " << count << endl;
	fRun = (strlen(Filename)>0);
	if (fRun)
	{
	    Name   = Filename;
	    n1     = Name.First("202");
	    n2     = Name.Last('_');
	    Result = Name(n1,n2-n1);
	    snprintf(tmp, sizeof(tmp), "IMU%d",count);
	    ProfName = tmp;
	    fProfile->SetTitle(Result);
	    cout << "RESULT: " << Result << endl;

	    // Process. 
	    if(OpenInputFile(Filename))
	    {
		// Loop over data, process it and then close the input file. 
		ProcessData(count);
		delete f5InputFile;
		f5InputFile = NULL;
		if (ftmg)
		{
		    fGraph->SetTitle(Result);
		    ftmg->Add(fGraph); 
		    fLegend->AddEntry(fGraph, Result);
		    // Create a new graph
		    fGraph = new TGraph();
		    fGraph->SetMarkerColor(count);
		    fGraph->SetLineColor(count);
		}
		fProfile->Write(ProfName);
		fProfile->Reset();
	    }
	    count++;
	}
    }
    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name : ProcessData
 *
 * Description : for each file, process the data. 
 *
 * Inputs : count - file number currently being processed. 
 *
 * Returns : NONE
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool Analysis::ProcessData(uint32_t count)
{
    SET_DEBUG_STACK;
    const double   *var;        // get a row at a time from H5 file
    double         varcpy[16];
    double         MTotal, FVal;
    double         T, X, Y, Z;
    struct tm      *tmnow;
    time_t         sec;
    // This assumes a 1/sec sample rate. 
    double         Norm = ((double)kSecPerDay)/((double) kNTimeBin);
    double         Day  = (double) count;
    double         W;   // Actual bin value
    const double   Low1 = 16.0*kSecPerDay;
    const double   Low2 = 17.0*kSecPerDay;


    // Sample at 2 points for printout/debug
    double         Sum[2];

    // number of entries in the file. 
    size_t N = f5InputFile->NEntries();
    cout << "Processing: " << N << " Entries. Day: " << Day << endl;

    time_t   iTime = f5InputFile->IndexFromName("Time");
    int32_t  iUTC  = f5InputFile->IndexFromName("UTC");
    uint32_t iMx   = f5InputFile->IndexFromName("Mx");
    uint32_t iMy   = f5InputFile->IndexFromName("My");
    uint32_t iMz   = f5InputFile->IndexFromName("Mz");
    memset (Sum, 0, 2*sizeof(double));

    for (size_t i=0 ;i<N; i++)
    {
	if(f5InputFile->DatasetReadRow(i))
	{
	    var = f5InputFile->RowData();
	    sec = (time_t) var[iTime];
	    tmnow = gmtime(&sec);
	    T = UTC2Sec(var[iUTC]);
	    X = var[iMx];
	    Y = var[iMy];
	    Z = var[iMz];
	    MTotal = sqrt(X*X + Y*Y + Z*Z);
	    FVal   = fFilter->Filter(MTotal);
	    fGraph->AddPoint(T, FVal);
	    fProfile->Fill(T,MTotal);
	    memcpy(varcpy, var, 15*sizeof(double));
	    // convert UTC HHMMSS.ss into sssss
	    varcpy[14] = UTC2Sec(var[14]);
	    varcpy[15] = tmnow->tm_yday+1; // start with Jan 1 is JD 1. 
	    if (fNtuple) fNtuple->Fill(varcpy);

	    W = MTotal/Norm;
	    if ((T>Low1) && (T<(Low1+Norm)))
	    {
		Sum[0] = Sum[0] + W;
	    }
	    if ((T>Low2) && (T<(Low2+Norm)))
	    {
		Sum[1] = Sum[1] + W;
	    }
	    f2D->Fill(Day, T, W);
	}
    }
    cout << "File Number: " << count
	 << " 1600 hrs: " << Sum[0]
	 << " 1700 hrs: " << Sum[1]
	 << endl;

    SET_DEBUG_STACK;
    return true;
}

/**
 ******************************************************************
 *
 * Function Name : OpenInputFile
 *
 * Description : Open and manage the HDF5 Input file
 *
 * Inputs : none
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on:  
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool Analysis::OpenInputFile(const char *Filename)
{
    SET_DEBUG_STACK;
    CLogger *pLogger = CLogger::GetThis();

    /*
     * open in read only mode. 
     */
    f5InputFile = new H5Logger( Filename, NULL, 0, true);
    if (f5InputFile->CheckError())
    {
	pLogger->Log("# Failed to open H5 input file: %s\n", Filename);
	delete f5InputFile;
	f5InputFile = NULL;
	return false;
    }

    /* Log that this was done in the local text log file. */
    pLogger->LogTime("Input file name %s\n", Filename);

    //cout << *f5InputFile ;
    return true;
}
/**
 ******************************************************************
 *
 * Function Name : CountFiles
 *
 * Description : Count the number of files on the input side
 *
 * Inputs : none
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on:  
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint32_t Analysis::CountFiles(void)
{
    SET_DEBUG_STACK;
    CLogger *pLogger = CLogger::GetThis();
    uint32_t count   = 0;
    char     Filename[256];
    bool     Run     = true;
    
    /*
     * file is already open, read all the lines and increment the 
     * count then return it. 
     */
    while(Run)
    {
	fInputFileList->getline( Filename, sizeof(Filename),'\n');
	Run = (strlen(Filename)>0);
	if (Run) count++;
    }
    // Rewind the file. 
    fInputFileList->seekg(0);
    pLogger->LogTime("Number input files %d\n", count);
    SET_DEBUG_STACK;
    return count;
}
/**
 ******************************************************************
 *
 * Function Name : ReadConfiguration
 *
 * Description : Open read the configuration file. 
 *
 * Inputs : none
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on:  
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool Analysis::ReadConfiguration(void)
{
    SET_DEBUG_STACK;
    CLogger *Logger = CLogger::GetThis();
    ClearError(__LINE__);
    Config *pCFG = new Config();
    string InputFile;
    double CutoffFrequency, SampleRate;
    bool   multi  = false;

    /*
     * Open the configuragtion file. 
     */
    try{
	pCFG->readFile(fConfigFileName);
    }
    catch( const FileIOException &fioex)
    {
	Logger->LogError(__FILE__,__LINE__,'F',
			 "I/O error while reading configuration file.\n");
	return false;
    }
    catch (const ParseException &pex)
    {
	Logger->Log("# Parse error at: %s : %d - %s\n",
		    pex.getFile(), pex.getLine(), pex.getError());
	return false;
    }


    /*
     * Start at the top. 
     */
    const Setting& root = pCFG->getRoot();

    // USER TO FILL IN
    // Output a list of all books in the inventory.
    try
    {
	int    Debug;
	/*
	 * index into group Analysis
	 */
	const Setting &MM = root["Analysis"];
	MM.lookupValue("Debug"         , Debug);
	MM.lookupValue("InputFile"     , InputFile);
	MM.lookupValue("CutoffFrequncy", CutoffFrequency);
	MM.lookupValue("SampleRate"    , SampleRate);
	MM.lookupValue("OutputFile"    , fOutputFileName);
	MM.lookupValue("Multigraph"    , multi);

	SetDebug(Debug);
	if (InputFile.length()>0)
	{
	    fInputFileName = InputFile;
	}
    }
    catch(const SettingNotFoundException &nfex)
    {
	// Ignore.
    }

    delete pCFG;
    pCFG = 0;


    // open the input filename. 
    fInputFileList = new ifstream(fInputFileName);
    if(fInputFileList->fail())
    {
	Logger->Log("# Failed to open input file list: %s\n", 
		    fInputFileName.data());
	return false;
    }
    else
    {
	Logger->Log("# Input file list: %s\n", fInputFileName.data());
    }
    // Setup filter
    if (SampleRate <= 0.0)
    {
	SampleRate = 1.0;
	Logger->Log("# Sample rate too low, set to 1.0\n");
    }
    if (CutoffFrequency <= 0.0)
    {
	CutoffFrequency = 0.01;
	Logger->Log("# Cutoff frequency too low, set to 0.01\n");
    }
    Logger->Log("# Filter parameters set to, Cutoff: %f, Sample Rate: %f\n", 
		CutoffFrequency, SampleRate);
    fFilter = new SFilter(CutoffFrequency, SampleRate);
    OpenOutputFile(fOutputFileName.data());

    if (multi)
    {
	ftmg = new TMultiGraph();
    }

    SET_DEBUG_STACK;
    return true;
}

/**
 ******************************************************************
 *
 * Function Name : WriteConfigurationFile
 *
 * Description : Write out final configuration
 *
 * Inputs : none
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on:  
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool Analysis::WriteConfiguration(void)
{
    SET_DEBUG_STACK;
    CLogger *Logger = CLogger::GetThis();
    ClearError(__LINE__);
    double CutoffFrequency = 1.0;
    double SampleRate      = 1.0;
    Config *pCFG           = new Config();
    Setting &root          = pCFG->getRoot();

    if (fFilter)
    {
	SampleRate      = fFilter->SampleRate();
	CutoffFrequency = fFilter->Cutoff();
    }

    // USER TO FILL IN
    // Add some settings to the configuration.
    Setting &MM = root.add("Analysis", Setting::TypeGroup);
    MM.add("Debug"          , Setting::TypeInt)    = 0;
    MM.add("InputFile"      , Setting::TypeString) = fInputFileName;
    MM.add("CutoffFrequncy" , Setting::TypeFloat)  = CutoffFrequency;
    MM.add("SampleFrequency", Setting::TypeFloat)  = SampleRate;
    MM.add("OutputFile"     , Setting::TypeString) = fOutputFileName;
    MM.add("Multigraph"     , Setting::TypeBoolean)= (ftmg != NULL);

    // Write out the new configuration.
    try
    {
	pCFG->writeFile(fConfigFileName);
	Logger->Log("# New configuration successfully written to: %s\n",
		    fConfigFileName);

    }
    catch(const FileIOException &fioex)
    {
	Logger->Log("# I/O error while writing file: %s \n",
		    fConfigFileName);
	delete pCFG;
	return(false);
    }
    delete pCFG;

    SET_DEBUG_STACK;
    return true;
}
