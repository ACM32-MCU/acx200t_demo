#ifndef IMAGEPRESSBUTTON_H
#define IMAGEPRESSBUTTON_H

#include <QWidget>
#include <QPushButton>
#include <QTimer>
#include <QPropertyAnimation>

class imagePressButton : public QWidget
{
    Q_OBJECT

private:
    int pixWidth;
    int pixHeight;
    int oldWidth;
    int oldHeight;
    int targetWidth;
    int targetHeight;

    QString text;
    QString image;

    bool isButtonInited;
    QPushButton *pushButton;
    QPropertyAnimation *enterAnimation;
    QPropertyAnimation *leaveAnimation;
    QTimer *timer;

public:
    explicit imagePressButton(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *);

private slots:
    void on_pushButton_clicked();
    void enterImageChanged(QVariant index);
    void leaveImageChanged(QVariant index);
    void timeout();

signals:
    void clicked();

public slots:
    void setText(QString text);
    void setImage(QString image);
};

#endif // IMAGEPRESSBUTTON_H
