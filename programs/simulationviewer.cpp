#include "simulationviewer.h"

SimulationViewer::SimulationViewer(Simulation *sim, QWidget *parent) :
    GLWidget(parent)
{
    simulation = sim;
}

void SimulationViewer::paint_GL_impl() {
    drawTrajectory();
    drawSample();
}

void SimulationViewer::drawTrajectory() {
    QColor color;
    const vector< vector<MCfloat>*> *trajectories = simulation->trajectories();
    int n = trajectories->size(); //number of walkers
    for (int k = 0; k < n; ++k) {
        glBegin(GL_LINE_STRIP);
        int i = 0, l=0;
        const vector<MCfloat>* traj = trajectories->at(k);
        while(i < traj->size()) {
            color = QColor((enum Qt::GlobalColor)(Qt::darkGray+l%15));
            glColor3f( color.redF(), color.greenF(), color.blueF() );
            glVertex3f(traj->at(i),traj->at(i+1),traj->at(i+2));
            i+=3; l++;
        }
        glEnd();
    }
}

void SimulationViewer::drawSample() {
    const deque<MCfloat> * sampleUpperZBoundaries = simulation->sample()->zBoundaries();
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    int i = 0;
    float prev = 0;

    while(i<sampleUpperZBoundaries->size()) {
        glColor3f(0,0,0);
        glLineWidth(2);
        glBegin(GL_QUAD_STRIP);
        float upperZBoundary = sampleUpperZBoundaries->at(i);
        const float height = 1000;
        float halfHeight = height/2;

        glVertex3f(halfHeight, halfHeight, prev);
        glVertex3f(-halfHeight, halfHeight, prev);
        glVertex3f(halfHeight, halfHeight, upperZBoundary);
        glVertex3f(-halfHeight, halfHeight, upperZBoundary);
        glVertex3f(halfHeight, -halfHeight, upperZBoundary);
        glVertex3f(-halfHeight, -halfHeight, upperZBoundary);

        glVertex3f(halfHeight, -halfHeight, prev);
        glVertex3f(-halfHeight, -halfHeight, prev);
        glVertex3f(halfHeight, halfHeight, prev);
        glVertex3f(-halfHeight, halfHeight, prev);

        glEnd();
        prev = upperZBoundary;
        i++;
    }

    glLineWidth(1.f); //reset linewidth
}
