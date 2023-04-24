#include "KDTree.hpp"
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
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts>

QT_CHARTS_USE_NAMESPACE

#define INTERVAL 1000

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

// 二维点
struct Point {
    int number;
    double x, y;
    Point(int number, double x = 0, double y = 0) : number(number), x(x), y(y) {}
};
double UI_ymin, UI_ymax, UI_rowCnt, UI_colCnt;
bool timerRuniing = true;
QTimer *m_pTimer;
QScatterSeries *polarSeries1 = new QScatterSeries();
QScatterSeries *rectangleSeries2 = new QScatterSeries();
QScatterSeries *referPointSeries3 = new QScatterSeries();
QScatterSeries *selectPointSeries4 = new QScatterSeries();
using namespace std;
string mag = "5"; // 2 3 4 5 6 6.5
map<int, double> mapAppRa, mapAppDec;
map<string, int> magMap = {
    {"2", 46},
    {"3", 165},
    {"4", 480},
    {"5", 1471},
    {"6", 4559},
    {"6.5", 7982},
};
map<string, int> markerSize = {
    {"2", 16},
    {"3", 14},
    {"4", 10},
    {"5", 8},
    {"6", 6},
    {"6.5", 4},
};

int starsNum;

QVector<QPointF> starPointsAllPolar;
QVector<QPointF> starPointsAllRectangle;
vector<Point> pointss;
ifstream inFile;
vector<vector<double>> starVec;
site_info loc = {39.6069, 115.8921}; // 温度10摄氏度 高度10米 大气压1013毫巴=1013百帕
vector<vector<double>> ref;

KDTree tree;
cat_entry fk5;
time_t now;
tm *utc;
clock_t start, endt; // 定义clock_t变量

int refNum;
vector<vector<double>> refPoints1;
QVector<QPointF> refPoints;
QVector<QPointF> willMarkPoints;
vector<int> ref2total;

