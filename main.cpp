#include <QtGui/QApplication>
//#include "mainwindow.h"
#include "mainwizard.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    MainWindow w;
    MainWizard w;
    w.show();
    return a.exec();
}
