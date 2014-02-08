#ifndef SIMULATIONVIEWER_H
#define SIMULATIONVIEWER_H

#include "glwidget.h"
#include "simulation.h"

class SimulationViewer : public GLWidget
{
public:
    SimulationViewer(Simulation *simulation, QWidget *parent = 0);

private:
    void drawTrajectory();
    void drawSample();
    void paint_GL_impl();

    Simulation *simulation;
};

#endif // SIMULATIONVIEWER_H
