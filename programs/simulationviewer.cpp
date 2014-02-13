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
    const vector<double> *trajectory = simulation->trajectory();
    glBegin(GL_LINE_STRIP);
    int i = 0, l=0;
    while(i < trajectory->size()) {
        color = QColor((enum Qt::GlobalColor)(Qt::darkGray+l%15));
        glColor3f( color.redF(), color.greenF(), color.blueF() );
        glVertex3f(trajectory->at(i),trajectory->at(i+1),trajectory->at(i+2));
        i+=3; l++;
    }
    glEnd();
}

void SimulationViewer::drawSample() {
    const deque<double> * sampleUpperZBoundaries = simulation->sample()->zBoundaries();
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
