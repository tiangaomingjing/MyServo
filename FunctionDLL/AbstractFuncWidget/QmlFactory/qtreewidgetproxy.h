#ifndef QTREEWIDGETPROXY_H
#define QTREEWIDGETPROXY_H

#include <QObject>
class QTreeWidget;

class QTreeWidgetProxy : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QTreeWidget * treeWidget READ treeWidget WRITE setTreeWidget)
  Q_PROPERTY(int  topLevelItemCount READ topLevelItemCount)
public:
  explicit QTreeWidgetProxy(QTreeWidget *treesrc=0,QObject *parent = 0);

  void setTreeWidget(QTreeWidget *treesrc){m_treeWidget=treesrc;}
  QTreeWidget * treeWidget(){return m_treeWidget;}
  int  topLevelItemCount(void);

  Q_INVOKABLE QString textTopLevel(int index,int columnIndex);
  Q_INVOKABLE QString textChild(int toplevelIndex,int childIndex,int columnIndex);
  Q_INVOKABLE void setTopLevelText(int index,int columnIndex,QString text);

signals:

public slots:
private:
  QTreeWidget *m_treeWidget;//这里不会生成树，只是指向已有的树
};

#endif // QTREEWIDGETPROXY_H
