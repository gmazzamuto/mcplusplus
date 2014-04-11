#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QSpinBox>
#include "glwidget.h"

class MyGLWidget : public GLWidget
{
    Q_OBJECT
public:
    MyGLWidget() { linePoints = NULL; }
    virtual ~MyGLWidget() { }

    void setLinePoints(const vector<MCfloat> *vector);

private:
    void paint_GL_impl();
    const vector<MCfloat> *linePoints;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

private slots:
    void onLensDistanceChanged(double val);
    void onFocusedWaistChanged(double val);

private:
    void replot();
    QDoubleSpinBox *lensDistSpinBox;
    QDoubleSpinBox *focusedWaistSpinBox;
    MCfloat lensDistance;
    MCfloat focusedWaist;
    GLWidget *glWidget;
    vector<MCfloat> linePoints;
};

#endif // MAINWINDOW_H
