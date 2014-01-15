#ifndef SOURCE_H
#define SOURCE_H

#include "walker.h"
#include "distributions.h"

/**
 * @brief Base Source class: constructs and initializes a Walker according to
 * given source term modeling.
 */

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


/**
 * @brief Pencil beam source \f$ \delta(\bm{r}) \, \delta (t) \, \delta (\bm{k})
 * \f$, with \f$ \bm{k} = (0,0,1) \f$.
 */

class PencilBeamSource : public Source
{
public:
    PencilBeamSource();
    Walker *constructWalker();
};

#endif // SOURCE_H
