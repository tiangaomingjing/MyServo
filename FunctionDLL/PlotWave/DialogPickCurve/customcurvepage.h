#ifndef CUSTOMCURVEPAGE_H
#define CUSTOMCURVEPAGE_H

#include <QWidget>
class QTreeWidget;
class QTreeWidgetItem;

namespace Ui {
class CustomCurvePage;
}

class CustomCurvePage : public QWidget
{
  Q_OBJECT

public:
  explicit CustomCurvePage(QWidget *parent = 0);
  ~CustomCurvePage();
  void setCurveTreeTemplate(QTreeWidget* treeTemplate);
  void setCurveTreeRAM(QTreeWidgetItem *treeRam);
signals:
  void resizeGeometry(bool isShow);
  void addScriptCurve(QTreeWidgetItem *curveItem);
private slots:
  void onTreeWidgetMouseMoveEvent(int x,int y);
  void onBtnAddClicked(void);
  void onBtnHideClicked(void);
  void onBtnRemoveClicked(void);
  void onBtnNewClicked(void);
  void onBtnSaveClicked(void);
  void onLabelStatusHide(void);

  void onTreeWidgetInputClicked(QTreeWidgetItem *item,int column);
  void onTreeWidgetInputDoubleClicked(QTreeWidgetItem *item,int column);
  void onTreeWidgetVariableDoubleClicked(QTreeWidgetItem *item,int column);
  void onTreeWidgetConstDoubleClicked(QTreeWidgetItem *item,int column);
  void onTreeWidgetItemExpanded(QTreeWidgetItem *item);

private:
  QTreeWidgetItem* findTopLevelItem(QTreeWidgetItem *item);
  void sciAddKeyword(QString str);
  void initialCurveTreeWidgetItem(void);
  void parserPattern(const QString &srcText,const QString &pattern, QStringList &list);
private:
  Ui::CustomCurvePage *ui;
  QTreeWidget *mp_treeTemplate;
  QTreeWidget *m_treeRAM;
  QTreeWidgetItem *m_curveItem;
  QStringList m_sciMathKeywordsList;

};

#endif // CUSTOMCURVEPAGE_H
