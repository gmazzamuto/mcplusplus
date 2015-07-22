/*
  This file is part of MCPlusPlus.

  MCPlusPlus is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Foobar is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with MCPlusPlus.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include "baseobject.h"
#include "walker.h"

namespace MCPP {

/**
 * @brief The Histogram class provides a flexible live histogramming interface
 *
 * A single Histogram object describes a single 1D or 2D histogram of the
 * simulated data. The histogram parameters such as bin size and range can be
 * specified using the respective setter functions; an extra last bin keeps
 * track of overflow counts. The data domain (i.e. exit time, space or angles)
 * and photon type (e.g. transmitted, reflected, etc.) to be histogrammed must
 * be specified through setDataDomain() and setPhotonTypeFlags() respectively.
 *
 * Multiple Histograms can be added to a Simulation object and are performed
 * live during the simulation; see Simulation::addHistogram().
 *
 * Histograms can be assigned a name through setName() and are saved in a H5
 * file in a dataset with that name at the end of the simulation. When saved,
 * data are scaled with the total number of simulated photons.
 *
 * \pre The following conditions must hold for a Histogram to be in a valid
 * state:
 * - data domain and photon type must be specified
 *
 * - a positive bin size must be specified
 *
 * - a valid upper limit for each axis must be specified. Minimum defaults to 0.
 *
 * - spatial variance can only be computed for 1D Histograms in the time domain
 */

class Histogram : public BaseObject
{
public:
    Histogram(BaseObject *parent=NULL);
    virtual ~Histogram();

    void setDataDomain(const enum MCData type1, const enum MCData type2 = DATA_NONE);
    void setMax(const double max1);
    void setMax(const double max1, const double max2);
    void setMax2(const double max2);
    void setMin(const double min1);
    void setMin(const double min1, const double min2);
    void setMin2(const double min2);
    void setBinSize(const double binSize1);
    void setBinSize(const double binSize1, const double binSize2);
    void setBinSize2(const double binSize2);
    void setSpatialVarianceEnabled(const bool enable);
    bool is1D() const;
    bool is2D() const;
    bool initialize();
    void setPhotonTypeFlags(int value);
    void run(const Walker * const buf, size_t bufSize);
    void appendCounts(const Histogram *rhs);
    void dump() const;
    void saveToFile(const char *fileName) const;
    void setScale(u_int64_t totalPhotons);
    void setName(const char *name);

private:
    virtual bool sanityCheck_impl() const;
    virtual bool pickPhoton_impl(const Walker * const w) const;
    virtual BaseObject* clone_impl() const;
    bool pickPhoton(const Walker * const w) const;

    string histName;

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
    u_int64_t *histo;
};

}

#endif // HISTOGRAM_H
