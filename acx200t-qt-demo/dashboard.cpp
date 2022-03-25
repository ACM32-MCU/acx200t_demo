#include "dashboard.h"

#include <QStyleOption>
#include <QPushButton>

Dashboard::Dashboard(QWidget *parent)
    : QWidget(parent)
{
//    setFixedSize(800, 480);

//   setStyleSheet("font:14px;");

    QString qss = "color:#BECEC6;background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:1 #1B3152,stop:0 #255C80);font:14px";
    setStyleSheet(qss);

//    //�������óɺ�ɫ
//    QPalette bgpal=palette();
//    bgpal.setColor(QPalette::Background,QColor(0, 0, 0));
//    setPalette (bgpal);

    minValue = 0;
    maxValue = 4200;

    value = 20;                         //��ǰֵ
    endvalue = 0;

    timer_msec = 0;

    title = "Speed";

//    setTimerType(20, 100);
    setTimerType(10, 1000);

    connect(&timer, SIGNAL(timeout()), this, SLOT(ontimeout()));

    QLabel *label = new QLabel(this);

    label->setFixedHeight(50);
    label->setFixedWidth(50);
    label->setGeometry(0, 0, 50, 50);

    QPixmap pixmap(":/image/exit.png");

    pixmap.scaled(label->size(), Qt::KeepAspectRatio);
    label->setScaledContents(true);
    label->setPixmap(pixmap);

    label->setStyleSheet("background-color: rgb(0,0,0,0);\nborder:none");

    QPushButton *exit_button = new QPushButton(this);
    exit_button->setFixedHeight(50);
    exit_button->setFixedWidth(50);
    exit_button->setFocusPolicy(Qt::NoFocus);
    exit_button->setStyleSheet("background-color: rgb(0,0,0,0);\nborder:none");
    exit_button->setGeometry(0, 0, 50, 50);

    connect(exit_button, SIGNAL(clicked()),this,SLOT(on_exitButton_clicked()));

}

Dashboard::~Dashboard(){}

void Dashboard::setTimerType(int msec, int v)
{
    timer_value = v;
    timer_msec  = msec;
    if(timer_msec)
    {
        timer.setInterval(timer_msec);

    }
    else if(timer_msec==0)
    {
        timer.stop();
    }
}

void Dashboard::paintEvent(QPaintEvent *e)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(e);

    QPainter painter(this);

    int width=this->width();
    int height = this->height() - 30;

    radius = ((width>height)?height:width)/2;

    centerR=radius*0.38;       //��������Բ��С

    //�������ƶ������·�
    painter.translate(width>>1,height*0.6);

    //���÷����
    painter.setRenderHint(QPainter::Antialiasing, true);

    //ȡ������
    painter.setPen(Qt::NoPen);

    //���û�ˢ��ɫ
    painter.setBrush(QColor(54, 156, 215));

//    painter.setBrush(QColor(32, 93, 135));

    //����13��С��
    DrawPoint(painter,radius + 10);

    //�����ڲ�����
    DrawDigital(painter,radius + 5);

    drawslideScaleGradientColor(painter);

    drawScaleColor(painter);

    drawbkColor(painter);  //������Բ

    //���ƽ���Բ��
    DrawCircle(painter,radius-35);

    //����ָ��
    DrawPointer(painter, radius-70, endvalue);
}

void Dashboard::ontimeout()
{
    if(value != endvalue)
    {

        if(timer_value==0)
        {
            value = endvalue;
            sendEnd();
        }
        else if( qAbs(value-endvalue) <= qAbs(timer_value))
        {
             value = endvalue;
             sendEnd();
        }
        else if(value>endvalue)
        {
             value -=qAbs(timer_value);
        }
        else
        {
             value +=qAbs(timer_value);
        }

        sendChangeValue();

        update();

//        qDebug("value!=endvalue, update  timer_value = %d value = %d endvalue = %d",
//                        timer_value, value, endvalue);
    }

    if(value==endvalue)
    {
        timer.stop();
    }
}

void Dashboard::on_exitButton_clicked()
{
//    qDebug("in dashboard click exit button\n");
    emit exit_button_clicked();
}

void Dashboard::DrawPoint(QPainter& painter,int radius)
{
    //��װ���·��ͼ
    QPainterPath pointPath;
    pointPath.moveTo(-2,-2);
    pointPath.lineTo(2,-2);
    pointPath.lineTo(2,2);
    pointPath.lineTo(0,4);
    pointPath.lineTo(-2,2);
    //����13��С��
    for(int i=0;i<13;++i){
        QPointF point(0,0);
        painter.save();
        //���㲢�ƶ���ͼ�������ĵ�
        point.setX(radius*qCos(((210-i*20)*M_PI)/180));
        point.setY(radius*qSin(((210-i*20)*M_PI)/180));
        //���㲢�ƶ���ͼ��������ĵ�
        painter.translate(point.x(),-point.y());
        //���㲢ѡ���ͼ��������
        painter.rotate(-120+i*20);
        //����·��
        painter.drawPath(pointPath);
        painter.restore();
    }
}

