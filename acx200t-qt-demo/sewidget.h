#ifndef SEWIDGET_H
#define SEWIDGET_H

#include <QWidget>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QRadioButton>
#include <QButtonGroup>
#include <QThread>

#include "dashboard.h"

class SeWidget : public QWidget
{
    Q_OBJECT

public:
    Dashboard dashboard;
    QPushButton button;

    char *SPI1_DEV = (char *)"/dev/spidev1.0";
    int se_fd;

    enum TEST_TYPE
    {
        SM2_SIGN_TEST = 1,
        SM2_VERIFY_TEST
    };

    int test_type;
    volatile int timeout_1s;
    volatile int stopThread;

    int cliecked_counter;
    QTimer *timer1S;

    SeWidget(QWidget *parent = 0);

    QButtonGroup *m_group;
    QRadioButton *m_btn_sign;
    QRadioButton *m_btn_verify;

signals:
    void exitWidget();

protected slots:
   void on_pushButton_clicked();
   void select_type(int id);

private slots:
   void on_exitButton_clicked();
   void onWarnning(QString msg);

public slots:
    void timer1SUpdate();

};

#endif // SEWIDGET_H
