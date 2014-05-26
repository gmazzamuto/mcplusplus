#include <stdio.h>
#include <QApplication>
#include <QMainWindow>
#include <QAction>
#include <QTimer>
#include "mcmoviecreator.h"

const char *progName;

void usage(FILE *f) {
    fprintf(f, "\n"
            "usage: %s [options] inputFile.h5\n"
            "\n"
            "[options]:\n"
            "\t -h display help\n"
            "\t -b [size] binning in time\n"
            "\t -t [tbrk] walker types (defaults to tb)\n"
            "\n", progName);
}

int main(int argc, char *argv[])
{
    progName = argv[0];

    unsigned int wFlags = FLAG_TRANSMITTED | FLAG_BALLISTIC;
    MCfloat binSize = 5e-2;

    //parse command line options
    char c;
    extern char *optarg;
    extern int optind;
    while ((c = getopt(argc, argv, "hb:t:")) != -1) {
        switch (c) {
        case 'h':
            usage(stdout);
            exit(EXIT_SUCCESS);
            break;

        case 'b':
            binSize = atof(optarg);
            break;

        case 't':
        {
            XMLParser parser;
            wFlags = parser.walkerSaveFlags(optarg);
        }
            break;

        default:
            break;
        }
    }

    if(argc-optind < 1) {
        fprintf(stderr,"Error: input file not specified\n");
        usage(stderr);
        exit(EXIT_FAILURE);
    }

    const char *fileName = argv[optind++];

    QApplication a(argc, argv);
    QMainWindow w;
    MCMovieCreator movieCreator(fileName);
    movieCreator.setBinSize(binSize);
    movieCreator.setWalkerFlags(wFlags);

    QAction *grabFrameBufferAct = new QAction("&Grab Frame Buffer", &w);
    grabFrameBufferAct->setShortcut(QObject::tr("Ctrl+G"));
    QObject::connect(grabFrameBufferAct, SIGNAL(triggered()),
            &movieCreator, SLOT(grab()));

    w.addAction(grabFrameBufferAct);
    w.setCentralWidget(&movieCreator);
    w.resize(800,800);
    w.show();

    QTimer timer;
    timer.setSingleShot(true);
    timer.setInterval(100);
    QObject::connect(&timer, SIGNAL(timeout()), &movieCreator, SLOT(createMovie()));

    timer.start();

    a.exec();
    return 0;
}

