#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QTextBrowser>
#include <QThread>
#include <QGraphicsWidget>
#include <QMap>
#include <pylon/PylonIncludes.h>
#include "CameraParam.h"
#include "CameraView.h"
#include "DioView.h"
#include "CameraGrabThread.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void DisplayImage();

private:
    void initializeApp();
    void initializeToolbar();
    void setButtonStatus (bool isOpen, bool canGrab = false, bool canStop = false);
    void resetValues();

    //camera functions
    void cameraAutoenumerate();
    bool cameraOpen(QString cameraName);
    bool cameraOpenAll();
    void cameraCloseAll();
    void cameraClose(QString cameraName);
    void cameraSnap(QString cameraName);
    void cameraStartGrab(QString cameraName);
    void cameraStopGrab(QString cameraName);
    void getAllEnumValues(QStringList &Featurelist, Pylon::CInstantCamera *camera, GenICam::gcstring EnumFeatureName);

    //dock windows
    void createTraceWindows(bool showWindow = false);
    void createParamWindows(bool showWindow = false);
    void createDevicesWindows(bool showWindow = true, bool openAllCameras = false);
    void createDioWindows(bool showWindow = false);

    void closeEvent(QCloseEvent *event);

public slots:
    //trace window
    void addTrace(QString text, QColor color = QColor("black"));
    void clearTrace(QString text = "");

    //camera parameters
    void initializeParam(QString cameraName, CameraParam *param);
    void setParamFloatValue(QString cameraName, QString nodeName,int value);
    void setParamIntergerValue(QString cameraName, QString nodeName,int value);
    void setParamStringValue(QString cameraName, QString nodeName, QString value);

private slots:
    void on_Toolbar_DeviceOpen();
    void on_Toolbar_DeviceClose();
    void on_Toolbar_SingleGrab();
    void on_Toolbar_ContinuesGrab();
    void on_Toolbar_GrabStop();

    //app info dialog
    void aboutDialog();

    int closeDialog();

private:
    Ui::MainWindow  *ui;
    CameraView      *view;
    DioView         *dio;
    QListWidget     *devices;
    QTextBrowser    *trace;
    CameraParam     *param;

    QMap<QString, SCamera*> cameras;

    //settings
    bool            allowShutDown;

    //tollbar icon actions
    QAction *toolbarActionopen ;
    QAction *toolbarActionclose ;
    QAction *toolbarActionSingleGrab;
    QAction *toolbarActionopenContinuesGrab ;
    QAction *toolbarActionPlaybackStop;
    QAction *toolbarActionRecordStart;
};

#endif // MAINWINDOW_H
