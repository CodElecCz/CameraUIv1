#include <QtCharts/QPieSlice>
#include <QStorageInfo>
#include <QHBoxLayout>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include "StorageView.h"
#include "ui_StorageView.h"


StorageView::StorageView(QString path, int storageMinLimit, int storageMinData, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StorageView),
    timer(new QTimer(this)),
    chartView(new QChartView(this)),
    series(new QPieSeries(this)),
    path(path),
    limit(storageMinLimit),
    limitData(storageMinData)
{
    ui->setupUi(this);

    updateGraphData();

    QStorageInfo info(path);
    int storageSize = info.bytesTotal()/1024/1024/1024;
    int storageAvailable = (info.bytesAvailable()/1024/1024/1024);
    int storageUsed = storageSize - storageAvailable;

    QChart *chart = new QChart();
    chart->addSeries(series);
    //chart->setTitle(info.name() + ": " + QString::number(storageSize) + " [GB]");
    chart->legend()->hide();
    //chart->legend()->setAlignment(Qt::AlignRight);
    //chart->setGeometry(0,0,200,200);
    chart->setMargins(QMargins(0,0,0,0));

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    //chartView->setMinimumWidth(300);
    //chartView->setMaximumWidth(300);
    //chartView->setMinimumHeight(250);
    //chartView->chart()->setTheme(QChart::ChartThemeLight);
    chartView->setBackgroundBrush(QBrush(Qt::white));

    QHBoxLayout *layout1 = new QHBoxLayout;
    layout1->setMargin(0);
    layout1->addWidget(chartView);

    QVBoxLayout *central = new QVBoxLayout(this);
    central->addLayout(layout1, 1);
    central->setMargin(0);

#define STORAGE_Info_Offset    15
#define STORAGE_Info_Size      13
#define STORAGE_Info_Width     120

    QLabel *text1 = new QLabel("Name: " + info.name() , this);
    text1->setGeometry(QRect(STORAGE_Info_Offset, 10, STORAGE_Info_Width, STORAGE_Info_Size));
    QLabel *text2 = new QLabel("System: " + info.fileSystemType() , this);
    text2->setGeometry(QRect(STORAGE_Info_Offset, 25, STORAGE_Info_Width, STORAGE_Info_Size));
    QLabel *text3 = new QLabel("HDD Free: " + QString::number(storageAvailable) + " [GB]" , this);
    text3->setGeometry(QRect(STORAGE_Info_Offset, 45, STORAGE_Info_Width, STORAGE_Info_Size));
    QLabel *text4 = new QLabel("HDD Size: " + QString::number(storageSize) + " [GB]" , this);
    text4->setGeometry(QRect(STORAGE_Info_Offset, 60, STORAGE_Info_Width, STORAGE_Info_Size));
    QLabel *text5 = new QLabel("Memory Free: -" , this);
    text5->setGeometry(QRect(STORAGE_Info_Offset, 80, STORAGE_Info_Width, STORAGE_Info_Size));
    QLabel *text6 = new QLabel("Memory Size: -" , this);
    text6->setGeometry(QRect(STORAGE_Info_Offset, 95, STORAGE_Info_Width, STORAGE_Info_Size));

    infoLabels.insert(ESTORAGE_Info_Name, text1);
    infoLabels.insert(ESTORAGE_Info_System, text2);
    infoLabels.insert(ESTORAGE_Info_Free, text3);
    infoLabels.insert(ESTORAGE_Info_Total, text4);
    infoLabels.insert(ESTORAGE_Info_Mem_Free, text5);
    infoLabels.insert(ESTORAGE_Info_Mem_Total, text6);

    connect(timer, SIGNAL(timeout()), this, SLOT(timer_timeout()));
    timer->setInterval(5000);
    timer->start();
}

StorageView::~StorageView()
{
    timer->stop();
    delete ui;
}

void StorageView::timer_timeout()
{
    updateGraphData();
    chartView->update();
}

void StorageView::updateGraphData()
{
    QStorageInfo info(path);
    int storageSize = info.bytesTotal()/1024/1024/1024;
    int storageAvailable = (info.bytesAvailable()/1024/1024/1024);
    int storageUsed = storageSize - storageAvailable;

    series->clear();
    series->append("Free: " + QString::number(storageAvailable) + " [GB]" , storageAvailable);
    series->append("Used",  storageUsed);

    QPieSlice *slice1 = series->slices().at(0);
    //slice1->setExploded();
    slice1->setLabelVisible();
    slice1->setPen(QPen(Qt::darkGreen, 2));
    slice1->setBrush(QBrush("light green"));

    QPieSlice *slice2 = series->slices().at(1);
    slice2->setBrush(QBrush("light blue"));

    QDateTime dt = QDateTime::currentDateTime();

    if(infoLabels.size()==ESTORAGE_Info_Size)
    {
        infoLabels[ESTORAGE_Info_Free]->setText("HDD Free: " + QString::number(storageAvailable) + " [GB]");
    }

    static bool limitExc = false;
    double storageUsedPercent = (double)storageAvailable/(double)storageSize * 100.0;
    if(storageUsedPercent < limit && !limitExc)
    {
        //red signalization
        chartView->setBackgroundBrush(QBrush(Qt::red));
        limitExc = true;
    }
    else if (storageUsedPercent > limit && limitExc)
    {
        chartView->setBackgroundBrush(QBrush(Qt::white));
        limitExc = false;
    }

    if(storageUsedPercent < limitData && limitData!=0)
    {
        //delete oldest date in  catalog
        QString pathToDel = getOldestPath(path);
        if(pathToDel.length()>0)
        {
            QDir dir(pathToDel);
            dir.removeRecursively();
        }
    }

    memoryUsage();
}

QString StorageView::getOldestPath(QString path)
{
    QString returnPath("");
    QDir dir(path);
    if(!dir.isEmpty())
    {
        QFileInfoList entries = dir.entryInfoList( QDir::NoDotAndDotDot | QDir::Dirs);
        if(entries.size()>0)
        {
            QString syear = entries.at(0).absoluteFilePath();
            QDir yearDir(syear);
            entries = yearDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs);
            if(entries.size()>0)
            {
                QString smounth = entries.at(0).absoluteFilePath();
                QDir mounthDir(smounth);
                entries = mounthDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs);
                if(entries.size()>0)
                {
                    QString sday = entries.at(0).absoluteFilePath();
                    //delete day
                    returnPath = sday;
                }
                else
                {
                    //delete mounth
                    returnPath = smounth;
                }
            }
            else
            {
                //delete year
                returnPath = syear;
            }
        }
    }

    return returnPath;
}

#define _AMD64_
#include <sysinfoapi.h>

void StorageView::memoryUsage()
{
    MEMORYSTATUSEX memory_status;
    ZeroMemory(&memory_status, sizeof(MEMORYSTATUSEX));
    memory_status.dwLength = sizeof(MEMORYSTATUSEX);
    if (GlobalMemoryStatusEx(&memory_status))
    {
        int total = memory_status.ullTotalPhys / (1024 * 1024);
        int avail = memory_status.ullAvailPhys / (1024 * 1024);

        //qDebug() << "Total mem: " + QString::number(total) + " [MB], Avail: " + QString::number(avail) + " [MB]";

        if(infoLabels.size()==ESTORAGE_Info_Size)
        {
            infoLabels[ESTORAGE_Info_Mem_Total]->setText("Memory Size: " + QString::number(total) + " [MB]");
            infoLabels[ESTORAGE_Info_Mem_Free]->setText("Memory Free: " + QString::number(avail) + " [MB]");
        }
    }
}


