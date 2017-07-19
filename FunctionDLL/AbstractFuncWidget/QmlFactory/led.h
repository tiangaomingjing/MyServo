#ifndef LED_H
#define LED_H

#include <QWidget>
class QLabel;

class Led : public QWidget
{
  Q_OBJECT
  Q_PROPERTY(bool state READ state WRITE setState )
  Q_PROPERTY(QString ledName READ ledName WRITE setLedName)
  Q_PROPERTY(QString ledOnFileName READ ledOnFileName WRITE setLedOnFileName)
  Q_PROPERTY(QString  ledOffFileName READ ledOffFileName WRITE setLedOffFileName)
  Q_PROPERTY(int  alignment READ alignment WRITE setAlignment)

public:
  explicit Led(QWidget *parent = 0);
  void setState(bool checked);
  bool state() {return m_ledState;}
  void setLedName(QString str);
  QString ledName(){return name;}
  void setLedOnFileName(QString str){m_onFileName=str;}
  QString ledOnFileName(){return m_onFileName;}
  void setLedOffFileName(QString str){m_offFileName=str;}
  QString ledOffFileName(){return m_offFileName;}
  int alignment();
  void setAlignment(int value);
private:
  QLabel *m_imageLabel;
  QLabel *m_nameLabel;
  bool m_ledState;
  QString name;
  QString m_onFileName;
  QString m_offFileName;
  int m_align;
};

#endif // LED_H
