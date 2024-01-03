/**
 ******************************************************************
 *
 * Module Name : Analysis.hh
 *
 * Author/Date : C.B. Lirakis / 02-Jan-24
 *
 * Description : Take in a list of file(s) for analysis and
 * make the appropriate plots. Uses root for the plots. 
 *
 * Restrictions/Limitations : none
 *
 * Change Descriptions :
 *
 * Classification : Unclassified
 *
 * References :
 *
 *
 *******************************************************************
 */
#ifndef __MAINMODULE_hh_
#define __MAINMODULE_hh_
#  include "CObject.hh" // Base class with all kinds of intermediate
#  include "H5Logger.hh"

class TFile;
class SFilter;
class TGraph;

class Analysis : public CObject
{
public:
    /** 
     * Build on CObject error codes. 
     */
    enum {ENO_FILE=1, ECONFIG_READ_FAIL, ECONFIG_WRITE_FAIL};
    /**
     * Constructor the lassen SK8 subsystem.
     * All inputs are in configuration file. 
     */
    Analysis(const char *ConfigFile);

    /**
     * Destructor for SK8. 
     */
    ~Analysis(void);

    /*! Access the This pointer. */
    static Analysis* GetThis(void) {return fAnalysis;};

    /**
     * Main Module DO
     * 
     */
    void Do(void);

    /**
     * Tell the program to stop. 
     */
    void Stop(void) {fRun=false;};

    /**
     * Control bits - control verbosity of output
     */
    static const unsigned int kVerboseBasic    = 0x0001;
    static const unsigned int kVerboseMSG      = 0x0002;
    static const unsigned int kVerboseFrame    = 0x0010;
    static const unsigned int kVerbosePosition = 0x0020;
    static const unsigned int kVerboseHexDump  = 0x0040;
    static const unsigned int kVerboseCharDump = 0x0080;
    static const unsigned int kVerboseMax      = 0x8000;
 
private:

    /// CERN Root stuff.
    TFile  *fRootFile;
    TGraph *fGraph;

    /// File management
    ifstream     *fInputFileList;
    string       fInputFileName;
    string       fOutputFileName;

    /// Main run stuff
    bool         fRun;

    /*!
     * Logging tool, HDF5 I/O. 
     */
    H5Logger    *f5InputFile;

    /*! 
     * Configuration file name. 
     */
    char        *fConfigFileName;

    /// Filtering of data. 
    SFilter     *fFilter;


    /* Private functions. ==============================  */

    /*!
     * Open the data logger. 
     */
    bool OpenInputFile(const char *filename);

    bool OpenOutputFile(const char *Filename);

    /*!
     * Read the configuration file. 
     */
    bool ReadConfiguration(void);
    /*!
     * Write the configuration file. 
     */
    bool WriteConfiguration(void);

    bool ProcessData(void);

    /*! The static 'this' pointer. */
    static Analysis *fAnalysis;

};
#endif
