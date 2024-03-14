/********************************************************************
 *
 * Module Name : Plotting.cpp
 *
 * Author/Date : C.B. Lirakis / 23-May-21
 *
 * Description : Generic module
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 *
 * Classification : Unclassified
 *
 * References :
 *
 ********************************************************************/
// System includes.

#include <iostream>
using namespace std;
#include <string>
#include <cmath>

// CERN root includes 
#include <TROOT.h>
#include <TFile.h>
#include <TNtupleD.h>
#include <TH2D.h>

// Local Includes.
#include "debug.h"
#include "CLogger.hh"
#include "YearDay.hh"
#include "Plotting.hh"
#include "AKRead.hh"

/**
 ******************************************************************
 *
 * Function Name : Plotting constructor
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
Plotting::Plotting (uint32_t NDays)
{
    SET_DEBUG_STACK;
    // Super wasteful ntuple since only K changes. 
    const char *Names = "DAY:UTC:Time:Lat:Lon:TYPE:INDEX";
    const char *Filename = "Sunspots.root";
    //CLogger *Logger = CLogger::GetThis();

    /*
     * Initialize Root package.
     * We don't really need to track the return pointer. 
     * We just need to initialize it. 
     */
    ::new TROOT("NOAA","NOAA Sunspot Data analysis");

    /* Create disk file */
    fRootFile = new TFile( Filename, "RECREATE", "generic data analysis");
    fRootFile->cd();
    //Logger->LogTime(" Output file %s opened.\n", Filename);

    fNtuple = new TNtupleD("NOAAtuple", "NOAA A and K", Names);


    f2D = new TH2D("KINDEX","Day by Day K INDEX", 
		   NDays, 0.0, (Double_t) NDays,           // Day is X
		   kNTimeBin, 0.0, (double) kSecPerDay);   // Time is Y

    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name : Plotting destructor
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
Plotting::~Plotting (void)
{
    /* close root file. */
    fRootFile->Write();
    fRootFile->Close();
    delete fRootFile;
    fRootFile = NULL;
}

/**
 ******************************************************************
 *
 * Function Name : Plotting function
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
void Plotting::Fill(const AKRecord &record)
{
//    static int dmo[12] = {0,31,59,90,120,151,181,212,243,273,304,334};
    time_t recordTime;
    struct tm tm_rec; 
    Double_t var[6];

    memset(&tm_rec, 0, sizeof(struct tm));

    uint32_t Day = YearDay(record.fYear, record.fMonth, record.fDay); 

//     uint32_t JulianDay = dmo[record.fMonth] + record.fDay;

//     if ((record.fMonth>1) && (record.fYear%2 == 0))
//     {
// 	JulianDay++;
//     }

    // time is GMT on A and K index
    tm_rec.tm_year = 120 + record.fYear;
    tm_rec.tm_mon  = record.fMonth;
    tm_rec.tm_mday = record.fDay;
    tm_rec.tm_hour = 0.0; // GMT, need to fix. 
    recordTime = mktime(&tm_rec);

    var[0] = Day;
    var[1] = 0;
    var[2] = recordTime;
    var[3] = record.fLat;
    var[4] = record.fLon;
    var[5] = 0; // A Index
    var[6] = record.fA_Index;
    fNtuple->Fill(var);


    // K index is every 3 hours. first one 0-3 GMT
    for (uint32_t i=0;i<8;i++)
    {
	tm_rec.tm_hour = 3.0 * i; // GMT, need to fix. 
	recordTime = mktime(&tm_rec);
	var[1] = 3.0 * i * 3600.0;
	var[2] = recordTime;
	var[5] = i+1; // K Index
	var[6] = record.fK_Index[i];
	fNtuple->Fill(var);
	f2D->Fill(Day, var[1], var[6]);
    }
}

