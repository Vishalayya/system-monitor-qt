#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include "systeminfo.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateSystemInfo();   // ✅ moved from lambda

private:
    Ui::MainWindow *ui;

    SystemInfo sys;
    QTimer *timer;
    void updateProcessTable();

    QChart *chart;
    QLineSeries *cpuSeries;
    QLineSeries *ramSeries;
    QValueAxis *axisX;
    QValueAxis *axisY;

    int x = 0;
    int maxPoints = 50;
};

#endif
