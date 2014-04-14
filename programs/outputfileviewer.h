#ifndef OUTPUTFILEVIEWER_H
#define OUTPUTFILEVIEWER_H

#include "simulation.h"
#include "glwidget.h"
#include "h5outputfile.h"

class OutputFileViewer : public GLWidget
{
    Q_OBJECT
public:
    explicit OutputFileViewer(const char *fileName, QWidget *parent = 0);
    ~OutputFileViewer();

signals:

public slots:


private:
    void paint_GL_impl();
    void drawTransmittedExitPoints();
    void drawReflectedExitPoints();

    H5OutputFile *file;
    Sample *sample;

    u_int64_t transmitted, reflected;
    MCfloat *transmittedExitPoints, *reflectedExitPoints;
    void drawSample();
};

#endif // OUTPUTFILEVIEWER_H
