#ifndef WALKER_H
#define WALKER_H

#include "MCglobal.h"
#include <vector>

using namespace std;

class Walker
{
public:
    Walker();
    ~Walker();

    inline void swap_r0_r1()
    {
        MCfloat *temp = r1;
        r1 = r0;
        r0 = temp;
    }

    inline void swap_k0_k1()
    {
        MCfloat *temp = k1;
        k1 = k0;
        k0 = temp;
    }
    void reset();

    MCfloat *r0;  /**< @brief position before the \f$ n \f$-th step*/
    MCfloat *r1;  /**< @brief position after the \f$ n \f$-th step*/
    MCfloat *k0;  /**< @brief direction unit vector for the \f$ (n-1) \f$-th step*/
    MCfloat *k1;  /**< @brief direction unit vector for the \f$ n \f$-th step*/
    MCfloat walkTime;  /**< @brief total walk time */
};

#endif // WALKER_H
