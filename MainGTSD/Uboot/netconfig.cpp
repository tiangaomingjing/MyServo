#include "netconfig.h"
#include <QJsonParseError>
#include <QJsonDocument>
#include <QVariantMap>
#include <QDebug>
#include <QFile>
#include <QTextStream>

NetConfig::NetConfig():
  m_netId(0),
  m_netName("PcDebug"),
  m_ipAddress("192.168.1.0"),
  m_ipPort(8086),
  m_rnStation(240)
{

}
NetConfig::~NetConfig()
{

}

bool NetConfig::initialDataFromJsonFile(QString &filePath)
{
  bool ret=true;
  QFile jsonFile(filePath);
  QTextStream in;
  if(!jsonFile.open(QIODevice::ReadOnly)){
    qDebug()<<"open error:"<<filePath;
    return false;
  }
  in.setDevice(&jsonFile);
  QString json;
  json=in.readAll();

  QJsonParseError error;
  QJsonDocument jsonDocument = QJsonDocument::fromJson(json.toUtf8(), &error);
  if (error.error == QJsonParseError::NoError) {
      if (jsonDocument.isObject()) {
          QVariantMap result = jsonDocument.toVariant().toMap();
          m_netId=result["netId"].toInt();
          m_netName=result["netName"].toString();

          qDebug() << "netId:" << m_netId;
          qDebug() << "netName:" << m_netName;

          QVariantMap parametersMap = result["parameters"].toMap();
          m_ipAddress=parametersMap["IpAddress"].toString();
          m_ipPort=parametersMap["IpPort"].toInt();
          m_rnStation=parametersMap["RnStation"].toInt();
          m_baudRate=parametersMap["BaudRate"].toUInt();
          m_ubootCmd=parametersMap["UartBootCmd"].toInt();
          m_ubootInputKey=parametersMap["UartInputKey"].toString();
          qDebug() << "IpAddress:" << m_ipAddress;
          qDebug() << "IpPort:" << m_ipPort;
          qDebug() << "RnStation:" << m_rnStation;
          qDebug() << "BaudRate:" << m_baudRate;
          qDebug() << "UartBootCmd:" << m_ubootCmd;
          qDebug() << "UartInputKey:" << m_ubootInputKey;
      }
  }
  else {
      qFatal(error.errorString().toUtf8().constData());
      ret=false;
  }
  jsonFile.close();
  return ret;
}

int NetConfig::netId() const
{
  return m_netId;
}

void NetConfig::setNetId(int netId)
{
  m_netId = netId;
}

QString NetConfig::netName() const
{
  return m_netName;
}

void NetConfig::setNetName(const QString &netName)
{
  m_netName = netName;
}

QString NetConfig::ipAddress() const
{
  return m_ipAddress;
}

void NetConfig::setIpAddress(const QString &ipAddress)
{
  m_ipAddress = ipAddress;
}

int NetConfig::ipPort() const
{
  return m_ipPort;
}

void NetConfig::setIpPort(int ipPort)
{
  m_ipPort = ipPort;
}

qint16 NetConfig::rnStation() const
{
  return m_rnStation;
}

void NetConfig::setRnStation(qint16 rnStation)
{
  m_rnStation = rnStation;
}

quint32 NetConfig::baudRate() const
{
    return m_baudRate;
}

void NetConfig::setBaudRate(quint32 baudRate)
{
    m_baudRate = baudRate;
}

int NetConfig::ubootCmd() const
{
    return m_ubootCmd;
}

void NetConfig::setUbootCmd(int ubootCmd)
{
    m_ubootCmd = ubootCmd;
}

QString NetConfig::ubootInputKey() const
{
    return m_ubootInputKey;
}

void NetConfig::setUbootInputKey(const QString &ubootInputKey)
{
    m_ubootInputKey = ubootInputKey;
}

