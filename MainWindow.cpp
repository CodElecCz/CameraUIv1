#include <QDebug>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QProcess>
#include <QSettings>
#include <QFileInfo>
#include <QFileDialog>
#include <QInputDialog>
#include "LightWidget.h"
#include "MainWindow.h"
#include "LoadView.h"
#include "ReportView.h"
#include "ui_MainWindow.h"
#include "ui_AboutDialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    view(new CameraView(this)),
    dio(nullptr),
    devices(nullptr),
    trace(nullptr),
    param(nullptr),
    storage(nullptr),
    barcode(nullptr),

    allowShutDown(false),
    imageQuality(90),
    barcodeLenght(0),
    storageMinLimit(10),
    storageMinData(5),
    delayAfterLight(0),
    traceErrorsToFile(false),

    toolbarActionOpen(nullptr),
    toolbarActionClose(nullptr),
    toolbarActionSingleGrab(nullptr),
    toolbarActionMultipleGrab(nullptr),
    toolbarActionopenContinuesGrab(nullptr),
    toolbarActionPlaybackStop(nullptr),
    toolbarActionRecordStart(nullptr)
{
    ui->setupUi(this);

    readAppSettings();

    initializeApp();
}

MainWindow::~MainWindow()
{   
    param = nullptr;
    devices = nullptr;
    trace = nullptr;

    delete ui;
}

void MainWindow::readAppSettings()
{
    QSettings settings("QT", "CameraUI");

    settings.beginGroup("MainWindow");
    if(settings.value("isFullScreen").toBool())
    {
        ;//showFullScreen();
    }
    else
    {
        resize(settings.value("size", QSize(800, 600)).toSize());
        move(settings.value("pos", QPoint(100, 100)).toPoint());
    }
    addToolBar((Qt::ToolBarArea)settings.value("toolBarArea").toUInt(), ui->mainToolBar);

    settings.endGroup();
}

void MainWindow::writeAppSettings()
{
    QSettings settings("QT", "CameraUI");

    settings.beginGroup("MainWindow");
    settings.setValue("isFullScreen", MainWindow::isFullScreen());
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.setValue("toolBarArea", toolBarArea(ui->mainToolBar));

    settings.endGroup();
}

