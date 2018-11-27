#ifndef CAMERAPARAM_H
#define CAMERAPARAM_H

#include <QWidget>
#include <QDockWidget>
#include <pylon/PylonIncludes.h>

namespace Ui {
class CameraParam;
}

class CameraParam : public QWidget
{
    Q_OBJECT

public:
    explicit CameraParam(QWidget *parent = nullptr, Pylon::VersionInfo info = Pylon::Sfnc_2_0_0);
    ~CameraParam();

    void resetValues();
    void setEnableCotrols(bool status);
    void setWidth(int val, int min, int max, int inc);
    void setHeight(int val, int min, int max, int inc);
    void setOffsetX(int val, int min, int max, int inc);
    void setOffsetY(int val, int min, int max, int inc);
    void setExposure(int val, int min, int max, int inc);
    void setPixelFormat(QString val, QStringList enumFeatureList);
    void setTestImageSelector(QString val, QStringList enumFeatureList);
    void setCameraName(QString name);

    void setResolution(int width, int height) { m_width = width; m_height = height; }
    void setDeviceInfo(Pylon::VersionInfo info) { m_DeviceSFNCVersion = info; }

private:
    QString getCameraName();

private slots:
    void on_spinBox_Height_editingFinished();
    void on_scrollBar_Height_sliderReleased();
    void on_spinBox_Width_editingFinished();
    void on_scrollBar_Width_sliderReleased();
    void on_spinBox_OffsetX_editingFinished();
    void on_scrollBar_OffsetX_sliderReleased();
    void on_spinBox_OffsetY_editingFinished();
    void on_scrollBar_OffsetY_sliderReleased();
    void on_spinBox_Exposure_editingFinished();
    void on_scrollBar_Exposure_sliderReleased();
    void on_comboBox_PixelFormat_currentIndexChanged(const QString &arg1);
    void on_comboBox_TestImageSelector_currentIndexChanged(const QString &arg1);

signals:
    void addTrace(QString text, QColor color = QColor("black"));
    void setFloatValue(QString cameraName, QString nodeName, int value);
    void setIntergerValue(QString cameraName, QString nodeName, int value);
    void setStringValue(QString cameraName, QString nodeName, QString value);

private:
    Ui::CameraParam     *ui;
    Pylon::VersionInfo  m_DeviceSFNCVersion;
    int                 m_width;
    int                 m_height;
};

#endif // CAMERAPARAM_H
