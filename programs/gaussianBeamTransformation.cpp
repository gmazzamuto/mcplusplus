#include <stdio.h>
#include "simulation.h"
#include "psigenerator.h"
#include <QApplication>
#include <QMainWindow>
#include "simulationviewer.h"

#include "gaussianraybundlesource.h"

int main(int argc, char *argv[])
{
    GaussianRayBundleSource *source = new GaussianRayBundleSource(200,10,2000);
    DeltaDistribution *delta0 = new DeltaDistribution(0);

    source->setWalkTimeDistribution(delta0);
//    source->setZLens(-1700);

    Sample *sample = new Sample();

    Material material0, material1, material2;

    material0.n = 1.5;
    material1.n = 3;
    sample->addLayer(material1,700);

    material2.n = 2;
    sample->addLayer(material2,300);
    sample->addLayer(material2,2000);

    sample->setSurroundingEnvironment(material0,material2);

    bool success;
    success = source->focus(1000,sample);
    if(!success) {
        printf("Cannot focus\n");
        exit(EXIT_FAILURE);
    }

    Simulation *sim = new Simulation();

    int n = 5000;

    sim->setSample(sample);
    sim->setSource(source);
    sim->setTotalWalkers(n);
    sim->setFresnelReflectionsEnabled(false);

    sim->setSeed(0);
    sim->setSaveTrajectoryEnabled(true);
    sim->run();

//    const vector<vector<MCfloat>*>* trajectories = sim->trajectories();

//    int c = 0;

//    MCfloat dz = 20;
//    int nPlanes = 600/dz;
//    MCfloat dr = 0.2;
//    MCfloat rMax = 200;
//    int nBins = rMax / dr;
//    int histogram[nPlanes][nBins];
//    MCfloat surfaces[nBins];

//    for (int i = 0; i < nBins; ++i) {
//        MCfloat R = dr + i*dr;
//        MCfloat r = i*dr;
//        surfaces[i] = R*R - r*r;
//    }

//    for (int i = 0; i < nPlanes; ++i) {
//        for (int j = 0; j < nBins; ++j) {
//            histogram[i][j]=0;
//        }
//    }

//    for (int i = 0; i < nBins; ++i) {
//        printf("%lf",(i+1)*dr/2);
//        if(i!=nBins-1)
//            printf(",");
//    }

//    printf("\n");

//    for (int i = 0; i < n; ++i) {
//        const vector <MCfloat> * traj = trajectories->at(i);
//        if(traj->size()!=15)
//            continue;

//        c++;
//        MCfloat x1,y1,z1;
//        MCfloat x2,y2,z2;
//        int j = 3;
//        x1 = traj->at(j++);
//        y1 = traj->at(j++);
//        z1 = traj->at(j++);
//        x2 = traj->at(j++);
//        y2 = traj->at(j++);
//        z2 = traj->at(j++);

//        printf("z1 = %lf\n",traj->at(2));

//        MCfloat norm = sqrt(pow(x2-x1,2) + pow(y2-y1,2) + pow(z2-z1,2));
//        MCfloat k[3];
//        k[0] = (x2-x1)/norm;
//        k[1] = (y2-y1)/norm;
//        k[2] = (z2-z1)/norm;

//        for (int p = 0; p < nPlanes; ++p) {
//            MCfloat z = 0 + p*dz;
//            MCfloat x = x1 + z * k[0]/k[2];
//            MCfloat y = y1 + z * k[1]/k[2];
//            MCfloat r = sqrt(x*x+y*y);
//            int binIndex = floor(r /dr);
//            if(binIndex<nBins)
//                histogram[p][binIndex]++;
//        }
//    }

//    for (int p = 0; p < nPlanes; ++p) {
//        for (int i = 0; i < nBins; ++i) {
//            printf("%lf",1.*histogram[p][i]/surfaces[i]);
//            if(i!=nBins-1)
//                printf(",");
//        }
//        printf("\n");
//    }

    QApplication a(argc, argv);
    QMainWindow w;
    SimulationViewer simViewer(sim);

    simViewer.setDisplayedOriginPos(0,0,-600);
    simViewer.setDisplayedAxisLength(500);

    w.setCentralWidget(&simViewer);
    w.show();

    a.exec();

    return 0;
}
