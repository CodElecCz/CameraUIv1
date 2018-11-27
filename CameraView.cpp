
#include "CameraView.h"
#include "ui_cameraview.h"

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
        controlsLayout1->addWidget(new CameraViewSingle(this, name), 1);
        cntLayout++;
        break;
    case 2:
        view = new CameraViewSingle(this, name);
        views.insert(name, view);
        controlsLayout2->addWidget(new CameraViewSingle(this, name), 1);
        cntLayout++;
        break;
    case 3:
        view = new CameraViewSingle(this, name);
        views.insert(name, view);
        controlsLayout2->addSpacing(1);
        controlsLayout2->addWidget(new CameraViewSingle(this, name), 1);
        cntLayout++;
        break;
    default:
        break;
    }
}

void CameraView::removeView(QString name)
{
    QList<QString> keys =  views.keys();
    int index = 0;
    CameraViewSingle* view = nullptr;

    foreach(QString key, keys)
    {
        if(0==QString::compare(key, name))
        {
            view = views[name];
            break;
        }
        else
           index++;
    }

    switch(index)
    {
    case 0:
        views.remove(name);
        if(view!=nullptr)
        {
        controlsLayout1->removeWidget(view);
        delete view;
        cntLayout--;
        }
        break;
    case 1:
        views.remove(name);
        if(view!=nullptr)
        {
        controlsLayout1->removeWidget(view);
        delete view;
        cntLayout--;
        }
        break;
    case 2:
        views.remove(name);
        if(view!=nullptr)
        {
        controlsLayout2->removeWidget(view);
        delete view;
        cntLayout--;
        }
        break;
    case 3:
        views.remove(name);
        if(view!=nullptr)
        {
        controlsLayout2->removeWidget(view);
        delete view;
        cntLayout--;
        }
        break;
    default:
        break;
    }

    //TODO: if view is in middle, resort/shift others view
}

void CameraView::updateImage(QString name, QImage image)
{
    CameraViewSingle* view = views[name];

    if(view!=nullptr)
    {
        view->updateImage(image);
    }
}

void CameraView::updateImage(QString name, QImage* image)
{
    CameraViewSingle* view = views[name];

    if(view!=nullptr)
    {
        view->updateImage(image);
    }
}

