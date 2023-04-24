#include "mainwindow.h"
#include "KDTree.hpp"

#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("mySkyMap");
    w.show();
    return a.exec();
}
