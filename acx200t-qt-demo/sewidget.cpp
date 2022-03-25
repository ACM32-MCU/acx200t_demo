#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTime>
#include <QFontDatabase>
#include <QFile>
#include <QMessageBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QFileInfo>
#include <QDockWidget>
#include <QRunnable>
#include <QThreadPool>

#include <unistd.h>
#include<sys/stat.h>
#include<fcntl.h>

#include "sewidget.h"
#include "widget.h"
#include "se_api.h"


class SM2TestTask : public QObject, public QRunnable
{
    Q_OBJECT

private:
    SeWidget *sewidget;


signals:
    void warnning_msg(QString msg);

public:

    SM2TestTask(SeWidget *sewidget)
    {
        this->sewidget = sewidget;
    }

    void reset(void)
    {
        sewidget->stopThread = 1;
        sewidget->button.setMaximumWidth(150);
        sewidget->button.setMaximumHeight(70);
        sewidget->button.setGeometry(sewidget->width()/2, sewidget->height() - 70, 150, 70);
        sewidget->button.setStyleSheet("border-image: url(:/image/start.png);border:none");
        sewidget->m_btn_sign->setVisible(true);
        sewidget->m_btn_verify->setVisible(true);
        if(sewidget->test_type == sewidget->SM2_SIGN_TEST)
        {
            sewidget->dashboard.setTitle(QStringLiteral("SM2签名速度"));
        }
        else
        {
            sewidget->dashboard.setTitle(QStringLiteral("SM2验签速度"));
        }
        sewidget->dashboard.setEndValue(0);
        sewidget->update();
        sewidget->cliecked_counter = 0;
    }

    void run()
    {
        unsigned char prvkey[32];
        unsigned char pubkey[64];
        unsigned char msg[256];
        unsigned char sig[64];
        unsigned char IDa[] =
        {
            0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
            0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
        };

        int test_counter = 0;
        int ret = 0;
        int i;
        QString info;

        QTime t = QTime::currentTime();
        qsrand(t.msec() + t.second()*1000);

        //generate some random datas as a sign or verify message data
        for(i = 0; i < sizeof(msg); i++)
        {
            msg[i] = qrand()%0xFF;
        }

        //generate a sm2 keypair
        ret = se_sm2_keygen(sewidget->se_fd, prvkey, pubkey);
        if(ret != 0)
        {
            qDebug("se_sm2_keygen failed: %d\n", ret);
            emit warnning_msg(QString("SE SM2生成密钥对失败:%1，请重试").arg(ret));
            reset();
            return;
        }

        //genrate the message signature for verify test
        ret = se_sm2_sign(sewidget->se_fd, IDa, sizeof(IDa), prvkey, pubkey, msg, sizeof(msg), sig);
        if(ret != 0)
        {
            qDebug("se_sm2_sign failed: %d\n", ret);
            emit warnning_msg(QString("SE SM2签名失败:%1，请重试").arg(ret));
            reset();
            return;
        }

        sewidget->stopThread = 0;
        while(sewidget->stopThread == 0)
        {
            if(sewidget->test_type == sewidget->SM2_SIGN_TEST)
            {
                //sm2 sign api
                ret = se_sm2_sign(sewidget->se_fd, IDa, sizeof(IDa), prvkey, pubkey, msg, sizeof(msg), sig);
                if(ret != 0)
                {
                    qDebug("se_sm2_sign failed: %d\n", ret);
                    emit warnning_msg(QString("SE SM2签名失败:%1，请重试").arg(ret));
                    reset();
                    break;
                }

                if(sewidget->timeout_1s != 0)
                {
                    info = QString("SM2签名速度: %1次/s").arg(test_counter);
                    sewidget->dashboard.setTitle(info);
                    sewidget->dashboard.setEndValue(test_counter);
                    sewidget->update();
                    test_counter = 0;
                    sewidget->timeout_1s = 0;
                    sewidget->timer1S->singleShot(1000, sewidget, &SeWidget::timer1SUpdate);
                }
            }
            else
            {
                //sm2 verify api
                ret = se_sm2_verify(sewidget->se_fd, IDa, sizeof(IDa), pubkey, msg, sizeof(msg), sig);
                if(ret != 0)
                {
                    qDebug("se_sm2_verify failed: %d\n", ret);
                    emit warnning_msg(QString("SE SM2验签失败:%1，请重试").arg(ret));
                    reset();
                    break;
                }

                if(sewidget->timeout_1s != 0)
                {
                    info = QString("SM2验签速度: %1次/s").arg(test_counter);
                    sewidget->dashboard.setTitle(info);
                    sewidget->dashboard.setEndValue(test_counter);
                    sewidget->update();
                    test_counter = 0;
                    sewidget->timeout_1s = 0;
                    sewidget->timer1S->singleShot(1000, sewidget, &SeWidget::timer1SUpdate);
                }
            }
            test_counter++;
        }
    }
};

//check the gpio status for SPI by libacx200t callback
int spi_1_gpio_status(void)
{
    uint8_t buffer[8];
    static int fd = -1;
    static char *SPI_1_GPIO = (char *)"/sys/class/gpio/gpio101/value";

    if(fd < 0)
    {
        fd = open(SPI_1_GPIO, O_RDONLY);
        if(fd < 0)
        {
            return -1;
        }
    }
    else
    {
        lseek(fd, 0, SEEK_SET);
    }

    read(fd, buffer, 1);

    return (buffer[0] == '0') ? 0 : 1;
}

