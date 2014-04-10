#ifndef OUTPUTFILEVIEWER_H
#define OUTPUTFILEVIEWER_H

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

    H5OutputFile *file;

    unsigned long int transmitted;
    MCfloat *transmittedExitPoints;
};

#endif // OUTPUTFILEVIEWER_H
