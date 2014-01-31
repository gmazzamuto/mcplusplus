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
    void onFocusedWaistChanged(double val);

private:
    void replot();
    QGraphicsScene* scene;
    QGraphicsView *graphicsView;
    QDoubleSpinBox *lensDistSpinBox;
    QDoubleSpinBox *focusedWaistSpinBox;
    double lensDistance;
    double focusedWaist;
};

#endif // MAINWINDOW_H
