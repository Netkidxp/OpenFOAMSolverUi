#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    if(argc>1)
    {
        QString url(argv[1]);
        w.showAndRun(url);
    }
    else
        w.showAndRun();
    return a.exec();
}
