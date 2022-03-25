#include "imagebutton.h"
#include "ui_imagebutton.h"

imageButtion::imageButtion(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::imageButtion)
{
    ui->setupUi(this);

    QString qss = "color:#BECEC6;background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 #1B3152,stop:1 #255C80);font:16px;";
    this->setStyleSheet(qss);

    setFocusPolicy(Qt::NoFocus);

    ui->widget1->setText("航芯公司");
    ui->widget1->setImage(":/image/ico-home.png");

    ui->widget2->setText("V2X芯片");
    ui->widget2->setImage(":/image/ico-chip.png");

    ui->widget3->setText("V2X方案");
    ui->widget3->setImage(":/image/ico-solution.png");

    ui->widget4->setText("签名验签演示");
    ui->widget4->setImage(":/image/ico-demo.png");

    connect(ui->widget1, &imagePressButton::clicked, this, &imageButtion::pressedButton1);
    connect(ui->widget2, &imagePressButton::clicked, this, &imageButtion::pressedButton2);
    connect(ui->widget3, &imagePressButton::clicked, this, &imageButtion::pressedButton3);
    connect(ui->widget4, &imagePressButton::clicked, this, &imageButtion::pressedButton4);
}

imageButtion::~imageButtion()
{
    delete ui;
}

void imageButtion::pressedButton1()
{
    emit pressedButton(1);
}

void imageButtion::pressedButton2()
{
    emit pressedButton(2);
}

void imageButtion::pressedButton3()
{
    emit pressedButton(3);
}

void imageButtion::pressedButton4()
{
    emit pressedButton(4);
}
