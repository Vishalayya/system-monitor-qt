#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCharts/QChartView>
#include <QVBoxLayout>
#include <QPen>
#include <windows.h>
#include <tlhelp32.h>
#include <QTabWidget>
#include <psapi.h>
#pragma comment(lib, "psapi.lib")

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->processTable->setSortingEnabled(true);

    // =======================
    // 🎨 Global Style
    // =======================
    this->setStyleSheet(
        "QMainWindow { background-color: #1e1e1e; color: white; }"
        "QLabel { color: white; font-size: 14px; }"
        "QProgressBar { background-color: #2e2e2e; border: 1px solid #555; }"
        "QProgressBar::chunk { border-radius: 5px; }"
        );

    // Title + Button styles (ONLY ONCE)
    ui->Title->setStyleSheet(
        "font-size: 22px; font-weight: bold; letter-spacing: 2px;"
        );

    ui->pushButton->setStyleSheet(
        "QPushButton { background:#333; border:1px solid #555; padding:8px; }"
        "QPushButton:hover { background:#444; }"
        );

    ui->CPUProgress->setStyleSheet(
        "QProgressBar { border: 1px solid #555; background: #2e2e2e; }"
        "QProgressBar::chunk { background-color: #00ff00; }"
        );
    ui->processTable->setColumnCount(3);
    ui->processTable->setHorizontalHeaderLabels(
        {"Process Name", "PID", "Memory"}
        );
    ui->processTable->horizontalHeader()->setStretchLastSection(true);
    // =======================
    // 📊 Chart Setup
    // =======================
    cpuSeries = new QLineSeries();
    ramSeries = new QLineSeries();

    cpuSeries->setName("CPU");
    ramSeries->setName("RAM");
    cpuSeries->setPointsVisible(false);
    ramSeries->setPointsVisible(false);
    chart = new QChart();
    chart->addSeries(cpuSeries);
    chart->addSeries(ramSeries);
    chart->setTitle("CPU & RAM Usage");
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    axisX = new QValueAxis();
    axisX->setRange(0, 50);

    axisY = new QValueAxis();
    axisY->setRange(0, 100);

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    cpuSeries->attachAxis(axisX);
    cpuSeries->attachAxis(axisY);

    ramSeries->attachAxis(axisX);
    ramSeries->attachAxis(axisY);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // Chart styling
    chart->setBackgroundBrush(QBrush(QColor("#2b2b2b")));
    chart->setPlotAreaBackgroundVisible(true);
    chart->setPlotAreaBackgroundBrush(QBrush(QColor("#1e1e1e")));

    ui->chart->setStyleSheet(
        "background-color: #2b2b2b;"
        "border-radius: 10px;"
        "padding: 10px;"
        );

    QVBoxLayout *layout = new QVBoxLayout(ui->chart);
    layout->addWidget(chartView);

    // =======================
    // ⏱ Timer Setup
    // =======================
    timer = new QTimer(this);

    connect(timer, &QTimer::timeout, this, &MainWindow::updateSystemInfo);

    timer->start(500);  // smoother + efficient
}

// =======================
// 🔁 Update Function
// =======================
void MainWindow::updateSystemInfo()
{
    double cpu = sys.getcpuUsage();
    double ram = sys.getRamUsage();

    // Graph colors
    QPen cpuPen;
    cpuPen.setColor(cpu < 40 ? Qt::green : (cpu < 70 ? Qt::yellow : Qt::red));
    cpuPen.setWidth(2);
    cpuSeries->setPen(cpuPen);

    QPen ramPen;
    ramPen.setColor(ram < 40 ? Qt::green : (ram < 70 ? Qt::yellow : Qt::red));
    ramPen.setWidth(2);
    ramSeries->setPen(ramPen);

    // Update graph
    cpuSeries->append(x, cpu);
    ramSeries->append(x, ram);

    if (cpuSeries->count() > maxPoints) {
        cpuSeries->remove(0);
        ramSeries->remove(0);
    }

    x++;

    // Update UI
    ui->cpu->setText("CPU: " + QString::number(cpu, 'f', 1) + "%");
    ui->ram->setText("RAM: " + QString::number(ram, 'f', 1) + "%");

    ui->CPUProgress->setValue((int)cpu);
    ui->RAMProgress->setValue((int)ram);

    // Status
    if (cpu < 40)
        ui->status->setText("Low Load");
    else if (cpu < 70)
        ui->status->setText("Moderate Load");
    else
        ui->status->setText("High Load");

    // RAM color only (dynamic)
    if (ram < 40)
        ui->RAMProgress->setStyleSheet("QProgressBar::chunk { background-color: green; }");
    else if (ram < 70)
        ui->RAMProgress->setStyleSheet("QProgressBar::chunk { background-color: yellow; }");
    else
        ui->RAMProgress->setStyleSheet("QProgressBar::chunk { background-color: red; }");
    updateProcessTable();
}
void MainWindow::updateProcessTable()
{
    ui->processTable->setRowCount(0); // clear old data

    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnap == INVALID_HANDLE_VALUE)
        return;

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);
    ui->processTable->setSortingEnabled(false);
    if (Process32First(hSnap, &pe))
    {
        int row = 0;

        do {
            if(row>30) break;
            ui->processTable->insertRow(row);

            // Process Name
            ui->processTable->setItem(row, 0,
                                      new QTableWidgetItem(QString::fromWCharArray(pe.szExeFile)));

            // PID
            ui->processTable->setItem(row, 1,
                                      new QTableWidgetItem(QString::number(pe.th32ProcessID)));

            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe.th32ProcessID);

            if (hProcess)
            {
                PROCESS_MEMORY_COUNTERS pmc;

                if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
                {
                    SIZE_T mem = pmc.WorkingSetSize / (1024 * 1024); // MB

                    ui->processTable->setItem(row, 2,
                                              new QTableWidgetItem(QString::number(mem) + " MB"));
                }

                CloseHandle(hProcess);
            }
            else
            {
                ui->processTable->setItem(row, 2,
                                          new QTableWidgetItem("N/A"));
            }
            row++;

        } while (Process32Next(hSnap, &pe));
    }
    ui->processTable->setSortingEnabled(true);

    CloseHandle(hSnap);
}

// =======================
// 🧹 Destructor
// =======================
MainWindow::~MainWindow()
{
    delete ui;
}
