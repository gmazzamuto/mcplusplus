#ifndef SOURCE_H
#define SOURCE_H

#include "walker.h"
#include "distributions.h"

class Source
{
public:
    Source();
    virtual Walker *constructWalker();
    void setr0Distribution(AbstractDistribution **distrArray);
    void setk0Distribution(AbstractDistribution **distrArray);
    void setWalkTimeDistribution(AbstractDistribution *distr);

private:
    AbstractDistribution *r0Distribution[3];
    AbstractDistribution *k0Distribution[3];
    AbstractDistribution *walkTimeDistribution;
};

#endif // SOURCE_H
