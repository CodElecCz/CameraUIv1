#include <QDockWidget>
#include <QTextBrowser>
#include <QFileInfo>
#include <QMessageBox>
#include <pylon/DeviceInfo.h>
#include "MainWindow.h"
#include "CameraParam.h"
#include "ui_MainWindow.h"

using namespace GenApi;
using namespace GenICam;

void MainWindow::createParamWindow(bool showWindow)
{
    QDockWidget *dock = new QDockWidget(tr("Parameters"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
    leftDockTabbing.insert(dock->windowTitle(), dock);

    param = new CameraParam(dock);

    dock->setWidget(param);
//    dock->resize(250,400);

    addDockWidget(Qt::LeftDockWidgetArea, dock);
    ui->menuTools->addAction(dock->toggleViewAction());

    //resize Qt buck
    dock->setFloating(true);
    dock->hide();
    dock->setFloating(false);

    if(showWindow)
        dock->show();

    connect(param, SIGNAL(setFloatValue(QString, QString, int)), this, SLOT(setParamFloatValue(QString, QString, int)));
    connect(param, SIGNAL(setIntergerValue(QString, QString, int)), this, SLOT(setParamIntergerValue(QString, QString, int)));
    connect(param, SIGNAL(setStringValue(QString, QString, QString)), this, SLOT(setParamStringValue(QString, QString, QString)));
    connect(param, SIGNAL(saveParameters(QString)), this, SLOT(saveParameters(QString)));
    connect(param, SIGNAL(loadParameters(QString)), this, SLOT(loadParameters(QString)));
}

void MainWindow::getAllEnumValues(QStringList &Featurelist, CInstantCamera *camera, GenICam::gcstring EnumFeatureName)
{
    GenApi::INodeMap& nodemap = camera->GetNodeMap();
    CEnumerationPtr EnumFeature(nodemap.GetNode(EnumFeatureName));
    GenApi::NodeList_t my_featurelist;
    EnumFeature->GetEntries(my_featurelist);

    for (GenApi::NodeList_t::iterator it = my_featurelist.begin(); it != my_featurelist.end(); ++it)
    {
        GenApi::CEnumEntryPtr pEnumEntry(*it);
        if (GenApi::IsAvailable(*it))
        {
            Featurelist.append(pEnumEntry->GetSymbolic().c_str());
        }
    }
}

void MainWindow::initializeParam(QString cameraName)
{
    param->setCameraName(cameraName);

    if(!cameras.contains(cameraName))
        return;

    SCamera* cam = cameras.value(cameraName);

    GenApi::INodeMap& nodemap = cam->camera->GetNodeMap();
    VersionInfo deviceSFNCVersion = cam->camera->GetSfncVersion();
    param->setDeviceInfo(deviceSFNCVersion);

    CIntegerPtr sensorWidth (nodemap.GetNode("SensorWidth"));
    CIntegerPtr sensorHeight (nodemap.GetNode("SensorHeight"));
    param->setResolution(sensorWidth->GetValue(), sensorHeight->GetValue());

    CIntegerPtr Width (nodemap.GetNode("Width"));
    param->setWidth(Width->GetValue(), Width->GetMin(), Width->GetMax(), Width->GetInc());

    CIntegerPtr Height (nodemap.GetNode("Height"));
    param->setHeight(Height->GetValue(), Height->GetMin(), Height->GetMax(), Height->GetInc());

    CIntegerPtr OffsetX (nodemap.GetNode("OffsetX"));
    param->setOffsetX(OffsetX->GetValue(), OffsetX->GetMin(), OffsetX->GetMax(), OffsetX->GetInc());

    CIntegerPtr OffsetY (nodemap.GetNode("OffsetY"));
    param->setOffsetY(OffsetY->GetValue(), OffsetY->GetMin(), OffsetY->GetMax(), OffsetY->GetInc());

    QStringList enumFeatureList;
    getAllEnumValues(enumFeatureList, cam->camera, "PixelFormat");
    CEnumerationPtr PixelFormat(nodemap.GetNode("PixelFormat"));
    QString sPixelFormat = PixelFormat->ToString().c_str();
    param->setPixelFormat(sPixelFormat, enumFeatureList);
    enumFeatureList.clear();

    if(deviceSFNCVersion < Sfnc_2_0_0 )  // ace GigE
    {
        CIntegerPtr ExposureTimeRaw (nodemap.GetNode("ExposureTimeRaw"));
        int exposure = ExposureTimeRaw->GetMax();
        if(exposure > 1000000) exposure = 1000000;
        param->setExposure(ExposureTimeRaw->GetValue(), ExposureTimeRaw->GetMin(), exposure, ExposureTimeRaw->GetInc());

        // Test Iamge Selector
        getAllEnumValues(enumFeatureList, cam->camera, "TestImageSelector");
        CEnumerationPtr TestImageSelector(nodemap.GetNode("TestImageSelector"));
        QString sTestImageSelector = TestImageSelector->ToString().c_str();
        param->setTestImageSelector(sTestImageSelector, enumFeatureList);
        enumFeatureList.clear();
    }
    else if (deviceSFNCVersion == Sfnc_2_1_0 )  // ace USB
    {   // Basler USB Cameras
        CFloatPtr ExposureTime (nodemap.GetNode("ExposureTime"));
        param->setExposure(ExposureTime->GetValue(), ExposureTime->GetMin(), ExposureTime->GetMax(), ExposureTime->GetInc());

        // Test Iamge Selector
        getAllEnumValues(enumFeatureList, cam->camera, "TestImageSelector");
        CEnumerationPtr TestImageSelector(nodemap.GetNode("TestImageSelector"));
        QString sTestImageSelector = TestImageSelector->ToString().c_str();
        param->setTestImageSelector(sTestImageSelector, enumFeatureList);
        enumFeatureList.clear();
    }
    else if (deviceSFNCVersion >= Sfnc_2_2_0 )  // Dart Cameras
    {   // Basler USB Cameras
        CFloatPtr ExposureTime (nodemap.GetNode("ExposureTime"));
        param->setExposure(ExposureTime->GetValue(), ExposureTime->GetMin(), ExposureTime->GetMax(), ExposureTime->GetInc());

        // Test Iamge Selector
        getAllEnumValues(enumFeatureList, cam->camera, "TestPattern");
        CEnumerationPtr TestImageSelector(nodemap.GetNode("TestPattern"));
        QString sTestImageSelector = TestImageSelector->ToString().c_str();
        param->setTestImageSelector(sTestImageSelector, enumFeatureList);
        enumFeatureList.clear();
    }

    param->setEnableCotrols(true);
}

void MainWindow::setParamIntergerValue(QString cameraName, QString nodeName, int value)
{
    try
    {
        if(cameras.contains(cameraName))
        {
            SCamera* cam = cameras.value(cameraName);

            GenApi::INodeMap *nodemap = &cam->camera->GetNodeMap();
            CIntegerPtr Parameter(nodemap->GetNode(nodeName.toStdString().c_str()));

            int set = value - (value % Parameter->GetInc());
            if( set >= Parameter->GetMin() && set <= Parameter->GetMax() )
            {
                Parameter->SetValue(set);
            }
        }
    }
    catch (GenICam::GenericException &e)
    {
        addTrace(QString("An exception occurred: ") + QString(e.GetDescription()), QColor("red"));
    }
}

void  MainWindow::setParamFloatValue(QString cameraName, QString nodeName, int value)
{
   try
   {
        if(cameras.contains(cameraName))
        {
            SCamera* cam = cameras.value(cameraName);

            GenApi::INodeMap *nodemap = &cam->camera->GetNodeMap();
            CFloatPtr Parameter(nodemap->GetNode(nodeName.toStdString().c_str()));

            if( value >= Parameter->GetMin() && value <= Parameter->GetMax() )
            {
                Parameter->SetValue((double)value);
            }
        }
    }
    catch (GenICam::GenericException &e)
    {
        addTrace(QString("An exception occurred: ") + QString(e.GetDescription()), QColor("red"));
    }
}

void MainWindow::setParamStringValue(QString cameraName, QString nodeName, QString value)
{
    try
    {
        if(cameras.contains(cameraName))
        {
            SCamera* cam = cameras.value(cameraName);

            GenApi::INodeMap *nodemap = &cam->camera->GetNodeMap();
            CEnumerationPtr Parameter(nodemap->GetNode(nodeName.toStdString().c_str()));

            if ( IsWritable( Parameter))
            {
                Parameter->FromString(value.toStdString().c_str());
            }
        }
    }
    catch (GenICam::GenericException &e)
    {
        addTrace(QString("An exception occurred: ") + QString(e.GetDescription()), QColor("red"));
    }
}

void MainWindow::saveParameters(QString cameraName)
{
    try
    {
        if(cameras.contains(cameraName))
        {
            SCamera* cam = cameras.value(cameraName);
            QString filePathParam(QCoreApplication::applicationDirPath() + "/" + cameraName + ".fps");

            CFeaturePersistence::Save(filePathParam.toStdString().c_str(), &cam->camera->GetNodeMap());

            QMessageBox::information(this, QString("Parameters"), QString("Camera parameters saved for camera: " + cameraName));
        }
    }
    catch (GenICam::GenericException &e)
    {
        addTrace(QString("An exception occurred: ") + QString(e.GetDescription()), QColor("red"));
    }
}

bool MainWindow::loadParameters(QString cameraName)
{
    bool status = false;

    try
    {
        if(cameras.contains(cameraName))
        {
            SCamera* cam = cameras.value(cameraName);
            QString filePathParam(QCoreApplication::applicationDirPath() + "/" + cameraName + ".fps");

            QFile file(filePathParam);
            if(file.exists())
            {
                CFeaturePersistence::Load(filePathParam.toStdString().c_str(), &cam->camera->GetNodeMap(), true);
                status = true;

                initializeParam(cameraName);
            }
        }
    }
    catch (GenICam::GenericException &e)
    {
        addTrace(QString("An exception occurred: ") + QString(e.GetDescription()), QColor("red"));
    }

    return status;
}

