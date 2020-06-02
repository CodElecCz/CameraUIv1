#ifndef BARCODEVIEW_H
#define BARCODEVIEW_H

#include <QWidget>
#include <QTimer>
#include <QDateTime>
#include "CLVLib/CLVProtocol.h"

namespace Ui {
class BarcodeView;
}

class BarcodeView : public QWidget
{
    Q_OBJECT

public:
    explicit BarcodeView(QWidget *parent = nullptr);
    ~BarcodeView();

public:
    bool Initialize(QString comPort, int serSpeed = 9600, int waitAfterReadDone = 5000, int delayProcessImage = 200, bool restartComm = false);
    void BarcodeClose();

private:
     void OnValueChanged(CLVProtocol *sender, const char* barcode, UINT64 barcodeLen);

private slots:
    void on_pushButton_pressed();
    void barcode_read(QString barcode);
    void checkTimer_timeout();

signals:
    void addTrace(QString text, QColor color = QColor("black"));
    void processImages(QString barcode);

private:
    Ui::BarcodeView *ui;
    CLVProtocol     *clv;
    QTimer          *checkTimer;
    QString         m_lastBarcode;
    UINT32          m_waitAfterReadDone;
    UINT32          m_delayProcessImage;
    QDateTime       m_lastReadTime;
    QString         m_comPort;
    int             m_comSpeed;
};

#endif // BARCODEVIEW_H
