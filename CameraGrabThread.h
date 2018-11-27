#ifndef GRABTHREAD_H
#define GRABTHREAD_H

#include <QThread>
#include <QImage>
#include <QPointer>
#include <QDebug>
#include <QVector>
#include <pylon/PylonIncludes.h>

using namespace Pylon;
using namespace std;

class CameraGrabThread : public QThread
{
    Q_OBJECT

public:
    CameraGrabThread(QString name, Pylon::CInstantCamera* camera):
        m_name(name),
        m_camera(camera),
        m_imageData(new std::vector<uchar>(1920*1080))
    {  }

    ~CameraGrabThread() override
    { delete m_imageData; }

    void run() override
    {
        CGrabResultPtr  ptrGrabResult;
        CImageFormatConverter fc;
        CPylonImage     image;
        bool            initTrace = true;

        while(m_camera->IsGrabbing() && !QThread::currentThread()->isInterruptionRequested())
        {
            bool isGrabbing = m_camera->RetrieveResult(2000, ptrGrabResult, TimeoutHandling_Return);

            // Image grabbed successfully?
            if (isGrabbing && ptrGrabResult->GrabSucceeded())
            {
                if(initTrace)
                {
                    initTrace = false;
                    emit addTrace(QString("Grab start, image %1").arg(ptrGrabResult->GetWidth()) + QString("x%1 [px]").arg(ptrGrabResult->GetHeight()));
                }

                fc.OutputPixelFormat = PixelType_RGB8packed;
                fc.Convert(image, ptrGrabResult);

                //image data alloc
                uchar *data = (uchar*)image.GetBuffer();
                size_t dataSize = image.GetAllocatedBufferSize();
                m_imageData->assign(data, data + dataSize);

                //m_image = new  QImage((uint8_t*)image.GetBuffer(), ptrGrabResult->GetWidth(), ptrGrabResult->GetHeight(), QImage::Format_RGB888);
                m_image = new  QImage(m_imageData->data(), ptrGrabResult->GetWidth(), ptrGrabResult->GetHeight(), QImage::Format_RGB888);

                if(!QThread::currentThread()->isInterruptionRequested())
                    emit updateImage(m_name, m_image);
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
    QImage                  *m_image;
    std::vector<uchar>      *m_imageData;

};

typedef struct _SCamera
{
    QString               cameraName;
    Pylon::CInstantCamera *camera;
    CameraGrabThread      *grab;
} SCamera;

#endif // GRABTHREAD_H
