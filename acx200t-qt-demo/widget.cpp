#include <QVBoxLayout>
#include <QScreen>
#include <QLabel>
#include <QRect>
#include <QGuiApplication>
#include <QPushButton>

#include "imagebutton.h"
#include "sewidget.h"
#include "widget.h"


Widget::Widget(QWidget *parent) : QWidget(parent),imgButton(this),label(this)
{
    QString qss = "color:#BECEC6;background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 #1B3152,stop:1 #255C80);font:16px;";
    this->setStyleSheet(qss);

    QScreen *screen = QGuiApplication::primaryScreen ();
    QRect mm = screen->availableGeometry() ;
    int screen_width = mm.width();
    int screen_height = mm.height();

    label.setMaximumHeight(screen_height - 145);
    label.setMaximumWidth(screen_width);

    QLabel *labelLast = new QLabel(this);
    labelLast->setFixedHeight(50);
    labelLast->setFixedWidth(50);
    labelLast->setGeometry(0, 150, 50, 50);

    QPixmap pixmapLast(":/image/last.png");
    pixmapLast.scaled(labelLast->size(), Qt::IgnoreAspectRatio);
    labelLast->setScaledContents(true);
    labelLast->setPixmap(pixmapLast);
    labelLast->setStyleSheet("background-color: rgb(0,0,0,0);\nborder:none");

    QPushButton *lastButton = new QPushButton(this);
    lastButton->setFixedHeight(50);
    lastButton->setFixedWidth(50);
    lastButton->setFocusPolicy(Qt::NoFocus);
    lastButton->setStyleSheet("background-color: rgb(0,0,0,0);\nborder:none");
    lastButton->setGeometry(0, 150, 50, 50);

    connect(lastButton, SIGNAL(clicked()),this,SLOT(lastButtonClicked()));

    QLabel *labelNext = new QLabel(this);
    labelNext->setFixedHeight(50);
    labelNext->setFixedWidth(50);
    labelNext->setGeometry(800-50, 150, 50, 50);

    QPixmap pixmapNext(":/image/next.png");
    pixmapNext.scaled(labelNext->size(), Qt::IgnoreAspectRatio);
    labelNext->setScaledContents(true);
    labelNext->setPixmap(pixmapNext);
    labelNext->setStyleSheet("background-color: rgb(0,0,0,0);\nborder:none");

    QPushButton *nextButton = new QPushButton(this);
    nextButton->setFixedHeight(50);
    nextButton->setFixedWidth(50);
    nextButton->setFocusPolicy(Qt::NoFocus);
    nextButton->setStyleSheet("background-color: rgb(0,0,0,0);\nborder:none");
    nextButton->setGeometry(800-50, 150, 50, 50);

    connect(nextButton, SIGNAL(clicked()),this,SLOT(nextButtonClicked()));

    imgType = CORPORATION_IMAGE;
    indexImage = 0;

    totalCorpImage = 0;
    imgCorpPath[totalCorpImage++] = ":/image/corp-1.png";
    imgCorpPath[totalCorpImage++] = ":/image/honor.png";
    imgCorpPath[totalCorpImage++] = ":/image/corp-grow-1.png";
    imgCorpPath[totalCorpImage++] = ":/image/corp-grow-2.png";
    imgCorpPath[totalCorpImage++] = ":/image/corp-grow-3.png";
    imgCorpPath[totalCorpImage++] = ":/image/corp-grow-4.png";
    imgCorpPath[totalCorpImage++] = ":/image/corp-grow-5.png";
    imgCorpPath[totalCorpImage++] = ":/image/corp-grow-6.png";
    imgCorpPath[totalCorpImage++] = ":/image/corp-grow-7.png";

    totalChipImage = 0;
    imgChipPath[totalChipImage++] = ":/image/acx200t-chip-1.png";
    imgChipPath[totalChipImage++] = ":/image/acx200t-chip-2.png";
    imgChipPath[totalChipImage++] = ":/image/acx200t-chip-3.png";

    totalSolutionImage = 0;
    imgSolutionPath[totalSolutionImage++] = ":/image/acx200t-solution-1.png";
    imgSolutionPath[totalSolutionImage++] = ":/image/acx200t-solution-2.png";
    imgSolutionPath[totalSolutionImage++] = ":/image/acx200t-solution-3.png";
    imgSolutionPath[totalSolutionImage++] = ":/image/acx200t-solution-4.png";
    imgSolutionPath[totalSolutionImage++] = ":/image/acx200t-solution-5.png";

    QPixmap pixmap(imgCorpPath[indexImage]);

    pixmap.scaled(label.size(), Qt::IgnoreAspectRatio);
    label.setScaledContents(true);
    label.setPixmap(pixmap);

    QVBoxLayout *layout = new QVBoxLayout();

    layout->addWidget(&label);
    layout->addWidget(&imgButton);
    layout->setMargin(0);
    this->setLayout(layout);
    update();

    connect(&imgButton, &imageButtion::pressedButton, this, &Widget::pressedButton);

    timerUpdateImage = new QTimer;
    connect(timerUpdateImage, &QTimer::timeout, this, &Widget::updateImage);
    timerUpdateImage->start(ImageUpdateInterval);

    timerIdle = new QTimer;
    connect(timerIdle, &QTimer::timeout, this, &Widget::idleProcess);
    timerIdle->start(IdleTimeout);

    connect(&seWidget, &SeWidget::exitWidget, this, &Widget::returnToWidget);
}

