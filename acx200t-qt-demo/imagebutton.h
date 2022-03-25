#ifndef IMAGEBUTTION_H
#define IMAGEBUTTION_H

#include <QWidget>

namespace Ui {
class imageButtion;
}

class imageButtion : public QWidget
{
    Q_OBJECT

public:
    explicit imageButtion(QWidget *parent = 0);
    ~imageButtion();

signals:
    void pressedButton(int index);

private slots:
    void pressedButton1();
    void pressedButton2();
    void pressedButton3();
    void pressedButton4();

private:
    Ui::imageButtion *ui;
};

#endif // IMAGEBUTTION_H
