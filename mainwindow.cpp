#include "mainwindow.h"
#include <QtGui>
#include "source.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    lensDistance = 50;
    focusedWaist = 10;

    scene = new QGraphicsScene(-1,-5,10,10);
    graphicsView = new QGraphicsView();
    lensDistSpinBox = new QDoubleSpinBox();
    lensDistSpinBox->setMaximum(1000);
    lensDistSpinBox->setValue(lensDistance);

    focusedWaistSpinBox = new QDoubleSpinBox();
    focusedWaistSpinBox->setMinimum(1);
    focusedWaistSpinBox->setMaximum(50);
    focusedWaistSpinBox->setValue(focusedWaist);

    QWidget *widget = new QWidget();
    QVBoxLayout *vertLayout = new QVBoxLayout();
    vertLayout->addWidget(graphicsView);
    vertLayout->addWidget(focusedWaistSpinBox);
    vertLayout->addWidget(lensDistSpinBox);
    widget->setLayout(vertLayout);

    graphicsView->setScene(scene);
    graphicsView->scale(10,10);
    resize(500,500);
    setCentralWidget(widget);


    connect(lensDistSpinBox,SIGNAL(valueChanged(double)),this,SLOT(onLensDistanceChanged(double)));
    connect(focusedWaistSpinBox,SIGNAL(valueChanged(double)),this,SLOT(onFocusedWaistChanged(double)));
    replot();
}

void MainWindow::replot() {
    scene->clear();
    GaussianRayBundleSource *src = new GaussianRayBundleSource(100,focusedWaist,lensDistance);
    DeltaDistribution *delta1 = new DeltaDistribution(1);
    src->setWalkTimeDistribution(delta1);
    src->setSeed(0);

    double t = lensDistance;

    QPen pen;
    pen.setWidthF(0.1);
    for (int i = 0; i < 1000; ++i) {
        pen.setColor((enum Qt::GlobalColor)(Qt::darkGray+i%15));
        Walker *walker = src->constructWalker();
        double z0 = walker->r0[2];
        double z1 = walker->r0[2]+t*walker->k0[2];
        double x0 = walker->r0[0];
        double x1 = walker->r0[0]+t*walker->k0[0];
        scene->addLine(z0,-1*x0,z1,-1*x1,pen);
        delete walker;
    }

    delete delta1;
    delete src;
}

void MainWindow::onLensDistanceChanged(double val) {
    lensDistance = val;
    replot();
}

void MainWindow::onFocusedWaistChanged(double val) {
    focusedWaist = val;
    replot();
}
