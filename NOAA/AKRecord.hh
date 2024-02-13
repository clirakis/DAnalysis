/**
 ******************************************************************
 *
 * Module Name : AKRecord.hh
 *
 * Author/Date : C.B. Lirakis / 12-Feb-24
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
#ifndef __AKRECORD_hh_
#define __AKRECORD_hh_
#  include <stdint.h>
#  include <fstream>

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
#endif
