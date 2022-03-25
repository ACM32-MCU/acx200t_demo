#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <QDebug>

#include <QtMath>
#include <QDialog>
#include <QPainter>
#include <QPaintEvent>
#include <QPainterPath>
#include <QRadialGradient>
#include <QTimer>
#include <QObject>
#include <QLabel>

class Dashboard : public QWidget
{
    Q_OBJECT

public:
    Dashboard(QWidget *parent = 0);
    ~Dashboard();

    int minValue;
    int maxValue;
    int value;                //current value
    int endvalue;             //to stop value
    int radius;               //outer radius
    qreal   centerR;          //center circle radius

    QList<QColor>   GradientSlideColor;
    QString title;
    QTimer   timer;
    int  timer_msec;
    int  timer_value;

    void setTimerType(int msec,int v);

protected:
    void paintEvent(QPaintEvent*);

protected slots:
    void ontimeout();

private slots:
    void on_exitButton_clicked();

private:
    void DrawPoint(QPainter&,int);
    void DrawDigital(QPainter&,int);
    void DrawCircle(QPainter&,int);
    void DrawSmallScale(QPainter&,int);
    void DrawBigScale(QPainter&,int);
    void DrawText(QPainter&,int);
    void DrawPointer(QPainter&,int);

    void DrawPointer(QPainter& painter,int radius, int value);

    void drawslideScaleGradientColor(QPainter &paint);
    void drawScaleColor(QPainter &paint);
    void drawbkColor(QPainter &paint);

public:
    void setTitle(QString title);
    void setRange(int min, int max);
    bool setEndValue(int v);
    bool setValues(int cV,int minV,int maxV,int endV);
    void setGradientColorMode(QList<QColor>& Qcolors);

signals:
    void arriveEnd(int endvalue);
    void currentValueChange(int CurrentValue);
    void arriveEnd();
    void exit_button_clicked();

public:
    void sendEnd()
    {
        emit arriveEnd(this->endvalue);
        emit arriveEnd();
    }

    void sendChangeValue()
    {
        emit currentValueChange(this->value);
    }
};
#endif
