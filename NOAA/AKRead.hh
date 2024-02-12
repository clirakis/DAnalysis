/**
 ******************************************************************
 *
 * Module Name : AKRead.hh
 *
 * Author/Date : C.B. Lirakis / 10-Feb-24
 *
 * Description : Class description for reading NOAA AK solar file. 
 *
 * Restrictions/Limitations : none
 *
 * Change Descriptions :
 *
 * Classification : Unclassified
 *
 * References :
 *
 * https://en.wikipedia.org/wiki/K-index
 * https://www.swpc.noaa.gov/products/station-k-and-indices
 * 
 *
 *******************************************************************
 */
#ifndef __MAINMODULE_hh_
#define __MAINMODULE_hh_
#  include <stdint.h>
#  include <fstream>
#  include "CObject.hh" // Base class with all kinds of intermediate

class AKRecord
{
public:
    AKRecord(void);
    bool Fill(const string &val);
    void Clear(void);

    string  fName;
    uint8_t fYear;
    uint8_t fMonth;
    uint8_t fDay;

    // Geomagnetic Dipole
    int32_t fLat;      // minus is South
    int32_t fLon;      // minus is West

    float  fA_Index;
    float  fK_Index[8];

    inline void FillDate(uint8_t Year, uint8_t Mon, uint8_t Day) 
	{fYear = Year; fMonth = Mon; fDay = Day;};

    friend ostream& operator<<(ostream &output, AKRecord &r);
};

class AKRead : public CObject
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
    AKRead(const char *Filename, const char *ConfigFile);

    /**
     * Destructor for SK8. 
     */
    ~AKRead(void);

    /*! Access the This pointer. */
    static AKRead* GetThis(void) {return fMainModule;};

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

    bool     fRun;
    uint8_t  fYear;
    uint8_t  fMonth;
    uint8_t  fDay;
    AKRecord fAKR;

    /*! 
     * Configuration file name. 
     */
    char   *fConfigFileName;

    std::ifstream *InData;

    /* Private functions. ==============================  */

    bool ProcessLine(const char *Line, const char *Location);
    void ProcessDate(const string &Line);

    /*!
     * Read the configuration file. 
     */
    bool ReadConfiguration(void);
    /*!
     * Write the configuration file. 
     */
    bool WriteConfiguration(void);


    /*! The static 'this' pointer. */
    static AKRead *fMainModule;

};
#endif