void Dashboard::DrawDigital(QPainter& painter,int radius)
{
    //���û���
    painter.setPen(QColor(54,156,215));
//    painter.setPen(QColor(32, 93, 135));


//    //��������
//    QFont font;
////    font.setFamily("SimHei");
////    font.setPointSize(16);
//    painter.setFont(font);

    int interval = (maxValue - minValue) / 12;

    //����13��С��
    for(int i=0;i<13;++i){
        QPointF point(0,0);
        painter.save();
        //���㲢�ƶ���ͼ�������ĵ�
        point.setX(radius*qCos(((210-i*20)*M_PI)/180));
        point.setY(radius*qSin(((210-i*20)*M_PI)/180));
        //���㲢�ƶ���ͼ��������ĵ�
        painter.translate(point.x(),-point.y());
        //���㲢ѡ���ͼ��������
        painter.rotate(-120+i*20);

        //����·��
        painter.drawText(-25, 0, 45, 20, Qt::AlignCenter,QString::number(i * interval));

        painter.restore();
    }
    //ȥ������
    painter.setPen(Qt::NoPen);
}

void Dashboard::DrawCircle(QPainter& painter,int radius)
{
    //�����ͼ����
    painter.save();
    //�����СԲ·��
    QPainterPath outRing;
    QPainterPath inRing;
    outRing.moveTo(0,0);
    inRing.moveTo(0,0);
    outRing.arcTo(-radius+10,-radius+10, 2*(radius-10),2*(radius-10),-31,242);
    inRing.addEllipse(-radius+20,-radius+20,2*(radius-20),2*(radius-20));
    outRing.closeSubpath();

    QColor obkColor=QColor(252,252,248);               //��Բ����ɫ

    QConicalGradient  Conical(0,0,90);

    Conical.setColorAt(0,obkColor.darker(130));
    Conical.setColorAt(0.5,obkColor.darker(130));

    Conical.setColorAt(0.12,obkColor.darker(80));
    Conical.setColorAt(0.88,obkColor.darker(80));

    Conical.setColorAt(0.4,obkColor.darker(50).darker(130));
    Conical.setColorAt(0.6,obkColor.darker(50));


    Conical.setColorAt(0.25,obkColor.darker(130));

    Conical.setColorAt(0.75,obkColor.darker(130));
    Conical.setColorAt(1,obkColor.darker(130));

    painter.setBrush(Conical);

    //��Բ��ȥСԲ�õ�Բ��
    painter.drawPath(outRing.subtracted(inRing));
    //�ָ���ͼ����
    painter.restore();
}

void Dashboard::DrawSmallScale(QPainter& painter,int radius)
{
    //��װ���·��ͼ
    QPainterPath pointPath;
    pointPath.moveTo(-2,-2);
    pointPath.lineTo(-1,-4);
    pointPath.lineTo(1,-4);
    pointPath.lineTo(2,-2);
    pointPath.lineTo(1,8);
    pointPath.lineTo(-1,8);
    //����121��С��
    for(int i=0;i<121;++i){
        QPointF point(0,0);
        painter.save();
        //���㲢�ƶ���ͼ�������ĵ�
        point.setX(radius*qCos(((210-i*2)*M_PI)/180));
        point.setY(radius*qSin(((210-i*2)*M_PI)/180));
        //���㲢�ƶ���ͼ��������ĵ�
        painter.translate(point.x(),-point.y());
        //���㲢ѡ���ͼ��������
        painter.rotate(-120+i*2);
        //����·��
        painter.drawPath(pointPath);
        painter.restore();
    }
}

void Dashboard::DrawBigScale(QPainter& painter,int radius)
{
    //��װ���·��ͼ
    QPainterPath pointPath1;
    pointPath1.moveTo(-2,-2);
    pointPath1.lineTo(-1,-4);
    pointPath1.lineTo(1,-4);
    pointPath1.lineTo(2,-2);
    pointPath1.lineTo(1,8);
    pointPath1.lineTo(-1,8);
    QPainterPath pointPath2;
    pointPath2.moveTo(-2,-2);
    pointPath2.lineTo(-1,-4);
    pointPath2.lineTo(1,-4);
    pointPath2.lineTo(2,-2);
    pointPath2.lineTo(1,15);
    pointPath2.lineTo(-1,15);
    //����25���̶�
    for(int i=0;i<25;++i){
        QPointF point(0,0);
        painter.save();
        //���㲢�ƶ���ͼ�������ĵ�
        point.setX(radius*qCos(((210-i*10)*M_PI)/180));
        point.setY(radius*qSin(((210-i*10)*M_PI)/180));
        //���㲢�ƶ���ͼ��������ĵ�
        painter.translate(point.x(),-point.y());
        //���㲢ѡ���ͼ��������
        painter.rotate(-120+i*10);
        //����·��
        if(i%2){
            painter.drawPath(pointPath1);
        }
        else{
            painter.drawPath(pointPath2);
        }
        painter.restore();
    }
}

