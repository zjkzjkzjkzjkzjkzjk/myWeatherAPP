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

void Widget::on_pushButton_clicked()
{
        ui->lineEdit_cityName->setText(weather->getCityName());
        ui->lineEdit_date->setText(weather->getDate());
        ui->lineEdit_currentWendu->setText(weather->getCurrentWendu()+QString::fromLocal8Bit("℃"));
        ui->lineEdit_Fengli->setText(weather->getFengLi());
        ui->lineEdit_type->setText(weather->getTianQiType());
        ui->lineEdit_Wendu->setText(weather->getWenDu());
        ui->textBrowser_ganmao->setText(weather->getGanMaoInfo());

        ui->tableWidget->clearContents();
        ui->tableWidget->setRowCount(0);
        QMap<QString,QMap<QString,QString>> map=weather->getDataMap();
        int n=0;
        foreach (QString date, map.keys()) {
            ui->tableWidget->insertRow(ui->tableWidget->rowCount());
            QMap<QString,QString> mapvalue=map.value(date);
            ui->tableWidget->setItem(n,0,new QTableWidgetItem(date));
            ui->tableWidget->setItem(n,1,new QTableWidgetItem(mapvalue.value("type")));
            QString low = mapvalue.value("low");
            QString high = mapvalue.value("high");
            ui->tableWidget->setItem(n,2,new QTableWidgetItem(low.mid(low.length()-3,4) +"-"+ high.mid(high.length()-3,4)));
            QString strength = mapvalue.value("fengli");
            strength.remove(0,8);
            strength.remove(strength.length()-2,2);
            ui->tableWidget->setItem(n,3,new QTableWidgetItem(mapvalue.value("fengxiang") + strength));
            n++;
        }

        qDebug()<<weather->getCityName();
        qDebug()<<QStringLiteral("风力风向")<<weather->getFengLi();
        qDebug()<<QStringLiteral("天气类型")<<weather->getTianQiType();
        qDebug()<<QStringLiteral("温度")<<weather->getWenDu();
        weather->getDataMap();
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
