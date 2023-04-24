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
#include <QLabel>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
//#include <QtWidgets/QApplication>
//#include <QtWidgets/QMainWindow>
#include <QtCharts>
#include <Qt>

QT_CHARTS_USE_NAMESPACE

#define MAX_ELEVATION 90
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
    Point(int number, double x = 0, double y = 0): number(number), x(x), y(y) {}
};

bool timerRuniing = true;
QTimer* m_pTimer;
QScatterSeries *series1 = new QScatterSeries();
QScatterSeries *series2 = new QScatterSeries();
QScatterSeries *series3 = new QScatterSeries();
QScatterSeries *series4 = new QScatterSeries();
bool updateGraph = true;
bool updateEquApp = true;
using namespace std;
string mag = "3"; // 2 3 4 5 6 6.5
map <int, double> mapAppRa, mapAppDec;
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

QVector<QPointF> starPoints(RowCount);
QVector<QPointF> starPoints1(RowCount);
vector<Point> pointss;
ifstream inFile("J2000_" + mag + ".csv");
//ofstream outFileName("J2000_" + mag + "hor.csv");
ofstream outFileName;
vector<vector<double>> vec(RowCount, vector<double>(11));
//site_info loc = {31.8642, 117.287}; // 温度10摄氏度 高度10米 大气压1013毫巴=1013百帕
site_info loc = {39.6069, 115.8921}; // 温度10摄氏度 高度10米 大气压1013毫巴=1013百帕
vector<vector<double>> benchmark;

double interval_degree = 360.0 / 16;
double plimit[2][2] = {{0, 0}, { 0, 0}};
const double vlimit[2][2] = { {-1.5, 1.5}, {-1.5, 1.5} }; // 设置速度限制
int PSO_N = 15, PSO_d = 2, PSO_ger = 10;
double PSO_w = 0.8, PSO_c1 = 0.5, PSO_c2 = 0.5;
// 使用默认随机数引擎
std::default_random_engine generator;
// 声明均匀分布
std::uniform_real_distribution<double> distribution(0.0, 1.0);

std::vector<std::vector<double>> PSO_xx(PSO_N, std::vector<double>(PSO_d, 0.0)); // 初始种群的位置
std::vector<std::vector<double>> PSO_xm(PSO_N, std::vector<double>(PSO_d, 0.0)); // 初始种群的位置
std::vector<std::vector<double>> PSO_v(PSO_N, std::vector<double>(PSO_d, 0.0)); // 初始种群的位置
vector<double> PSO_ym = { 0, 0};
vector<double> PSO_fxm;
double PSO_fym = INT_MIN;
int PSO_n;
vector<int> PSO_fx;
//KDNode *root;
KDTree tree;
//vector<vector<double>> rev;
cat_entry fk5;
time_t now;
tm *utc;
clock_t start, endt;                 // 定义clock_t变量