void MainWindow::initializeApp()
{
    LoadView *info = new LoadView;
    info->show();

    //Layout
    info->setText("Loading: Settings");

    //config.ini
    QString configFilePath = QFileInfo(QCoreApplication::applicationFilePath()).filePath();
    configFilePath.replace(".exe", ".ini");
    QFile configFile(configFilePath);
    if(!configFile.exists())
    {
        QString msg = "Config file not found: " + configFilePath;
        //QMessageBox::question(this, "Warning", msg, QMessageBox::Ok);
        qDebug()<<msg;
    }
    QSettings *config = new QSettings(configFilePath, QSettings::IniFormat);
    config->setIniCodec("UTF-8");

    //Menubar
    bool menuBarHide = config->value(QString("Aplication/MenuBarHide")).toBool();
    if(menuBarHide)
    {
        menuBar()->hide();
    }
    else
    {
        //About dialog
        connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(aboutDialog()));
    }

    //Toolbar
    bool toolBarHide = config->value(QString("Aplication/ToolBarHide")).toBool();
    if(toolBarHide)
    {
        ui->mainToolBar->hide();
    }
    else
    {
        initializeToolbar();
    }

    //Statusbar
    bool statusBarHide = config->value(QString("Aplication/StatusBarHide")).toBool();
    if(statusBarHide)
    {
        ui->statusBar->hide();
    }
    LightWidget *statusDbs = new LightWidget(Qt::white, "DBS: ");
    LightWidget *statusDio = new LightWidget(Qt::white, "DIO: ");
    LightWidget *statusClv = new LightWidget(Qt::white, "CLV: ");
    ui->statusBar->addPermanentWidget(statusDbs);
    ui->statusBar->addPermanentWidget(statusDio);
    ui->statusBar->addPermanentWidget(statusClv);
    //ui->statusBar->showMessage("test");

    //widgets
    QWidget *centralWidget = new QWidget;
    QVBoxLayout *controlsLayout = new QVBoxLayout;
    controlsLayout->addWidget(view, 1);
    controlsLayout->setSpacing(0);
    controlsLayout->setMargin(0);

    centralWidget->setLayout(controlsLayout);
    setCentralWidget(centralWidget);

    //DOCK1
    bool traceWindowHide = config->value(QString("Aplication/TraceWindowHide")).toBool();
    int traceWindowHistory = config->value(QString("Aplication/TraceWindowHistory")).toInt();
    traceErrorsToFile = config->value(QString("Aplication/TraceErrorsToFile")).toBool();
    if(traceWindowHistory<=0) traceWindowHistory = 100;
    createTraceWindow(!traceWindowHide, traceWindowHistory);

    //DOCK2
    bool paramWindowHide = config->value(QString("Aplication/ParametersWindowHide")).toBool();
    createParamWindow(!paramWindowHide);

    //DOCK3
    bool deviceWindowHide = config->value(QString("Aplication/DeviceWindowHide")).toBool();
    bool openAllCameras = config->value(QString("Aplication/OpenAllCameras")).toBool();
    createDevicesWindow(!deviceWindowHide, openAllCameras);

    //DOCK4
    bool reportWindowHide = config->value(QString("Aplication/ReportWindowHide")).toBool();
    bool errorQueryToFile = config->value(QString("Aplication/ErrorQueryToFile")).toBool();
    createReportWindow(!reportWindowHide, errorQueryToFile);

    //DOCK5
    bool storageWindowHide = config->value(QString("Aplication/StorageWindowHide")).toBool();
    storageMinLimit = config->value(QString("Storage/MinLimitPercent")).toInt();
    storageMinData = config->value(QString("Storage/MinStoragePercent")).toInt();
    QString storagePath = config->value(QString("Catalog/Path")).toString();
    createStorageWindow(!storageWindowHide, storagePath);

    allowShutDown = config->value(QString("Aplication/AllowShutDown")).toBool();
    delayAfterLight = config->value(QString("Aplication/DelayAfterLight")).toInt();

    //image catalog
    catalogPath = config->value(QString("Catalog/Path")).toString();
    imageFormat = config->value(QString("Catalog/Format")).toString();
    imageQuality = config->value(QString("Catalog/Quality")).toInt();

    //DIO
    info->setText("Loading: DIO");

    bool fvedioUse = config->value(QString("FVEDIO/Use")).toBool();
    if(!fvedioUse)
    {
        statusDio->setMsg("disabled");
    }
    else
    {
        //DOCK6
        bool dioWindowHide = config->value(QString("FVEDIO/DioWindowHide")).toBool();
        createDioWindow(!dioWindowHide);

        QString port = config->value(QString("FVEDIO/Port")).toString();
        bool status = dio->Initialize(port);
        if(status)
        {
            statusDio->setMsg("connected");
            statusDio->setColor(QColor("light green"));

            dio->SetDO(EDIO_3Lights_Green, 1);
            dio->SetDO(EDIO_3Lights_Red, 0);
        }
        else
        {
            statusDio->setMsg("error");
            statusDio->setColor(QColor("red"));
        }
    }

    //CLV
    info->setText("Loading: Barcode SICK");

    bool clvUse = config->value(QString("CLV/Use")).toBool();
    if(!clvUse)
    {
        statusClv->setMsg("disabled");
    }
    else
    {
        //DOCK7
        bool barcodeWindowHide = config->value(QString("CLV/BarcodeWindowHide")).toBool();
        createBarcodeWindow(!barcodeWindowHide);

        QString port = config->value(QString("CLV/Port")).toString();
        int speed = config->value(QString("CLV/Speed")).toInt();
        int waitAfterReadDone = config->value(QString("CLV/WaitAfterReadDone")).toInt();
        int delayProcessImage = config->value(QString("CLV/DelayProcessImage")).toInt();
        barcodeLenght = config->value(QString("CLV/BarcodeLenght")).toInt();
        bool restartTimer = config->value(QString("CLV/RestartTimer")).toBool();
        bool status = barcode->Initialize(port, speed, waitAfterReadDone, delayProcessImage, restartTimer);
        if(status)
        {
            statusClv->setMsg("connected");
            statusClv->setColor(QColor("light green"));
        }
        else
        {
            statusClv->setMsg("error");
            statusClv->setColor(QColor("red"));
        }
    }

    //DBS
    info->setText("Loading: Database");

    bool dbsUse = config->value(QString("Database/Use")).toBool();
    if(!dbsUse)
    {
        statusDbs->setMsg("disabled");
    }
    else
    {
        QString dbsHostName = config->value(QString("Database/HostName")).toString();
        QString dbsDatabase = config->value(QString("Database/Database")).toString();
        QString dbsUser = config->value(QString("Database/User")).toString();
        QString dbsPassword = config->value(QString("Database/Password")).toString();

        bool status = initializeDatabase(dbsHostName, dbsDatabase, dbsUser, dbsPassword);
        if(status)
        {
            statusDbs->setMsg("connected");
            statusDbs->setColor(QColor("light green"));
        }
        else
        {
            statusDbs->setMsg("error");
            statusDbs->setColor(QColor("red"));
        }
    }

    //SNAP
    for(int i = 1; i <= 4 ; i++)
    {
        SProcess p;
        p.cameraName = config->value(QString("Snap%1/CameraName").arg(i)).toString();
        p.led0 = config->value(QString("Snap%1/Led0").arg(i)).toBool();
        p.led1 = config->value(QString("Snap%1/Led1").arg(i)).toBool();
        p.led2 = config->value(QString("Snap%1/Led2").arg(i)).toBool();
        p.led3 = config->value(QString("Snap%1/Led3").arg(i)).toBool();
        p.snapDelayMs = config->value(QString("Snap%1/SnapDelayMs").arg(i)).toInt();

        process.append(p);
    }

    info->close();
    delete info;

    bool fullScreen = config->value(QString("Aplication/FullScreen")).toBool();
    if(fullScreen)
        showFullScreen();
}

