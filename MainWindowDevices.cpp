#include <QDockWidget>
#include <QMessageBox>
#include <QDebug>
#include <pylon/DeviceInfo.h>
#include <QDebug>
#include "MainWindow.h"
#include "ui_mainwindow.h"
#include "ui_cameraparam.h"

/*
 * TODO:
 * multi cameras, devices clicked -> initializeParam()
 * cameraAutoenumerate() by timer
 *
 */

using namespace GenApi;
using namespace GenICam;

void MainWindow::createDevicesWindows(bool showWindow, bool openAllCameras)
{
    QDockWidget *dock = new QDockWidget(tr("Devices"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);

    devices = new QListWidget(dock);
    devices->setMinimumWidth(200);
    devices->setMaximumWidth(200);
//    devices->setGeometry(0,0,200,300);

    dock->setWidget(devices);
//    dock->setGeometry(0,0,200,300);
//    dock->resize(200,300);

    addDockWidget(Qt::LeftDockWidgetArea, dock);
    ui->menuTools->addAction(dock->toggleViewAction());

    //resize Qt buck
    dock->setFloating(true);

    dock->hide();
    dock->setFloating(false);

    cameraAutoenumerate();
    if(openAllCameras)
    {
        cameraOpenAll();
    }

    if(showWindow)
    {
        dock->show();
    }
}

void MainWindow::cameraAutoenumerate()
{
    DeviceInfoList_t dList;
    CTlFactory::GetInstance().EnumerateDevices(dList, true);

    if ( dList.size() == 0 )
    {
        addTrace("No camera found", QColor("red"));
    }
    else
    {
        bool found = false;  // in case that auto can is active, it will add item only if a new one is found.

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
            }
            else
            {
                addTrace(QString("Camera already open!"), QColor("red"));
            }
        }
    }
    catch(Pylon::GenericException &e)
    {
        QString sexc = QString("An exception occurred: ") + QString(e.GetDescription());
        qDebug() << sexc;
        addTrace(sexc, QColor("red"));
    }

    return open;
}

bool MainWindow::cameraOpenAll()
{
    bool open = true;

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
                initializeParam(cameraName, param);
            }

            view->addView(cameraName);

            addTrace(QString("Camera open, ") + cameraName);
        }
    }

    if(open && devices->count()>0)
    {
        devices->setItemSelected(devices->item(0), true);

        setButtonStatus(true, true, false);
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

                addTrace(QString("Camera close, ") + cam->cameraName);
            }
        }
    }
    catch(Pylon::GenericException &e)
    {
        QString sexc = QString("An exception occurred: ") + QString(e.GetDescription());
        qDebug() << sexc;
        addTrace(sexc, QColor("red"));
    }
}

void MainWindow::cameraCloseAll()
{
    foreach(SCamera* cam, cameras)
    {
        cameraClose(cam->cameraName);
        view->removeView(cam->cameraName);
    }

    cameras.clear();

    setButtonStatus(false);
    resetValues();

    param->setEnableCotrols(false);
}

void MainWindow::cameraSnap(QString cameraName)
{
    try
    {
        SCamera* cam = cameras[cameraName];

        INodeMap& Nodemap = cam->camera->GetNodeMap();
        CEnumerationPtr TriggerMode(Nodemap.GetNode("TriggerMode"));
        TriggerMode->FromString("Off");

        CGrabResultPtr ptrGrabResult;
        CImageFormatConverter fc;
        CPylonImage image;

        cam->camera->MaxNumBuffer = 1;
        cam->camera->StartGrabbing(1);
        qDebug() << cameraName + QString(" StartGrabbing()");
        cam->camera->RetrieveResult(2000, ptrGrabResult, TimeoutHandling_Return);

        if (ptrGrabResult->GrabSucceeded())
        {
            addTrace(QString("Snap, image %1").arg(ptrGrabResult->GetWidth()) + QString("x%1 [px]").arg(ptrGrabResult->GetHeight()));

            fc.OutputPixelFormat = PixelType_RGB8packed;
            fc.Convert(image, ptrGrabResult);
            QImage qtImage((uint8_t*)image.GetBuffer(), ptrGrabResult->GetWidth(), ptrGrabResult->GetHeight(),QImage::Format_RGB888);

            view->updateImage(cameraName, qtImage);
        }
        else
        {
            addTrace(QString("An exception occurred: ") + QString(ptrGrabResult->GetErrorDescription()), QColor("red"));
        }

        cam->camera->StopGrabbing();
        qDebug() << cameraName + QString(" StopGrabbing()");
     }
     catch (Pylon::GenericException &e)
     {
        QString sexc = QString("An exception occurred: ") + QString(e.GetDescription());
        qDebug() << sexc;
        addTrace(sexc, QColor("red"));
     }
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
        QString sexc = QString("An exception occurred: ") + QString(e.GetDescription());
        qDebug() << sexc;
        addTrace(sexc, QColor("red"));
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
       QString sexc = QString("An exception occurred: ") + QString(e.GetDescription());
       qDebug() << sexc;
       addTrace(sexc, QColor("red"));
    }
}

void MainWindow::on_Toolbar_DeviceOpen()
{
    cameraOpenAll();
}

void MainWindow::on_Toolbar_DeviceClose()
{
    cameraCloseAll();
}

void MainWindow::on_Toolbar_SingleGrab()
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

void MainWindow::on_Toolbar_ContinuesGrab()
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

void MainWindow::on_Toolbar_GrabStop()
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