int benchMarkNum = 64;
vector<vector<double>> ret2;
QVector<QPointF> benchMarkPoints;
QVector<QPointF> willMarkPoints(benchMarkNum);
vector<int> rev(benchMarkNum, -1);
void MainWindow::updateProgress() {
    start = clock(); // 开始时间
    double ra, dec, zd, az,  tjd;
    string lineStr;
    int cur = 0;
    now = time(0);
    utc = gmtime(&now);
//    tjd = julian_date(1900 + utc->tm_year, 1 + utc->tm_mon, utc->tm_mday, utc->tm_hour + utc->tm_min / 60.0 + utc->tm_sec / 3600.0);
    tjd = 2460022.1547208796;
    qDebug() << " tjd " << QString::number(tjd, 'f', 10);
//    if (outFile) {
//        outFileName.open("J2000_" + mag + "hor.csv", ios::out);
////        points.clear();
//    }
    while (cur < RowCount ) {
        fk5.ra = vec[cur][1];
        fk5.dec = vec[cur][2];
        fk5.promora = vec[cur][3];
        fk5.promodec = vec[cur][4];
        fk5.parallax = vec[cur][5];
        app_star(tjd, &fk5, &ra, &dec);
        equ2hor(tjd, &loc, ra, dec, &zd, &az);
        vec[cur][7] = az;
        vec[cur][8] = 90 - zd;
        vec[cur][9] = ra * 15;
        vec[cur][10] = dec;
        ui->tableWidget->setItem(cur, 1, new QTableWidgetItem(QString::number(az)));
        ui->tableWidget->setItem(cur, 2, new QTableWidgetItem(QString::number(90 - zd)));
        if ( updateEquApp ) {
            ui->tableWidget->setItem(cur, 4, new QTableWidgetItem(QString::number(ra * 15)));
            ui->tableWidget->setItem(cur, 5, new QTableWidgetItem(QString::number(dec)));
            mapAppRa[vec[cur][0]] = ra * 15;
            mapAppDec[vec[cur][0]] = dec;
        }

        if (updateGraph && zd <= 75 && zd >= 10) {
//            series1->append(az, zd);
//            starPoints << QPoint(az, zd);
            starPoints[cur].setX(az);
            starPoints1[cur].setX(az);
            starPoints[cur].setY(zd);
            starPoints1[cur].setY(90 - zd);
//            series2->append(az, 90 - zd);
        }

        cur++;
    }

    series1->replace(starPoints);
    series2->replace(starPoints1);
    series4->replace(willMarkPoints);
    updateEquApp = false;

    vector <double> tmp(benchMarkNum, -500);
    for ( int ii = 0; ii < RowCount; ii++ ) {
        if (vec[ii][8] <= 15 || vec[ii][8] >= 80 )continue;
        auto daan = tree.nearest_pointIndex({vec[ii][7], vec[ii][8]});
        double daan_x = daan.first[0];
        double daan_y = daan.first[1];
        double minDist =  sqrt((daan_x - vec[ii][7]) * (daan_x - vec[ii][7]) + (daan_y - vec[ii][8]) * (daan_y - vec[ii][8]));


        double value =  -minDist - vec[ii][6];
        if ( value > tmp[daan.second] ) {
            tmp[daan.second] = value;
            rev[daan.second] = ii;
        }
    }
    for ( int ii = 0; ii < benchMarkNum; ii++ ) {

        int curnum = rev[ii];
        if ( curnum == -1 ) {
            ui->tableWidget_2->setItem(ii, 0, new QTableWidgetItem(QString::number(-1)));
            continue;
        }
        ui->tableWidget_2->setItem(ii, 0, new QTableWidgetItem(QString::number(vec[curnum][0])));
        ui->tableWidget_2->setItem(ii, 1, new QTableWidgetItem(QString::number(vec[curnum][7])));
        ui->tableWidget_2->setItem(ii, 2, new QTableWidgetItem(QString::number(vec[curnum][8])));
        ui->tableWidget_2->setItem(ii, 3, new QTableWidgetItem(QString::number(vec[curnum][6])));
        ui->tableWidget_2->setItem(ii, 4, new QTableWidgetItem(QString::number(mapAppRa[vec[curnum][0]])));
        ui->tableWidget_2->setItem(ii, 5, new QTableWidgetItem(QString::number(mapAppDec[vec[curnum][0]])));
        willMarkPoints[ii] = QPointF(vec[curnum][7], vec[curnum][8]);
    }

//    if ( outFile ) {
//        outFile = false;


//        KDNode *root = build(points, 0, points.size(), 0);
//        Point target(0, 350, 350);
//        Point ans(0, 0, 0);
//        double minDist = 1e9;
//        findNearest(root, target, ans, minDist, 0);
//        cout << "Nearest point: (" << ans.x << ", " << ans.y << ")" << ans.number << " mindist " << minDist << endl;
///////////////////////
//        for ( int i = 0; i < PSO_N; i++ ) {
//            PSO_xx[i][0] = plimit[0][0] + (plimit[0][1] - plimit[0][0]) * distribution(generator);
//            PSO_xm[i][0] = PSO_xx[i][0];
//            PSO_xx[i][1] = plimit[1][0] + (plimit[1][1] - plimit[1][0]) * distribution(generator);
//            PSO_xm[i][1] = PSO_xx[i][1] ;
//            PSO_v[i][0] = distribution(generator);
//            PSO_v[i][1] = distribution(generator);
//        }
//        for ( int iter = 0; iter < 1/*PSO_ger*/; iter++ ) {
//            for ( int inn = 0; inn < 1/*PSO_N*/; inn++) {
//                vector<vector<double>> benchmark1(PSO_n, vector<double>(2, 0.0));
//                points.clear();
//                for ( int iPSO_n = 0; iPSO_n < PSO_n; iPSO_n++ ) {
//                    benchmark1[iPSO_n][0] = benchmark[iPSO_n][0] + PSO_xx[inn][0];
//                    if (benchmark1[iPSO_n][0] > 360) benchmark1[iPSO_n][0] -= 360.0;
//                    benchmark1[iPSO_n][1] = benchmark[iPSO_n][1] + PSO_xx[inn][1];
//                    points.push_back({ iPSO_n,  benchmark1[iPSO_n][0], benchmark1[iPSO_n][1]});
//                }
//                root = build(points, 0, benchmark1.size(), 0);
//                vector<double> tmp(PSO_n, -9999);
//                for( int iRowCount = 0; iRowCount < RowCount; iRowCount++ ) {
//                    if (vec[iRowCount][8] <= 0) continue;
//                    Point target(vec[iRowCount][0], vec[iRowCount][7], vec[iRowCount][8]);
//                    Point ans(0, 0, 0);
//                    double minDist = 1e9;
//                    findNearest(root, target, ans, minDist, 0);
//                    double value = -minDist - vec[iRowCount][6];
////                    qDebug() << "a" << ans.number << "|" << ans.x << "*" << ans.y  << "," << target.x << "." << target.y << "-" << target.number << "+" << minDist;
//                    if ( value > tmp[iRowCount] ) {
//                        tmp[iRowCount] = value;
//                        rev[iRowCount] = {vec[iRowCount][7], vec[iRowCount][8]};
//                    }
//                    PSO_fx[inn] = accumulate(tmp.begin(), tmp.end(), 0);
//                    if (PSO_fxm[inn] <  PSO_fx[inn] ) {
//                        PSO_fxm[inn] =  PSO_fx[inn];
//                        PSO_xm[inn] = {PSO_xx[inn][0], PSO_xx[inn][1]};
//                    }
//                }
//                auto maxvth = max_element(PSO_fxm.begin(), PSO_fxm.end());
//                if (PSO_fym < *maxvth ) {
//                    PSO_fym = *maxvth;
//                    PSO_ym = PSO_xm[maxvth - PSO_fxm.begin()];
//                }
////                PSO_v = PSO_v * PSO_w + PSO_c1 * (PSO_xx - PSO_)

//            }

//        }
//        qDebug() << ":::: ";
///////////////////////
//        for ( int i = 0; i < xxx.size(); i++ ) {
//            for ( int j = 0 ; j < xxx[i].size(); j++ ) {
//                cout << xxx[i][j] << " ";
//            }
//            cout << endl;
//        }
    qDebug() << "save OK" << double(clock() - start) / CLOCKS_PER_SEC << "s" << endl;
//    }
    outFileName.close();
    endt = clock();                                                            // 结束时间
    qDebug()  << "time = " << double(endt - start) / CLOCKS_PER_SEC << "s" << endl; // 输出时间（单位：ｓ）

}

