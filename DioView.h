#ifndef DIO_H
#define DIO_H

#include <QWidget>
#include <QButtonGroup>
#include "FVEDIOLib/FVEDIOProtocol.h"

namespace Ui {
class DIO;
}

class DioView : public QWidget
{
    Q_OBJECT

public:
    explicit DioView(QWidget *parent = nullptr);
    ~DioView();

public:
    void Initialize(QString comPort, int serSpeed = 57600);

private slots:
    void buttonClicked(int id);
    void on_dial_LED0_sliderReleased();
    void on_dial_LED1_sliderReleased();
    void on_dial_LED2_sliderReleased();
    void on_dial_LED3_sliderReleased();

private:
    Ui::DIO         *ui;
#ifdef FVEDIO
    FVEDIOProtocol  *fvedio;
#endif
    QButtonGroup    *groupDO;
};

#endif // DIO_H
