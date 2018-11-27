#include <QHBoxLayout>
#include <QPushButton>
#include <time.h>
#include "CameraViewSingle.h"
#include "ui_cameraviewsingle.h"

CameraViewSingle::CameraViewSingle(QWidget *parent, QString name) :
    QWidget(parent),
    ui(new Ui::CameraViewSingle),
    view(new QGraphicsView(this)),
    scene(new QGraphicsScene(this)),
    toolbox(new QButtonGroup(this)),
    cameraName(new QLabel(this)),
    cameraInfo(new QLabel(this)),
    initFitInView(false)
{
    ui->setupUi(this);

    QVBoxLayout *centralLayout = new QVBoxLayout(this);
    centralLayout->setSpacing(0);
    centralLayout->setMargin(0);
    //centralLayout->addSpacing(17); //label on top
    centralLayout->addWidget(view, 1);

    QHBoxLayout *toolboxLayout = new QHBoxLayout;
    toolboxLayout->setSpacing(0);
    toolboxLayout->setMargin(0);

    QPushButton *buttonPlus = new QPushButton(QIcon("://icons/zoom_in.png"), QString(""), this);

    buttonPlus->setMaximumWidth(32);
    toolboxLayout->addWidget(buttonPlus);

    QPushButton *buttonMinus = new QPushButton(QIcon("://icons/zoom_out.png"), QString(""), this);
    buttonMinus->setMaximumWidth(32);
    toolboxLayout->addWidget(buttonMinus);

    QPushButton *buttonFit = new QPushButton(QIcon("://icons/zoom_fit.png"), QString(""), this);
    buttonFit->setMaximumWidth(32);
    toolboxLayout->addWidget(buttonFit);

    toolboxLayout->addStretch();
    toolboxLayout->addWidget(cameraName);
    toolboxLayout->addWidget(cameraInfo);

    centralLayout->addLayout(toolboxLayout);

    toolbox->addButton(buttonPlus, 0);
    toolbox->addButton(buttonMinus, 1);
    toolbox->addButton(buttonFit, 2);
    connect(toolbox, SIGNAL(buttonClicked(int)), this, SLOT(buttonClicked(int)));

    view->setScene(scene);
    setViewName(name);
}

CameraViewSingle::~CameraViewSingle()
{
    delete ui;
}

void CameraViewSingle::setViewName(QString name)
{
    cameraName->setText(name);
}

void CameraViewSingle::updateImage(QImage image)
{
    scene->clear();
    scene->addPixmap(QPixmap::fromImage(image));

    //ROIs
    //scene->addRect(20,20,100,100, QPen(QColor("green")), QBrush());

    view->update();

    //first image adjustment
    if(!initFitInView)
    {
        initFitInView = true;
        view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
    }
}

void CameraViewSingle::updateImage(QImage *image)
{
    static clock_t time = 0;

    if(image!=nullptr)
    {
        scene->clear();
        scene->addPixmap(QPixmap::fromImage(*image));

        //ROIs
        //scene->addRect(20,20,100,100, QPen(QColor("green")), QBrush());

        view->update();
        delete image;

        //first image adjustment
        if(!initFitInView)
        {
            initFitInView = true;
            view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
        }

        if(time>0)
        {
            double dt = (double)(clock() - time)/1000.0;
            if(dt<1.0)
            {
                QString stime = QString("; Time: %1 [s]; Framerate: %2").arg(QString::number(dt, 'f', 3), QString::number(1.0/dt, 'f', 0));
                cameraInfo->setText(stime);
            }
        }
        time = clock();
    }
}

void CameraViewSingle::buttonClicked(int id)
{
    switch(id)
    {
    case 0: //plus
        view->scale(2.0, 2.0);
        break;
    case 1: //minus
        view->scale(0.5, 0.5);
        break;
    case 2: //minus
        view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
        break;
    default:
        break;
    }
}
