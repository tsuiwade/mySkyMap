
#include <QtCharts>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <fstream>
#include <sstream>
#include <QDebug>
#include <myNovas.h>
#include <QTimer>
#include <QMap>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QPolarChart>
#include <QtCharts/QAbstractAxis>
#include <QtCharts/QValueAxis>
#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
//#include <QtWidgets/QApplication>
//#include <QtWidgets/QMainWindow>
#include <QtCharts>

QT_CHARTS_USE_NAMESPACE

#define MAX_ELEVATION 90
QScatterSeries *series1 = new QScatterSeries();

using namespace std;
string mag = "5"; // 2 3 4 5 6 6.5

map <string, int> magMap = {
    {"2",   46},
    {"3",   165},
    {"4",   480},
    {"5",   1471},
    {"6",   4559},
    {"6.5", 7982},
};
map <string, int> markerSize = {
    {"2",   16},
    {"3",   14},
    {"4",   10},
    {"5",   8},
    {"6",   6},
    {"6.5", 4},
};
int RowCount =  magMap[mag];

ifstream inFile("J2000_" + mag + ".csv");
vector<vector<double>> vec(RowCount, vector<double>(7));
site_info loc = {31.8642, 117.287}; // 温度10摄氏度 高度10米 大气压1013毫巴=1013百帕

cat_entry fk5;
time_t now;
tm *utc;
clock_t start, endt;                 // 定义clock_t变量
void MainWindow::updateProgress() {

    qDebug() << " gu ";
    series1->clear();

    start = clock(); // 开始时间
    double ra, dec, zd, az,  tjd;
    string lineStr;
    int cur = 0;
    now = time(0);
    utc = gmtime(&now);
    tjd = julian_date(1900 + utc->tm_year, 1 + utc->tm_mon, utc->tm_mday, utc->tm_hour + utc->tm_min / 60.0 + utc->tm_sec / 3600.0);
    while (cur < RowCount ) {
        fk5.ra = vec[cur][1];
        fk5.dec = vec[cur][2];
        fk5.promora = vec[cur][3];
        fk5.promodec = vec[cur][4];
        fk5.parallax = vec[cur][5];
        app_star(tjd, &fk5, &ra, &dec);
        equ2hor(tjd, &loc, ra, dec, &zd, &az);
        ui->tableWidget->setItem(cur, 1, new QTableWidgetItem(QString::number(az)));
        ui->tableWidget->setItem(cur, 2, new QTableWidgetItem(QString::number(90 - zd)));
        series1->append(az, zd);

        cur++;
    }
    endt = clock();                                                            // 结束时间
    qDebug()  << "time = " << double(endt - start) / CLOCKS_PER_SEC << "s" << endl; // 输出时间（单位：ｓ）

}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    ui->tableWidget->setRowCount(RowCount);
    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
//    ui->tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:rgb(40,143,218);color: white;};");
//    QStringList strs = {QString::fromLocal8Bit("hip序号"), QString::fromLocal8Bit("方位角"), QString::fromLocal8Bit("高度角"), QString::fromLocal8Bit("星等")};
    QStringList strs = {QString("hip序号"), QString("方位角"), QString("高度角"), QString("星等")};
    ui->tableWidget->setHorizontalHeaderLabels(strs);
    QTimer* m_pTimer = new QTimer();
    m_pTimer->setInterval(3000);



    int cur = 0;
    string lineStr;
    while (getline(inFile, lineStr)) {
        stringstream ss(lineStr);
        string str;
        getline(ss, str, ',');
        vec[cur][0] = stod(str);
        getline(ss, str, ',');
        vec[cur][1] = stod(str);
        getline(ss, str, ',');
        vec[cur][2] = stod(str);
        getline(ss, str, ',');
        vec[cur][3] = stod(str);
        getline(ss, str, ',');
        vec[cur][4] = stod(str);
        getline(ss, str, ',');
        vec[cur][5] = stod(str);
        getline(ss, str, ',');
        vec[cur][6] = stod(str);
        ui->tableWidget->setItem(cur, 0, new QTableWidgetItem(QString::number(vec[cur][0])));
        ui->tableWidget->setItem(cur, 3, new QTableWidgetItem(QString::number(vec[cur][6]))); // hgmag
        cur ++;
    }
    const qreal angularMin = 0;
    const qreal angularMax = 360;

    const qreal radialMin = 0;
    const qreal radialMax = 110;

    QPolarChart *chart = new QPolarChart();



    series1->setName("scatter");
//    for (int i = angularMin; i <= angularMax; i += 10) {
//        series1->append(i, 90 - (i / 4.0 / radialMax) * radialMax - 8.0);
//        qDebug() << i << (i / 4.0 / radialMax) * radialMax + 8.0;
//    }

    chart->addSeries(series1);

    //![2]
    QValueAxis *angularAxis = new QValueAxis();
    angularAxis->setTickCount(9); // First and last ticks are co-located on 0/360 angle.
    angularAxis->setLabelFormat("%d");
    angularAxis->setShadesVisible(true);
    angularAxis->setShadesBrush(QBrush(QColor(249, 249, 255)));
    angularAxis->setRange(angularMin, angularMax);
    chart->addAxis(angularAxis, QPolarChart::PolarOrientationAngular);
    series1->attachAxis(angularAxis);

//    QValueAxis *radialAxis = new QValueAxis();
//    radialAxis->setTickCount(12);
//    radialAxis->setLabelFormat("%d");
//    radialAxis->setRange(radialMin, radialMax);
//    radialAxis->setlab
//    chart->addAxis(radialAxis, QPolarChart::PolarOrientationRadial);
//    series1->attachAxis(radialAxis);
//    //![2]
    QCategoryAxis *elevationAxis = new QCategoryAxis;
    elevationAxis->setRange(0, MAX_ELEVATION);
    for(unsigned int i = 0; i <= MAX_ELEVATION; i += 10)
        elevationAxis->append(QString::number(MAX_ELEVATION - i), i);
    elevationAxis->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
    elevationAxis->setLabelsVisible(true);
    chart->addAxis(elevationAxis, QPolarChart::PolarOrientationRadial);
    series1->attachAxis(elevationAxis);
    series1->setMarkerSize(markerSize[mag]);

    ui->graphicsView->setChart(chart);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);

    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(updateProgress()));

    m_pTimer->start();

}


MainWindow::~MainWindow() {
    delete ui;
}

