#include <QDockWidget>
#include <QMessageBox>
#include <QDebug>
#include <pylon/DeviceInfo.h>
#include <QDebug>
#include "MainWindow.h"
#include "ui_MainWindow.h"

/*
 * TODO:
 * cameraAutoenumerate() by timer
 *
 */

using namespace GenApi;
using namespace GenICam;

void MainWindow::createDevicesWindow(bool showWindow, bool openAllCameras)
{
    QDockWidget *dock = new QDockWidget(tr("Cameras"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
    leftDockTabbing.insert(dock->windowTitle(), dock);

    devices = new QListWidget(dock);
    devices->setMinimumWidth(200);
//    devices->setMaximumWidth(200);
//    devices->setGeometry(0,0,200,300);

    dock->setWidget(devices);
//    dock->setGeometry(0,0,200,300);
//    dock->resize(200,300);

    addDockWidget(Qt::LeftDockWidgetArea, dock);
    if(leftDockTabbing.size()>1)
        tabifyDockWidget(leftDockTabbing.values().at(0), leftDockTabbing.values().at(leftDockTabbing.size()-1));

    ui->menuTools->addAction(dock->toggleViewAction());

    //resize Qt buck
    dock->setFloating(true);

    dock->hide();
    dock->setFloating(false);

    int nbOfCam = cameraAutoenumerate();
    if(openAllCameras && nbOfCam>0)
    {
        cameraOpenAll();
    }

    if(showWindow)
    {
        dock->show();
    }

    connect(devices, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(devices_itemClicked(QListWidgetItem*)));
}

int MainWindow::cameraAutoenumerate()
{
    DeviceInfoList_t dList;
    CTlFactory::GetInstance().EnumerateDevices(dList, true);

    if ( dList.size() == 0 )
    {
        emit addTrace("No camera found", QColor("red"));
    }
    else
    {
        bool found = false;  // in case that auto scan is active, it will add item only if a new one is found.

        for( uint i = 0; i < dList.size(); i++ )
        {
            QListWidgetItem *item = new QListWidgetItem(QString(dList[i].GetFriendlyName()));

            QAbstractItemModel* model = devices->model() ;
            for(int j = 0; j < devices->count(); j++)
            {
                if(model->index(j, 0).data(Qt::DisplayRole).toString().contains(dList[i].GetFriendlyName().c_str()))
                {
                    found = true;
                    break;
                }
            }

            if(!found)
            {
                devices->addItem(item);

                addTrace(item->text());
            }
            found = false;
        }

        if(devices->count() > (int)dList.size())
        {
            devices->clear();
            for( uint i=0; i < dList.size(); i++ )
            {
                QListWidgetItem *item = new QListWidgetItem(QString(dList[i].GetFriendlyName()));
                devices->addItem(item);
            }
        }
    }

    return dList.size();
}

bool MainWindow::cameraOpen(QString cameraName)
{
    bool open = false;

    try
    {
        if(cameraName.length() > 0)
        {
            CDeviceInfo info;
            info.SetFriendlyName(cameraName.toStdString().c_str());

            SCamera* cam = new SCamera;
            cam->cameraName = cameraName;
            cam->camera = new Pylon::CInstantCamera();
            cameras.insert(cameraName, cam);

            cam->camera->Attach(CTlFactory::GetInstance().CreateFirstDevice(info));
            qDebug() << cameraName + QString(" Attach()");

            bool isOpen = cam->camera->IsOpen();
            if(!isOpen)
            {
                cam->camera->Open();
                qDebug() << cameraName + QString(" Open()");
                open = true;

                bool isParam = loadParameters(cameraName);
                if(!isParam)
                {
                    addTrace(QString(cameraName + ", Camera open"));
                }
                else
                {
                    addTrace(QString(cameraName + ", Camera open with parameters file"));
                }

            }
            else
            {
                addTrace(QString(cameraName + ", Camera already open!"), QColor("red"));
            }
        }
    }
    catch(Pylon::GenericException &e)
    {
        QString sexc = QString(cameraName + ", An exception occurred: ") + QString(e.GetDescription());
        qDebug() << sexc;
        addTrace(sexc, QColor("red"));
    }

    return open;
}

bool MainWindow::cameraOpenAll()
{
    bool open = true;

    if(devices->count()>0)
    {
        clearTrace();

        for(int i = 0; i<devices->count(); i++)
        {
            QListWidgetItem *item = devices->item(i);
            QString cameraName = item->text();

            open &= cameraOpen(cameraName);

            //parameters
            if(open)
            {
                if(i==0)
                {
                    initializeParam(cameraName);
                }

                view->addView(cameraName);
            }
        }

        if(open)
        {
            devices->setItemSelected(devices->item(0), true);
            setButtonStatus(true, true, false);
        }
        else
            open = false;
    }
    else
        open = false;

    return open;
}

void MainWindow::cameraClose(QString cameraName)
{
    try
    {
        if(cameraName.length() > 0)
        {
            SCamera* cam = cameras[cameraName];

            if(cam->camera->IsOpen())
            {
                cameraStopGrab(cam->cameraName);

                cam->camera->Close();
                qDebug() << cameraName + QString(" Close()");

                delete cam->camera;
                cam->camera = nullptr;

                addTrace(QString(cameraName + ", Camera close"));
            }
        }
    }
    catch(Pylon::GenericException &e)
    {
        QString sexc = QString(cameraName + ", An exception occurred: ") + QString(e.GetDescription());
        qDebug() << sexc;
        addTrace(sexc, QColor("red"));
    }
}

void MainWindow::cameraCloseAll()
{
    //disable parameters editing
    param->setEnableCotrols(false);

    foreach(SCamera* cam, cameras)
    {
        cameraClose(cam->cameraName);
    }

    view->removeView();

    cameras.clear();

    setButtonStatus(false);
    resetValues();
}

QImage* MainWindow::toQImage(CPylonImage &pylonImage, QImage::Format format)
{
    int width = static_cast<int>(pylonImage.GetWidth());
    int height = static_cast<int>(pylonImage.GetHeight());
    void *buffer = pylonImage.GetBuffer();
    int step = static_cast<int>(pylonImage.GetAllocatedBufferSize()) / height;
    QImage* img = new QImage(static_cast<uchar*>(buffer), width, height, step, format);
    return img;
}

bool MainWindow::cameraSnap(QString cameraName)
{
    bool status = false;
    clock_t time = clock();

    try
    {
        SCamera* cam = cameras[cameraName];

        INodeMap& Nodemap = cam->camera->GetNodeMap();
        CEnumerationPtr TriggerMode(Nodemap.GetNode("TriggerMode"));
        TriggerMode->FromString("Off");

        CEnumerationPtr PixelFormat(Nodemap.GetNode("PixelFormat"));
        QString sPixelFormat = PixelFormat->ToString().c_str();

        CGrabResultPtr ptrGrabResult;
        CImageFormatConverter fc;
        CPylonImage image;
        QImage *qtImage;

        cam->camera->MaxNumBuffer = 1;
        cam->camera->StartGrabbing(1);
        qDebug() << cameraName + QString(" StartGrabbing()");
        cam->camera->RetrieveResult(2000, ptrGrabResult, TimeoutHandling_Return);

        if (ptrGrabResult.IsValid() && ptrGrabResult->GrabSucceeded())
        {
            if(QString::compare(sPixelFormat, "Mono8")==0)
            {
                fc.OutputPixelFormat = PixelType_Mono8;
                fc.Convert(image, ptrGrabResult);

                qtImage = toQImage(image, QImage::Format_Grayscale8);
            }
            else
            {
                fc.OutputPixelFormat = PixelType_RGB8packed;
                fc.Convert(image, ptrGrabResult);
                qtImage = toQImage(image, QImage::Format_RGB888);
            }

            view->updateImage(cameraName, *qtImage);
            delete qtImage;

            status = true;

            int time_ms = (clock() - time);
            addTrace(QString(cameraName + ", Snap image %1x%2 [px], %3 [ms]").arg(QString::number(ptrGrabResult->GetWidth()), QString::number(ptrGrabResult->GetHeight()), QString::number(time_ms)));
        }
        else
        {
            view->setError(cameraName, QString(ptrGrabResult->GetErrorDescription()));

            addTrace(QString(cameraName + ", An exception occurred: ") + QString(ptrGrabResult->GetErrorDescription()), QColor("red"));
        }

        cam->camera->StopGrabbing();
        qDebug() << cameraName + QString(" StopGrabbing()");
    }
    catch (Pylon::GenericException &e)
    {
        QString sexc = QString(cameraName + ", An exception occurred: ") + QString(e.GetDescription());

        view->setError(cameraName, QString(e.GetDescription()));

        addTrace(sexc, QColor("red"));

        qDebug() << sexc;
    }

    return status;
}

void MainWindow::cameraStartGrab(QString cameraName)
{
    try
    {
         SCamera *cam = cameras[cameraName];

         INodeMap& Nodemap = cam->camera->GetNodeMap();
         CEnumerationPtr TriggerMode(Nodemap.GetNode("TriggerMode"));
         TriggerMode->FromString("Off");

         cam->camera->MaxNumBuffer = 20;
         cam->camera->StartGrabbing();
         qDebug() << cameraName + QString(" StartGrabbing()");

         cam->grab = new CameraGrabThread(cameraName, cam->camera);

         connect(cam->grab, SIGNAL(updateImage(QString, QImage*)), this->view, SLOT(updateImage(QString, QImage*)));
         connect(cam->grab, SIGNAL(addTrace(QString, QColor)), this, SLOT(addTrace(QString, QColor)));

         cam->grab->start();
     }
     catch (Pylon::GenericException &e)
     {
        QString sexc = QString(cameraName + ", An exception occurred: ") + QString(e.GetDescription());
        addTrace(sexc, QColor("red"));

        qDebug() << sexc;
     }
}

void MainWindow::cameraStopGrab(QString cameraName)
{
    try
    {
        SCamera* cam = cameras[cameraName];

        if(cam->camera->IsGrabbing())
        {
            cam->grab->requestInterruption();
            while(!cam->grab->isFinished())
            {
                Sleep(10);
                //TODO: timeout 2s
            }

            //proces pending signals
            QApplication::processEvents();

            disconnect(cam->grab, SIGNAL(updateImage(QString, QImage*)), this->view, SLOT(updateImage(QString, QImage*)));
            disconnect(cam->grab, SIGNAL(addTrace(QString, QColor)), this, SLOT(addTrace(QString, QColor)));

            delete cam->grab;
            cam->grab = nullptr;
        }
    }
    catch (Pylon::GenericException &e)
    {
       QString sexc = QString(cameraName + ", An exception occurred: ") + QString(e.GetDescription());
       addTrace(sexc, QColor("red"));

       qDebug() << sexc;
    }
}

void MainWindow::toolbar_DeviceOpen()
{
    cameraOpenAll();
}

void MainWindow::toolbar_DeviceClose()
{
    cameraCloseAll();
}

void MainWindow::toolbar_SingleGrab()
{
    QList<QListWidgetItem*> list = devices->selectedItems();
    if(list.count()>0)
    {
       QString cameraName = list.first()->text();

       setButtonStatus(true, false, true);

       cameraSnap(cameraName);
    }
    else
    {
         QMessageBox::information(this, "Message", "Please select a camera first", 0);
    }

    //finished
    setButtonStatus(true, true, false);
}

void MainWindow::toolbar_MultipleGrab()
{
    if(devices->count() >0)
    {
       setButtonStatus(true, false, true);

       foreach(SCamera* cam, cameras)
       {
           cameraSnap(cam->cameraName);
       }
    }
    else
    {
         QMessageBox::information(this, "Message", "No camera", 0);
    }

    //finished
    setButtonStatus(true, true, false);
}

void MainWindow::toolbar_ContinuesGrab()
{
    QList<QListWidgetItem*> list = devices->selectedItems();
    if(list.count()>0)
    {
       QString cameraName = list.first()->text();

       cameraStartGrab(cameraName);

       setButtonStatus(true, false, true);
    }
    else
    {
         QMessageBox::information(this, "Message", "Please select a camera first", 0);
         setButtonStatus(true, true, false);
    }
}

void MainWindow::toolbar_GrabStop()
{
    QList<QListWidgetItem*> list = devices->selectedItems();
    if(list.count()>0)
    {
        QString cameraName = list.first()->text();

        cameraStopGrab(cameraName);

        setButtonStatus(true, true, false);
    }
    else
    {
         QMessageBox::information(this, "Message", "Please select a camera first", 0);
         setButtonStatus(true, true, false);
    }
}

void MainWindow::devices_itemClicked(QListWidgetItem* item)
{
    QString cameraName = item->text();

    bool isParam = loadParameters(cameraName);
    if(!isParam)
        initializeParam(cameraName);
}