void MainWindow::updateProgress() {

    start = clock(); // 开始时间
    double ra, dec, zd, az, tjd;
    string lineStr;

    now = time(0);
    utc = gmtime(&now);
    tjd = julian_date(1900 + utc->tm_year, 1 + utc->tm_mon, utc->tm_mday, utc->tm_hour + utc->tm_min / 60.0 + utc->tm_sec / 3600.0);
    int i = 0;
    while (i < starsNum) {
        fk5.ra = starVec[i][1];
        fk5.dec = starVec[i][2];
        fk5.promora = starVec[i][3];
        fk5.promodec = starVec[i][4];
        fk5.parallax = starVec[i][5];
        app_star(tjd, &fk5, &ra, &dec);
        equ2hor(tjd, &loc, ra, dec, &zd, &az);
        starVec[i][7] = az;
        starVec[i][8] = 90 - zd;
        starVec[i][9] = ra * 15;
        starVec[i][10] = dec;
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(az)));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(90 - zd)));
        ui->tableWidget->setItem(i, 4, new QTableWidgetItem(QString::number(ra * 15)));
        ui->tableWidget->setItem(i, 5, new QTableWidgetItem(QString::number(dec)));
        mapAppRa[starVec[i][0]] = ra * 15;
        mapAppDec[starVec[i][0]] = dec;

        if (zd <= 90 - UI_ymin && zd >= 90 - UI_ymax) {
            starPointsAllPolar[i].setX(az);
            starPointsAllRectangle[i].setX(az);
            starPointsAllPolar[i].setY(zd);
            starPointsAllRectangle[i].setY(90 - zd);
        }
        i++;
    }

    polarSeries1->replace(starPointsAllPolar);
    rectangleSeries2->replace(starPointsAllRectangle);
    selectPointSeries4->replace(willMarkPoints);
    vector<double> tmp(refNum, -5000);
    for (int ii = 0; ii < starsNum; ii++) {
        if (starVec[ii][8] <= UI_ymin || starVec[ii][8] >= UI_ymax)
            continue;
        auto searchResult = tree.nearest_pointIndex({starVec[ii][7], starVec[ii][8]});
        double resultX = searchResult.first[0];
        double resultY = searchResult.first[1];
        double minDist = sqrt((resultX - starVec[ii][7]) * (resultX - starVec[ii][7]) + (resultY - starVec[ii][8]) * (resultY - starVec[ii][8]));

        double value = -minDist - starVec[ii][6];
        if (value > tmp[searchResult.second]) {
            tmp[searchResult.second] = value;
            ref2total[searchResult.second] = ii;
        }
    }
    for (int ii = 0; ii < refNum; ii++) {
        int curnum = ref2total[ii];
        if (curnum == -1) {
            ui->tableWidget_2->setItem(ii, 0, new QTableWidgetItem(QString::number(-1)));
            for ( int iii = 0; iii < 6; iii++ )
                ui->tableWidget_2->setItem(ii, iii, new QTableWidgetItem(QString::number(-1)));
            continue;
        }
        ui->tableWidget_2->setItem(ii, 0, new QTableWidgetItem(QString::number(starVec[curnum][0])));
        ui->tableWidget_2->setItem(ii, 1, new QTableWidgetItem(QString::number(starVec[curnum][7])));
        ui->tableWidget_2->setItem(ii, 2, new QTableWidgetItem(QString::number(starVec[curnum][8])));
        ui->tableWidget_2->setItem(ii, 3, new QTableWidgetItem(QString::number(starVec[curnum][6])));
        ui->tableWidget_2->setItem(ii, 4, new QTableWidgetItem(QString::number(mapAppRa[starVec[curnum][0]])));
        ui->tableWidget_2->setItem(ii, 5, new QTableWidgetItem(QString::number(mapAppDec[starVec[curnum][0]])));
        willMarkPoints[ii] = QPointF(starVec[curnum][7], starVec[curnum][8]);
    }
    endt = clock();                                                                // 结束时间
    qDebug() << "time = " << double(endt - start) / CLOCKS_PER_SEC << "s" << endl; // 输出时间（单位：ｓ）

}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {

    ui->setupUi(this);
    ui->splitter_2->setStretchFactor(0, 1);
    ui->splitter_2->setStretchFactor(1, 1);
    m_pTimer = new QTimer();
    m_pTimer->setInterval(INTERVAL);
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(updateProgress()));
    on_pushButton_clicked();


    QPolarChart *chart = new QPolarChart();

    chart->addSeries(polarSeries1);
    chart->legend()->hide();

    //![2]
    QValueAxis *angularAxis = new QValueAxis();
    angularAxis->setTickCount(9); // First and last ticks are co-located on 0/360 angle.
    angularAxis->setLabelFormat("%d");
    angularAxis->setShadesVisible(true);
    angularAxis->setShadesBrush(QBrush(QColor(249, 249, 255)));
    angularAxis->setRange(0, 360);
    chart->addAxis(angularAxis, QPolarChart::PolarOrientationAngular);
    polarSeries1->attachAxis(angularAxis);

    QCategoryAxis *elevationAxis = new QCategoryAxis;
    elevationAxis->setRange(0, 90);
    for (unsigned int i = 0; i <= 90; i += 10)
        elevationAxis->append(QString::number(90 - i), i);
    elevationAxis->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
    elevationAxis->setLabelsVisible(true);
    chart->addAxis(elevationAxis, QPolarChart::PolarOrientationRadial);
    polarSeries1->attachAxis(elevationAxis);
    polarSeries1->setMarkerSize(markerSize[mag]);
    referPointSeries3->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    referPointSeries3->setMarkerSize(10);
    referPointSeries3->setBorderColor(Qt::black);
    referPointSeries3->setColor(Qt::transparent);
    selectPointSeries4->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    selectPointSeries4->setMarkerSize(10);
    selectPointSeries4->setBorderColor(Qt::red);
    selectPointSeries4->setColor(Qt::transparent);

    QChart *chart1 = new QChart();
    referPointSeries3->replace(refPoints);
    chart1->addSeries(rectangleSeries2);
    chart1->addSeries(referPointSeries3);
    chart1->addSeries(selectPointSeries4);
    chart1->legend()->hide();
    chart1->createDefaultAxes();
    chart1->axes(Qt::Horizontal).first()->setRange(0, int(360));
    chart1->axes(Qt::Vertical).first()->setRange(0, int(90));
    QValueAxis *axisY = qobject_cast<QValueAxis *>(chart1->axes(Qt::Vertical).first());
    axisY->setLabelFormat("%d");
    QValueAxis *axisX = qobject_cast<QValueAxis *>(chart1->axes(Qt::Horizontal).first());
    axisX->setLabelFormat("%d");
    rectangleSeries2->setMarkerSize(markerSize[mag]);

    ui->graphicsView->setChart(chart);
    ui->graphicsView_2->setChart(chart1);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView_2->setRenderHint(QPainter::Antialiasing);

}

