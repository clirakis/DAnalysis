/**
 ******************************************************************
 *
 * Module Name : Plotting.hh
 *
 * Author/Date : C.B. Lirakis / 11-Feb-24
 *
 * Description : 
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
#ifndef __PLOTTING_hh_
#define __PLOTTING_hh_

class TFile;
class AKRecord;
class TNtupleD;
class TH2D;

/// Plotting documentation here. 
class Plotting {
public:
    /// Default Constructor
    Plotting(uint32_t NDays=44);
    /// Default destructor
    ~Plotting(void);
    /// Plotting function
    /*!
     * Description: 
     *   
     *
     * Arguments:
     *   
     *
     * Returns:
     *
     * Errors:
     *
     */
    void Fill(const AKRecord &record);


private:
    // Number of time bins
    const   uint32_t kSecPerDay = 86400;
    const   uint32_t kNTimeBin  = 8;  // 3 hour intervals

    TFile    *fRootFile;
    TNtupleD *fNtuple;
    TH2D     *f2D;
};
#endif
