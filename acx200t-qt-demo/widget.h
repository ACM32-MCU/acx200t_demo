#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QString>
#include <QTimer>
#include "sewidget.h"
#include "imagebutton.h"

class Widget : public QWidget
{
    Q_OBJECT
public:
    Widget(QWidget *parent = 0);

    enum IMAGE_TYPE
    {
        CORPORATION_IMAGE = 1,
        CHIP_IMAGE,
        SOLUTION_IMAGE
    };

    SeWidget seWidget;

    int imgType;
    volatile int indexImage;
    int totalCorpImage;
    int totalChipImage;
    int totalSolutionImage;

    //image update interval 5s
    static const int ImageUpdateInterval = 5000;

    //idle status 60s to home page
    static const int IdleTimeout = 60000;

    QString imgCorpPath[16];
    QString imgChipPath[4];
    QString imgSolutionPath[8];

    QLabel label;
    imageButtion imgButton;

    QTimer *timerUpdateImage;
    QTimer *timerIdle;

signals:

public slots:
    void pressedButton(int index);
    void updateImage();
    void lastButtonClicked();
    void nextButtonClicked();
    void idleProcess();
    void jumpToSeWidget();
    void returnToWidget();
};

#endif // WIDGET_H
