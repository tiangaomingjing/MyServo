#ifndef FPGADIALOGSETTING_H
#define FPGADIALOGSETTING_H

#include <QWidget>
#include <QTreeWidgetItem>
#include <QDebug>
namespace Ui {
  class FpgaDialogSetting;
}
class QTreeWidget;
class MainWindow;
class HighlightText;

class FpgaDialogSetting : public QWidget
{
	Q_OBJECT

public:
	//FpgaDialogSetting(QWidget *parent = 0);
  explicit FpgaDialogSetting( QTreeWidget *tree,MainWindow *mainWindow,QWidget *parent=0);
	~FpgaDialogSetting();

private slots:
	void onExpandRefresh(QTreeWidgetItem *item);
	void onDecHexRadioClicked(void);
	void onTreeWidgetItemClicked(QTreeWidgetItem* item, int col);
  void onTreeItemChanged(QTreeWidgetItem *item,int column);
  void onItemSelectionChanged();
	void onExecuteEnterKey(void);

private:
	int getTopLevelItemIndex(QTreeWidgetItem *item);
	void findTopLevelItem(QTreeWidgetItem *item);
	quint32 operationValue(quint32 bitstart, quint32 bitwidth);
	QString uiTreeWidgetItemUpdateFormat(const quint32 value, quint8 hexWidth,bool hasOX);
	bool eventFilter(QObject *watchObj, QEvent *eve);
	qint32 getValueFromItemText(QTreeWidgetItem *item,int col);

private:
	Ui::FpgaDialogSetting *ui;
	QTreeWidgetItem *m_currentParentItemPtr;
  HighlightText *m_hightlightText;
  MainWindow *mp_mainWindow;
};

#endif // FPGADIALOGSETTING_H
