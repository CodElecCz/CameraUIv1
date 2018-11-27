#ifndef CAMERAVIEWSINGLE_H
#define CAMERAVIEWSINGLE_H

#include <QWidget>
#include <QGraphicsView>
#include <QButtonGroup>
#include <QLabel>

namespace Ui {
class CameraViewSingle;
}

class CameraViewSingle : public QWidget
{
    Q_OBJECT

public:
    explicit CameraViewSingle(QWidget *parent = nullptr, QString name = "");
    ~CameraViewSingle();

public:
    void setViewName(QString name);
    void updateImage(QImage image);
    void updateImage(QImage *image);

private slots:
    void buttonClicked(int id);

private:
    Ui::CameraViewSingle    *ui;
    QGraphicsView           *view;
    QGraphicsScene          *scene;
    QButtonGroup            *toolbox;
    QLabel                  *cameraName;
    QLabel                  *cameraInfo;
    bool                    initFitInView;
};

#endif // CAMERAVIEWSINGLE_H
