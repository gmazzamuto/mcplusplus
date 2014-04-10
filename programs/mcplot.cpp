#include <stdio.h>
#include <QApplication>
#include <QMainWindow>
#include "outputfileviewer.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QMainWindow w;
    OutputFileViewer viewer("mcpp-output.h5");

    viewer.setDisplayedOriginPos(0,0,-600);
    viewer.setDisplayedAxisLength(500);

    w.setCentralWidget(&viewer);
    w.show();

    a.exec();

    return 0;
}
