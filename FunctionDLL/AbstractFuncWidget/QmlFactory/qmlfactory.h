#ifndef QMLFACTORY_H
#define QMLFACTORY_H

#include <QObject>
#include <QQmlListProperty>
#include <QList>
#include "person.h"
class QLineEdit;
class QLabel;
class QPushButton;
class Led;
class QHBoxLayout ;
class QSpacerItem ;
class QVBoxLayout ;
class QHBoxLayoutProxy;
class QVBoxLayoutProxy;
class IconLabel;
class QFormLayoutProxy;
class QSpacerItemProxy;
class QTreeWidgetProxy;

class QmlFactory : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QQmlListProperty<Person> persons READ persons )
  Q_CLASSINFO("DefaultProperty", "Person")
public:
  explicit QmlFactory(QObject *parent = 0);
  ~QmlFactory();

  QQmlListProperty<Person> persons() {return QQmlListProperty<Person>(this,m_persons); }
  Q_INVOKABLE int personsCount() const {return m_persons.count();}
  Person *personAt(int index) {return m_persons[index];}

  Q_INVOKABLE QLineEdit* createQLineEdit(QWidget *parent=0);
  Q_INVOKABLE QLabel * createQLabel(QWidget *parent=0);
  Q_INVOKABLE QPushButton *createQPushButton(QWidget *parent=0);
  Q_INVOKABLE Led *createLed(QWidget *parent=0);
  Q_INVOKABLE QVBoxLayout *createVBoxLayout(QWidget *parent=0);
  Q_INVOKABLE QHBoxLayoutProxy *createQHBoxLayoutProxy(QWidget *layoutParent=0,QObject *parent=0);
  Q_INVOKABLE QVBoxLayoutProxy *createQVBoxLayoutProxy(QWidget *layoutParent=0,QObject *parent=0);
  Q_INVOKABLE QFormLayoutProxy *createQFormLayoutProxy(QWidget *layoutParent=0,QObject *parent=0);
  Q_INVOKABLE IconLabel *createIconLabel(QWidget *parent=0);
  Q_INVOKABLE QSpacerItemProxy *createQSpacerItemProxy(int type=0,QObject *parent = 0);
  Q_INVOKABLE QTreeWidgetProxy *createQTreeWidgetProxy(QTreeWidget *treesrc=0,QObject *parent=0);

signals:
  void initialFactory();
  void signalTest(QString str);

public slots:
private slots:
  void onFactoryCreated();
private:
  QList<Person*>m_persons;
};

#endif // QMLFACTORY_H