void Widget::pressedButton(int index)
{
    timerIdle->stop();
    timerIdle->start(IdleTimeout);

    indexImage = 0;
    timerUpdateImage->stop();
    if(index == CORPORATION_IMAGE)
    {
        imgType = CORPORATION_IMAGE;
        timerUpdateImage->start(ImageUpdateInterval);
        updateImage();
    }
    else if(index == CHIP_IMAGE)
    {
        imgType = CHIP_IMAGE;
        timerUpdateImage->start(ImageUpdateInterval);
        updateImage();
    }
    else if(index == SOLUTION_IMAGE)
    {
        imgType = SOLUTION_IMAGE;
        timerUpdateImage->start(ImageUpdateInterval);
        updateImage();
    }
    else
    {
        QTimer *timer = new QTimer;
        timer->singleShot(500, this, SLOT(jumpToSeWidget()));
    }
}

void Widget::updateImage()
{
    QString path;

    if(imgType == CORPORATION_IMAGE)
    {
        if(indexImage >= totalCorpImage)
        {
            indexImage = 0;
        }
        path = imgCorpPath[indexImage];
    }
    else if(imgType == SOLUTION_IMAGE)
    {
        if(indexImage >= totalSolutionImage)
        {
            indexImage = 0;
        }
        path = imgSolutionPath[indexImage];
    }
    else
    {
        if(indexImage >= totalChipImage)
        {
            indexImage = 0;
        }
        path = imgChipPath[indexImage];
    }

    QPixmap pixmap(path);
    pixmap.scaled(label.size(), Qt::IgnoreAspectRatio);
    label.setPixmap(pixmap);
    indexImage++;
    update();
}

void Widget::lastButtonClicked()
{
    timerIdle->stop();
    timerUpdateImage->stop();
    if(imgType == CORPORATION_IMAGE)
    {
        if(indexImage > 1)
        {
            indexImage = indexImage - 2;
        }
        else
        {
            indexImage = totalCorpImage - 1;
        }
    }
    else if(imgType == SOLUTION_IMAGE)
    {
        if(indexImage > 1)
        {
            indexImage = indexImage - 2;
        }
        else
        {
            indexImage = totalSolutionImage - 1;
        }
    }
    else
    {
        if(indexImage > 1)
        {
            indexImage = indexImage - 2;
        }
        else
        {
            indexImage = totalChipImage - 1;
        }
    }

    updateImage();
    timerUpdateImage->start(ImageUpdateInterval);
    timerIdle->start(IdleTimeout);
}

void Widget::nextButtonClicked()
{
    timerIdle->stop();
    timerUpdateImage->stop();

    updateImage();
    timerUpdateImage->start(ImageUpdateInterval);
    timerIdle->start(IdleTimeout);
}

void Widget::idleProcess()
{
    if(imgType != CORPORATION_IMAGE)
    {
        timerUpdateImage->stop();
        indexImage = 0;
        imgType = CORPORATION_IMAGE;
        timerUpdateImage->start();
    }
}

void Widget::jumpToSeWidget()
{
    timerUpdateImage->stop();
    timerIdle->stop();

    this->hide();
    seWidget.show();
}

void Widget::returnToWidget()
{
    indexImage = 0;
    imgType = CORPORATION_IMAGE;
    timerUpdateImage->start(ImageUpdateInterval);
    timerIdle->start(IdleTimeout);

    seWidget.hide();
    this->show();
    updateImage();
}
