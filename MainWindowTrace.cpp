#include <QDockWidget>
#include <QTextBrowser>
#include "MainWindow.h"
#include "ui_mainwindow.h"

void MainWindow::createTraceWindows(bool showWindow)
{
    QDockWidget *dock = new QDockWidget(tr("Trace"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);

    trace = new QTextBrowser(dock);

    dock->setWidget(trace);
    dock->resize(600,300);

    addDockWidget(Qt::BottomDockWidgetArea, dock);
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

void MainWindow::addTrace(QString text, QColor color)
{
    if(trace!=nullptr)
        trace->append(text);
}

void MainWindow::clearTrace(QString text)
{
    if(trace!=nullptr)
    {
        trace->clear();
        trace->append(text);
    }
}
