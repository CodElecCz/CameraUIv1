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
    void updateImage(const QImage& image);
    void updateImage(QImage *image);
    bool saveImage(QString filePath, QString imageFormat = "PNG", int imageQuality = -1);

    //store last snap error
    void setError(QString msg);
    QString getError() { return error; }
    void clearError() { error.clear(); }
    bool isError() { return (error.length()>0); }

    void setPath(QString path) { filePath = path; }
    QString getPath() { return filePath; }

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
    QString                 error;
    QString                 filePath;

    QPixmap                 pixMap;
    QImage                  qImage;
};

#endif // CAMERAVIEWSINGLE_H
