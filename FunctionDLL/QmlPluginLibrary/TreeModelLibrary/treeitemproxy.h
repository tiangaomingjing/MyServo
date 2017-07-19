#ifndef TREEITEMPROXY_H
#define TREEITEMPROXY_H

#include <QObject>
#include <QVariant>
class TreeItem;

class TreeItemProxy : public QObject
{
  Q_OBJECT
public:
  explicit TreeItemProxy(QObject *parent = 0);
  Q_INVOKABLE void setTreeItem(TreeItem *item){m_treeItem=item;}

  Q_INVOKABLE QVariant data(int column) const;
  Q_INVOKABLE TreeItemProxy *child(int number);

signals:

public slots:
private:
  TreeItem *m_treeItem;
  TreeItemProxy *m_child;
};

#endif // TREEITEMPROXY_H
