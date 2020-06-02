#ifndef STORAGEVIEW_H
#define STORAGEVIEW_H

#include <QWidget>
#include <QTimer>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QMap>
#include <QLabel>

QT_CHARTS_USE_NAMESPACE

namespace Ui {
class StorageView;
}

enum ESTORAGE_Info
{
    ESTORAGE_Info_Name = 0,
    ESTORAGE_Info_System,
    ESTORAGE_Info_Total,
    ESTORAGE_Info_Free,
    ESTORAGE_Info_Mem_Total,
    ESTORAGE_Info_Mem_Free,
    //...
    ESTORAGE_Info_Size
};

class StorageView : public QWidget
{
    Q_OBJECT

public:
    explicit StorageView(QString path = "c:/", int storageMinLimit = 10, int storageMinData = 5, QWidget *parent = nullptr);
    ~StorageView();

private:
    void updateGraphData();
    void memoryUsage();
    QString getOldestPath(QString path);

private slots:
    //on_{ObjectName}_{SignalName}
    void timer_timeout();

private:
    Ui::StorageView *ui;
    QTimer          *timer;
    QChartView      *chartView;
    QPieSeries      *series;
    QString         path;
    int             limit;
    int             limitData;
    QMap<ESTORAGE_Info, QLabel*> infoLabels;
};

#endif // STORAGEVIEW_H
