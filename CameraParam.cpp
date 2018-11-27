#include "CameraParam.h"
#include "ui_cameraparam.h"

CameraParam::CameraParam(QWidget *parent, Pylon::VersionInfo info) :
    QWidget(parent),
    ui(new Ui::CameraParam),
    m_DeviceSFNCVersion(info),
    m_width(-1),
    m_height(-1)
{
    ui->setupUi(this);

    setEnableCotrols(false);
}

CameraParam::~CameraParam()
{
    setEnableCotrols(false);

    delete ui;
}

void CameraParam::resetValues()
{
    //reset the value of editable controlls on GUI
    ui->scrollBar_Exposure->setRange(0,10);
    ui->scrollBar_Exposure->setValue(0);
    ui->scrollBar_Exposure->update();
    ui->scrollBar_Height->setRange(0,10);
    ui->scrollBar_Height->setValue(0);
    ui->scrollBar_Width->setRange(0,10);
    ui->scrollBar_Width->setValue(0);
    ui->scrollBar_OffsetX->setRange(0,10);
    ui->scrollBar_OffsetX->setValue(0);
    ui->scrollBar_OffsetY->setRange(0,10);
    ui->scrollBar_OffsetY->setValue(0);
    ui->spinBox_Height->setValue(0);
    ui->spinBox_Width->setValue(0);
    ui->spinBox_OffsetX->setValue(0);
    ui->spinBox_OffsetY->setValue(0);
    ui->spinBox_Exposure->setValue(0);
}

void CameraParam::setEnableCotrols(bool status)
{
    ui->scrollBar_Exposure->setEnabled(status);
    ui->scrollBar_Height->setEnabled(status);
    ui->scrollBar_Width->setEnabled(status);
    ui->scrollBar_OffsetX->setEnabled(status);
    ui->scrollBar_OffsetY->setEnabled(status);
    ui->comboBox_PixelFormat->setEnabled(status);
    ui->comboBox_TestImageSelector->setEnabled(status);
    ui->spinBox_Exposure->setEnabled(status);
    ui->spinBox_Height->setEnabled(status);
    ui->spinBox_Width->setEnabled(status);
    ui->spinBox_OffsetX->setEnabled(status);
    ui->spinBox_OffsetY->setEnabled(status);

    if(status)
    {
        connect(ui->spinBox_Height, SIGNAL(editingFinished()), this, SLOT(on_spinBox_Height_editingFinished()));
        connect(ui->scrollBar_Height, SIGNAL(sliderReleased()), this, SLOT(on_scrollBar_Height_sliderReleased()));

        connect(ui->spinBox_Width, SIGNAL(editingFinished()), this, SLOT(on_spinBox_Width_editingFinished()));
        connect(ui->scrollBar_Width, SIGNAL(sliderReleased()), this, SLOT(on_scrollBar_Width_sliderReleased()));

        connect(ui->spinBox_OffsetX, SIGNAL(editingFinished()), this, SLOT(on_spinBox_OffsetX_editingFinished()));
        connect(ui->scrollBar_OffsetX, SIGNAL(sliderReleased()), this, SLOT(on_scrollBar_OffsetX_sliderReleased()));

        connect(ui->spinBox_OffsetY, SIGNAL(editingFinished()), this, SLOT(on_spinBox_OffsetY_editingFinished()));
        connect(ui->scrollBar_OffsetY, SIGNAL(sliderReleased()), this, SLOT(on_scrollBar_OffsetY_sliderReleased()));

        connect(ui->spinBox_Exposure, SIGNAL(editingFinished()), this, SLOT(on_spinBox_Exposure_editingFinished()));
        connect(ui->scrollBar_Exposure, SIGNAL(sliderReleased()), this, SLOT(on_scrollBar_Exposure_sliderReleased()));

        connect(ui->comboBox_PixelFormat, SIGNAL(currentIndexChanged(const QString)), this, SLOT(on_comboBox_PixelFormat_currentIndexChanged(const QString)));
        connect(ui->comboBox_TestImageSelector, SIGNAL(currentIndexChanged(const QString)), this, SLOT(on_comboBox_TestImageSelector_currentIndexChanged(const QString)));
    }
    else
    {
        disconnect(ui->spinBox_Height, SIGNAL(editingFinished()), this, SLOT(on_spinBox_Height_editingFinished()));
        disconnect(ui->scrollBar_Height, SIGNAL(sliderReleased()), this, SLOT(on_scrollBar_Height_sliderReleased()));

        disconnect(ui->spinBox_Width, SIGNAL(editingFinished()), this, SLOT(on_spinBox_Width_editingFinished()));
        disconnect(ui->scrollBar_Width, SIGNAL(sliderReleased()), this, SLOT(on_scrollBar_Width_sliderReleased()));

        disconnect(ui->spinBox_OffsetX, SIGNAL(editingFinished()), this, SLOT(on_spinBox_OffsetX_editingFinished()));
        disconnect(ui->scrollBar_OffsetX, SIGNAL(sliderReleased()), this, SLOT(on_scrollBar_OffsetX_sliderReleased()));

        disconnect(ui->spinBox_OffsetY, SIGNAL(editingFinished()), this, SLOT(on_spinBox_OffsetY_editingFinished()));
        disconnect(ui->scrollBar_OffsetY, SIGNAL(sliderReleased()), this, SLOT(on_scrollBar_OffsetY_sliderReleased()));

        disconnect(ui->spinBox_Exposure, SIGNAL(editingFinished()), this, SLOT(on_spinBox_Exposure_editingFinished()));
        disconnect(ui->scrollBar_Exposure, SIGNAL(sliderReleased()), this, SLOT(on_scrollBar_Exposure_sliderReleased()));

        disconnect(ui->comboBox_PixelFormat, SIGNAL(currentIndexChanged(const QString)), this, SLOT(on_comboBox_PixelFormat_currentIndexChanged(const QString)));
        disconnect(ui->comboBox_TestImageSelector, SIGNAL(currentIndexChanged(const QString)), this, SLOT(on_comboBox_TestImageSelector_currentIndexChanged(const QString)));

        setCameraName(QString(""));
    }
}

