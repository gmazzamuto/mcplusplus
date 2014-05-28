#ifndef MCMOVIECREATOR_H
#define MCMOVIECREATOR_H

#include <QString>
#include <QRectF>
#include "h5outputfile.h"

class MCMovieCreator : public H5FileHelper
{

public:
    explicit MCMovieCreator(QString fileName, BaseObject *parent = 0);
    void setBinSize(const MCfloat ps);
    void setBinSizeX(const MCfloat um);
    void setWalkerFlags(const unsigned int value);
    void setSquare(const QRectF rect);
    void createMovie(const QString fileName);
    void setTimeRange(const MCfloat startTime, const MCfloat endTime);

private:
    MCfloat *dataTimes[4];
    MCfloat *dataPoints[4];
    H5OutputFile file;
    MCfloat binSize;
    MCfloat binSizeX;
    unsigned int wFlags;
    QRectF sq;
    MCfloat startTime, endTime;
};

#endif // MCMOVIECREATOR_H
