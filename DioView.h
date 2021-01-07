#ifndef DIO_H
#define DIO_H

#include <QWidget>
#include <QButtonGroup>
#include "Lib/FVEDIOLib/FVEDIOProtocol.h"

namespace Ui {
class DioView;
}

enum EDIO
{
    EDIO_3Lights_Red = 4,
    EDIO_3Lights_Orange = 5,
    EDIO_3Lights_Green = 6,
};

class DioView : public QWidget
{
    Q_OBJECT

public:
    explicit DioView(QWidget *parent = nullptr);
    ~DioView();

public:
    bool Initialize(QString comPort, int serSpeed = 57600);
    void DioClose();

    void LED0(uint8_t intensity0_100);
    void LED1(uint8_t intensity0_100);
    void LED2(uint8_t intensity0_100);
    void LED3(uint8_t intensity0_100);
    void SetDO(int pos, int val);

private slots:
    //on_{ObjectName}_{SignalName}
    void buttonClicked(int id);
    void on_dialLED0_sliderReleased();
    void on_dialLED1_sliderReleased();
    void on_dialLED2_sliderReleased();
    void on_dialLED3_sliderReleased();

signals:
    void addTrace(QString text, QColor color = QColor("black"));

private:
    Ui::DioView     *ui;
    FVEDIOProtocol  *fvedio;
    QButtonGroup    *groupDO;
};

#endif // DIO_H
