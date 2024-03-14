/**
 ******************************************************************
 *
 * Module Name : AKRead.cpp
 *
 * Author/Date : C.B. Lirakis / 11-Feb-24
 *
 * Description : Lassen control entry points. 
 *
 * Restrictions/Limitations : none
 *
 * Change Descriptions : 
 *
 * Classification : Unclassified
 *
 * References : 
 *     ftp.swpc.noaa.gov.pub/lists/geomag
 *
 *
 *******************************************************************
 */  
// System includes.
#include <iostream>
using namespace std;

#include <string>
#include <cmath>
#include <unistd.h>
#include <fstream>
#include <errno.h>
#include <cstdlib>
#include <cstring>

#include <libconfig.h++>
using namespace libconfig;

/// Local Includes.
#include "AKRead.hh"
#include "CLogger.hh"
#include "tools.h"
#include "debug.h"
#include "Plotting.hh"

AKRead* AKRead::fMainModule;

/**
 ******************************************************************
 *
 * Function Name : AKRead constructor
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
AKRead::AKRead(const char* ConfigFile) : CObject()
{
    CLogger *Logger = CLogger::GetThis();

    /* Store the this pointer. */
    fMainModule = this;
    SetName("AKRead");
    SetError(); // No error.

    fRun = true;
    fInputFileName = strdup("Default.txt");
    fInputFileList = NULL;
    fPlotting      = NULL;
    fNDays         = 1;

    /* 
     * Set defaults for configuration file. 
     */
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

    Logger->Log("# AKRead constructed.\n");

    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name : AKRead Destructor
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
AKRead::~AKRead(void)
{
    SET_DEBUG_STACK;
    CLogger *Logger = CLogger::GetThis();

    delete fPlotting;
    

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

    // Make sure all file streams are closed
    Logger->Log("# AKRead closed.\n");
    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name : ProcessDate
 *
 * Description : parse the data into indivial items. 
 *
 * Inputs : Line containing date information. 
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
void AKRead::ProcessDate(const string &Line)
{
    static const char *Month[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
			      "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    SET_DEBUG_STACK;
    size_t   pos = 0;
    uint8_t  i   = 0;
    uint32_t year;

    // Local copy of input line
    string  LCopy(Line);
    string  token;

    // Three fields
    pos   = LCopy.find(" ");
    token = LCopy.substr(0, pos);
    year  = stoi(token);
    fYear = year - 2020;
    LCopy.erase(0, pos+1);

    pos   = LCopy.find(" ");
    token = LCopy.substr(0, pos);
    // Loop over letters and find match. 
    do {
	if (token.find(Month[i]) != string::npos)
	{
	    break;
	}
	i++;
    } while(i<12);
    fMonth = i;
    LCopy.erase(0, pos+1);

    pos   = LCopy.find(" ");
    token = LCopy.substr(0, pos);
    fDay = stoi(token);

    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name : ProcessLine
 *
 * Description : Decide what to do about the current line. 
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
bool AKRead::ProcessLine(const char *Line, const char *Location)
{
    SET_DEBUG_STACK;
    bool rc = true; 
    string SLine(Line);

    fAKR.Clear();
    if (Location == NULL)
	return false;

    if (strlen(Line)<=1)
    {
	// blank, just return. 
	rc = false;
    }
    else if ((SLine.find(':')!=string::npos)||(SLine.find("#")!=string::npos))
    {
	// Ignore comment lines
	rc = false;
    }
    else if (SLine.find("202") == 0)
    {
	// Date line??
	ProcessDate(Line);
	// This is incomplete, return a false. 
	rc = false;
    }
    else if (SLine.find(Location) == 0)
    {
	fAKR.Fill(Line);
	fAKR.FillDate(fYear, fMonth, fDay);
    }
    else
    {
// 	AKRecord akr;
// 	akr.Fill(Line);
// 	akr.FillDate(fYear, fMonth, fDay);
// 	cout << akr;
	rc = false;
    }
    SET_DEBUG_STACK;
    return rc;
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
void AKRead::Do(void)
{
    uint32_t count = 0;
    char Filename[256];

    while (fRun)
    {
	fInputFileList->getline( Filename, sizeof(Filename),'\n');
	cout << "Input: " << Filename << ", count: " << count << endl;
	fRun = (strlen(Filename)>0);
	if (fRun)
	{
	    ProcessFile(Filename);
	    count++;
	}
    }
}
/**
 ******************************************************************
 *
 * Function Name : ProcessFile
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
bool AKRead::ProcessFile(const char *Filename)
{
    SET_DEBUG_STACK;
    CLogger *Logger = CLogger::GetThis();
    char Line[256];
    int  count = 0;

    std::ifstream InData(Filename);
    // is_open?
    if (InData.fail())
    {
	Logger->LogTime("Could not open input file: %s\n",  Filename);
	SetError(-1, __LINE__);
	return false;
    }

    while (!InData.eof())
    {
	InData.getline(Line, sizeof(Line));
	if (ProcessLine(Line,"Fredericksburg"))
	{
	    count++;
	    cout << fAKR;
	    fPlotting->Fill(fAKR);
	}
    }
    InData.close();
    std::cout << "Processed: " << count << " lines. " << std::endl;
    SET_DEBUG_STACK;
    return true;
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
bool AKRead::ReadConfiguration(void)
{
    SET_DEBUG_STACK;
    CLogger  *Logger = CLogger::GetThis();
    ClearError(__LINE__);
    Config   *pCFG = new Config();
    string   InputFile;

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
	 * index into group AKRead
	 */
	const Setting &MM = root["AKRead"];
	MM.lookupValue("Debug"    ,     Debug);
	MM.lookupValue("InputFile", InputFile);
	MM.lookupValue("Days"     , fNDays);

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
    fPlotting = new Plotting(fNDays);

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
bool AKRead::WriteConfiguration(void)
{
    SET_DEBUG_STACK;
    CLogger *Logger = CLogger::GetThis();
    ClearError(__LINE__);
    Config *pCFG = new Config();

    Setting &root = pCFG->getRoot();

    // USER TO FILL IN
    // Add some settings to the configuration.
    Setting &MM = root.add("AKRead", Setting::TypeGroup);
    MM.add("Debug"    , Setting::TypeInt)     = 0;
    MM.add("Logging"  , Setting::TypeBoolean) = true;
    MM.add("InputFile", Setting::TypeString)  = fInputFileName;
    MM.add("Days"     , Setting::TypeInt)     = fNDays;

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
