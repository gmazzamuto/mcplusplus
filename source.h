#ifndef SOURCE_H
#define SOURCE_H

#include "walker.h"
#include "distributions.h"

class Source
{
public:
    Source();
    virtual Walker *constructWalker();

private:
    AbstractDistribution *r0Distribution[3];
    AbstractDistribution *k0Distribution[3];
    AbstractDistribution *walkTimeDistribution;
};

#endif // SOURCE_H