void CameraParam::setWidth(int val, int min, int max, int inc)
{
    ui->scrollBar_Width->setMaximum(max);
    ui->scrollBar_Width->setMinimum(min);
    ui->scrollBar_Width->setSingleStep(inc);
    ui->scrollBar_Width->setValue(val);

    ui->spinBox_Width->setMaximum(max);
    ui->spinBox_Width->setMinimum(min);
    ui->spinBox_Width->setSingleStep(inc);
    ui->spinBox_Width->setValue(val);
}

void CameraParam::setHeight(int val, int min, int max, int inc)
{
    ui->scrollBar_Height->setMaximum(max);
    ui->scrollBar_Height->setMinimum(min);
    ui->scrollBar_Height->setSingleStep(inc);
    ui->scrollBar_Height->setValue(val);

    ui->spinBox_Height->setMaximum(max);
    ui->spinBox_Height->setMinimum(min);
    ui->spinBox_Height->setSingleStep(inc);
    ui->spinBox_Height->setValue(val);
}

void CameraParam::setOffsetX(int val, int min, int max, int inc)
{
    ui->scrollBar_OffsetX->setMaximum(max);
    ui->scrollBar_OffsetX->setMinimum(min);
    ui->scrollBar_OffsetX->setSingleStep(inc);
    ui->scrollBar_OffsetX->setValue(val);

    ui->spinBox_OffsetX->setMaximum(max);
    ui->spinBox_OffsetX->setMinimum(min);
    ui->spinBox_OffsetX->setSingleStep(inc);
    ui->spinBox_OffsetX->setValue(val);
}

void CameraParam::setOffsetY(int val, int min, int max, int inc)
{
    ui->scrollBar_OffsetY->setMaximum(max);
    ui->scrollBar_OffsetY->setMinimum(min);
    ui->scrollBar_OffsetY->setSingleStep(inc);
    ui->scrollBar_OffsetY->setValue(val);

    ui->spinBox_OffsetY->setMaximum(max);
    ui->spinBox_OffsetY->setMinimum(min);
    ui->spinBox_OffsetY->setSingleStep(inc);
    ui->spinBox_OffsetY->setValue(val);
}