void Dashboard::DrawText(QPainter& painter,int radius)
{
    painter.save();
    //���û���
    painter.setPen(QColor(98,246,255));

    //��������
//    QFont font;
//    font.setFamily("Microsoft YaHei");
//    font.setPointSize(16);
//    painter.setFont(font);


    painter.drawText(-25, -radius, 50, 20,Qt::AlignCenter,QString("/s"));
    painter.restore();
}

void Dashboard::DrawPointer(QPainter& painter,int radius)
{
    //��װ���·��ͼ
    QPainterPath pointPath;
    pointPath.moveTo(10,0);
    pointPath.lineTo(1,-radius);
    pointPath.lineTo(-1,-radius);
    pointPath.lineTo(-10,0);
    pointPath.arcTo(-10,0,20,20,180,180);
    QPainterPath inRing;
    inRing.addEllipse(-5,-5,10,10);
    painter.save();
    //���㲢ѡ���ͼ��������
    painter.rotate(-120);
    //���û�ˢ
    painter.setBrush(QColor(255,0,0,150));
    //����·��
    painter.drawPath(pointPath.subtracted(inRing));
    painter.restore();
}

void Dashboard::DrawPointer(QPainter& painter,int radius, int value)
{
    //��װ���·��ͼ
    QPainterPath pointPath;
    pointPath.moveTo(10,0);
    pointPath.lineTo(1,-radius);
    pointPath.lineTo(-1,-radius);
    pointPath.lineTo(-10,0);
    pointPath.arcTo(-10,0,20,20, 180, 180);
    QPainterPath inRing;
    inRing.addEllipse(-5,-5,10,10);
    painter.save();
    //���㲢ѡ���ͼ��������

    double degRotate = (360.0 - 30.0 - 90.0) / (maxValue - minValue) * (value - minValue);

    painter.rotate(degRotate - 120.0);
    //���û�ˢ
    painter.setBrush(QColor(255,0,0,150));
    //����·��
    painter.drawPath(pointPath.subtracted(inRing));

    painter.restore();



    painter.save();

     //set label text
    qreal ValueSize=centerR/4;

    painter.setFont(QFont("wqy-zenhei", 11, QFont::Normal));
    painter.setPen(QColor(255, 255, 255));
//    painter.setPen(QColor(32, 93, 135));

    painter.drawText(QRectF(-ValueSize*6,centerR*1.1,ValueSize*12,ValueSize*2),Qt::AlignCenter,title);

    painter.restore();
}

void Dashboard::drawslideScaleGradientColor(QPainter &paint)
{
    qreal angles[GradientSlideColor.count()];    //�Զ��彥����ɫλ��

    qreal top=0.8666668;
    qreal bottom=0.8223336;


    int Star_Angle= 210*16-(int)(((qreal)(value - minValue)/(maxValue - minValue))*240*16);

    int spanAngle  =  210*16 - Star_Angle;

    if(spanAngle == 0)
        return ;


    for(int i=0;i<GradientSlideColor.count();i++)
    {
        angles[GradientSlideColor.count()-i-1] =(qreal)(240/360.0)*(qreal) i/ (qreal)(GradientSlideColor.count()-1);

    }


    paint.save();

    QConicalGradient Conical(0,0,-30);

    /*�������*/
    for(int i =0 ; i<GradientSlideColor.count(); i++)
         Conical.setColorAt(angles[i],GradientSlideColor[i]);

    Conical.setColorAt(angles[0]+0.01,Qt::transparent);
    Conical.setColorAt(1,Qt::transparent);

    paint.setPen(Qt::NoPen);
    paint.setBrush(Conical);

    paint.drawPie(QRectF((qreal)-radius*top,(qreal)-radius*top,(qreal)radius*top*2,(qreal)radius*top*2),Star_Angle,spanAngle);



    paint.setBrush(QColor(26,26,26));
    paint.drawPie(QRectF((qreal)-radius*bottom,(qreal)-radius*bottom,(qreal)radius*bottom*2, (qreal)radius*bottom*2) ,Star_Angle,spanAngle);


    /*����ǳ����*/
    QList<QColor> Tint_Colors = GradientSlideColor;

    for(int i=0; i< GradientSlideColor.count();i++)
       Tint_Colors[i].setAlpha(90);

    for(int i =0 ; i<GradientSlideColor.count(); i++)
         Conical.setColorAt(angles[i],Tint_Colors[i]);


    paint.setBrush(Conical);
    paint.drawPie(QRectF((qreal)-radius*0.90,(qreal)-radius*0.90,(qreal)radius*1.80,(qreal)radius*1.80),Star_Angle,spanAngle);


    paint.restore();
}

