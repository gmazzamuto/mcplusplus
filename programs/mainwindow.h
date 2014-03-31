#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QSpinBox>
#include "glwidget.h"

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
};

#endif // MAINWINDOW_H
