#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QTextBrowser>
#include <QThread>
#include <QGraphicsWidget>
#include <QMap>
#include <QSqlDatabase>
#include <pylon/PylonIncludes.h>
#include "CameraParam.h"
#include "CameraView.h"
#include "StorageView.h"
#include "DioView.h"
#include "BarcodeView.h"
#include "ReportView.h"
#include "CameraGrabThread.h"

namespace Ui {
class MainWindow;
}

typedef struct _SCamera
{
    QString               cameraName;
    Pylon::CInstantCamera *camera;
    CameraGrabThread      *grab;
} SCamera;

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

    //aplication settings
    void writeAppSettings();
    void readAppSettings();

    //camera functions
    int cameraAutoenumerate();
    bool cameraOpen(QString cameraName);
    bool cameraOpenAll();
    void cameraCloseAll();
    void cameraClose(QString cameraName);
    bool cameraSnap(QString cameraName);
    void cameraStartGrab(QString cameraName);
    void cameraStopGrab(QString cameraName);
    void getAllEnumValues(QStringList &Featurelist, Pylon::CInstantCamera *camera, GenICam::gcstring EnumFeatureName);

    static QImage* toQImage(CPylonImage &pylonImage, QImage::Format format = QImage::Format_RGB888);

    //database
    bool initializeDatabase(QString dbsHostName, QString dbsDatabase, QString dbsUser, QString dbsPassword);
    void closeDatabase();

    //dock windows
    void createTraceWindow(bool showWindow = false, int traceSize = 100);
    void createParamWindow(bool showWindow = false);
    void createDevicesWindow(bool showWindow = true, bool openAllCameras = false);
    void createDioWindow(bool showWindow = false);
    void createReportWindow(bool showWindow = false, bool errorQueryToFile = true);
    void createStorageWindow(bool showWindow = false, QString storagePath = "c:/");
    void createBarcodeWindow(bool showWindow);

    void closeBarcode();
    void closeTrace();

    void closeDio();

    void closeEvent(QCloseEvent *event);

public slots:
    //trace window
    void addTrace(QString text, QColor color = QColor("black"));
    void clearTrace(QString text = "");

    //camera parameters
    void initializeParam(QString cameraName);
    void setParamFloatValue(QString cameraName, QString nodeName,int value);
    void setParamIntergerValue(QString cameraName, QString nodeName,int value);
    void setParamStringValue(QString cameraName, QString nodeName, QString value);
    void saveParameters(QString cameraName);
    bool loadParameters(QString cameraName);

    //store images
    void processImages(QString barcode);

private slots:
    void toolbar_DeviceOpen();
    void toolbar_DeviceClose();
    void toolbar_SingleGrab();
    void toolbar_MultipleGrab();
    void toolbar_ContinuesGrab();
    void toolbar_GrabStop();
    void devices_itemClicked(QListWidgetItem*);

    //on_{ObjectName}_{SignalName}
    void on_actionSaveImages_triggered(bool);
    void on_actionRecordImages_triggered(bool);
    void on_actionCloseImages_triggered(bool);
    void on_actionOpenAll_triggered(bool);
    void on_actionCloseAll_triggered(bool);

    //dialogs
    void aboutDialog();
    int closeDialog();

    void readImageFile(QString camera, QString imageName, QString error = "");
    void recordImageFiles(QString label);
    void saveImageFiles(QString targetDir);

private:
    Ui::MainWindow  *ui;
    CameraView      *view;
    DioView         *dio;
    QListWidget     *devices;
    QTextBrowser    *trace;
    CameraParam     *param;
    ReportView      *report;
    StorageView     *storage;
    BarcodeView     *barcode;

    QMap<QString, SCamera*>     cameras;
    QMap<QString, QDockWidget*> leftDockTabbing;

    QSqlDatabase    db;

    //settings
    bool            allowShutDown;
    QString         catalogPath;
    QString         imageFormat;
    int             imageQuality;
    int             barcodeLenght;
    int             storageMinLimit;
    int             storageMinData;
    int             delayAfterLight;
    bool            traceErrorsToFile;

    //tollbar icon actions
    QAction *toolbarActionOpen;
    QAction *toolbarActionClose;
    QAction *toolbarActionSingleGrab;
    QAction *toolbarActionMultipleGrab;
    QAction *toolbarActionopenContinuesGrab ;
    QAction *toolbarActionPlaybackStop;
    QAction *toolbarActionRecordStart;
};

#endif // MAINWINDOW_H