void Dashboard::drawScaleColor(QPainter &paint)
{
    #define  SmallTop        3
    #define  SmallBottom     2

    #define  BigTop          5
    #define  BigBottom       3

    //С�̶�(�������Ϊ3,�ײ�Ϊ2)
    const QPointF smallPoint[4] = {
        QPointF( - SmallTop / 2, radius*0.90),
        QPointF( SmallTop / 2, radius*0.90),
        QPointF(SmallBottom / 2, radius*0.81),
        QPointF(-SmallBottom / 2, radius*0.81)
    };


    //��̶�(�������Ϊ4,�ײ�Ϊ3)
    const QPointF BigPoint[4] = {
        QPointF(- BigTop / 2, radius*0.90),
        QPointF(BigTop / 2, radius*0.90),
        QPointF(BigBottom / 2, radius*0.77),
        QPointF(-BigBottom / 2, radius*0.77)
    };




    paint.save();

    paint.rotate(60);            //��ת�����=210��

    QColor ScaleColor = QColor(185,185,185);            //���̶̿���ɫ
    QColor slideScaleColor = QColor(255,255,255);     //�����Ŀ̶���ɫ

    for (int scale = 0; scale <= 24; ++scale)   //210�㵽-30��,��7����̶�
    {
        qreal Current_Value =(qreal)scale*((maxValue-minValue)/24);

        if((Current_Value>(value-minValue)&&(paint.brush().color()!=ScaleColor)))
        {
            paint.setBrush(ScaleColor);

        }
        else if((Current_Value<=(value-minValue)&&(paint.brush().color()!=slideScaleColor)))
        {
            paint.setBrush(slideScaleColor);
        }

      if(scale%4==0)
      {
       paint.drawConvexPolygon(BigPoint, 4);
      }
      else
        paint.drawConvexPolygon(smallPoint, 4);

      paint.rotate(10);
    }

     paint.restore();
}

void Dashboard::drawbkColor(QPainter &paint)       //������Բ
{
    paint.save();

    QColor bkColor=QColor(26,26,26);                   //�ڶ���Բ����ɫ

//    bkColor=QColor(0x25, 0x5c, 0x80);
    bkColor=QColor(0x1B, 0x31, 0x52);

    paint.setBrush(bkColor);
    paint.drawEllipse(QPointF(0,0), radius*0.77-2,radius*0.77-2);

    paint.setBrush(bkColor.darker(200));

    QRadialGradient Radial(0,0,centerR*1.06,0,0);

    Radial.setColorAt(1,bkColor);
    Radial.setColorAt(0.98,bkColor.darker(100));
    Radial.setColorAt(0.95,bkColor.darker(110));
    Radial.setColorAt(0.90,bkColor.darker(120));

    paint.setBrush(Radial);

    paint.drawEllipse(QPointF(0,0),centerR*1.18,centerR*1.18);

    paint.restore();
}

void Dashboard::setTitle(QString title)
{
    this->title = title;
}

void Dashboard::setRange(int min, int max)
{
    this->minValue = min;
    this->maxValue = max;
}

bool Dashboard::setEndValue(int v)
{

    if((v<minValue)||(v>maxValue))
    {
        return false;
    }

    endvalue = v;

    if(timer_msec==0)           //��ʾ��ʱ������Ҫ��ʱ,ֱ�������յ�
    {

      value = v;

      sendEnd();
      sendChangeValue();
      update();
    }
    else if(value==v)
    {
        sendEnd();
        sendChangeValue();

    }
    else
    {
        if(!timer.isActive())
        {
           timer.start();
        }
    }

    return true;
}

bool Dashboard::setValues(int cV,int minV,int maxV,int endV)
{
    bool ret = true;

    if((cV>=minV)&&(cV<=maxV)&&(endV>=minV)&&(endV<=maxV))
    {
        endvalue =endV;

        value = cV;

        minValue = minV;

        maxValue = maxV;

        update();
    }
    else
    {
        ret = false;
    }

    return ret;
}

void Dashboard::setGradientColorMode(QList<QColor> &Qcolors)
{
    GradientSlideColor.clear();
    GradientSlideColor = Qcolors;
    update();
}
