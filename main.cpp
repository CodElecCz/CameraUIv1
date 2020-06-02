#include "MainWindow.h"
#include <QApplication>
#include <pylon/PylonIncludes.h>

using namespace Pylon;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //SetGenICamLogConfig(qApp->applicationDirPath().toStdString().c_str());
    PylonInitialize();

    MainWindow w;
    w.show();

    int ret = a.exec();

    PylonTerminate();

    return ret;
}
