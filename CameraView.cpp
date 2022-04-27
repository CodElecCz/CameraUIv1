#include "CameraView.h"
#include "ui_CameraView.h"

CameraView::CameraView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CameraView),
    controlsLayout1(new QHBoxLayout),
    controlsLayout2(new QHBoxLayout),
    centralLayout(new QVBoxLayout(this)),
    cntLayout(0)
{
    ui->setupUi(this);

    controlsLayout1->setSpacing(0);
    controlsLayout1->setMargin(0);

    controlsLayout2->setSpacing(0);
    controlsLayout2->setMargin(0);

    centralLayout->setSpacing(1);
    centralLayout->setMargin(0);
    centralLayout->addLayout(controlsLayout1);
    centralLayout->addLayout(controlsLayout2);
}

CameraView::~CameraView()
{
    delete ui;
}

void CameraView::addView(QString name)
{
    CameraViewSingle* view;

    switch(cntLayout)
    {
    case 0:
        view = new CameraViewSingle(this, name);
        views.insert(name, view);
        controlsLayout1->addWidget(view, 1);
        cntLayout++;
        break;
    case 1:
        view = new CameraViewSingle(this, name);
        views.insert(name, view);
        controlsLayout1->addSpacing(1);
        controlsLayout1->addWidget(view, 1);
        cntLayout++;
        break;
    case 2:
        view = new CameraViewSingle(this, name);
        views.insert(name, view);
        controlsLayout2->addWidget(view, 1);
        cntLayout++;
        break;
    case 3:
        view = new CameraViewSingle(this, name);
        views.insert(name, view);
        controlsLayout2->addSpacing(1);
        controlsLayout2->addWidget(view, 1);
        cntLayout++;
        break;
    default:
        break;
    }
}

void CameraView::removeView()
{
    switch(cntLayout)
    {
    case 4:
        controlsLayout2->removeWidget(views.values().at(3));
        delete views.values().at(3);
    case 3:
        controlsLayout2->removeWidget(views.values().at(2));
        delete views.values().at(2);
    case 2:
        controlsLayout1->removeWidget(views.values().at(1));
        delete views.values().at(1);
    case 1:
        controlsLayout1->removeWidget(views.values().at(0));
        delete views.values().at(0);
        break;
    default:
        break;
    }

    views.clear();
    cntLayout = 0;
}

void CameraView::updateImage(QString name, const QImage& image)
{   
    if(views.contains(name))
    {
        CameraViewSingle* view = views[name];
        if(view)
        {
            view->updateImage(image);

            //clear last error
            view->clearError();
        }
    }
    else
    {        
        addView(name);

        CameraViewSingle* view = views[name];
        if(view)
        {
            view->updateImage(image);
        }
    }
}

void CameraView::updateImage(QString name, QImage* image)
{
    if(views.contains(name))
    {
        CameraViewSingle* view = views.value(name);
        if(view)
        {
            view->updateImage(image);

            //clear last error
            view->clearError();
        }
    }
}

bool CameraView::saveImage(QString name, QString filePath, QString imageFormat, int imageQuality)
{
    bool res = false;
    CameraViewSingle* view = views[name];

    if(view!=nullptr)
    {
        res = view->saveImage(filePath, imageFormat, imageQuality);
    }

    return res;
}

QStringList CameraView::getViewNames()
{
    return views.keys();
}

void CameraView::setError(QString name, QString msg)
{
    CameraViewSingle* view = views[name];

    if(view!=nullptr)
    {
        view->setError(msg);
    }
}
QString CameraView::getError(QString name)
{
    CameraViewSingle* view = views[name];
    QString msg("");

    if(view!=nullptr)
    {
        msg = view->getError();
    }

    return msg;
}

void CameraView::clearError(QString name)
{
    CameraViewSingle* view = views[name];

    if(view!=nullptr)
    {
        view->clearError();
    }
}

bool CameraView::isError(QString name)
{
    CameraViewSingle* view = views[name];
    bool isError = false;

    if(view!=nullptr)
    {
        isError = view->isError();
    }

    return isError;
}

void CameraView::setPath(QString name, QString path)
{
    CameraViewSingle* view = views[name];

    if(view!=nullptr)
    {
        view->setPath(path);
    }
}

QString CameraView::getPath(QString name)
{
    CameraViewSingle* view = views[name];
    QString path("");

    if(view!=nullptr)
    {
        path = view->getPath();
    }

    return path;
}