MainWindow::~MainWindow() {
    delete ui;
}




void MainWindow::on_pushButton_clicked() {
    m_pTimer->stop();
    mag = ui->comboBox->currentText().toStdString();
    starsNum = magMap[mag];
    starPointsAllPolar.clear();
    starPointsAllRectangle.clear();
    starVec.clear();
    starPointsAllPolar.resize(starsNum);
    starPointsAllRectangle.resize(starsNum);
    starVec.resize(starsNum, vector<double>(11));
    inFile.close();
    inFile.open("J2000_" + mag + ".csv");
    UI_rowCnt = ui->spinBox->value();
    UI_colCnt = ui->spinBox_2->value();
    refNum = UI_rowCnt * UI_colCnt;
    willMarkPoints.clear();
    willMarkPoints.resize(refNum);
    ref2total.clear();
    ref2total.resize(refNum, -1);
    UI_ymin = ui->lineEdit->text().toDouble();
    UI_ymax = ui->lineEdit_2->text().toDouble();

    ui->tableWidget->setRowCount(starsNum);
    ui->tableWidget_2->setRowCount(refNum);
    ui->tableWidget->setColumnCount(6);
    ui->tableWidget_2->setColumnCount(6);
    ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget_2->setAlternatingRowColors(true);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget_2->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QStringList strs = {QString("hip序号"), QString("方位角"), QString("高度角"), QString("星等"), QString("视赤经"), QString("视赤纬")};
    ui->tableWidget->setHorizontalHeaderLabels(strs);
    ui->tableWidget_2->setHorizontalHeaderLabels(strs);


    int i = 0;
    string lineStr;

    while (getline(inFile, lineStr)) {
        stringstream ss(lineStr);
        string str;
        getline(ss, str, ',');
        starVec[i][0] = stod(str);
        getline(ss, str, ',');
        starVec[i][1] = stod(str);
        getline(ss, str, ',');
        starVec[i][2] = stod(str);
        getline(ss, str, ',');
        starVec[i][3] = stod(str);
        getline(ss, str, ',');
        starVec[i][4] = stod(str);
        getline(ss, str, ',');
        starVec[i][5] = stod(str);
        getline(ss, str, ',');
        starVec[i][6] = stod(str);
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(starVec[i][0])));
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(QString::number(starVec[i][6]))); //
        i++;
    }

    refPoints1.clear();
    refPoints.clear();
    double pointX = 360.0 / UI_colCnt / 2, pointY = UI_ymin;
    for ( int i = 0; i < UI_rowCnt; i++ ) {
        pointX = 360.0 / UI_colCnt / 2;
        for ( int j = 0; j < UI_colCnt; j++ ) {
            refPoints1.push_back({pointX, pointY});
            refPoints << QPoint(pointX, pointY);
            pointX += 360.0 / UI_colCnt;
        }
        pointY += (UI_ymax - UI_ymin) / (UI_rowCnt - 1);
    }
    referPointSeries3->replace(refPoints);
    pointVec pointsTmp;
    for (int i = 0; i < refNum; i++) {
        pointsTmp.push_back({refPoints1[i][0], refPoints1[i][1]});
    }
    tree = KDTree(pointsTmp);
    m_pTimer->start();
}



void MainWindow::on_tableWidget_2_cellDoubleClicked(int row, int column) {
    qDebug() << "chijing: " << ui->tableWidget_2->item(row, 4)->text() << " chiwei " << ui->tableWidget_2->item(row, 5)->text();
}
