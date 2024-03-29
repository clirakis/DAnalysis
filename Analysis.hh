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
 * 03-Jan-24 CBL Added in a TMultigraph and multiple TGraphs to 
 *               be able to modify the marker and color for each 
 *               dataset day. This is on a switch. 
 *
 * 28-Jan-24     Create Ntuple too
 * 13-Feb-24     Add in K-index style 2D histo.
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
class TMultiGraph;
class TLegend;
class TNtupleD;
class TProfile;
class TH2D;

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
     * Destructor
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

    // Number of time bins
    const   uint32_t kSecPerDay = 86400;
    const   uint32_t kNTimeBin  = 288;

    /// CERN Root stuff.
    TFile       *fRootFile;
    TGraph      *fGraph;
    TProfile    *fProfile;
    TMultiGraph *ftmg;        // if this is non-null, use multiple graphs
    TLegend     *fLegend;   
    TNtupleD    *fNtuple;   
    TH2D        *f2D;         // Binned 2 D data - high res bin
    TH2D        *f2DZ;        // Binned 2 D data - high res bin, Z only
    TH2D        *f2DK;        // binned on 3 hour intervals. K_Index
    uint32_t    fExpected;    // Number of files expected. 

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

    bool CreateNTuple(void);

    uint32_t CountFiles(void);

    /*!
     * Read the configuration file. 
     */
    bool ReadConfiguration(void);
    /*!
     * Write the configuration file. 
     */
    bool WriteConfiguration(void);

    bool ProcessData(uint32_t count);

    /*! The static 'this' pointer. */
    static Analysis *fAnalysis;

};
#endif
