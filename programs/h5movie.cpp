#include "h5movie.h"

H5Movie::H5Movie() :
   H5FileHelper()
{
   frameData = NULL;
}

/**
 * @brief Allocates the memory needed to store a single frame.
 *
 * The newly allocated memory is accessible through the frameData pointer.
 */

void H5Movie::allocateFrameDataMemory() {
    if(frameData !=NULL)
        free(frameData);
    frameData = new double[getNpixelsPerFrame()];
}

void H5Movie::closeDataSet_impl()
{
    if(frameData != NULL) {
        delete [] frameData;
        frameData = NULL;
    }
}


/**
 * @brief Loads a given frame in memory
 * @param t The frame to be loaded
 * @param destBuffer The destination buffer for the frame data
 *
 * If destBuffer is left unspecified, frameData will be used to store the data.
 * If specified, it must be valid and big enough to hold all the frame data.
 */

void H5Movie::loadFrame(hsize_t t, double *destBuffer) {
    hsize_t start[3];
    hsize_t count[3];
    start[0] = 0;
    start[1] = 0;
    start[2] = t;
    count[0] = getNx();
    count[1] = getNy();
    count[2] = 1;
    double *buf = NULL;
    if(destBuffer==NULL) {
        if(frameData==NULL)
            allocateFrameDataMemory();
        buf = frameData;
    }
    else
        buf = destBuffer;
    loadHyperSlab(start, count, buf);
}


/**
 * @brief Writes a frame in the current dataset
 * @param srcBuffer The buffer where the data to be written is stored
 * @param t The time frame to be written
 */

void H5Movie::writeFrame(double *srcBuffer, hsize_t t) {
    hsize_t start[3];
    hsize_t count[3];
    start[0] = 0;
    start[1] = 0;
    start[2] = t;
    count[0] = getNx();
    count[1] = getNy();
    count[2] = 1;
    writeHyperSlab(start,count,srcBuffer);
}

/**
 * @brief Writes a frame in the current dataset
 * @param srcBuffer The buffer where the data to be written is stored
 * @param t The time frame to be written
 */

void H5Movie::writeFrame(u_int64_t *srcBuffer, hsize_t t) {
    hsize_t start[3];
    hsize_t count[3];
    start[0] = 0;
    start[1] = 0;
    start[2] = t;
    count[0] = getNx();
    count[1] = getNy();
    count[2] = 1;
    writeHyperSlab(start,count,srcBuffer);
}


/**
 * @brief Returns the total number of pixels along X for the current dataset
 * @return
 */

hsize_t H5Movie::getNx() {
    return dims[0];
}

/**
 * @brief Returns the total number of pixels along Y for the current dataset
 * @return
 */

hsize_t H5Movie::getNy() {
    return dims[1];
}

/**
 * @brief Returns the total number of pixels for frames in the current dataset
 * @return Nx*Ny
 */

hsize_t H5Movie::getNpixelsPerFrame() {
    return getNx()*getNy();
}
