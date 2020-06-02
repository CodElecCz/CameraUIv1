#include <QDockWidget>
#include <QTextBrowser>
#include <QSqlError>
#include <QDir>
#include <QDateTime>
#include "MainWindow.h"
#include "ui_MainWindow.h"

void MainWindow::createReportWindow(bool showWindow, bool errorQueryToFile)
{
    QDockWidget *dock = new QDockWidget(tr("Report"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);

    report = new ReportView(errorQueryToFile, dock);

    dock->setWidget(report);
 //   dock->resize(250,400);

    addDockWidget(Qt::RightDockWidgetArea, dock);
    ui->menuTools->addAction(dock->toggleViewAction());

    //resize Qt buck
    dock->setFloating(true);
    dock->hide();
    dock->setFloating(false);

    if(showWindow)
        dock->show();

    connect(report, SIGNAL(addTrace(QString,QColor)), this, SLOT(addTrace(QString,QColor)));
    connect(report, SIGNAL(readImageFile(QString,QString,QString)), this, SLOT(readImageFile(QString,QString,QString)));
}

bool MainWindow::initializeDatabase(QString dbsHostName, QString dbsDatabase, QString dbsUser, QString dbsPassword)
{
    bool status = false;

    db = QSqlDatabase::addDatabase("QODBC");
    db.setHostName(dbsHostName);
    db.setDatabaseName(dbsDatabase);
    db.setUserName(dbsUser);
    db.setPassword(dbsPassword);

    status = db.open();
    if(!status)
    {
        QString msg = "Database initialization failed! Error: " + db.lastError().text() +
                "; " + dbsHostName +
                "/" + dbsDatabase +
                "/" + dbsUser;
        emit addTrace(msg);

        //QMessageBox::warning(this, "Warning", msg);
        //qDebug() << msg;
    }

    report->setDbs(&db);

    return status;
}

void MainWindow::closeDatabase()
{
    bool status = db.isOpen();
    if(status)
    {
        db.close();
        qDebug() << "Database Close()";
    }
}

void MainWindow::readImageFile(QString camera, QString imageName, QString error)
{
    if(imageName.length()>0)
    {
        QString sdate = imageName.split("T").at(0);
        QStringList list = sdate.split("-");

        //  C:\Catalog\2018\12\08\2018-12-08T15.20.37_323456789_1.jpg
        QString filePath = catalogPath + "/" + list.at(0) + "/" + list.at(1) + "/" + list.at(2) + "/" + imageName;

        QFile imageFile(filePath);
        if(!imageFile.exists())
        {
            emit addTrace("Image file not exist: " + filePath);
        }
        else
        {
            QImage image(filePath);
            view->updateImage(camera, image);
            view->setPath(camera, filePath);
        }
    }
    else
    {
        view->setError(camera, error);
    }
}

void MainWindow::recordImageFiles(QString label)
{
    QStringList camList = view->getViewNames();
    QDateTime dt = QDateTime::currentDateTime();
    QString sdtPath = dt.toString("yyyy/MM/dd/");
    QString sdtFile = dt.toString("yyyy-MM-ddThh.mm.ss");
    bool res;

    QString fileDir = catalogPath + "/" + sdtPath;
    QDir dir(fileDir);
    if(!dir.exists())
    {
        dir.mkpath(fileDir);
        addTrace("Directory created, " + fileDir);
    }

    int i = 1;
    foreach(QString cam, camList)
    {
        QString imageExt = QString::number(i) + "." + imageFormat.toLower();
        QString filePath =  fileDir + sdtFile + "_" + label + "_" + imageExt;

        if(!view->isError(cam))
        {
            bool res = view->saveImage(cam, filePath, imageFormat, imageQuality);
            if(!res)
            {
                addTrace("Image not saved, " + filePath);
            }
            else
            {
                //QFile file(filePath);
                //int fileSize = file.size() / 1024;
                //addTrace("Image saved, " + filePath + ", Size: " + QString::number(fileSize) + " [kB]");

                res = report->storeResult(sdtFile, cam, label, imageExt);
            }
        }
        else
        {
            res = report->storeResult(sdtFile, cam, label, QString(""), view->getError(cam));
        }

        i++;
    }
}

void MainWindow::saveImageFiles(QString targetDir)
{
    QStringList camList = view->getViewNames();
    QDir dir(targetDir);

    foreach(QString cam, camList)
    {
        if(!view->isError(cam))
        {
            QString sourcePath = view->getPath(cam);

            QFile file(sourcePath);
            if(file.exists())
            {
                QFileInfo fileInfo(file.fileName());
                QString filename(fileInfo.fileName());
                QString targetFile = targetDir + "/" + filename;
                bool res = file.copy(targetFile);
            }
        }
    }
}
