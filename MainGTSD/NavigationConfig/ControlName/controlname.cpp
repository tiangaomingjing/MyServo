#include "controlname.h"
#include <QJsonParseError>
#include <QJsonDocument>
#include <QVariantMap>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QObject>

ControlName::ControlName(QObject *parent) : QObject(parent)
{

}
ControlName::~ControlName()
{

}

QUrl ControlName::source() const
{
    return m_source;
}

void ControlName::setSource(const QUrl &source)
{
    m_source = source;
    m_map.clear();
    QString localFile=source.toLocalFile();
    qDebug()<<"localFile:"<<localFile;

    QFile jsonFile(localFile);
    QTextStream in;
    if(!jsonFile.open(QIODevice::ReadOnly)){
      qDebug()<<"open error";
    }
    in.setDevice(&jsonFile);
    QString json;
    json=in.readAll();

    QJsonParseError error;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(json.toUtf8(), &error);
    if (error.error == QJsonParseError::NoError) {
        if (jsonDocument.isObject()) {
            QVariantMap result = jsonDocument.toVariant().toMap();
            QList<QVariant> encoderList = result["EncoderConfigPage"].toList();
            int count=encoderList.count();
            QString key;
            QString value;
            for(int i=0;i<count;i++)
            {
              QVariantMap item=encoderList.at(i).toMap();
              key=item.keys().at(0);
              value=item.values().at(0).toString();
              m_map.insert(key,value);
            }


            QList<QVariant> limitList = result["LimitConfigPage"].toList();
            count=limitList.count();
            for(int i=0;i<count;i++)
            {
              QVariantMap item=limitList.at(i).toMap();
              key=item.keys().at(0);
              value=item.values().at(0).toString();
              m_map.insert(key,value);
            }
            QList<QVariant> motionList = result["MotionConfigPage"].toList();
            count=motionList.count();
            for(int i=0;i<count;i++)
            {
              QVariantMap item=motionList.at(i).toMap();
              key=item.keys().at(0);
              value=item.values().at(0).toString();
              m_map.insert(key,value);
            }
            foreach (QString str, m_map.keys()) {
              qDebug()<<"key:"<<str;

            }
            foreach (QString ss, m_map.values()) {
              qDebug()<<"value:"<<ss;
            }

        }
    } else {
        qFatal(error.errorString().toUtf8().constData());
    }
    emit sourceChanged(m_source);
}

QString ControlName::value(const QString &key) const
{
  QString value;
  if(m_map.contains(key))
    value=m_map.value(key);
  else
    value="null";
  return value;
}

