#ifndef H5MOVIE_H
#define H5MOVIE_H

#include "h5filehelper.h"

using namespace MCPP;

class H5Movie : public H5FileHelper
{
public:
    H5Movie();

    void loadFrame(hsize_t t, double *destBuffer);
    void writeFrame(double *srcBuffer, hsize_t t);
    void writeFrame(u_int64_t *srcBuffer, hsize_t t);

    hsize_t getNx();
    hsize_t getNy();
    hsize_t getNpixelsPerFrame();

    double *frameData; /**< \brief Default buffer where the frame data is stored.
                            \see loadFrame() **/

private:
    void allocateFrameDataMemory();
    virtual void closeDataSet_impl();
};

#endif // H5MOVIE_H
