#include <QHBoxLayout>
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QFile>
#include <QTimer>
#include "ReportView.h"
#include "ui_ReportView.h"

ReportView::ReportView(bool errorQueryToFile, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ReportView),
    db(nullptr)
{
    ui->setupUi(this);
    m_errorQueryToFile = errorQueryToFile;

    //reinit timer
    checkTimer = new QTimer(this);
    m_lastReadTime = QDateTime::currentDateTime();
    connect(checkTimer, SIGNAL(timeout()), this, SLOT(checkTimer_timeout()));
    checkTimer->start(60*1000); //each minute check
}

ReportView::~ReportView()
{
    delete ui;
}

void ReportView::checkTimer_timeout()
{

    QDateTime dt = QDateTime::currentDateTime();

    if((dt.toSecsSinceEpoch()-m_lastReadTime.toSecsSinceEpoch())>60*60)
    {
        QSqlQuery query;
        QString squery;

        squery = "SELECT NOW()";
        bool ok = query.exec(squery);
        if(!ok)
        {

        }
        else
        {
            m_lastReadTime = QDateTime::currentDateTime();
        }
    }
}

bool ReportView::storeResult(QString date, QString cam, QString barcode, QString image, QString error)
{
    QSqlQuery query;
    QString squery;
    bool lostQuery = false;

    if(error.length()>0)
    {
        if(error.length()>120)
            error.resize(120);

        squery = "INSERT INTO hyundai.catalog (date, barcode, image, camera, error) VALUES ('" + date + "', '" + barcode + "', '" + image + "', '" + cam + "', '" + error + "')";
    }
    else
        squery = "INSERT INTO hyundai.catalog (date, barcode, image, camera) VALUES ('" + date + "', '" + barcode + "', '" + image + "', '" + cam + "')";

    bool ok = query.exec(squery);
    if(!ok)
    {
        lostQuery = true;
        QString msg = "query.exec() Error: " + query.lastError().text();
        //int errNb = query.lastError().number();

        //try to reconnect
        if(db!=nullptr)
        {
            //bool isOpen = db->isOpen();
            //bool isValid = db->isValid();

            /*
            query.exec() Error: [MySQL][ODBC 5.3(w) Driver][mysqld-5.7.21-log]Lost connection to MySQL server during query QODBC3: Unable to execute statement
            query.exec() Error: [MySQL][ODBC 5.3(w) Driver][mysqld-5.7.21-log]MySQL server has gone away QODBC3: Unable to execute statement

            MySQL server has gone away - query exceeds the wait_timeout = 28800 (8h)
            */

            if(msg.contains("Lost connection to MySQL server", Qt::CaseInsensitive)
               || msg.contains("MySQL server has gone away", Qt::CaseInsensitive))
            {
                bool status = db->open();
                qDebug() << QString("Database reconnect: ") + ((status)? "success":"failed");

                if(status)
                {
                    ok = query.exec(squery);
                    if(ok)
                        lostQuery = false;
                }
                else
                {
                    QString msg2 = "db->open() Error: " + db->lastError().text();
                    emit addTrace(msg2);
                }
            }
        }

        //lost query
        if(lostQuery)
        {
            emit addTrace(squery);
            emit addTrace(msg);
            //qDebug() << msg;
        }

        //lost query log to file
        if(lostQuery && m_errorQueryToFile)
        {
            QString filePathLog(QCoreApplication::applicationDirPath() + "/" + "query.txt");
            QFile log(filePathLog);
            if(log.open(QIODevice::WriteOnly | QIODevice::Append))
            {
                QTextStream txt(&log);
                txt << squery;
                if(!squery.contains("\n"))
                    txt << "\r\n";

                log.close();
            }
        }
    }
    else
    {
        m_lastReadTime = QDateTime::currentDateTime();

        ; //emit statusDatabase();

        //QString scal = calendar->selectedDate().toString("yyyy-MM-dd");
        //int i = QString::compare(scal, date);
    }

    return ok;
}

void ReportView::on_calendar_clicked(QDate date)
{
    QSqlQuery query;
    QString sdate = date.toString("yyyy-MM-dd");
    QString squery = "SELECT * FROM hyundai.catalog WHERE date>='" + sdate + " 00:00:00' AND date<='" + sdate + " 23:59:59' GROUP BY date ORDER BY date DESC";

    bool ok = query.exec(squery);
    if(!ok)
    {
        QString msg = "query.exec() Error: " + query.lastError().text();

        emit addTrace(squery);
        emit addTrace(msg);

        //qDebug() << msg;
    }
    else
        ; //emit statusDatabase();

    ui->list->clear();
    ui->list->setFont(QFont("MS Shell Dlg 2", 10));
    while (query.next())
    {
        QString sdate = query.value(1).toString();
        QString sbarcode = query.value(2).toString();

        QListWidgetItem *item = new QListWidgetItem(sbarcode);

        sdate.remove(".000");
        item->setToolTip(sdate);

        ui->list->addItem(item);

        ui->list->setAccessibleDescription(sdate);
    }

    if(ui->list->count()>0)
    {
        QListWidgetItem *item = ui->list->item(0);
        ui->list->setCurrentItem(item);
        on_list_itemClicked(item);
    }
}

void ReportView::on_list_itemClicked(QListWidgetItem* item)
{
    QSqlQuery query;
    QString sbarcode = item->text();
    QString sdate = item->toolTip();
    QString squery = "SELECT * FROM hyundai.catalog WHERE barcode='" + sbarcode + "' AND date='" + sdate + "' ORDER BY date";

    bool ok = query.exec(squery);
    if(!ok)
    {
        QString msg = "query.exec() Error: " + query.lastError().text();

        emit addTrace(squery);
        emit addTrace(msg);

        //qDebug() << msg;
    }
    else
        ; //emit statusDatabase();

    QString simage;
    QString scam;
    QString serror;
    QString imageName("");
    while (query.next())
    {
        simage = query.value(3).toString();
        scam = query.value(4).toString();
        serror = query.value(5).toString();

        sdate.replace(":", ".");

        if(simage.length()>0)
            imageName = sdate + "_" + sbarcode + "_" + simage;
        else
            imageName = "";

        emit readImageFile(scam, imageName);
    }
}

void ReportView::on_searchButton_clicked()
{
    QSqlQuery query;
    QString sbarcode = ui->searchText->text();
    QString squery = "SELECT * FROM hyundai.catalog WHERE barcode LIKE '" + sbarcode + "' GROUP BY date ORDER BY date DESC LIMIT 100";

    bool ok = query.exec(squery);
    if(!ok)
    {
        QString msg = "query.exec() Error: " + query.lastError().text();

        emit addTrace(squery);
        emit addTrace(msg);

        //qDebug() << msg;
    }
    else
        ; //emit statusDatabase();

    ui->list->clear();
    ui->list->setFont(QFont("MS Shell Dlg 2", 10));
    while (query.next())
    {
        QString sdate = query.value(1).toString();
        QString sbarcode = query.value(2).toString();

        QListWidgetItem *item = new QListWidgetItem(sbarcode);

        sdate.remove(".000");
        item->setToolTip(sdate);

        ui->list->addItem(item);
        ui->list->setAccessibleDescription(sdate);
    }

    if(ui->list->count()>0)
    {
        QListWidgetItem *item = ui->list->item(0);
        ui->list->setCurrentItem(item);
        on_list_itemClicked(item);
    }
}
