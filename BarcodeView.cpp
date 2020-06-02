#include <QThread>
#include <QDebug>
#include <QTimer>
#include "BarcodeView.h"
#include "ui_BarcodeView.h"
#include "CLVLib/CLVException.h"

using namespace Utilities::CLV;

BarcodeView::BarcodeView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BarcodeView),
    clv(nullptr),
    m_lastBarcode("none"),
    m_waitAfterReadDone(2000),
    m_delayProcessImage(200),
    m_comPort("")
{
    ui->setupUi(this);

    ui->textBrowser->document()->setMaximumBlockCount(10);

    //reinit timer
    checkTimer = new QTimer(this);
    connect(checkTimer, SIGNAL(timeout()), this, SLOT(checkTimer_timeout()));
}

BarcodeView::~BarcodeView()
{
    delete clv;
    delete ui;
}

void BarcodeView::checkTimer_timeout()
{
    try
    {
        QDateTime dt = QDateTime::currentDateTime();

        if((dt.toSecsSinceEpoch()-m_lastReadTime.toSecsSinceEpoch())>30*60)
        {
            if(clv->IsReading())
                clv->StopReading();

            delete clv;

            Serial::BaudRate br = Serial::BR_9600;
            switch(m_comSpeed)
            {
            case 9600:
                br = Serial::BR_9600;
                break;
            case 38400:
                br = Serial::BR_38400;
                break;
            case 57600:
                br = Serial::BR_57600;
                break;
            default:
                break;
            }

            clv = new CLVProtocol(m_comPort.toStdString().c_str(), br);
            m_lastReadTime = QDateTime::currentDateTime();

            //clv->SelfTest();
            clv->ReadingMode();
            clv->StartReading(true, m_waitAfterReadDone);
        }
    }
    catch (CLVException& e)
    {
        QString msg = QString("Barcode reinit failed! Error: ") + QString(e.what());
        emit addTrace(msg);

        //QMessageBox::warning(this, "Warning", msg);
        //qDebug() << msg;
    }
}

bool BarcodeView::Initialize(QString comPort, int comSpeed, int waitAfterReadDone, int delayProcessImage, bool restartComm)
{
    bool status = false;
    m_waitAfterReadDone = static_cast<UINT32>(waitAfterReadDone);
    m_delayProcessImage = static_cast<UINT32>(delayProcessImage);
    m_comSpeed = comSpeed;
    m_comPort = comPort;

    try
    {
        Serial::BaudRate br = Serial::BR_9600;
        switch(comSpeed)
        {
        case 9600:
            br = Serial::BR_9600;
            break;
        case 38400:
            br = Serial::BR_38400;
            break;
        case 57600:
            br = Serial::BR_57600;
            break;
        default:
            break;
        }

        clv = new CLVProtocol(comPort.toStdString().c_str(), br);
        m_lastReadTime = QDateTime::currentDateTime();

        if(restartComm)
            checkTimer->start(10*1000);

        clv->SelfTest();
        clv->ReadingMode();
        clv->OnValueChanged += event_handler(this, &BarcodeView::OnValueChanged);

        QTimer::singleShot(2000, this, [=]
        {
            clv->StartReading(true, m_waitAfterReadDone);
        });
        ui->pushButton->setText("Stop Reading");

        status = true;
    }
    catch (CLVException& e)
    {
        QString msg = QString("Barcode initialization (") + comPort + QString(") failed! Error: ") + QString(e.what());
        emit addTrace(msg);

        //QMessageBox::warning(this, "Warning", msg);
        //qDebug() << msg;
    }

    return status;
}

void BarcodeView::BarcodeClose()
{
    try
    {
        if(clv!=nullptr)
        {
            if(clv->IsReading())
                clv->StopReading();

            clv->SelfTest(false);

            qDebug() << "Barcode Close()";

            delete clv;
            clv = nullptr;
        }
    }
    catch (CLVException& e)
    {
        QString msg = QString("Barcode failed! Error: ") + QString(e.what());

        qDebug() << msg;
    }
}

void BarcodeView::on_pushButton_pressed()
{
    try
    {
        if(clv)
        {
            if(clv->IsReading())
            {
                clv->StopReading();
                ui->pushButton->setText("Start Reading");

                emit addTrace("Barcode readed, stop");
            }
            else
            {
                clv->StartReading(true, m_waitAfterReadDone);
                ui->pushButton->setText("Stop Reading");

                emit addTrace("Barcode readed, start");
            }
        }
    }
    catch (CLVException& e)
    {
        QString msg = QString("Barcode failed! Error: ") + QString(e.what());
        emit addTrace(msg);

        //QMessageBox::warning(this, "Warning", msg);
        //qDebug() << msg;
    }
}

void BarcodeView::OnValueChanged(CLVProtocol* sender, const char* barcode, UINT64 barcodeLen)
{
    if (thread()!=QThread::currentThread())
    {
        QString *nodeId = new QString(barcode);
        qDebug() << "OnValueChanged() [" + *nodeId + "] threadId:0x" + QString::number((int)thread()->currentThreadId(), 16);

        QGenericArgument arg("QString", nodeId);
        QMetaObject::invokeMethod(this, "barcode_read", arg);
    }
    else
        emit this->barcode_read(QString(barcode));
}

void BarcodeView::barcode_read(QString barcode)
{
    if(barcode.length()>0)
    {
        m_lastReadTime = QDateTime::currentDateTime();

        //trace
        ui->textBrowser->append(m_lastReadTime.toString("[hh:mm:ss.zzz] ") + barcode);

        //start image snap
        int cmp = QString::compare(m_lastBarcode, barcode);
        if(cmp!=0)
        {
            emit addTrace("Barcode read, " + barcode);

            if(m_delayProcessImage>0)
                Sleep(m_delayProcessImage);

            emit processImages(barcode);

            m_lastBarcode = barcode;
        }
    }
}
