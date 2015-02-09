#ifndef WALKER_H
#define WALKER_H

#include "MCglobal.h"
#include <vector>

namespace MCPP {

using namespace std;

class Walker
{
public:
    Walker();
    ~Walker();

    void reset();

    MCfloat r0[3];  /**< @brief position before the \f$ n \f$-th step*/
    MCfloat k0[3];  /**< @brief direction unit vector for the \f$ (n-1) \f$-th step*/
    MCfloat walkTime;  /**< @brief total walk time */
    int type;
};

}
#endif // WALKER_H
