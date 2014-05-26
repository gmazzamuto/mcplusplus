#ifndef MCMOVIECREATOR_H
#define MCMOVIECREATOR_H

#include "glwidget.h"
#include "h5outputfile.h"

class MCMovieCreator : public GLWidget
{
    Q_OBJECT
public:
    explicit MCMovieCreator(QString fileName, QWidget *parent = 0);
    void setPoints(const MCfloat *points, size_t n);
    void setBinSize(const MCfloat ps);
    void setWalkerFlags(unsigned int value);

signals:

public slots:
    void createMovie();

private:
    virtual void paint_GL_impl();


private:
    MCfloat *dataTimes[4];
    MCfloat *dataPoints[4];
    H5OutputFile file;
    QString currentText;
    const MCfloat *points;
    size_t nPoints;
    MCfloat binSize;
    unsigned int wFlags;
};

#endif // MCMOVIECREATOR_H
