#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setToolTip("BLE&shutdown");
    w.setWindowIcon(QIcon(":/icon/icon.png"));
    w.show();
    a.setWindowIcon(QIcon(":/icon/icon.png"));
    return a.exec();
}
