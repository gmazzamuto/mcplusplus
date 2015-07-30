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

#ifndef SIMULATION_H
#define SIMULATION_H

#include "source.h"
#include "sample.h"
#include "costhetagenerator.h"
#include "histogram.h"

#define WALKER_BUFSIZE 1000

namespace MCPP {

/**
 * @brief The Simulation class is the core of the MC method
 *
 * In order to set up a simulation, you first have to specify a Sample and a
 * Source using setSample() and setSource(). Use setNWalkers() to specify the
 * total number of walkers to be simulated.
 *
 * Several Histograms can be performed with every simulation; see addHistogram.
 * The output file can be specified using setOutputFileName(). Additionally,
 * raw output with the data of each single simulated photons can be enabled
 * using setRawOutputEnabled(). In the latter case output flags can be
 * specified with their setter functions: setWalkTimesSaveFlags(), etc.; keep
 * in mind that this causes heavy memory usage and big output file sizes.
 *
 * Before running the simulation, a RNG has to be initialized by either calling
 * setSeed(), loadGeneratorState() or setGeneratorState(). Use run() to start
 * the simulation.
 *
 * If you want to run the simulation in parallel threads, use setNThreads() to
 * specify the number of threads to be used. In this case multiple RNG states
 * can be loaded with setMultipleRNGStates(), otherwise sequential numbers from
 * 0 to the number of threads will be used as seeds for each thread.
 *
 * <h2>Signals</h2> The progress of the simulation currently running can be
 * printed on stderr by sending the USR1 signal to the process instantiating a
 * Simulation object. Progress of all threads can be printed by sending
 * SIGUSR2. Finally the TERM signal causes the simulation to terminate
 * gracefully (see terminate()).
 */

class Simulation : public BaseRandom
{
public:
    Simulation(BaseObject *parent=NULL);
    ~Simulation();

    const Sample *sample() const;
    void setSaveTrajectoryEnabled(bool enabled = true);
    void setNPhotons(const u_int64_t N);
    void setNWalkers(const u_int64_t N);
#ifdef SWIG
    %apply SWIGTYPE *DISOWN {Sample *sample};
#endif
    void setSample(Sample *sample);
#ifdef SWIG
    %apply SWIGTYPE *DISOWN {Source *source};
#endif
    void setSource(Source *source);
    void setFresnelReflectionsEnabled(bool enable);
    void setNThreads(unsigned int value);
    u_int64_t nPhotons() const;
    u_int64_t currentPhoton() const;
    void setOutputFileName(const char *name);
    void run();
    void clear();
    const vector<vector<MCfloat> *> *trajectories() const;
    void reportProgress() const;
    void setMultipleRNGStates(const vector<string> states);
    void setTimeOriginZ(const MCfloat z);
    void setWalkTimesSaveFlags(unsigned int value);
    void setExitPointsSaveFlags(unsigned int value);
    void setExitKVectorsSaveFlags(unsigned int value);
    void setExitKVectorsDirsSaveFlags(unsigned int value);
    void terminate();
    uint nThreads();
#ifdef SWIG
    %apply SWIGTYPE *DISOWN {Histogram *hist};
#endif
    void addHistogram(Histogram *hist);
    void setRawOutputEnabled(bool enable);

private:
    unsigned int layerAt(const MCfloat *r0) const;
    void handleInterface();
    void reflect();
    void refract();

    void appendTrajectoryPoint(MCfloat *point);
    void appendWalkTime();
    void appendExitPoint(walkerType type);
    void appendExitKVector(walkerType type);
    void appendWalker(walkerType type);

    void runMultipleThreads();
    bool runSingleThread();

    void switchToLayer(const uint layer);
    void updateLayerVariables(const uint layer);
    void flushHistogram();
    void saveRawOutput();

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

    virtual BaseObject* clone_impl() const;
    virtual bool sanityCheck_impl() const;
    virtual void describe_impl() const;
    virtual void setRNG_impl();

    const Sample *_sample;
    const Source *source;

    //flags
    bool saveTrajectory;
    bool fresnelReflectionsEnabled;
    bool rawOutputEnabled;
    unsigned int walkTimesSaveFlags, exitPointsSaveFlags, exitKVectorsDirsSaveFlags, exitKVectorsSaveFlags;
    MCfloat timeOriginZ;

    //walker counters
    u_int64_t _totalWalkers;  /**< @brief total number of walkers to be simulated*/
    u_int64_t photonCounters[4];
    u_int64_t n;

    uint nBuf;
    uint _nThreads;

    //trajectories
    vector<vector <MCfloat>*> *trajectoryPoints;
    vector<MCfloat> *currentTrajectory;

    vector<MCfloat> exitPoints[4];
    vector<MCfloat> walkTimes[4];
    vector<MCfloat> exitKVectors[4];

    //internal temporary variables
    CosThetaGenerator deflCosine;
    MCfloat currLayerLowerBoundary, currLayerUpperBoundary;
    const Material *currentMaterial;
    const MCfloat *mus;
    MCfloat currentMus;
    const MCfloat  *upperZBoundaries;
    const Material *materials;
    unsigned int nLayers;
    unsigned int layer0;
    unsigned int layer1;
    MCfloat *r0, *r1;
    MCfloat *k0, *k1;
    const vector<u_int64_t> *_nInteractions;
    MCfloat totalLengthInCurrentLayer;
    bool kNeedsToBeScattered;
    bool walkerExitedSample;
    time_t startTime;
    Walker walker;
    MCfloat n0, n1, cosTheta1;

    const char *outputFile;
    vector<string> multipleRNGStates;
    vector<Histogram *> hists;
    bool forceTermination;
    Walker walkerBuf[WALKER_BUFSIZE];
};

}
#endif // SIMULATION_H
