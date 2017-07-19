#ifndef LEDPROXY_H
#define LEDPROXY_H

#include <QObject>
class QWidget;
class QLabel;

class LedProxy : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QWidget * led READ led )
  Q_PROPERTY(bool state READ state WRITE setState)
  Q_PROPERTY(QString ledName READ ledName WRITE setLedName )
public:
  explicit LedProxy(QWidget *ledparent=0,QObject *parent = 0);
  QWidget * led(){return m_led;}
  bool state(){return m_state;}
  void setState(bool sta);
  QString &ledName(){return m_name;}
  void setLedName(QString &str);
signals:

public slots:
private:
  QWidget *m_led;
  QLabel *m_imageLabel;
  QLabel *m_nameLabel;
  bool m_state;
  QString m_name;
};

#endif // LEDPROXY_H