void CameraParam::setExposure(int val, int min, int max, int inc)
{
    ui->scrollBar_Exposure->setMaximum(max);
    ui->scrollBar_Exposure->setMinimum(min);
    ui->scrollBar_Exposure->setSingleStep(inc);
    ui->scrollBar_Exposure->setValue(val);

    ui->spinBox_Exposure->setMaximum(max);
    ui->spinBox_Exposure->setMinimum(min);
    ui->spinBox_Exposure->setSingleStep(inc);
    ui->spinBox_Exposure->setValue(val);
}

void CameraParam::setPixelFormat(QString val, QStringList enumFeatureList)
{
    ui->comboBox_PixelFormat->clear();
    ui->comboBox_PixelFormat->addItems(enumFeatureList);
    ui->comboBox_PixelFormat->setCurrentText(val);
}

void CameraParam::setTestImageSelector(QString val, QStringList enumFeatureList)
{
    ui->comboBox_TestImageSelector->clear();
    ui->comboBox_TestImageSelector->addItems(enumFeatureList);
    ui->comboBox_TestImageSelector->setCurrentText(val);
}

void CameraParam::on_spinBox_Height_editingFinished()
{
    int val = ui->spinBox_Height->value();

    if( val !=  ui->scrollBar_Height->value())  // to avoid a loop
    {
        emit setIntergerValue(getCameraName(), "Height", val);
        ui->scrollBar_Height->setValue(val);

        if(m_height>0 && ui->spinBox_Height->isEnabled())
        {
            int diff = m_height - val;
            ui->spinBox_OffsetY->setMaximum(diff);
            ui->scrollBar_OffsetY->setMaximum(diff);
        }
    }
}

void CameraParam::on_scrollBar_Height_sliderReleased()
{
    int val = ui->scrollBar_Height->value();

    if(val != ui->spinBox_Height->value())
    {
        emit setIntergerValue(getCameraName(), "Height", val);
        ui->spinBox_Height->setValue(val);

        if(m_height>0 && ui->scrollBar_Height->isEnabled())
        {
            int diff = m_height - val;
            ui->spinBox_OffsetY->setMaximum(diff);
            ui->scrollBar_OffsetY->setMaximum(diff);
        }
    }
}

void CameraParam::on_spinBox_Width_editingFinished()
{
    int val = ui->spinBox_Width->value();

    if(val !=  ui->scrollBar_Width->value())
    {
        emit setIntergerValue(getCameraName(), "Width", val);
        ui->scrollBar_Width->setValue(val);

        if(m_width>0 && ui->spinBox_Width->isEnabled())
        {
            int diff = m_width - val;
            ui->spinBox_OffsetX->setMaximum(diff);
            ui->scrollBar_OffsetX->setMaximum(diff);
        }
    }
}

void CameraParam::on_scrollBar_Width_sliderReleased()
{
    int val = ui->scrollBar_Width->value();

    if(val != ui->spinBox_Width->value())
    {
        emit setIntergerValue(getCameraName(), "Width", val);
        ui->spinBox_Width->setValue(val);

        if(m_width>0 && ui->scrollBar_Width->isEnabled())
        {
            int diff = m_width - val;
            ui->spinBox_OffsetX->setMaximum(diff);
            ui->scrollBar_OffsetX->setMaximum(diff);
        }
    }
}

void CameraParam::on_spinBox_OffsetX_editingFinished()
{
    int val = ui->spinBox_OffsetX->value();

    if(val !=  ui->scrollBar_OffsetX->value())
    {
        emit setIntergerValue(getCameraName(), "OffsetX", val);
        ui->scrollBar_Width->setValue(val);

        if(m_width>0 && ui->spinBox_OffsetX->isEnabled())
        {
            int diff = m_width - val;
            ui->spinBox_Width->setMaximum(diff);
            ui->scrollBar_Width->setMaximum(diff);
        }
    }
}