MainWindow::MainWindow(QWidget * parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    ui->splitter_3->setStretchFactor(0, 1);
    ui->splitter_3->setStretchFactor(1, 3);


    ui->tableWidget->setRowCount(RowCount);
    ui->tableWidget_2->setRowCount(benchMarkNum);
    ui->tableWidget->setColumnCount(6);
    ui->tableWidget_2->setColumnCount(6);
    ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget_2->setAlternatingRowColors(true);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget_2->setEditTriggers(QAbstractItemView::NoEditTriggers);
//    ui->tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:rgb(40,143,218);color: white;};");
//    QStringList strs = {QString::fromLocal8Bit("hip序号"), QString::fromLocal8Bit("方位角"), QString::fromLocal8Bit("高度角"), QString::fromLocal8Bit("星等")};
    QStringList strs = {QString("hip序号"), QString("方位角"), QString("高度角"), QString("星等"), QString("视赤经"), QString("视赤纬")};
    ui->tableWidget->setHorizontalHeaderLabels(strs);
    ui->tableWidget_2->setHorizontalHeaderLabels(strs);
    m_pTimer = new QTimer();
    m_pTimer->setInterval(INTERVAL);

    double xx = 19.7408, yy = 15.3451 ;
    while ( xx < 360 && yy <= 80) {
        if ( xx >= -0.01 ) {
            ret2.push_back({xx, yy});
            benchMarkPoints << QPoint(xx, yy);
//            qDebug() << xx << yy;
        }
        xx += interval_degree;
        if ( xx >= 359.9) {
            xx -= (360 + interval_degree / 2);
            yy += interval_degree / 2 * sqrt(3);
        }
    }
//    qDebug() << ret2.size();
//    for ( int i = 0; i < benchmark.size(); i++ ) {
//        for ( int j = 0 ; j < benchmark[i].size(); j++ ) {
//            cout << benchmark[i][j] << " ";
//        }
//        cout << endl;
//    }
    pointVec pointsss;
    for ( int i = 0; i < benchMarkNum; i++ ) {
//        pointss.push_back({ i, ret2[i][0], ret2[i][1]});
        pointsss.push_back({ ret2[i][0], ret2[i][1]});
//        qDebug() << ret2[i][0] << ret2[i][1];
    }
    tree = KDTree(pointsss);
//    root = build_kd_tree(pointss, 0, pointss.size() - 1, 0);
//    root = build(pointss, 0, benchMarkNum, 0);
//    plimit[0][1] = interval_degree;
//    plimit[1][1] = 90 - benchmark.back()[1];
//    PSO_n = benchmark.size();
//    PSO_fx.resize(PSO_n, 0);
//    PSO_fxm.resize(PSO_N, INT_MIN);
//    rev.resize(PSO_n, vector<double>(2, 0));
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
//    const qreal radialMin = 0;
//    const qreal radialMax = 110;

    QPolarChart *chart = new QPolarChart();



//    series1->setName("scatter");
//    for (int i = angularMin; i <= angularMax; i += 10) {
//        series1->append(i, 90 - (i / 4.0 / radialMax) * radialMax - 8.0);
//        qDebug() << i << (i / 4.0 / radialMax) * radialMax + 8.0;
//    }

    chart->addSeries(series1);
    chart->legend()->hide();

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
    series3->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    series3->setMarkerSize(10);
    series3->setBorderColor(Qt::black);
    series3->setColor(Qt::transparent);
    series4->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    series4->setMarkerSize(10);
    series4->setBorderColor(Qt::red);
    series4->setColor(Qt::transparent);


    QChart *chart1 = new QChart();
    series3->replace(benchMarkPoints);
    chart1->addSeries(series2);
    chart1->addSeries(series3);
    chart1->addSeries(series4);
    chart1->legend()->hide();
    chart1->createDefaultAxes();
    chart1->axes(Qt::Horizontal).first()->setRange(0, int(360));
    chart1->axes(Qt::Vertical).first()->setRange(0, int(90));
    QValueAxis *axisY = qobject_cast<QValueAxis*>(chart1->axes(Qt::Vertical).first());
    axisY->setLabelFormat("%d");
    QValueAxis *axisX = qobject_cast<QValueAxis*>(chart1->axes(Qt::Horizontal).first());
    axisX->setLabelFormat("%d");
    series2->setMarkerSize(markerSize[mag]);




    ui->graphicsView->setChart(chart);
    ui->graphicsView_2->setChart(chart1);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView_2->setRenderHint(QPainter::Antialiasing);

    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(updateProgress()));

    m_pTimer->start();

}


