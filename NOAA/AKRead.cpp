/**
 ******************************************************************
 *
 * Module Name : AKRead.cpp
 *
 * Author/Date : C.B. Lirakis / 05-Mar-19
 *
 * Description : Lassen control entry points. 
 *
 * Restrictions/Limitations : none
 *
 * Change Descriptions : 
 *
 * Classification : Unclassified
 *
 * References : lassen-sk8.pdf - Manual
 *              tsip.pdf - main binary interface control document
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

AKRead* AKRead::fMainModule;

/**
 ******************************************************************
 *
 * Function Name : 
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
AKRecord::AKRecord(void)
{
    SET_DEBUG_STACK;
    Clear();
}
/**
 ******************************************************************
 *
 * Function Name : 
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
void AKRecord::Clear(void)
{
    SET_DEBUG_STACK;
    fYear  = 0;
    fMonth = 0;
    fDay   = 0;

    // Geomagnetic Dipole
    fLat   = 0;      // minus is South
    fLon   = 0;      // minus is West

    fA_Index = 0.0;
    memset( fK_Index, 0, 8*sizeof(float));
}

/**
 ******************************************************************
 *
 * Function Name : Fill
 *
 * Description : Fill a record from an input line. The input 
 * file is extremely structured. This should be easy. 
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
bool AKRecord::Fill(const string &val)
{
    SET_DEBUG_STACK;
    bool    rv = true;
    size_t  pos = 0;
    string  tok;

    /*
     * Example line
     * 
     * 0                17  21  25    (step by 4 for 9 records)
     * Fredericksburg   N38 W 78    6    0     1     0     2     2     2     2     3
     */

    /* 
     * If provisional is found, the beginning parse is a bit different. 
     */

    if ((val.find("provisional") != string::npos) ||
	(val.find("estimated") != string::npos))
    {
	pos = val.find(")") + 1;
	fName = val.substr(0,pos);
	fLat = 0;
	fLon = 0;
    }
    else
    {
	fName = val.substr(0, 17);

	tok   = val.substr(18,2);
	if (tok.compare("--") == 0)
	{
	    // Skip
	    fLat = 0.0;
	}
	else
	{
	    fLat  = stoi(tok);
	    if (val.compare(17,1,"S") == 0) fLat *= -1;
	}

	tok   = val.substr(22,3);
	if (tok.compare("---")==0)
	{
	    fLon = 0.0;
	}
	else
	{
	    fLon  = stoi(tok);
	    if (val.compare(21,1,"W") == 0) fLon *= -1;
	}
    }
    pos   = 25;
    tok   = val.substr(pos,5); pos += 5;
    fA_Index  = stof(tok);

    for (uint8_t i=0;i<8;i++)
    {
	tok = val.substr(pos,5); pos+=6;
	fK_Index[i] = stof(tok);
    }

    SET_DEBUG_STACK;
    return rv;
}
/**
 ******************************************************************
 *
 * Function Name : 
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
ostream& operator<<(ostream &output, AKRecord &r)
{
    SET_DEBUG_STACK;
    output << "==============================================" << endl
	   << " Name: " << r.fName << endl
	   << "     Year: " << (int) r.fYear << " Month: " << (int) r.fMonth
	   << " Day: " << (int) r.fDay 
	   << "     Lat: " << r.fLat << " Lon: " << r.fLon << endl
	   << "     A Index: " << r.fA_Index 
	   << "     K Index: ";

    for (uint32_t i=0;i<8;i++)
    {
	output << (int) r.fK_Index[i] << " ";
    }
    output << endl
	   << "==============================================" << endl;

    return output;
}

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
AKRead::AKRead(const char *Filename, const char* ConfigFile) : CObject()
{
    CLogger *Logger = CLogger::GetThis();

    /* Store the this pointer. */
    fMainModule = this;
    SetName("AKRead");
    SetError(); // No error.

    fRun = true;

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

    InData = new std::ifstream(Filename);
    // is_open?
    if (InData->fail())
    {
	Logger->LogTime("Could not open input file: %s\n",  Filename);
	SetError(-1, __LINE__);
	InData = NULL;
	return;
    }

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

    // Do some other stuff as well. 
    if(!WriteConfiguration())
    {
	SetError(ECONFIG_WRITE_FAIL,__LINE__);
	Logger->LogError(__FILE__,__LINE__, 'W', 
			 "Failed to write config file.\n");
    }
    free(fConfigFileName);

    /* Clean up */
    delete InData;

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
    SET_DEBUG_STACK;
    char Line[256];
    int  count = 0;

    while (!InData->eof() && fRun)
    {
	InData->getline(Line, sizeof(Line));
	if (ProcessLine(Line,"Fredericksburg"))
	{
	    count++;
	    cout << fAKR;
	}
    }
    std::cout << "Processed: " << count << " lines. " << std::endl;
    SET_DEBUG_STACK;
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
    CLogger *Logger = CLogger::GetThis();
    ClearError(__LINE__);
    Config *pCFG = new Config();

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
	MM.lookupValue("Debug",     Debug);
	SetDebug(Debug);
    }
    catch(const SettingNotFoundException &nfex)
    {
	// Ignore.
    }

    delete pCFG;
    pCFG = 0;
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
    MM.add("Debug",     Setting::TypeInt)     = 0;
    MM.add("Logging",   Setting::TypeBoolean)     = true;

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
