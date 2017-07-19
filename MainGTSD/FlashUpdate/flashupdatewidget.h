#ifndef FLASHUPDATEWIDGET_H
#define FLASHUPDATEWIDGET_H
#define FLAG_NEW 0x02
#define FLAG_OLD 0x01

#include <QWidget>
#include<QTreeWidgetItemIterator>
#include<QXmlStreamWriter>

namespace Ui {
class FlashUpdateWidget;
}
class QTreeWidget;
class QTreeWidgetItem;
class QXmlStreamReader;
class FlashUpdateWidget : public QWidget
{
  Q_OBJECT

public:
  explicit FlashUpdateWidget(QWidget *parent = 0);
  ~FlashUpdateWidget();

private slots:
  void onBtnExitClicked(void);
  void onBtnOldClicked(void);
  void onBtnNewClicked(void);
  void onBtnUpdateXML(void);
  void onBtnCompareClicked(void);
  void onTreeNewDoubleClicked(QTreeWidgetItem*item,int column);
  void onCheckedBoxSyncClicked(bool checked);

private:
  bool loadTree(QString &filename, QTreeWidget *treeWidget);
  bool saveXMLFile(QTreeWidget* tree);
  bool eventFilter(QObject *watchObj, QEvent *event);
  void setTreeViewMoveSync(bool sync);

  void delayms(quint16 ms);
private:
  Ui::FlashUpdateWidget *ui;
  quint8 m_flagNewOld;//点成功加载新，旧标志:new 1<<1 old 1<<0

  enum COL_INDEX:quint8
  {
    COL_NAME_INDEX,
    COL_VALUE_INDEX,
    COL_OFST_INDEX,
    COL_TYPE_INDEX,

    COL_TOTAL
  };

  long m_totalItemNumber;
  QString m_lastFilePathName;
};

#endif // FLASHUPDATEWIDGET_H
