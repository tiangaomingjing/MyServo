#ifndef DLGSELECTCURVE_H
#define DLGSELECTCURVE_H

#include <QObject>
#include <QDialog>
namespace Ui {
  class DlgSelectCurve;
}
class QTreeWidgetItem;
class QTreeWidget;
class DlgSelectCurve : public QDialog
{
  Q_OBJECT
public:
  DlgSelectCurve(QTreeWidget *srcTree,QWidget *parent=0);
  ~DlgSelectCurve();
signals:
  void treeItemDoubleClicked(QTreeWidgetItem *item,int column);
private slots:
  void onBtnExitClicked(void);
  void onTreeItemExpanded(QTreeWidgetItem* item);
private :
  void initialUi(void);
  void setTreeStyle(void);
private:
  Ui::DlgSelectCurve *ui;
  QTreeWidget *mp_tree;
};

#endif // DLGSELECTCURVE_H
