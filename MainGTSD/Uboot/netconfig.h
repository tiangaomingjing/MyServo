#ifndef NETCONFIG_H
#define NETCONFIG_H

#include <QtCore>

class NetConfig
{
public:
  NetConfig();
  ~NetConfig();

  bool initialDataFromJsonFile(QString &filePath);

  int netId() const;
  void setNetId(int netId);

  QString netName() const;
  void setNetName(const QString &netName);

  QString ipAddress() const;
  void setIpAddress(const QString &ipAddress);

  int ipPort() const;
  void setIpPort(int ipPort);

  qint16 rnStation() const;
  void setRnStation(qint16 rnStation);

  quint32 baudRate() const;
  void setBaudRate(quint32 baudRate);

  int ubootCmd() const;
  void setUbootCmd(int ubootCmd);

  QString ubootInputKey() const;
  void setUbootInputKey(const QString &ubootInputKey);

private:
  int m_netId;
  QString m_netName;
  QString m_ipAddress;
  int m_ipPort;
  qint16 m_rnStation;
  quint32 m_baudRate;
  int m_ubootCmd;
  QString m_ubootInputKey;
};

#endif // NETCONFIG_H
