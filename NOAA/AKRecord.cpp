/**
 ******************************************************************
 *
 * Module Name : AKRecord.cpp
 *
 * Author/Date : C.B. Lirakis / 12-Feb-24
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
 * 
 *
 *
 *******************************************************************
 */  
// System includes.
#include <iostream>
using namespace std;

#include <string>
#include <cstring>

/// Local Includes.
#include "AKRecord.hh"
#include "CLogger.hh"
#include "tools.h"
#include "debug.h"


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

