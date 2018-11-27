#ifndef LOADVIEW_H
#define LOADVIEW_H

#include <QWidget>

namespace Ui {
class LoadView;
}

class LoadView : public QWidget
{
    Q_OBJECT

public:
    explicit LoadView(QWidget *parent = nullptr);
    ~LoadView();

    void setText(QString msg);
    void setTitle(QString msg);
    void setSubTitle(QString msg);

private:
    Ui::LoadView         *ui;
};

#endif // LOADVIEW_H
