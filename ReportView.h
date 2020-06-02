#ifndef REPORTVIEW_H
#define REPORTVIEW_H

#include <QWidget>
#include <QCalendarWidget>
#include <QListWidget>
#include <QSqlDatabase>

namespace Ui {
class ReportView;
}

class ReportView : public QWidget
{
    Q_OBJECT

public:
    explicit ReportView(bool errorQueryToFile = true, QWidget *parent = nullptr);
    ~ReportView();

    bool storeResult(QString date, QString cam, QString barcode, QString image, QString error = "");
    void setDbs(QSqlDatabase *database) { db = database; }

private:
    void listChanged(QString label);

private slots:
    //on_{ObjectName}_{SignalName}
    void on_calendar_clicked(QDate date);
    void on_list_itemClicked(QListWidgetItem* item);
    void on_searchButton_clicked();
    void checkTimer_timeout();

signals:
    void addTrace(QString text, QColor color = QColor("black"));
    void readImageFile(QString camera, QString imageFile, QString error = "");

private:
    Ui::ReportView  *ui;
    QSqlDatabase    *db;
    QTimer          *checkTimer;
    QDateTime       m_lastReadTime;
    bool            m_errorQueryToFile;
};

#endif // REPORTVIEW_H
