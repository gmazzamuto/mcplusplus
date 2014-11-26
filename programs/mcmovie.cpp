#include <stdio.h>
#include "mcmoviecreator.h"

#include "h5movie.h"

#include <boost/optional.hpp>

using namespace boost;

const char *progName;

void usage(FILE *f) {
    fprintf(f, "\n"
            "usage: %s [options] inputFile.h5 outputFile.h5\n"
            "\n"
            "[options]:\n"
            "\t -h display help\n"
            "\t -b [size] binning in time (in ps)\n"
            "\t -s [size] binning in space (in um)\n"
            "\t -t [tbrk] walker types (defaults to tb)\n"
            "\t -x [pos] x coordinate of the top-right corner of the square\n"
            "\t -y [pos] y coordinate of the top-right corner of the square\n"
            "\t -w [size] square width\n"
            "\t -i [ps] start time\n"
            "\t -j [ps] end time\n"
            "\n", progName);
}

int main(int argc, char *argv[])
{
    progName = argv[0];

    unsigned int wFlags = FLAG_TRANSMITTED | FLAG_BALLISTIC;
    MCfloat binSize = 5e-2;
    MCfloat binSizeX = 5e-2;
    optional<MCfloat> x;
    optional<MCfloat> y;
    optional<MCfloat> width;
    MCfloat startTime = 0;
    MCfloat endTime = 0;

    //parse command line options
    char c;
    extern char *optarg;
    extern int optind;
    while ((c = getopt(argc, argv, "hb:s:t:x:y:w:i:j:")) != -1) {
        switch (c) {
        case 'h':
            usage(stdout);
            exit(EXIT_SUCCESS);
            break;

        case 'b':
            binSize = atof(optarg);
            break;

        case 's':
            binSizeX = atof(optarg);
            break;

        case 'x':
            x = atof(optarg);
            break;

        case 'y':
            y = atof(optarg);
            break;

        case 'w':
            width = atof(optarg);
            break;

        case 'i':
            startTime = atof(optarg);
            break;

        case 'j':
            endTime = atof(optarg);
            break;

        case 't':
            wFlags = walkerSaveFlags(optarg);
            break;

        default:
            break;
        }
    }

    if(startTime == endTime) {
        fprintf(stderr,"Error: please specify a valid time range\n");
        usage(stderr);
        exit(EXIT_FAILURE);
    }

    if(!x.is_initialized() || !y.is_initialized()) {
        fprintf(stderr,"Error: square coordinates not specified\n");
        usage(stderr);
        exit(EXIT_FAILURE);
    }

    if(!width.is_initialized()) {
        fprintf(stderr,"Error: square length not specified\n");
        usage(stderr);
        exit(EXIT_FAILURE);
    }

    if(argc-optind < 2) {
        fprintf(stderr,"Error: files not specified\n");
        usage(stderr);
        exit(EXIT_FAILURE);
    }

    const char *fileName = argv[optind++];
    const char *outputFileName = argv[optind++];

    MCMovieCreator movieCreator(fileName);
    movieCreator.setBinSize(binSize);
    movieCreator.setBinSizeX(binSizeX);
    movieCreator.setWalkerFlags(wFlags);
    movieCreator.setTimeRange(startTime,endTime);
    QRectF sq;
    sq.setTopRight(QPointF(x.get(),y.get()));
    sq.setBottomLeft(QPointF(x.get()-width.get(),y.get()-width.get()));
    movieCreator.setSquare(sq);

    movieCreator.createMovie(outputFileName);
    return 0;
}

