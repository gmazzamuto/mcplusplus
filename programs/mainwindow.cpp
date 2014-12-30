#include "mainwindow.h"
#include <QtGui>
#include "gaussianraybundlesource.h"
#include "glwidget.h"

using namespace MCPP;

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

    MyGLWidget *mglw = new MyGLWidget();
    mglw->setLinePoints(&linePoints);
    glWidget = mglw;

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
    linePoints.clear();

    GaussianRayBundleSource *src = new GaussianRayBundleSource(100,focusedWaist,lensDistance);
    DeltaDistribution *delta1 = new DeltaDistribution(1);
    src->setWalkTimeDistribution(delta1);
    src->setSeed(0);

    MCfloat t = lensDistance;

    Walker *walker = new Walker();
    for (int i = 0; i < 1000; ++i) {
        src->spin(walker);

        float r0[3];
        r0[0] = walker->r0[0];
        r0[1] = walker->r0[1];
        r0[2] = walker->r0[2];

        float r1[3];
        r1[0] = walker->r0[0] + t*walker->k0[0] / walker->k0[2];
        r1[1] = walker->r0[1] + t*walker->k0[1] / walker->k0[2];
        r1[2] = walker->r0[2] + t;
        for (int i = 0; i < 3; ++i) {
            linePoints.push_back(r0[i]);
        }
        for (int i = 0; i < 3; ++i) {
            linePoints.push_back(r1[i]);
        }
    }

    delete walker;
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


void MyGLWidget::setLinePoints(const vector<MCfloat> *vector)
{
    linePoints = vector;
}

void MyGLWidget::paint_GL_impl()
{
    glBegin(GL_LINES);

    QColor color;
    size_t i = 0;
    size_t l = 0;
    while(i < linePoints->size()) {
        color = QColor((enum Qt::GlobalColor)(Qt::darkGray+l%15));
        glColor3f( color.redF(), color.greenF(), color.blueF() );
        glVertex3f(linePoints->at(i),linePoints->at(i+1),linePoints->at(i+2));
        i+=3;
        glVertex3f(linePoints->at(i),linePoints->at(i+1),linePoints->at(i+2));
        i+=3;
        l++;
    }

    glEnd();
}
