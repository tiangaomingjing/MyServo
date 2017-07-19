#ifndef FPGADIALOGSETTINGRNNET_H
#define FPGADIALOGSETTINGRNNET_H

#include <QWidget>
#include <QMap>

namespace Ui {
class FpgaDialogSettingRnNet;
}
class QTreeWidget;
class MainWindow;
class HighlightText;
class QTreeWidgetItem;

class FpgaDialogSettingRnNet : public QWidget
{
  Q_OBJECT

public:
  explicit FpgaDialogSettingRnNet(QTreeWidget *tree,MainWindow *mainWindow,QWidget *parent = 0);
  ~FpgaDialogSettingRnNet();
  enum TREE_COLUMN_INDEX{
    TREE_COLUMN_NAME,TREE_COLUMN_VALUE,TREE_COLUMN_ADDR,TREE_COLUMN_TYPE,
    TREE_COLUMN_BITSTART,TREE_COLUMN_BITWIDTH,TREE_COLUMN_OPERATION,TREE_COLUMN_NOTE,
    TREE_COLUMN_COUNT};
private slots:
  void onExpandRefresh(QTreeWidgetItem *item);
  void onDecHexRadioClicked(void);
  void onTreeWidgetItemClicked(QTreeWidgetItem* item, int col);
  void onTreeItemChanged(QTreeWidgetItem *item,int column);
  void onItemSelectionChanged();
  void onExecuteEnterKey(void);
private:
  void setTreeWidgetStyleSheet();
  quint32 operationValue(quint32 bitstart, quint32 bitwidth);
  quint16 getParameterDSPNumberByTopLevelItem(QTreeWidgetItem *item);
  QTreeWidgetItem *getTopLevelItem(QTreeWidgetItem *item);
  QString uiTreeWidgetItemUpdateFormat(const quint32 value, quint8 hexWidth,bool hasOX);
  bool eventFilter(QObject *watchObj, QEvent *eve);
  qint32 getValueFromItemText(QTreeWidgetItem *item,int col);

private:
  Ui::FpgaDialogSettingRnNet *ui;
  QTreeWidgetItem *m_currentParentItemPtr;
  HighlightText *m_hightlightText;
  MainWindow *mp_mainWindow;
  QMap<QString ,int>m_prmMap;
};

#endif // FPGADIALOGSETTINGRNNET_H