void MainWindow::initializeToolbar()
{
    toolbarActionOpen = new QAction(tr("Open All Cameras"),this);
    toolbarActionClose = new QAction(tr("Close All Cameras"),this);
    toolbarActionSingleGrab = new QAction(tr("Single Snap"),this);
    toolbarActionMultipleGrab = new QAction(tr("Multiple Snap"),this);
    toolbarActionopenContinuesGrab = new QAction(tr("Continuos Grab"),this);
    toolbarActionPlaybackStop = new QAction(tr("Stop Grab"),this);
    toolbarActionRecordStart = new QAction(tr("Record"),this);

    QIcon icon("://icons/connect.png");
    QSize size(30,30);
    ui->mainToolBar->setIconSize(size);
    ui->mainToolBar->setObjectName("Toolbar");
    this->addToolBar(Qt::TopToolBarArea, ui->mainToolBar);

    toolbarActionOpen->setIcon(icon);
    ui->mainToolBar->addAction(toolbarActionOpen);
    connect(toolbarActionOpen, SIGNAL(triggered()), this, SLOT(toolbar_DeviceOpen()));

    icon.addFile("://icons/disconnect.png");
    toolbarActionClose->setIcon(icon);
    ui->mainToolBar->addAction(toolbarActionClose);
    connect(toolbarActionClose, SIGNAL(triggered()), this, SLOT(toolbar_DeviceClose()));

    ui->mainToolBar->addSeparator();

    icon.addFile("://icons/snap.png");
    toolbarActionSingleGrab->setIcon(icon);
    ui->mainToolBar->addAction(toolbarActionSingleGrab);
    connect(toolbarActionSingleGrab, SIGNAL(triggered()), this, SLOT(toolbar_SingleGrab()));

    icon.addFile("://icons/snap_all.png");
    toolbarActionMultipleGrab->setIcon(icon);
    ui->mainToolBar->addAction(toolbarActionMultipleGrab);
    connect(toolbarActionMultipleGrab, SIGNAL(triggered()), this, SLOT(toolbar_MultipleGrab()));

    ui->mainToolBar->addSeparator();

    icon.addFile("://icons/grab.png");
    toolbarActionopenContinuesGrab->setIcon(icon);
    ui->mainToolBar->addAction(toolbarActionopenContinuesGrab);
    connect(toolbarActionopenContinuesGrab, SIGNAL(triggered()), this, SLOT(toolbar_ContinuesGrab()));

    QIcon iconPlyback("://icons/record-stop.png");
    toolbarActionPlaybackStop->setIcon(iconPlyback);
    ui->mainToolBar->addAction(toolbarActionPlaybackStop);
    connect(toolbarActionPlaybackStop, SIGNAL(triggered()), this, SLOT(toolbar_GrabStop()));

#if 0
    QIcon iconReordStart("://icons/record.png");
    toolbarActionRecordStart->setIcon(iconReordStart);
    ui->mainToolBar->addAction(toolbarActionRecordStart);
    connect(toolbarActionRecordStart, SIGNAL(triggered()), this, SLOT(toolbar_RecordStart()));
    toolbarActionRecordStart->setVisible(false);
#endif

    ui->mainToolBar->addSeparator();

    setButtonStatus(false);
}

void MainWindow::setButtonStatus (bool isOpen, bool canGrab, bool canStop)
{
    if(!ui->mainToolBar->isHidden())
    {
        toolbarActionOpen->setEnabled(!isOpen);
        toolbarActionClose->setEnabled(isOpen);
        toolbarActionSingleGrab->setEnabled(isOpen && canGrab);
        toolbarActionMultipleGrab->setEnabled(isOpen && canGrab);
        toolbarActionopenContinuesGrab->setEnabled(isOpen && canGrab);
        toolbarActionPlaybackStop->setEnabled(isOpen && canStop);
        toolbarActionRecordStart->setEnabled(isOpen && canGrab);
    }
}

