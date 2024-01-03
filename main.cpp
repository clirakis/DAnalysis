/**
 ******************************************************************
 *
 * Module Name : main.cpp
 *
 * Author/Date : C.B. Lirakis / 02-Jan-24
 *
 * Description : Analyze data taken by Raspberry Pi DA system
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 *
 * Classification : Unclassified
 *
 * References :
 *
 *******************************************************************
 */
// System includes.
#include <iostream>
using namespace std;
#include <cstring>
#include <cmath>
#include <csignal>
#include <unistd.h>
#include <time.h>
#include <fstream>
#include <cstdlib>

/// Local Includes.
#include "debug.h"
#include "tools.h"
#include "CLogger.hh"
#include "UserSignals.hh"
#include "Version.hh"
#include "Analysis.hh"

/** Control the verbosity of the program output via the bits shown. */
static unsigned int VerboseLevel = 0;

/** Pointer to the logger structure. */
static CLogger   *logger;

/**
 ******************************************************************
 *
 * Function Name : Help
 *
 * Description : provides user with help if needed.
 *
 * Inputs : none
 *
 * Returns : none
 *
 * Error Conditions : none
 *
 *******************************************************************
 */
static void Help(void)
{
    SET_DEBUG_STACK;
    cout << "********************************************" << endl;
    cout << "* Test file for text Logging.              *" << endl;
    cout << "* Built on "<< __DATE__ << " " << __TIME__ << "*" << endl;
    cout << "* Available options are :                  *" << endl;
    cout << "*                                          *" << endl;
    cout << "********************************************" << endl;
}
/**
 ******************************************************************
 *
 * Function Name :  ProcessCommandLineArgs
 *
 * Description : Loop over all command line arguments
 *               and parse them into useful data.
 *
 * Inputs : command line arguments. 
 *
 * Returns : none
 *
 * Error Conditions : none
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
static void
ProcessCommandLineArgs(int argc, char **argv)
{
    int option;
    SET_DEBUG_STACK;
    do
    {
        option = getopt( argc, argv, "hHnv");
//        option = getopt( argc, argv, "f:hHnv");
        switch(option)
        {
//        case 'f':
//            break;
        case 'h':
        case 'H':
            Help();
        Terminate(0);
        break;
	case 'v':
	    VerboseLevel = atoi(optarg);
            break;
        }
    } while(option != -1);
}
/**
 ******************************************************************
 *
 * Function Name : Initialize
 *
 * Description : Initialze the process
 *               - Setup traceback utility
 *               - Connect all signals to route through the terminate 
 *                 method
 *               - Perform any user initialization
 *
 * Inputs : none
 *
 * Returns : true on success. 
 *
 * Error Conditions : depends mostly on user code
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
static bool Initialize(void)
{
    SET_DEBUG_STACK;
    char   msg[32];
    double version;

    SetSignals();
    // User initialization goes here. 
    sprintf(msg, "%d.%d",MAJOR_VERSION, MINOR_VERSION);
    version = atof( msg);
    logger = new CLogger("Analysis.log", "Analysis", version);
    logger->SetVerbose(VerboseLevel);

    return true;
}

/**
 ******************************************************************
 *
 * Function Name : main
 *
 * Description : It all starts here:
 *               - Process any command line arguments
 *               - Do any necessary initialization as a result of that
 *               - Do the operations
 *               - Terminate and cleanup
 *
 * Inputs : command line arguments
 *
 * Returns : exit code
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
int main(int argc, char **argv)
{
    ProcessCommandLineArgs(argc, argv);
    if (Initialize())
    {
	Analysis *pModule = new Analysis("Analysis.cfg");
	if (pModule->Error() == 0)
	{
	    pModule->Do();
	}
    }
    Terminate(0);
}
