#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui {
class DialogPickCurve;
}
class PlotWave;
class MainWindow;
class QTreeWidgetItem;
class QTableWidget;
class QComboBox;
class QMenu;
class UsrCurveTreeManager;
class CustomCurvePage;

class DialogPickCurve : public QDialog
{
  Q_OBJECT

public:
  explicit DialogPickCurve(PlotWave *plotWidget,QWidget *parent = 0);
  ~DialogPickCurve();
  QTableWidget *getSelectAxisTableWidget(void) const;

signals:
  void expertTreeItemDoubleClicked(QTreeWidgetItem *item,int column);
  void usrTableCellDoubleClicked(int row, QTableWidget* axisTable,QComboBox* comboBox);
  void deleteScriptCurveAtId(int id);//发送给PlotWave窗口，通知其删除该id曲线（数据列表，表格显示，customplot)

protected:

private slots:
  void onBtnUserRoleClicked(void);
  void onTreeItemExpanded(QTreeWidgetItem* item);
  void onUsrTableCellDoubleClicked(int row, int column);
  void onTablePopMenu(QPoint pos);
  void onTablePopActionClicked();//右键弹出删除对应的行,发送信号deleteScriptCurveAtId给主画图类，通知其删除有这个曲线的信息，并修改ID

  //slots customcurvepage
  void onCustomPageResizeGeometry(bool isShow);
  void onAddScriptCurve(QTreeWidgetItem *item);
private:
  void initailUi(void);
  void addUserTableCurveRow(int i, QTreeWidgetItem *itemCurveRow);
private:
  Ui::DialogPickCurve *ui;
  PlotWave *mp_plotWave;
  MainWindow *mp_mainWindow;
  QMenu *m_popuMenuTable;
  UsrCurveTreeManager *mp_usrCurveTreeManager;
  CustomCurvePage *m_customCurvePageUi;

};

#endif // DIALOG_H
