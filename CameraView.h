#ifndef CAMERAVIEW_H
#define CAMERAVIEW_H

#include <QWidget>
#include <QHBoxLayout>
#include <QMap>
#include "CameraViewSingle.h"

namespace Ui {
class CameraView;
}

class CameraView : public QWidget
{
    Q_OBJECT

public:
    explicit CameraView(QWidget *parent = nullptr);
    ~CameraView();

public:
    void addView(QString name);
    void removeView(QString name);
    void updateImage(QString name, QImage image);

public slots:
    void updateImage(QString name, QImage* image);

private:
    Ui::CameraView  *ui;
    QHBoxLayout     *controlsLayout1;
    QHBoxLayout     *controlsLayout2;
    QVBoxLayout     *centralLayout;
    int             cntLayout;

    QMap<QString, CameraViewSingle*> views;
};

#endif // CAMERAVIEW_H
