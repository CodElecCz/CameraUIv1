#include <QDockWidget>
#include <QTextBrowser>
#include "MainWindow.h"
#include "BarcodeView.h"
#include "ui_MainWindow.h"

void MainWindow::createBarcodeWindow(bool showWindow)
{
    QDockWidget *dock = new QDockWidget(tr("Barcode CLV"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);

    barcode = new BarcodeView(dock);

    dock->setWidget(barcode);
 //   dock->resize(250,400);

    addDockWidget(Qt::BottomDockWidgetArea, dock);
    ui->menuTools->addAction(dock->toggleViewAction());

    //resize Qt buck
    dock->setFloating(true);
    dock->hide();
    dock->setFloating(false);

    if(showWindow)
        dock->show();

    connect(barcode, SIGNAL(addTrace(QString,QColor)), this, SLOT(addTrace(QString,QColor)));
    connect(barcode, SIGNAL(processImages(QString)), this, SLOT(processImages(QString)));
}

void MainWindow::closeBarcode()
{
    if(barcode)
    {
        disconnect(barcode, SIGNAL(addTrace(QString,QColor)), this, SLOT(addTrace(QString,QColor)));
        disconnect(barcode, SIGNAL(processImages(QString)), this, SLOT(processImages(QString)));

        barcode->BarcodeClose();
    }
}
