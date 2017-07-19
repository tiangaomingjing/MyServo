#include "treeitemproxy.h"
#include "treeitem.h"

TreeItemProxy::TreeItemProxy(QObject *parent) : QObject(parent),
  m_child(new TreeItemProxy(this))
{
}
QVariant TreeItemProxy::data(int column) const
{
  return m_treeItem->data(column);
}
TreeItemProxy * TreeItemProxy::child(int number)
{
  m_child->setTreeItem(m_treeItem->child(number));
  return m_child;
}
