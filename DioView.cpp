#include <QMessageBox>
#include "DioView.h"
#include "ui_dio.h"
#ifdef FVEDIO
#include "FVEDIOLib/FVEDIOException.h"
#endif

using namespace Utils::FVEDIO;

DioView::DioView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DIO),
#ifdef FVEDIO
    fvedio(nullptr),
#endif
    groupDO(new QButtonGroup(this))
{
    ui->setupUi(this);

    groupDO->addButton(ui->radio_DO_0);
    groupDO->addButton(ui->radio_DO_1);
    groupDO->addButton(ui->radio_DO_2);
    groupDO->addButton(ui->radio_DO_3);
    groupDO->addButton(ui->radio_DO_4);
    groupDO->addButton(ui->radio_DO_5);
    groupDO->addButton(ui->radio_DO_6);
    groupDO->addButton(ui->radio_DO_7);
    groupDO->addButton(ui->radio_DO_8);
    groupDO->addButton(ui->radio_DO_9);
    groupDO->addButton(ui->radio_DO_10);
    groupDO->addButton(ui->radio_DO_11);
    groupDO->addButton(ui->radio_DO_12);
    groupDO->addButton(ui->radio_DO_13);
    groupDO->addButton(ui->radio_DO_14);
    groupDO->addButton(ui->radio_DO_15);
    groupDO->setExclusive(false);

    connect(groupDO, SIGNAL(buttonClicked(int)), this, SLOT(buttonClicked(int)));

    connect(ui->dial_LED_0, SIGNAL(sliderReleased()), this, SLOT(on_dial_LED0_sliderReleased()));
    connect(ui->dial_LED_1, SIGNAL(sliderReleased()), this, SLOT(on_dial_LED1_sliderReleased()));
    connect(ui->dial_LED_2, SIGNAL(sliderReleased()), this, SLOT(on_dial_LED2_sliderReleased()));
    connect(ui->dial_LED_3, SIGNAL(sliderReleased()), this, SLOT(on_dial_LED3_sliderReleased()));
}

DioView::~DioView()
{
#ifdef FVEDIO
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
        ;
    }

    delete fvedio;
#endif
    delete ui;
}

void DioView::Initialize(QString comPort, int serSpeed)
{
#ifdef FVEDIO
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
    }
    catch (FVEDIOException& e)
    {
        QMessageBox::warning(QApplication::allWidgets().at(0), "Warning",
                             QString("Initialization of DIO (")
                             + comPort + QString(") failed!\nError: ")
                             + QString(e.what()));
    }
#endif
}

void DioView::buttonClicked(int id)
{
#ifdef FVEDIO
    try
    {
        if(fvedio!=nullptr)
            fvedio->WriteDO(0xaaff);
    }
    catch (FVEDIOException& e)
    {
        QMessageBox::warning(QApplication::allWidgets().at(0), "Warning", QString(e.what()));
    }
#endif
}

void DioView::on_dial_LED0_sliderReleased()
{
    int val = ui->dial_LED_0->value();
#ifdef FVEDIO
    try
    {
        if(fvedio!=nullptr)
            fvedio->LED0(val);
    }
    catch (FVEDIOException& e)
    {
        QMessageBox::warning(QApplication::allWidgets().at(0), "Warning", QString(e.what()));
    }
#endif
}

void DioView::on_dial_LED1_sliderReleased()
{
    int val = ui->dial_LED_1->value();
#ifdef FVEDIO
    try
    {
        if(fvedio!=nullptr)
            fvedio->LED1(val);
    }
    catch (FVEDIOException& e)
    {
        QMessageBox::warning(QApplication::allWidgets().at(0), "Warning", QString(e.what()));
    }
#endif
}

void DioView::on_dial_LED2_sliderReleased()
{
    int val = ui->dial_LED_2->value();
#ifdef FVEDIO
    try
    {
        if(fvedio!=nullptr)
            fvedio->LED2(val);
    }
    catch (FVEDIOException& e)
    {
        QMessageBox::warning(QApplication::allWidgets().at(0), "Warning", QString(e.what()));
    }
#endif
}

void DioView::on_dial_LED3_sliderReleased()
{
    int val = ui->dial_LED_3->value();
#ifdef FVEDIO
    try
    {
        if(fvedio!=nullptr)
            fvedio->LED3(val);
    }
    catch (FVEDIOException& e)
    {
        QMessageBox::warning(QApplication::allWidgets().at(0), "Warning", QString(e.what()));
    }
#endif
}
