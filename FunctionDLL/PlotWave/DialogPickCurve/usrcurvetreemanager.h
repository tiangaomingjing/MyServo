#ifndef USRCURVETREEMANAGER_H
#define USRCURVETREEMANAGER_H

#include <QObject>
class QTreeWidget;
class QTreeWidgetItem;

class UsrCurveTreeManager : public QObject
{
  Q_OBJECT
public:
  explicit UsrCurveTreeManager(QTreeWidget *srcAllTreePtr,QString &curveTemplateFileName,QObject *parent = 0);
  ~UsrCurveTreeManager();

  void setNewSrcAllTree(QTreeWidget *srcNewTree);//当版本改变时调用
  QTreeWidget *usrTreeTemplate() const;
  QList<QTreeWidget *> usrTreeList() const;
  void addTreeCurve(QTreeWidgetItem *itemCurve);
  bool removeTreeCurve(int index);
  void saveTreeCurveTemplate(QString &fileName);

signals:

public slots:
private:
  void clearUsrTreeWidgetList();
  void createUsrTreeListBaseAxisSize(int axisSize);
  void updateUsrTreeList(void);
  void updateTreeItemOffsetAddrBySrcTree( QTreeWidgetItem *item,QTreeWidget *treeSrcAxis);
  void removeTreeItem(int index, QTreeWidget *treeWidget);

private:
  QTreeWidget *mp_srcAllTree;
  QTreeWidget *m_usrTreeTemplate;
  QList<QTreeWidget *>m_usrTreeList;
  QString m_treeFileName;
  int m_currentAxisNumber;


};

#endif // USRCURVETREEMANAGER_H
