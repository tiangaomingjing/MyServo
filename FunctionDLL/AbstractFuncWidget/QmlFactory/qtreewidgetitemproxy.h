#ifndef QTREEWIDGETITEMPROXY_H
#define QTREEWIDGETITEMPROXY_H

#include <QObject>
class QTreeWidgetItem;//QTreeWidgetItem不是QObject的子类，所以Qml中不能识别该类型，因此没有此property

class QTreeWidgetItemProxy : public QObject
{
  Q_OBJECT
  Q_PROPERTY(int  childCount READ childCount)
public:
  explicit QTreeWidgetItemProxy(QObject *parent = 0);
  ~QTreeWidgetItemProxy();
  Q_INVOKABLE QString itemText(int column);
  Q_INVOKABLE QTreeWidgetItemProxy* child(int index);
  int  childCount(void);

  void setQTreeWidgetItem(QTreeWidgetItem* item) {m_item=item;}
signals:

public slots:
private:
  QTreeWidgetItem *item(){return m_item;}
private:
  QTreeWidgetItem* m_item;
  QTreeWidgetItemProxy *m_childItem;
};

#endif // QTREEWIDGETITEMPROXY_H
