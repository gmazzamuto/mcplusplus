#include <stdio.h>
#include <QApplication>
#include <QMainWindow>
#include <QAction>
#include <QTimer>
#include "mcmoviecreator.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QMainWindow w;
    MCMovieCreator movieCreator(argv[1]);

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

