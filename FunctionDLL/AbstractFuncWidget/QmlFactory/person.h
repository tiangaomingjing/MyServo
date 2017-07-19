#ifndef PERSON_H
#define PERSON_H

#include <QObject>
#include <QtWidgets>

class Person : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged)
  Q_PROPERTY(int age READ getAge WRITE setAge NOTIFY ageChanged)
public:
  explicit Person(QObject *parent = 0);
  ~Person(){qDebug()<<"release person ->";}
  QString getName(void) const{return m_name;}
  void setName(const QString &name){m_name=name;emit nameChanged();}
  int getAge(void) const {return m_age;}
  void setAge(const int age){m_age=age;emit ageChanged();}
  Q_INVOKABLE QColor getColor(void)const {return QColor(Qt::blue);}
  Q_INVOKABLE void setNameAndAge(void) {setName("Lucy");setAge(18);}

signals:
  void nameChanged(void);
  void ageChanged(void);
public slots:
private:
  QString m_name;
  int m_age;
};

#endif // PERSON_H
