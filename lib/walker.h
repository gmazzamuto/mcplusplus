#ifndef WALKER_H
#define WALKER_H

#include "MCtypes.h"
#include <vector>

using namespace std;

class Walker
{
public:
    Walker();
    ~Walker();

    void swap_r0_r1();
    void swap_k0_k1();
    void reset();

    MCfloat *r0;  /**< @brief position before the \f$ n \f$-th step*/
    MCfloat *r1;  /**< @brief position after the \f$ n \f$-th step*/
    MCfloat *k0;  /**< @brief direction unit vector for the \f$ (n-1) \f$-th step*/
    MCfloat *k1;  /**< @brief direction unit vector for the \f$ n \f$-th step*/
    MCfloat walkTime;  /**< @brief total walk time */
};

#endif // WALKER_H
