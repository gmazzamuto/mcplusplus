#include "outputfileviewer.h"

OutputFileViewer::OutputFileViewer(const char *fileName, QWidget *parent) :
    GLWidget(parent)
{
    file = NULL;
    transmittedExitPoints = NULL;


    file = new H5OutputFile();
    file->openFile(fileName);


    transmitted = file->transmitted();
    transmittedExitPoints = (MCfloat*)malloc(2*sizeof(MCfloat)*transmitted);
    file->loadTransmittedExitPoints(transmittedExitPoints);
}

OutputFileViewer::~OutputFileViewer()
{
    file->close();
    delete file;
    if(transmittedExitPoints != NULL)
        free(transmittedExitPoints);
}

void OutputFileViewer::paint_GL_impl() {
    drawTransmittedExitPoints();
}

void OutputFileViewer::drawTransmittedExitPoints()
{
    QColor color;
    color = QColor((enum Qt::GlobalColor)(Qt::blue));
    glBegin(GL_POINTS);
    for (int i = 0; i < 2*transmitted; i+=2) {
        glColor3f( color.redF(), color.greenF(), color.blueF() );
        glVertex3f(transmittedExitPoints[i],transmittedExitPoints[i+1],200);
    }
    glEnd();
}