SeWidget::SeWidget(QWidget *parent) : QWidget(parent), button(this)
{
    QString qss = "color:#BECEC6;background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:0,stop:0 #1B3152,stop:1 #255C80);font:16px;";
    this->setStyleSheet(qss);

    setGeometry(0, 0, 800, 480);

    dashboard.setTitle(QStringLiteral("SM2签名速度"));
    dashboard.setValues(0, 0, 5100, 0);
    dashboard.setGradientColorMode(QList<QColor>()<<QColor(45,0,255)<<QColor(0,233,255)<<QColor(45,255,0)<<QColor(255,243,0)<<QColor(255,0,0));

    dashboard.setMinimumSize(300,300);
    dashboard.setEndValue(0);

    m_btn_sign = new QRadioButton("SM2签名测试", this);
    m_btn_verify = new QRadioButton("SM2验签测试", this);

    m_btn_sign->setFont(QFont("wqy-zenhei", 11, QFont::Normal));
    m_btn_verify->setFont(QFont("wqy-zenhei", 11, QFont::Normal));

    m_group = new QButtonGroup(this);
    m_group->addButton(m_btn_sign, 1);
    m_group->addButton(m_btn_verify, 2);

    connect(m_group, SIGNAL(buttonClicked(int )), this, SLOT(select_type(int )));

    QVBoxLayout *layout = new QVBoxLayout();

    QHBoxLayout *layoutButton = new QHBoxLayout();

    button.setMaximumWidth(150);
    button.setMaximumHeight(70);
    button.setGeometry(this->width()/2, this->height() - 70, 150, 70);
    button.setStyleSheet("border-image: url(:/image/start.png);border:none");
    button.setFocusPolicy(Qt::NoFocus);

    m_btn_sign->setFixedHeight(70);
    m_btn_sign->setFixedWidth(150);
    m_btn_verify->setFixedHeight(70);
    m_btn_verify->setFixedWidth(150);

    m_btn_sign->setFocusPolicy(Qt::NoFocus);
    m_btn_verify->setFocusPolicy(Qt::NoFocus);

    //set sign button as default selected
    m_btn_sign->setChecked(true);

    m_btn_sign->setStyleSheet("color:white");
    m_btn_verify->setStyleSheet("color:white");

    layoutButton->addWidget(m_btn_sign);
    layoutButton->addWidget(&button);
    layoutButton->addWidget(m_btn_verify);

    connect(&dashboard, &Dashboard::exit_button_clicked, this, &SeWidget::on_exitButton_clicked);

    layout->addWidget(&dashboard);
    layout->addLayout(layoutButton);

    this->setLayout(layout);

    connect(&button, SIGNAL(clicked()),this,SLOT(on_pushButton_clicked()));

    cliecked_counter = 0;

    test_type = SM2_SIGN_TEST;

    //Initial SE API
    se_fd = se_api_init(SPI1_DEV, SPI_INF_MODE, spi_1_gpio_status);
    if(se_fd <= 0)
    {
        qDebug("se spi init failed: %d\n", se_fd);
    }

    timer1S = new QTimer;
}

void SeWidget::on_pushButton_clicked()
{
    cliecked_counter++;
    if(cliecked_counter%2)
    {
        button.setMaximumWidth(150);
        button.setMaximumHeight(70);
        button.setGeometry(this->width()/2, this->height() - 70, 150, 70);
        button.setStyleSheet("border-image: url(:/image/stop.png);border:none");
        m_btn_sign->setVisible(false);
        m_btn_verify->setVisible(false);

        stopThread = 0;

        timeout_1s = 0;
        timer1S->singleShot(1000, this, SLOT(timer1SUpdate()));

        SM2TestTask *speedTask = new SM2TestTask(this);
        connect(speedTask, SIGNAL(warnning_msg(QString)), this, SLOT(onWarnning(QString)));
        QThreadPool::globalInstance()->start(speedTask);
    }
    else
    {
        timer1S->stop();

        stopThread = 1;
        button.setMaximumWidth(150);
        button.setMaximumHeight(70);
        button.setGeometry(this->width()/2, this->height() - 70, 150, 70);
        button.setStyleSheet("border-image: url(:/image/start.png);border:none");
        m_btn_sign->setVisible(true);
        m_btn_verify->setVisible(true);

        if(test_type == SM2_SIGN_TEST)
        {
            dashboard.setTitle(QStringLiteral("SM2签名速度"));
        }
        else
        {
            dashboard.setTitle(QStringLiteral("SM2验签速度"));
        }
        dashboard.setEndValue(0);
        cliecked_counter = 0;
    }
    update();
}

void SeWidget::select_type(int id)
{
    test_type = id;

    if(test_type == SM2_SIGN_TEST)
    {
        dashboard.setTitle(QStringLiteral("SM2签名速度"));
        dashboard.setEndValue(0);
    }
    else
    {
        dashboard.setTitle(QStringLiteral("SM2验签速度"));
        dashboard.setEndValue(0);
    }
    update();
}

void SeWidget::timer1SUpdate()
{
    timeout_1s++;
}

void SeWidget::onWarnning(QString msg)
{
    QMessageBox::warning(this, "Warning", msg);
}

void SeWidget::on_exitButton_clicked()
{
    if(cliecked_counter%2)
    {
        qDebug("sm2 is testing, do not return widget\n");
        return;
    }

    select_type(SM2_SIGN_TEST);
    m_btn_sign->setChecked(true);

    emit exitWidget();
}


#include "sewidget.moc"
