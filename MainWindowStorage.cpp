#include <QDockWidget>
#include <QTextBrowser>
#include "MainWindow.h"
#include "StorageView.h"
#include "ui_MainWindow.h"

void MainWindow::createStorageWindow(bool showWindow, QString storagePath)
{
    QDockWidget *dock = new QDockWidget(tr("Storage"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);

    storage = new StorageView(storagePath, storageMinLimit, storageMinData, dock);
    storage->setMinimumWidth(250);
    //storage->setMaximumWidth(250);

    dock->setWidget(storage);
 //   dock->resize(250,400);

    addDockWidget(Qt::BottomDockWidgetArea, dock);
    ui->menuTools->addAction(dock->toggleViewAction());

    //resize Qt buck
    dock->setFloating(true);
    dock->hide();
    dock->setFloating(false);

    if(showWindow)
        dock->show();
}
