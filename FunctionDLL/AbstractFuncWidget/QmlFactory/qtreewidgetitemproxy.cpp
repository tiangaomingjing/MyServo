#include "qtreewidgetitemproxy.h"
#include <QTreeWidgetItem>

QTreeWidgetItemProxy::QTreeWidgetItemProxy(QObject *parent) : QObject(parent),
  m_childItem(new QTreeWidgetItemProxy(parent))
{

}
QTreeWidgetItemProxy::~QTreeWidgetItemProxy()
{

}

QString QTreeWidgetItemProxy::itemText(int column)
{
  return m_item->text(column);
}
QTreeWidgetItemProxy* QTreeWidgetItemProxy::child(int index)
{
  m_childItem->setQTreeWidgetItem(m_item->child(index));
  return m_childItem;
}

int  QTreeWidgetItemProxy::childCount()
{
  return m_item->childCount();
}
