#include <QDockWidget>
#include <QTextBrowser>
#include "MainWindow.h"
#include "DioView.h"
#include "ui_mainwindow.h"

void MainWindow::createDioWindows(bool showWindow)
{
    QDockWidget *dock = new QDockWidget(tr("Digital IO"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);

    dio = new DioView(dock);

    dock->setWidget(dio);
 //   dock->resize(250,400);

    addDockWidget(Qt::RightDockWidgetArea, dock);
    ui->menuTools->addAction(dock->toggleViewAction());

    //resize Qt buck
    dock->setFloating(true);
    dock->hide();
    dock->setFloating(false);

    if(showWindow)
    {
        dock->show();
    }
}
