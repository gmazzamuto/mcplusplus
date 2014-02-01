#include "mainwindow.h"
#include <QtGui>
#include "source.h"
#include "glwidget.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    lensDistance = 50;
    focusedWaist = 10;

    lensDistSpinBox = new QDoubleSpinBox();
    lensDistSpinBox->setMaximum(1000);
    lensDistSpinBox->setValue(lensDistance);

    focusedWaistSpinBox = new QDoubleSpinBox();
    focusedWaistSpinBox->setMinimum(1);
    focusedWaistSpinBox->setMaximum(50);
    focusedWaistSpinBox->setValue(focusedWaist);

    glWidget = new GLWidget();

    QWidget *widget = new QWidget();
    QVBoxLayout *vertLayout = new QVBoxLayout();
    vertLayout->addWidget(glWidget);
    vertLayout->addWidget(focusedWaistSpinBox);
    vertLayout->addWidget(lensDistSpinBox);
    widget->setLayout(vertLayout);

    resize(500,800);
    setCentralWidget(widget);

    connect(lensDistSpinBox,SIGNAL(valueChanged(double)),this,SLOT(onLensDistanceChanged(double)));
    connect(focusedWaistSpinBox,SIGNAL(valueChanged(double)),this,SLOT(onFocusedWaistChanged(double)));
    replot();
}

void MainWindow::replot() {
    glWidget->clear();

    GaussianRayBundleSource *src = new GaussianRayBundleSource(100,focusedWaist,lensDistance);
    DeltaDistribution *delta1 = new DeltaDistribution(1);
    src->setWalkTimeDistribution(delta1);
    src->setSeed(0);

    double t = lensDistance;

    for (int i = 0; i < 1000; ++i) {
        Walker *walker = src->constructWalker();

        float r0[3];
        r0[0] = walker->r0[0];
        r0[1] = walker->r0[1];
        r0[2] = walker->r0[2];

        float r1[3];
        r1[0] = walker->r0[0] + t*walker->k0[0];
        r1[1] = walker->r0[1] + t*walker->k0[1];
        r1[2] = walker->r0[2] + t*walker->k0[2];
        glWidget->addLine(r0,r1);
        delete walker;
    }

    delete delta1;
    delete src;
    glWidget->updateGL();
}

void MainWindow::onLensDistanceChanged(double val) {
    lensDistance = val;
    replot();
}

void MainWindow::onFocusedWaistChanged(double val) {
    focusedWaist = val;
    replot();
}
