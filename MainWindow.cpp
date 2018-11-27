#include <QDebug>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QProcess>
#include <QSettings>
#include <QFileInfo>
#include <QDebug>
#include "MainWindow.h"
#include "LoadView.h"
#include "ui_mainwindow.h"
#include "ui_aboutdialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    view(new CameraView(this)),
    dio(nullptr),
    devices(nullptr),
    trace(nullptr),
    param(nullptr),

    toolbarActionopen(nullptr),
    toolbarActionclose(nullptr),
    toolbarActionSingleGrab(nullptr),
    toolbarActionopenContinuesGrab(nullptr),
    toolbarActionPlaybackStop(nullptr),
    toolbarActionRecordStart(nullptr),

    allowShutDown(false)
{
    ui->setupUi(this);

    initializeApp();
}

MainWindow::~MainWindow()
{   
    param = nullptr;
    devices = nullptr;
    trace = nullptr;

    delete ui;
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

        //exit
        //connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(closeDialog()));
        //connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(closeDialog()));
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

    QWidget *centralWidget = new QWidget;
    QVBoxLayout *controlsLayout = new QVBoxLayout;
    controlsLayout->addWidget(view, 1);
    controlsLayout->setSpacing(0);
    controlsLayout->setMargin(0);

    centralWidget->setLayout(controlsLayout);
    setCentralWidget(centralWidget);

    bool traceWindowHide = config->value(QString("Aplication/TraceWindowHide")).toBool();
    createTraceWindows(!traceWindowHide);

    bool paramWindowHide = config->value(QString("Aplication/ParametersWindowHide")).toBool();
    createParamWindows(!paramWindowHide);

    bool deviceWindowHide = config->value(QString("Aplication/DeviceWindowHide")).toBool();
    bool openAllCameras = config->value(QString("Aplication/OpenAllCameras")).toBool();
    createDevicesWindows(!deviceWindowHide, openAllCameras);

    allowShutDown = config->value(QString("Aplication/AllowShutDown")).toBool();

    //DIO
    info->setText("Loading: DIO");

    bool fvedio = config->value(QString("FVEDIO/Use")).toBool();
    if(fvedio)
    {
        createDioWindows(false);

        QString port = config->value(QString("FVEDIO/Port")).toString();
        dio->Initialize(port);
    }

    info->close();
    delete info;

    bool fullScreen = config->value(QString("Aplication/FullScreen")).toBool();
    if(fullScreen)
        showFullScreen();
}

void MainWindow::initializeToolbar()
{
    toolbarActionopen = new QAction(tr("Open"),this);
    toolbarActionclose = new QAction(tr("Close"),this);
    toolbarActionSingleGrab = new QAction(tr("Single"),this);
    toolbarActionopenContinuesGrab = new QAction(tr("Continuos"),this);
    toolbarActionPlaybackStop = new QAction(tr("Stop"),this);
    toolbarActionRecordStart = new QAction(tr("Record"),this);

    QIcon icon("://icons/connect.png");
    QSize size(30,30);
    ui->mainToolBar->setIconSize(size);
    ui->mainToolBar->setObjectName("Toolbar");
    this->addToolBar(Qt::TopToolBarArea, ui->mainToolBar);

    toolbarActionopen->setIcon(icon);
    ui->mainToolBar->addAction(toolbarActionopen);
    connect(toolbarActionopen, SIGNAL(triggered()), this, SLOT(on_Toolbar_DeviceOpen()));

    icon.addFile("://icons/disconnect.png");
    toolbarActionclose->setIcon(icon);
    ui->mainToolBar->addAction(toolbarActionclose);
    connect(toolbarActionclose, SIGNAL(triggered()), this, SLOT(on_Toolbar_DeviceClose()));

    icon.addFile("://icons/snap.png");
    toolbarActionSingleGrab->setIcon(icon);
    ui->mainToolBar->addAction(toolbarActionSingleGrab);
    connect(toolbarActionSingleGrab, SIGNAL(triggered()), this, SLOT(on_Toolbar_SingleGrab()));

    icon.addFile("://icons/grab.png");
    toolbarActionopenContinuesGrab->setIcon(icon);
    ui->mainToolBar->addAction(toolbarActionopenContinuesGrab);
    connect(toolbarActionopenContinuesGrab, SIGNAL(triggered()), this, SLOT(on_Toolbar_ContinuesGrab()));

    QIcon iconPlyback("://icons/record-stop.png");
    toolbarActionPlaybackStop->setIcon(iconPlyback);
    ui->mainToolBar->addAction(toolbarActionPlaybackStop);
    connect(toolbarActionPlaybackStop, SIGNAL(triggered()), this, SLOT(on_Toolbar_GrabStop()));

#if 0
    QIcon iconReordStart("://icons/record.png");
    toolbarActionRecordStart->setIcon(iconReordStart);
    ui->mainToolBar->addAction(toolbarActionRecordStart);
    connect(toolbarActionRecordStart, SIGNAL(triggered()), this, SLOT(on_Toolbar_RecordStart()));
    toolbarActionRecordStart->setVisible(false);
#endif

    setButtonStatus(false);
}

void MainWindow::setButtonStatus (bool isOpen, bool canGrab, bool canStop)
{
    if(!ui->mainToolBar->isHidden())
    {
        toolbarActionopen->setEnabled(!isOpen);
        toolbarActionclose->setEnabled(isOpen);
        toolbarActionSingleGrab->setEnabled(isOpen && canGrab);
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
   QDialog *info = new QDialog(this, 0);
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
        if (reply == 0 || reply==1)
        {
            this->close();

            //shutdown window
            LoadView *info = new LoadView;
            info->setTitle("Application close");
            info->show();

            //close all cameras
            cameraCloseAll();

            //shutdown pc
            if(reply == 1)
            {
                qDebug() << "shutdown -s -f -t 10";
                QProcess::startDetached("shutdown -s -f -t 10");
            }

            QApplication::quit();
        }
    }
    else
    {
        QMessageBox::StandardButton sreply;
        sreply = QMessageBox::question(this, QString("Quit"), QString("Close application?"), QMessageBox::Yes|QMessageBox::No);
        if (sreply == QMessageBox::Yes)
        {
            reply = EButtonClose_CLOSE;

            this->close();

            //shutdown window
            LoadView *info = new LoadView;
            info->setTitle("Application close");
            info->show();

            //close all cameras
            cameraCloseAll();

            QApplication::quit();
        }
        else
            reply = EButtonClose_CANCEL;
    }

    return reply;
}
