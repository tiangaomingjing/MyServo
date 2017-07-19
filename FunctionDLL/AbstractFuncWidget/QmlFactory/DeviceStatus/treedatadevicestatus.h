#ifndef TREEDATADEVICESTATUS_H
#define TREEDATADEVICESTATUS_H

#include <QObject>
#include <QVector>

class TreeDataDeviceStatus : public QObject
{
  Q_OBJECT
  Q_PROPERTY(double currState READ currState)
  Q_PROPERTY(double adcVdc READ adcVdc)
  Q_PROPERTY(double cmdId READ cmdId)
  Q_PROPERTY(double currId READ currId)
  Q_PROPERTY(double usrMode READ usrMode)
  Q_PROPERTY(bool almFlag READ almFlag )
  Q_PROPERTY(QVector<bool> bits READ bits)
public:
  explicit TreeDataDeviceStatus(QObject *parent = 0);
  double currState(){return m_currState;}
  double adcVdc(){return m_adcVdc;}
  double cmdId(){return m_cmdId;}
  double currId(){return m_currId;}
  double usrMode(){return m_usrMode;}
  bool almFlag(){return m_almFlag;}
  QVector<bool> bits(){return m_bits;}

  void setCurrState(double value){m_currState=value;}
  void setAdcVdc(double value){m_adcVdc=value;}
  void setCmdId(double value){m_cmdId=value;}
  void setCurrId(double value){m_currId=value;}
  void setUsrMode(double value){m_usrMode=value;}
  void setAlmFlag(bool value){m_almFlag=value;}
  void setBits(quint16 index,bool value);

signals:

public slots:
private :
  double m_currState;
  double m_adcVdc;
  double m_cmdId;
  double m_currId;
  double m_usrMode;
  bool m_almFlag;
  QVector<bool>m_bits;
};

#endif // TREEDATADEVICESTATUS_H
