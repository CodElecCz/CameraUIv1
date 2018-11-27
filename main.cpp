#include "MainWindow.h"
#include <QApplication>
#include <pylon/PylonIncludes.h>

using namespace Pylon;

int main(int argc, char *argv[])
{
    PylonInitialize();

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    int ret = a.exec();

    PylonTerminate();

    return ret;
}
