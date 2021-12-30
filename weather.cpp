#include "weather.h"

Weather::Weather(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(replyFinished(QNetworkReply*)));
}

Weather::Weather(QString cityName)
{
    manager = new QNetworkAccessManager(this);
    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(replyFinished(QNetworkReply*)));
    this->cityName=cityName;
    refresh();
}

void Weather::setCityName(QString cityName)
{
    this->cityName=cityName;
}

QString Weather::getCityName()
{
    return this->cityName;
}

QString Weather::getDate()
{
    return this->date;
}

QString Weather::getFengLi()
{
    return this->fengli;
}

QString Weather::getWenDu()
{
    return this->wendu;
}

QString Weather::getTianQiType()
{
    return this->weather_type;
}

QString Weather::getCurrentWendu()
{
    return this->currentwendu;
}

QString Weather::getGanMaoInfo()
{
    return this->ganmao;
}

QString Weather::getAllInfo()
{
    return this->allinfo;
}

bool Weather::isGetDataSuccessd()
{
    return this->isGetData;
}

void Weather::refresh()
{
    this->currentwendu.clear();
    this->wendu.clear();
    this->ganmao.clear();
    this->fengli.clear();
    this->weather_type.clear();
    this->allinfo.clear();
    queryWeather();
    dataMap.clear();
}

QMap<QString, QMap<QString, QString> > Weather::getDataMap(bool *ok)
{
    bool Oktemp;
    if(ok==nullptr)
        ok=&Oktemp;
    if(!this->dataMap.isEmpty())
    {
        *ok=true;
        return this->dataMap;
    }
    *ok=false;
    if(!this->isGetData)
        return this->dataMap;
    QJsonParseError err;
    QJsonDocument json_recv = QJsonDocument::fromJson(allinfo.toUtf8(),&err);
    qDebug() <<"Json-Error:"<< err.error;
    if(!json_recv.isNull())
    {
        QJsonObject object = json_recv.object();

        if(object.contains("data"))
        {
            QJsonValue value = object.value("data");
            if(value.isObject())
            {
                QJsonObject object_data = value.toObject();
                if(object_data.contains("yesterday")&&object_data.contains("forecast"))
                {
                    QJsonValue value=object_data.value("yesterday");
                    if(value.isObject())
                    {
                        QMap<QString,QString>mapvalue;
                        mapvalue["high"]=value.toObject().value("high").toString();
                        mapvalue["low"]=value.toObject().value("low").toString();
                        mapvalue["fengxiang"]=value.toObject().value("fx").toString();
                        mapvalue["fengli"]=value.toObject().value("fl").toString();
                        mapvalue["type"]=value.toObject().value("type").toString();
                        dataMap[value.toObject().value("date").toString()]=mapvalue;
                    }
                    value = object_data.value("forecast");
                    if(value.isArray())
                    {
                        QJsonArray valueArray=value.toArray();
                        qDebug()<<"WeatherData count:"<<valueArray.count();
                        for(int i=0;i<valueArray.count();i++)
                        {
                            QJsonObject object = valueArray.at(i).toObject();
                            QMap<QString,QString>mapvalue;
                            mapvalue["high"]=object.value("high").toString();
                            mapvalue["low"]=object.value("low").toString();
                            mapvalue["fengxiang"]=object.value("fengxiang").toString();
                            mapvalue["fengli"]=object.value("fengli").toString();
                            mapvalue["type"]=object.value("type").toString();
                            dataMap[object.value("date").toString()]=mapvalue;
                        }


                        qDebug()<<QString::fromLocal8Bit("获取天气成功");
                        *ok=true;
                        return dataMap;
                    }
                }
            }
        }
    }else
    {
        qDebug()<<"json_recv is NULL or is not a object !!";
    }
    return dataMap;
}
void Weather::print_Debug_allinfoMap()
{
    getDataMap();
    qDebug()<<endl;
    qDebug()<<"city:"<<this->cityName;
    qDebug()<<"wendu:"<<this->wendu;
    qDebug()<<"currentwendu:"<<this->currentwendu;
    qDebug()<<"fengli:"<<this->fengli;
    qDebug()<<"weather_type:"<<this->weather_type;
    qDebug()<<"ganmao:"<<this->ganmao;
    QString str;
    foreach (QString key, dataMap.keys()) {
        str="date"+key+"[";
        foreach (QString key1, dataMap.value(key).keys()) {
            str+=key1+':'+dataMap.value(key).value(key1)+' ';
        }
        str+=']';
        qDebug()<<str;
    }
    dataMap.clear();
}

void Weather::replyFinished(QNetworkReply *reply)
{
    this->isGetData=false;
    qDebug()<<"recv weather data!!";
    allinfo = reply->readAll();

    QJsonParseError err;
    QJsonDocument json_recv = QJsonDocument::fromJson(allinfo.toUtf8(),&err);
    qDebug() <<"Json-Error:"<< err.error;
    if(!json_recv.isNull())
    {
        QJsonObject object = json_recv.object();
        if(object.contains("data"))
        {
            QJsonValue value = object.value("data");
            if(value.isObject())
            {
                QJsonObject object_data = value.toObject();
                this->cityName=object_data.value("city").toString();
                this->currentwendu=object_data.value("wendu").toString();
                this->ganmao=object_data.value("ganmao").toString();
                if(object_data.contains("forecast"))
                {
                    QJsonValue value = object_data.value("forecast");
                    if(value.isArray())
                    {
                        QJsonObject today_weather = value.toArray().at(0).toObject();
                        weather_type = today_weather.value("type").toString();
                        date = today_weather.value("date").toString();

                        QString low = today_weather.value("low").toString();
                        QString high = today_weather.value("high").toString();
                        wendu = low.mid(low.length()-3,4) +"-"+ high.mid(high.length()-3,4);
                        QString strength = today_weather.value("fengli").toString();
                        strength.remove(0,8);
                        strength.remove(strength.length()-2,2);
                        fengli = today_weather.value("fengxiang").toString() + strength;
                        this->isGetData=true;
                    }
                }
            }
        }

    }else
    {
        qDebug()<<"json_recv is NULL or is not a object !!";
    }
    reply->deleteLater();
    if(isGetData)
    {
        qDebug()<<QString::fromLocal8Bit("获取天气成功");
        emit this->getDataSuccessedSignal();
    }
    else
    {
        qDebug()<<QString::fromLocal8Bit("获取天气失败");
        emit this->getDataFailedSignal();
    }
    emit this->getDataFinisedSignal();
}
void Weather::queryWeather()
{
    char quest_array[256]="http://wthrcdn.etouch.cn/weather_mini?city=";
    QNetworkRequest quest;
    sprintf(quest_array,"%s%s",quest_array,cityName.toUtf8().data());
    quest.setUrl(QUrl(quest_array));
    quest.setHeader(QNetworkRequest::UserAgentHeader,"RT-Thread ART");
    manager->get(quest);
}
