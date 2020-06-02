#include <QDockWidget>
#include <QTextBrowser>
#include <QDateTime>
#include "MainWindow.h"
#include "ui_MainWindow.h"

void MainWindow::createTraceWindow(bool showWindow, int traceSize)
{
    QDockWidget *dock = new QDockWidget(tr("Trace"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);

    trace = new QTextBrowser(dock);
    trace->document()->setMaximumBlockCount(traceSize);
    //int rows = trace->document()->blockCount();
    addTrace("Application Start");

    dock->setWidget(trace);
//    dock->resize(600,300);

    addDockWidget(Qt::BottomDockWidgetArea, dock);
    ui->menuTools->addAction(dock->toggleViewAction());

    //resize Qt buck
    dock->setFloating(true);
    dock->hide();
    dock->setFloating(false);

    if(showWindow)
        dock->show();
}

void MainWindow::closeTrace()
{
    addTrace("Application Closed");
}

void MainWindow::addTrace(QString text, QColor color)
{
    if(trace!=nullptr)
    {
        QDateTime dt = QDateTime::currentDateTime();

        //trace->append(dt.toString("[yyyy-MM-dd hh:mm:ss.zzz] ") + text);
        text.remove("\n");
        trace->append(dt.toString("[hh:mm:ss.zzz] ") + text);
        ui->statusBar->showMessage(dt.toString("[hh:mm:ss.zzz] ") + text);

        if(traceErrorsToFile)
        {
            //log to file: error, exception
            if(text.contains("error", Qt::CaseInsensitive)
                    || text.contains("exception", Qt::CaseInsensitive)
                    || text.contains("application", Qt::CaseInsensitive))
            {
                QString filePathLog(QCoreApplication::applicationDirPath() + "/" + "trace.txt");
                QFile log(filePathLog);
                if(log.open(QIODevice::WriteOnly | QIODevice::Append))
                {
                    QTextStream txt(&log);
                    txt << dt.toString("[yyyy-MM-dd hh:mm:ss] ");
                    txt << text;
                    if(!text.contains("\n"))
                        txt << "\n";

                    log.close();
                }
            }
        }
    }
}

void MainWindow::clearTrace(QString text)
{
    if(trace!=nullptr)
    {
        trace->clear();
        trace->append(text);
    }
}
