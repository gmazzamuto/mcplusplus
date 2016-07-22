#!/usr/bin/env python3

from pymcplusplus import *

# define a sample consisting of two layers of different materials
sample = Sample()

mat = Material()
mat.n = 1.5
mat.g = 0
mat.ls = 1000

# add a layer of material mat and thickness 1000um
sample.addLayer(mat, 1000)

mat2 = Material()
mat2.n = 1.3
mat.g = 0.5
mat.ls = 700

sample.addLayer(mat2, 500)

externalMaterial = Material()
externalMaterial.n = 1

sample.setSurroundingEnvironment(externalMaterial)

# define a photon source
source = PencilBeamSource()

# define the main simulation object, simulate 1e6 photons using 8 parallel
# threads. Note that each thread will use increasing seeds starting from 0.
sim = Simulation()
sim.setSample(sample)
sim.setSource(source)
sim.setNPhotons(1000000)
sim.setNThreads(8)
sim.setSeed(0)
sim.setOutputFileName("example.h5")

# define and add several histograms to the simulation

# 1) a histogram of the exit times
hist = Histogram()
hist.setDataDomain(DATA_TIMES)
hist.setPhotonTypeFlags(FLAG_TRANSMITTED)
hist.setMax(1000)
hist.setBinSize(2)
hist.setName("times")
hist.addMomentExponent(2)  # compute spatial variance
sim.addHistogram(hist)

# 2) a histogram of the exit distances from the center
hist = Histogram()
hist.setDataDomain(DATA_POINTS)
hist.setPhotonTypeFlags(FLAG_TRANSMITTED)
hist.setMax(1000000)
hist.setBinSize(50)
hist.setName("points")
sim.addHistogram(hist)

# 3) a bivariate histogram of the exit distances as a function of time
hist = Histogram()
hist.setDataDomain(DATA_POINTS, DATA_TIMES)
hist.setPhotonTypeFlags(FLAG_TRANSMITTED)
hist.setMax(100000, 1000)
hist.setBinSize(500, 2)
hist.setName("points_vs_times")
sim.addHistogram(hist)

sim.run()