void CameraParam::on_scrollBar_OffsetX_sliderReleased()
{
    int val = ui->scrollBar_OffsetX->value();

    if(val != ui->spinBox_OffsetX->value())
    {
        emit setIntergerValue(getCameraName(), "OffsetX", val);
        ui->spinBox_OffsetX->setValue(val);

        if(m_width>0 && ui->scrollBar_OffsetX->isEnabled())
        {
            int diff = m_width - val;
            ui->spinBox_Width->setMaximum(diff);
            ui->scrollBar_Width->setMaximum(diff);
        }
    }
}

void CameraParam::on_spinBox_OffsetY_editingFinished()
{
    int val = ui->spinBox_OffsetY->value();

    if(val !=  ui->scrollBar_OffsetY->value())
    {
        emit setIntergerValue(getCameraName(), "OffsetY", val);
        ui->scrollBar_OffsetY->setValue(val);

        if(m_height>0 && ui->spinBox_OffsetY->isEnabled())
        {
            int diff = m_height - val;
            ui->spinBox_Height->setMaximum(diff);
            ui->scrollBar_Height->setMaximum(diff);
        }
    }
}

void CameraParam::on_scrollBar_OffsetY_sliderReleased()
{
    int val = ui->scrollBar_OffsetY->value();

    if(val != ui->spinBox_OffsetY->value())
    {
        emit setIntergerValue(getCameraName(), "OffsetY", val);
        ui->spinBox_OffsetY->setValue(val);

        if(m_height>0 && ui->scrollBar_OffsetY->isEnabled())
        {
            int diff = m_height - val;
            ui->spinBox_Height->setMaximum(diff);
            ui->scrollBar_Height->setMaximum(diff);
        }
    }
}

void CameraParam::on_spinBox_Exposure_editingFinished()
{
    int val = ui->spinBox_Exposure->value();
    if(val !=  ui->scrollBar_Exposure->value())
    {
        if(m_DeviceSFNCVersion < Pylon::Sfnc_2_0_0 )
        {
            emit setIntergerValue(getCameraName(), "ExposureTimeRaw", val);
        }
        else if (m_DeviceSFNCVersion == Pylon::Sfnc_2_1_0)
        {
            emit setFloatValue(getCameraName(), "ExposureTime", val);
        }
        else if (m_DeviceSFNCVersion == Pylon::Sfnc_2_2_0)
        {
            emit setFloatValue(getCameraName(), "ExposureTime", val);
        }
        ui->scrollBar_Exposure->setValue(val);
        ui->scrollBar_Exposure->setSliderPosition(val);
    }
}

void CameraParam::on_scrollBar_Exposure_sliderReleased()
{
    int val = ui->scrollBar_Exposure->value();

    if(val != ui->spinBox_Exposure->value())
    {
        if(m_DeviceSFNCVersion < Pylon::Sfnc_2_0_0)
        {
             emit setIntergerValue(getCameraName(), "ExposureTimeRaw", val);
        }
        else if (m_DeviceSFNCVersion == Pylon::Sfnc_2_1_0) // Ace USB
        {
             emit setFloatValue(getCameraName(), "ExposureTime", val);
        }
        else if (m_DeviceSFNCVersion == Pylon::Sfnc_2_2_0)   // Dart Cameras
        {
            emit setFloatValue(getCameraName(), "ExposureTime", val);
        }

        ui->spinBox_Exposure->setValue(val);
    }
}

void CameraParam::on_comboBox_PixelFormat_currentIndexChanged(const QString &arg1)
{
    emit setStringValue(getCameraName(), "PixelFormat", arg1);
}

void CameraParam::on_comboBox_TestImageSelector_currentIndexChanged(const QString &arg1)
{
    if (m_DeviceSFNCVersion < Pylon::Sfnc_2_2_0)
    {
         emit setStringValue(getCameraName(), "TestImageSelector", arg1);
    }
    else if(m_DeviceSFNCVersion == Pylon::Sfnc_2_2_0)
    {
        emit setStringValue(getCameraName(), "TestPattern", arg1);
    }
}

void CameraParam::setCameraName(QString name)
{
    ui->label_Camera->setText(name);
}


QString CameraParam::getCameraName()
{
    return ui->label_Camera->text();
}



