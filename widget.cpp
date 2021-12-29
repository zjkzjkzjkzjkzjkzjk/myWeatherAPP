#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QMessageBox>
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    weather=new Weather(QStringLiteral("东莞"));
    connect(weather,&Weather::getDataSuccessedSignal,this,&Widget::on_pushButton_clicked);
    connect(weather,SIGNAL(getDataFailedSignal()),this,SLOT(getWeatherFailedSlot()));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::getWeatherFailedSlot()
{
    QMessageBox::warning(this,QString::fromLocal8Bit("获取天气提示"),QString::fromLocal8Bit("获取天气失败，请确认网络是否连接，或城市名的输入"));
}


void Widget::on_pushButton_2_clicked()
{
    weather->refresh();
}

void Widget::on_pushButton_3_clicked()
{
    if(ui->lineEdit->text().isEmpty())
        return;
    weather->setCityName(ui->lineEdit->text());
    weather->refresh();
}

void Widget::on_pushButton_4_clicked()
{
    weather->print_Debug_allinfoMap();
}
