#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include "baseobject.h"
#include "walker.h"

namespace MCPP {

class Histogram : public BaseObject
{
public:
    Histogram(BaseObject *parent=NULL);
    virtual ~Histogram();

    void setType(const enum MCData type1, const enum MCData type2 = DATA_NONE);
    void setMax(const MCfloat max1, const MCfloat max2 = 0);
    void setMax2(const MCfloat max2);
    void setMin(const MCfloat min1, const MCfloat min2 = 0);
    void setMin2(const MCfloat min2);
    void setBinSize(const MCfloat binSize1, const MCfloat binSize2 = 0);
    void setBinSize2(const MCfloat binSize2);
    void setSpatialVarianceEnabled(const bool enable);
    bool is1D() const;
    bool is2D() const;
    bool initialize();
    void setPhotonTypeFlags(int value);
    void run(const Walker * const buf, size_t bufSize);
    bool pickPhoton(const Walker * const w);
    void appendCounts(const Histogram *rhs);
    void dump() const;
    void saveToFile(const char *fileName) const;
    void setScale(u_int64_t totalPhotons);
    void setName(const char *name);
    u_int64_t *histo;


private:
    virtual bool sanityCheck_impl() const;
    virtual bool pickPhoton_impl(const Walker * const w);
    virtual BaseObject* clone_impl() const;

    const char *histName;

    enum MCData type[2];
    MCfloat min[2], max[2];
    MCfloat binSize[2];
    bool computeSpatialVariance;
    int photonTypeFlags;

    MCfloat firstBinCenter[2];
    MCfloat firstBinEdge[2];
    size_t nBins[2];
    MCfloat *variance;

    MCfloat degPerRad;
    size_t totBins;
    u_int64_t scale;
};

}

#endif // HISTOGRAM_H
