#ifndef WALKER_H
#define WALKER_H

#include "MCtypes.h"
#include <vector>

using namespace std;

class Walker
{
public:
    Walker();
    MCfloat r0[3];  /**< @brief position before the \f$ n \f$-th step*/
    MCfloat r1[3];  /**< @brief position after the \f$ n \f$-th step*/
    MCfloat k0[3];  /**< @brief direction unit vector for the \f$ (n-1) \f$-th step*/
    MCfloat k1[3];  /**< @brief direction unit vector for the \f$ n \f$-th step*/
    MCfloat walkTime;  /**< @brief time elapsed up to the current position*/
    vector<MCfloat> nInteractions;  /**< @brief number of scattering events occured in each layer*/
};

#endif // WALKER_H
