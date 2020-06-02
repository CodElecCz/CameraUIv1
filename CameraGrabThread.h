#ifndef GRABTHREAD_H
#define GRABTHREAD_H

#include <QThread>
#include <QImage>
#include <QPointer>
#include <QDebug>
#include <QVector>
#include <QImage>
#include <pylon/PylonIncludes.h>
#include <pylon/DeviceInfo.h>

using namespace Pylon;
using namespace GenApi;
using namespace GenICam;
using namespace std;

class CameraGrabThread : public QThread
{
    Q_OBJECT

public:
    CameraGrabThread(QString name, Pylon::CInstantCamera* camera):
        m_name(name),
        m_camera(camera)
    {
        INodeMap& Nodemap = camera->GetNodeMap();
        CEnumerationPtr PixelFormat(Nodemap.GetNode("PixelFormat"));
        m_pixelFormat = PixelFormat->ToString().c_str();
    }

    ~CameraGrabThread() override
    {  }

    QImage* toQImage(CPylonImage &pylonImage, QImage::Format format)
    {
        int width = static_cast<int>(pylonImage.GetWidth());
        int height = static_cast<int>(pylonImage.GetHeight());
        void *buffer = pylonImage.GetBuffer();
        int step = static_cast<int>(pylonImage.GetAllocatedBufferSize()) / height;
        QImage* img = new QImage(static_cast<uchar*>(buffer), width, height, step, format);
        return img;
    }

    void run() override
    {
        CGrabResultPtr          ptrGrabResult;
        CImageFormatConverter   fc;
        bool                    initTrace = true;
        QImage                  *qImage = nullptr;

        while(m_camera->IsGrabbing() && !QThread::currentThread()->isInterruptionRequested())
        {
            bool isGrabbing = m_camera->RetrieveResult(2000, ptrGrabResult, TimeoutHandling_Return);

            // Image grabbed successfully?
            if (isGrabbing && ptrGrabResult->GrabSucceeded())
            {
                if(initTrace)
                {
                    initTrace = false;
                    emit addTrace(QString(m_name + ", Grab start image %1").arg(ptrGrabResult->GetWidth()) + QString("x%1 [px]").arg(ptrGrabResult->GetHeight()));
                }

                if(QString::compare(m_pixelFormat, "Mono8")==0)
                {
                    fc.OutputPixelFormat = PixelType_Mono8;
                    fc.Convert(m_image, ptrGrabResult);
                    qImage = toQImage(m_image, QImage::Format_Grayscale8);
                }
                else
                {
                    fc.OutputPixelFormat = PixelType_RGB8packed;
                    fc.Convert(m_image, ptrGrabResult);
                    qImage = toQImage(m_image, QImage::Format_RGB888);
                }

                if(!QThread::currentThread()->isInterruptionRequested())
                    emit updateImage(m_name, qImage);
            }
            else if(isGrabbing)
            {
                emit addTrace(QString("Grab exception, ") + QString(ptrGrabResult->GetErrorDescription()), QColor("red"));
            }
            else //StopGrabbing()
                break;
        }

        m_camera->StopGrabbing();
        qDebug() << m_name + QString(" StopGrabbing()");

        emit addTrace(QString("Grab stop"));
    }

signals:
    void updateImage(QString cameraName, QImage* image);
    void addTrace(QString text, QColor color = QColor("black"));

private:
    QString                 m_name;
    Pylon::CInstantCamera   *m_camera;

    //image
    CPylonImage             m_image;
    QString                 m_pixelFormat;

};

#endif // GRABTHREAD_H
