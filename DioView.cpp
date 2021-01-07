#include <QMessageBox>
#include <QDebug>
#include "DioView.h"
#include "ui_DioView.h"
#include "Lib/FVEDIOLib/FVEDIOException.h"

using namespace Utilities::FVEDIO;

DioView::DioView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DioView),
    fvedio(nullptr),
    groupDO(new QButtonGroup(this))
{
    ui->setupUi(this);

    groupDO->addButton(ui->radio_DO_0, 0);
    groupDO->addButton(ui->radio_DO_1, 1);
    groupDO->addButton(ui->radio_DO_2, 2);
    groupDO->addButton(ui->radio_DO_3, 3);
    groupDO->addButton(ui->radio_DO_4, 4);
    groupDO->addButton(ui->radio_DO_5, 5);
    groupDO->addButton(ui->radio_DO_6, 6);
    groupDO->addButton(ui->radio_DO_7, 7);
    groupDO->addButton(ui->radio_DO_8, 8);
    groupDO->addButton(ui->radio_DO_9, 9);
    groupDO->addButton(ui->radio_DO_10, 10);
    groupDO->addButton(ui->radio_DO_11, 11);
    groupDO->addButton(ui->radio_DO_12, 12);
    groupDO->addButton(ui->radio_DO_13, 13);
    groupDO->addButton(ui->radio_DO_14, 14);
    groupDO->addButton(ui->radio_DO_15, 15);
    groupDO->setExclusive(false);

    connect(groupDO, SIGNAL(buttonClicked(int)), this, SLOT(buttonClicked(int)));

//    connect(ui->dial_LED_0, SIGNAL(sliderReleased()), this, SLOT(on_dial_LED0_sliderReleased()));
//    connect(ui->dial_LED_1, SIGNAL(sliderReleased()), this, SLOT(on_dial_LED1_sliderReleased()));
//    connect(ui->dial_LED_2, SIGNAL(sliderReleased()), this, SLOT(on_dial_LED2_sliderReleased()));
//    connect(ui->dial_LED_3, SIGNAL(sliderReleased()), this, SLOT(on_dial_LED3_sliderReleased()));
}

DioView::~DioView()
{
    delete fvedio;
    delete ui;
}

void DioView::DioClose()
{
    try
    {
        //cleanup
        if(fvedio!=nullptr)
        {
            fvedio->WriteDO(0xffff);
            fvedio->LED0(0);
            fvedio->LED1(0);
            fvedio->LED2(0);
            fvedio->LED3(0);

        }
    }
    catch (FVEDIOException& e)
    {
        QString msg = QString("Digital IO failed! Error: ") + QString(e.what());
        qDebug() << msg;
    }
}

bool DioView::Initialize(QString comPort, int serSpeed)
{
    bool status = false;

    try
    {
        Serial::BaudRate br = Serial::BR_57600;

        switch(serSpeed)
        {
        case 57600:
            br = Serial::BR_57600;
            break;
        default:
            break;
        }

        fvedio = new FVEDIOProtocol(comPort.toStdString().c_str(), Serial::BR_57600);
        fvedio->WriteDO(0xffff);
        fvedio->LED0(0);
        fvedio->LED1(0);
        fvedio->LED2(0);
        fvedio->LED3(0);

        status = true;
    }
    catch (FVEDIOException& e)
    {
        QString msg = QString("Digital IO initialization (") + comPort + QString(") failed! Error: ") + QString(e.what());
        emit addTrace(msg);

        //QMessageBox::warning(this, "Warning", msg);
        //qDebug() << msg;
    }

    return status;
}

void DioView::buttonClicked(int id)
{
    bool val = groupDO->buttons().at(id)->isChecked();
    SetDO(id, val);
}

void DioView::SetDO(int pos, int val)
{
    try
    {
        if(fvedio!=nullptr)
            fvedio->WriteSingleDO(pos, (val)? 1:0);
    }
    catch (FVEDIOException& e)
    {
        QString msg = QString("Digital IO failed! Error: ") + QString(e.what());
        emit addTrace(msg);

        //QMessageBox::warning(this, "Warning", msg);
        //qDebug() << msg;
    }
}

void DioView::on_dialLED0_sliderReleased()
{
    int val = ui->dialLED0->value();
    LED0((uint8_t)val);
}

void DioView::on_dialLED1_sliderReleased()
{
    int val = ui->dialLED1->value();
    LED1((uint8_t)val);
}

void DioView::on_dialLED2_sliderReleased()
{
    int val = ui->dialLED2->value();
    LED2((uint8_t)val);
}

void DioView::on_dialLED3_sliderReleased()
{
    int val = ui->dialLED3->value();
    LED3((uint8_t)val);
}

void DioView::LED0(uint8_t intensity0_100)
{
    try
    {
        if(fvedio!=nullptr)
            fvedio->LED0(intensity0_100);
    }
    catch (FVEDIOException& e)
    {
        QString msg = QString("Digital IO failed! Error: ") + QString(e.what());
        emit addTrace(msg);

        //QMessageBox::warning(this, "Warning", msg);
        //qDebug() << msg;
    }
}

void DioView::LED1(uint8_t intensity0_100)
{
    try
    {
        if(fvedio!=nullptr)
        {
            fvedio->LED1(intensity0_100);
            Sleep(100);
        }
    }
    catch (FVEDIOException& e)
    {
        QString msg = QString("Digital IO failed! Error: ") + QString(e.what());
        emit addTrace(msg);

        //QMessageBox::warning(this, "Warning", msg);
        //qDebug() << msg;
    }
}

void DioView::LED2(uint8_t intensity0_100)
{
    try
    {
        if(fvedio!=nullptr)
        {
            fvedio->LED2(intensity0_100);
            Sleep(100);
        }
    }
    catch (FVEDIOException& e)
    {
        QString msg = QString("Digital IO failed! Error: ") + QString(e.what());
        emit addTrace(msg);

        //QMessageBox::warning(this, "Warning", msg);
        //qDebug() << msg;
    }
}

void DioView::LED3(uint8_t intensity0_100)
{
    try
    {
        if(fvedio!=nullptr)
        {
            fvedio->LED3(intensity0_100);
            Sleep(100);
        }
    }
    catch (FVEDIOException& e)
    {
        QString msg = QString("Digital IO failed! Error: ") + QString(e.what());
        emit addTrace(msg);

        //QMessageBox::warning(this, "Warning", msg);
        //qDebug() << msg;
    }
}
