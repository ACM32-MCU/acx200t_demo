#include "imagepressbutton.h"

#include <QPainter>
#include <QVariant>

imagePressButton::imagePressButton(QWidget *parent) : QWidget(parent)
{
    pushButton = new QPushButton(this);
    isButtonInited = false;

    pixWidth = 0;
    pixHeight = 0;
    oldWidth = 0;
    oldHeight = 0;

    enterAnimation = new QPropertyAnimation(this, "");
    enterAnimation->setStartValue(0);
    enterAnimation->setEndValue(5);
    enterAnimation->setDuration(200);
    connect(enterAnimation, SIGNAL(valueChanged(QVariant)), this, SLOT(enterImageChanged(QVariant)));

    leaveAnimation = new QPropertyAnimation(this, "");
    leaveAnimation->setStartValue(0);
    leaveAnimation->setEndValue(5);
    leaveAnimation->setDuration(200);
    connect(leaveAnimation, SIGNAL(valueChanged(QVariant)), this, SLOT(leaveImageChanged(QVariant)));

    setFocusPolicy(Qt::NoFocus);

    pushButton->setFocusPolicy(Qt::NoFocus);
    pushButton->setStyleSheet("background-color: rgb(0,0,0,0);\nborder:none");

    connect(pushButton,  &QPushButton::clicked, this, &imagePressButton::on_pushButton_clicked);

    timer = new QTimer;
    connect(timer, &QTimer::timeout, this, &imagePressButton::timeout);

}

void imagePressButton::paintEvent(QPaintEvent *)
{
    if (image.isEmpty()) {
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPixmap pix(image);
    pix = pix.scaled(targetWidth, targetHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    int pixX = rect().center().x() - targetWidth / 2;
    int pixY = rect().center().y() - targetHeight / 2 - 10;


    if(isButtonInited == false)
    {
        isButtonInited = true;
        pushButton->setGeometry(pixX, pixY, targetWidth, targetHeight);
    }

    QPoint point(pixX, pixY);
    painter.drawPixmap(point, pix);
    painter.drawText(QRectF(0, height() - 20, width(), 20), Qt::AlignCenter, text);
}

void imagePressButton::on_pushButton_clicked()
{
    pixWidth = pixWidth - 25;
    pixHeight = pixHeight - 25;
    enterAnimation->start();
    emit clicked();
}

void imagePressButton::enterImageChanged(QVariant index)
{
    int i = index.toInt();
    targetWidth = pixWidth + i * 3;
    targetHeight = pixHeight + i * 3;
    update();

    if(i == 5)
    {
        timer->start(10);
    }
}

void imagePressButton::leaveImageChanged(QVariant index)
{
    int i = index.toInt();
    targetWidth = pixWidth - i * 3;
    targetHeight = pixWidth - i * 3;
    update();
}

void imagePressButton::timeout()
{
    timer->stop();
    pixWidth = oldWidth;
    pixHeight = oldHeight;
    leaveAnimation->start();
}

void imagePressButton::setText(QString text)
{
    this->text = text;
    update();
}

void imagePressButton::setImage(QString image)
{
    this->image = image;
    QPixmap pix(image);

    pixWidth = pix.width();
    pixHeight = pix.height();
    oldWidth = pixWidth;
    oldHeight = pixHeight;
    targetWidth = pixWidth - 15;
    targetHeight = pixHeight - 15;

    update();
}
