#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QSpinBox>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

private slots:
    void onLensDistanceChanged(double val);

private:
    void replot();
    QGraphicsScene* scene;
    QGraphicsView *graphicsView;
    QDoubleSpinBox *spinBox;
    double lensDistance;
};

#endif // MAINWINDOW_H
