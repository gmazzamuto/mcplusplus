/*
  This file is part of MCPlusPlus.

  MCPlusPlus is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Mcplusplus is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with MCPlusPlus.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef H5OUTPUTFILE_H
#define H5OUTPUTFILE_H

#include "h5filehelper.h"
#include "sample.h"

namespace MCPP {

/**
 * @brief The H5OutputFile class allows to manipulate the HDF5 files generated
 * by MCPlusPlus
 *
 * The H5 files created by MCPlusPlus are organized in groups and datasets.
 *
 * The root group contains the following datasets:
 *
 * - photon-counters: an array of 4 elements (one per walkerType), containing
 * the total number of photons of that type
 *
 * The "RNGStates" group contains as many datasets as the maximum number of
 * threads used up to that moment. Each thread uses its own RNG, so the state
 * of each thread's RNG is saved in a dedicated dataset.
 *
 * Three other groups contain the actual simulated data: "exit-k-vectors",
 * "exit-points", "walk-times". Each group contains as many datasets as are the
 * photon types currently saved (see walkerType).
 *
 * The "exit-points" dataset contains the exit \f$ (x,y) \f$ coordinates
 * written sequentially for each photon. The same applies for the saved
 * components of the exit k vectors.
 */

class H5OutputFile : public H5FileHelper
{
public:
    H5OutputFile();
    virtual ~H5OutputFile();

    virtual bool newFile(const char *fileName, bool create_datasets=true);
    void appendExitKVectors(walkerType type, const MCfloat *buffer,
                            const hsize_t size);
    void appendExitPoints(walkerType type, const MCfloat *buffer,
                          const hsize_t size);
    void appendWalkTimes(walkerType type, const MCfloat *buffer,
                         const hsize_t size);
    bool loadExitPoints(walkerType type, MCfloat *destBuffer,
                        const hsize_t *start=NULL, const hsize_t *count=NULL);
    bool loadWalkTimes(walkerType type, MCfloat *destBuffer,
                       const hsize_t *start=NULL, const hsize_t *count=NULL);
    bool loadExitKVectors(walkerType type, MCfloat *destBuffer,
                          const hsize_t *start=NULL, const hsize_t *count=NULL);
    bool loadData(MCData group, walkerType type, MCfloat *destBuffer,
                  const hsize_t *start=NULL, const hsize_t *count=NULL);

    void saveRNGState(const uint seed, const string s);
    string readRNGState(const uint seed) const;
    void appendPhotonCounts(const u_int64_t transmitted,
                            const u_int64_t ballistic,
                            const u_int64_t reflected,
                            const u_int64_t backReflected);
    void appendPhotonCounts(const u_int64_t *counters);
    u_int64_t transmitted() const;
    u_int64_t ballistic() const;
    u_int64_t reflected() const;
    u_int64_t backReflected() const;
    const u_int64_t *photonCounters() const;
    void saveSample(const Sample *sample);

private:
    bool createDatasets(uint walkTimesSaveFlags, uint exitPointsSaveFlags,
                        uint exitKVectorsSaveFlags);
    bool createRNGDataset();
    void appendTo1Ddataset(const char *datasetName, const MCfloat *buffer,
                           const hsize_t size);
    bool loadFrom1Ddataset(const char *datasetName, MCfloat *destBuffer,
                           const hsize_t *start, const hsize_t *count);
    void writeVLenString(const char *datasetName, const string str);
    string readVLenString(const char *datasetName) const;
    bool openFile_impl();

    u_int64_t _photonCounters[4];
};

}
#endif // H5OUTPUTFILE_H
