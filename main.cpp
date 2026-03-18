#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 中文显示
    QFont font("SimHei");
    a.setFont(font);

    MainWindow w;
    w.show();

    return a.exec();
}