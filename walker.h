#ifndef WALKER_H
#define WALKER_H

class Walker
{
public:
    Walker();
    double r0[3];  /**< @brief position before the \f$ n \f$-th step*/
    double r1[3];  /**< @brief position after the \f$ n \f$-th step*/
    double k0[3];  /**< @brief direction unit vector for the \f$ (n-1) \f$-th step*/
    double k1[3];  /**< @brief direction unit vector for the \f$ n \f$-th step*/
    double walkTime;  /**< @brief time elapsed up to the current position*/
    double walkDistance;  /**< @brief distance covered to the current position*/
};

#endif // WALKER_H
