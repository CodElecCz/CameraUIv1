#include <QDockWidget>
#include <QTextBrowser>
#include "MainWindow.h"
#include "DioView.h"
#include "ui_MainWindow.h"

void MainWindow::createDioWindow(bool showWindow)
{
    QDockWidget *dock = new QDockWidget(tr("Digital IO"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
    leftDockTabbing.insert(dock->windowTitle(), dock);

    dio = new DioView(dock);

    dock->setWidget(dio);
 //   dock->resize(250,400);

    addDockWidget(Qt::LeftDockWidgetArea, dock);
    if(leftDockTabbing.size()>2)
        tabifyDockWidget(leftDockTabbing.values().at(leftDockTabbing.size()-1), leftDockTabbing.values().at(1));

    ui->menuTools->addAction(dock->toggleViewAction());

    //resize Qt buck
    dock->setFloating(true);
    dock->hide();
    dock->setFloating(false);

    if(showWindow)
        dock->show();

    connect(dio, SIGNAL(addTrace(QString,QColor)), this, SLOT(addTrace(QString,QColor)));
}

void MainWindow::closeDio()
{
    if(dio)
    {
        disconnect(dio, SIGNAL(addTrace(QString,QColor)), this, SLOT(addTrace(QString,QColor)));

        dio->DioClose();
    }
}