MainWindow::~MainWindow() {
    delete ui;
}


//void MainWindow::on_pushButton_clicked() {
//    updateGraph = !updateGraph;
//    if ( !updateGraph) {
//        ui->pushButton->setText("更新图像");
//    } else ui->pushButton->setText("停止更新图像");
//}

void MainWindow::on_pushButton_3_clicked() {
    updateEquApp = true;
}

void MainWindow::on_pushButton_2_clicked() {
//    outFile = 1;
    if  (timerRuniing ) {
        m_pTimer->stop();
        timerRuniing = !timerRuniing;
    } else {
        m_pTimer->start();
        timerRuniing = !timerRuniing;
    }
}


//void MainWindow::on_pushButton_2_clicked(bool checked) {
//    if (checked) {
//        m_pTimer->stop();
//    } else {
//        m_pTimer->start();
//    }
//}

void MainWindow::on_pushButton_clicked() {
//    m_pTimer->stop();
    outFileName.open("J2000_" + mag + "hor.csv", ios::out);
    int cur = 0;
    while (cur < RowCount ) {
        double az = vec[cur][7];
        double zd  = vec[cur][8];
        double ra  = vec[cur][9];
        double dec  = vec[cur][10];
        if ( zd <= 80 && zd >= 15) {
            outFileName << vec[cur][0] << "," << az << "," << zd << "," << vec[cur][6] << "," << ra << "," << dec << "\n";
        }
        cur++;
    }
    qDebug() << " OK ";
    outFileName.close();
//    m_pTimer->start();
}
