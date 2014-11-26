#include "outputfileviewer.h"

OutputFileViewer::OutputFileViewer(const char *fileName, QWidget *parent) :
    GLWidget(parent)
{
    file = NULL;
    transmittedExitPoints = NULL;

    file = new H5OutputFile();
    file->openFile(fileName);

//    FIXME XMLParser
//    XMLParser parser;
//    parser.parseString(file->readXMLDescription());
//    sample = parser.sample();

    transmitted = file->transmitted();
    transmittedExitPoints = (MCfloat*)malloc(2*sizeof(MCfloat)*transmitted);
    file->loadExitPoints(TRANSMITTED, transmittedExitPoints);

    reflected = file->reflected();
    reflectedExitPoints = (MCfloat*)malloc(2*sizeof(MCfloat)*reflected);
    file->loadExitPoints(REFLECTED, reflectedExitPoints);
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
    drawReflectedExitPoints();
    drawSample();
}

void OutputFileViewer::drawTransmittedExitPoints()
{
    QColor color;
    color = QColor((enum Qt::GlobalColor)(Qt::blue));
    glBegin(GL_POINTS);
    const MCfloat z = sample !=NULL ? sample->zBoundaries()->at(sample->nLayers()) : 0;
    for (u_int64_t i = 0; i < 2*transmitted; i+=2) {
        glColor3f( color.redF(), color.greenF(), color.blueF() );
        glVertex3f(transmittedExitPoints[i],transmittedExitPoints[i+1],z);
    }
    glEnd();
}

void OutputFileViewer::drawReflectedExitPoints()
{
    QColor color;
    color = QColor((enum Qt::GlobalColor)(Qt::gray));
    glBegin(GL_POINTS);
    const MCfloat z = sample !=NULL ? sample->zBoundaries()->at(0) : 0;
    for (u_int64_t i = 0; i < 2*reflected; i+=2) {
        glColor3f( color.redF(), color.greenF(), color.blueF() );
        glVertex3f(reflectedExitPoints[i],reflectedExitPoints[i+1],z);
    }
    glEnd();
}

void OutputFileViewer::drawSample() {
    if(sample == NULL)
        return;
    const deque<MCfloat> * sampleUpperZBoundaries = sample->zBoundaries();
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    size_t i = 0;
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