void MainWindow::resetValues()
{
    //reset the value of editable controlls on GUI
    param->resetValues();
}

void MainWindow::aboutDialog()
{
   QDialog *info = new QDialog(this, nullptr);
   Ui_aboutDialog aboutUi;
   aboutUi.setupUi(info);
   info->show();
}

typedef enum _EButtonClose
{
    EButtonClose_CLOSE = 0,
    EButtonClose_SHUTDOWN,
    EButtonClose_CANCEL
} EButtonClose;

void MainWindow::closeEvent(QCloseEvent *event)
{
    int reply = closeDialog();
    if(reply==EButtonClose_CANCEL)
        event->ignore();
}

int MainWindow::closeDialog()
{
    int reply = EButtonClose_CANCEL;

    if(allowShutDown)
    {
        reply = QMessageBox::question(this, QString("Quit"), QString("Close application?"), QString("Close"), QString("Shutdown PC"), QString("Cancel"), 2);
    }
    else
    {
        QMessageBox::StandardButton sreply;
        sreply = QMessageBox::question(this, QString("Quit"), QString("Close application?"), QMessageBox::Yes|QMessageBox::No);
        if (sreply == QMessageBox::Yes)
        {
            reply = EButtonClose_CLOSE;
        }
        else
            reply = EButtonClose_CANCEL;
    }

    if(reply != EButtonClose_CANCEL)
    {
        writeAppSettings();

        this->close();

        //shutdown window
        LoadView *info = new LoadView;
        info->setTitle("Application close");
        info->show();

        //barcode
        info->setText("Barcode: Close");
        closeBarcode();

        //close database
        info->setText("Database: Close");
        closeDatabase();

        //close dio
        info->setText("DIO: Close");
        closeDio();
        if(dio!=nullptr)
            dio->SetDO(EDIO_3Lights_Red, 1);

        //close all cameras
        info->setText("Camera: Close");
        cameraCloseAll();

        closeTrace();

        //shutdown pc
        if(reply == EButtonClose_SHUTDOWN)
        {
            info->setText("PC: Close");

            qDebug() << "shutdown -s -f -t 10";
            QProcess::startDetached("shutdown -s -f -t 10");
        }

        info->setText("Application: Quit");
        Sleep(1000);
        QApplication::quit();
    }

    return reply;
}

void MainWindow::on_actionSaveImages_triggered(bool)
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setDirectory(catalogPath);

    QStringList targetDir;
    if (dialog.exec())
    {
        targetDir = dialog.selectedFiles();
        if(targetDir.size()>0)
            saveImageFiles(targetDir.at(0));
    }
}

void MainWindow::on_actionRecordImages_triggered(bool)
{
    QString barcode = QInputDialog::getText(this, "Insert Barcode", "Barcode");
    if(barcode.length()>0)
        recordImageFiles(barcode);
}

void MainWindow::on_actionCloseImages_triggered(bool)
{
    view->removeView();
}

void MainWindow::on_actionOpenAll_triggered(bool)
{
    QList<QDockWidget *> dockWidgets = findChildren<QDockWidget *>();

    foreach(QDockWidget* dock, dockWidgets)
    {
        if(!dock->isVisible())
            dock->show();
    }
}

void MainWindow::on_actionCloseAll_triggered(bool)
{
    QList<QDockWidget *> dockWidgets = findChildren<QDockWidget *>();

    foreach(QDockWidget* dock, dockWidgets)
    {
        if(dock->isVisible())
            dock->hide();
    }
}

void MainWindow::processImages(QString barcode)
{    
    bool isSnap, isOpen;
    QString camName;

    foreach(auto p, process)
    {
        if(cameras.contains(p.cameraName))
        {
            camName = p.cameraName;

            if(dio && p.led0)
                dio->LED0(100);
            if(dio && p.led1)
                dio->LED1(100);
            if(dio && p.led2)
                dio->LED2(100);
            if(dio && p.led3)
                dio->LED3(100);

            if(delayAfterLight>0)
                Sleep(delayAfterLight);

            isSnap = cameraSnap(camName);
            if(!isSnap)
            {
                cameraClose(camName);
                isOpen = cameraOpen(camName);
                if(isOpen)
                {
                  isSnap = cameraSnap(camName);
                }
            }

            if(dio && p.led0)
                dio->LED0(0);
            if(dio && p.led1)
                dio->LED1(0);
            if(dio && p.led2)
                dio->LED2(0);
            if(dio && p.led3)
                dio->LED3(0);
        }
    }

    //save images and store to database
    if(barcodeLenght!=0 && barcode.length()>barcodeLenght)
        barcode.resize(barcodeLenght);

    recordImageFiles(barcode);
}
