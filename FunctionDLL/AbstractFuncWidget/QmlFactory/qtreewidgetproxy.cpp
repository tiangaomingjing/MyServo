#include "qtreewidgetproxy.h"
#include "qtreewidgetitemproxy.h"
#include <QTreeWidget>

QTreeWidgetProxy::QTreeWidgetProxy(QTreeWidget *treesrc, QObject *parent) : QObject(parent),
  m_treeWidget(treesrc)
{

}

int  QTreeWidgetProxy::topLevelItemCount()
{
  return m_treeWidget->topLevelItemCount();
}

QString QTreeWidgetProxy::textTopLevel(int index,int columnIndex)
{
  return m_treeWidget->topLevelItem(index)->text(columnIndex);
}

QString QTreeWidgetProxy::textChild(int toplevelIndex,int childIndex,int columnIndex)
{
  return m_treeWidget->topLevelItem(toplevelIndex)->child(childIndex)->text(columnIndex);
}
void QTreeWidgetProxy::setTopLevelText(int index, int columnIndex, QString text)
{
  m_treeWidget->topLevelItem(index)->setText(columnIndex,text);
}
