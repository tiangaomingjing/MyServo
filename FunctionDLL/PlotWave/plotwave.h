#ifndef PLOTWAVE_H
#define PLOTWAVE_H

#include "plotwave_global.h"
#include "globaldef.h"
#include "plotwave_globaldef.h"
#include "MotionStatus/motionstatus.h"
#include "MuParser/include/muParser.h"
#include "UserActionWidget/useractionwidget.h"

#include <QWidget>
#include <QList>
#include <QJSValue>
#include <QVector>

namespace Ui {
class PlotWave;
}
class MainWindow;
class QTableWidgetItem;
class DlgSetXYRange;
class BoxZoom;
class MeaCursorX;
class MeaCursorY;
class MeasureTag;
class QTreeWidget;
class DlgSelectCurve;
class ThreadSample;
class QDoubleSpinBox;
class DftWidget;
class ServoGeneralCmd;
class QListWidgetItem;
class QProgressBar;
class QJSEngine;
class QTableWidget;
class QComboBox;
class UsrCurveTreeManager;
class RollBoxWidget;

class TableParameters
{
public:
  TableParameters() {}
public:
  bool isDraw;
  QString fullName;
  QColor color;
  int bytes;
  int axisNum;
  quint16 offsetAddr;
  QVector<double>curveKey;
  QVector<double>curveValue;
};
QDataStream &operator<<(QDataStream &out,const TableParameters &par);
QDataStream &operator>>(QDataStream &in,TableParameters &par);

/**
 * @brief The ServoStatus class 记录tab1的伺服状态,主要是用来保存
 */
class ServoStatus
{
public:
  ServoStatus() {}
  bool isOn;
  quint16 currentModeIndex;
  double m_posAdj;
  double m_ua;
  double m_ub;
  double m_uc;
  double m_ud;
  double m_uq;
  double m_id;
  double m_iq;
  double m_closeVel;
  double m_profileVel;
  double m_syncVel;
  double m_posFollow;
};

class PLOTWAVESHARED_EXPORT PlotWave:public QWidget
{
Q_OBJECT
public:
  PlotWave(QWidget *parnet);
  ~PlotWave();

  QList<PlotTablePrm> &getTableParameterList(void){return m_tablePlotPrmList;}
  void updateTablePlotPrmListConstValueWhenSampling(void);

  ViewSetting &getViewSetting(void) {return m_viewSetting;}
  UserConfig *getUserConfigFromMainWidow(void);
  double getSamplingTime(void);
  void updateCurrentServoStatus();//在主界面中点击连接时调用

  QTreeWidget *getUserCurveTree() const;
  UsrCurveTreeManager *getUsrCurveTreeManager() const;

  MainWindow *getMainWindow() const;

  void resetPlotWarve(void);

  quint16 getCurveBaseAddress(const QString&name);



signals:
  void showMax(bool max);
  void measureXLeftChanged(double value);
  void measureXRightChanged(double value);

public slots:

  void onClearPlotWave(void);//连接到主窗口中新建版本的更新请求

  void onStopThreadSampling();//连接到主窗口中停止采样请求（发生断线时,如果现在正在采样的话）

private slots:

  //----------------------table widget--------------------------------------------
  void onCellEnteredMoreDetail(int row, int col);//鼠标悬停，可以显示详细的名称
  void onTableItemClicked(QTableWidgetItem * item);
  void onTableItemDoubleClicked(QTableWidgetItem * item);
  void onTableItemChanged(QTableWidgetItem * item);
  void onTableHorizontalHeaderDoubleClicked(int index);
  void onTablePopMenu(QPoint pos);//响应表格右键弹出
  void onTablePopuMenuClicked();

  //-----------------Btn按钮相关---------------------------------------------------
  void onBtnSelectAllClicked(void);
  void onBtnAutoScaleClicked(bool checked);
  void onBtnResetClicked(void);
  void onBtnHorizontalMeasureClicked(bool checked);
  void onBtnVerticalMeasureClicked(bool checked);
  void onBtnViewMinClicked(void);
  void onBtnViewMaxClicked(void);
  void onBtnViewSettingClicked(void);
  void onBtnRemoveClicked(void);
  void onBtnClearClicked(void);
  void onBtnPrevViewClicked(void);
  void onBtnNextViewClicked(void);
  void onBtnTagClicked(bool checked);
  void onBtnAddClicked(void);
  void onBtnStartClicked(void);//开启采样
  void onBtnOpenClicked(bool checked);//打开保存文件
  void onBtnSaveClicked();//保存文件
  void onBtnFFTClicked();
  void onBtnMeasureXLeftClicked(bool checked);//2017-1-10变化为左右的切换
  void onBtnMeasureXRightClicked(bool checked);//2017-1-10变化为全选
  void onBtnMeasureYDownClicked(bool checked);//2017-1-10变化为上下的切换
  void onBtnMeasureYUpClicked(bool checked);//2017-1-10变化为全选

  void onViewSettingUpdate(ViewSetting setting);//响应设置视图窗口

  //-------------------------------增加曲线对话相关--------------------------------------------------------
  void onTreeWidgetItemDoubleClicked(QTreeWidgetItem *item,int column);//双击增加树专家页
  void onUsrTableWidgetCellDoubleClicked(int row, QTableWidget* axisTable, QComboBox* unitComboBox);//双击曲线表用户页
  void onDeleteScriptCurveIdAt(int id);

  //----------customplot-------------------------------------------
  void onPlotStyleChanged(int index);
  void onPlotMouseMoveEvent(QMouseEvent *event);
  void onPlotMousePressEvent(QMouseEvent *event);
  void onPlotMouseReleaseEvent(QMouseEvent *event);
  void onPlotMouseWheelEvent(QWheelEvent *event);

  //!接收画图曲线发出来的数据，做相应的处理
  void onSendPlotData(int dspNumber, int axisFirstCount, long getDataLength, CurveData data);
  void onSamplingStopEvaluateScript(void);//停止画图后解析曲线数据还原


  //tab 1-----------------------------------------------------------------
  void onListWidgetModeIndexChanged(int index);
  void onListWidgetAxisClicked(int index);//轴号列表选择
  void onDoubleSpinBoxValueChanged(double value);
  void onBtnServoOnClicked(bool checked);//打开伺服
  void onTest1(double value);
  void onTest2(double value);
  void onDftWidgetClose();//响应dft分析窗口关闭事件，主要是关了游标
  void onDftSaveCurve(QString &fileName, QVector<quint16> &curveIndexVector);
  void onShowCurrentModeInfo(int axis,int index);//当滚轮拖拉，鼠标悬停时显示信息
  void onRollBoxBtnClicked(int axis);
  void onRollBoxStopedAt(int axis,int index);
  void onTabWidgetCurrentChanged(int index);//响应tab切换


  //tab2-----------------------------------------------------------
  void onUpdateMotionTabUi(int axisIndex);//当选择listWidget轴改变时，根据控制源..更新tab2界面
  void onListWidgetMotionSrcTab2ItemClicked(QListWidgetItem *item);
  void onBtnControlSourcePCClicked(void);
  void onBtnConrtolSourceGLink2Clicked(void);
  void onCheckBoxVelocityClicked(bool checked);
  void onBtnMotionRunClicked(bool run);//开始运动
  void onMotionStatusPlanVelTimeOut(int axisIndex);

protected:
  bool eventFilter(QObject *obj, QEvent *event);
private:
  void setUiStyleSheet();
  void initialUi(void);//初始化Ui
  QTreeWidget* readTreeWidgetFromFile(QString &filename);
  bool readInitialFile(QString &fileName);//从 plot配置文件中更新 m_viewSetting m_tablePlotPrmList
  void setTableRowPrm(int row,PlotTablePrm tablePrm);
  void resetTableWidget(void);
  void updateTableWidgetFromFile(QString &fileName);
  void updateUserStyleComboBox(QStringList &userList);
  void updatePlotView(ViewSetting *view);
  void initialPlotCurve(QList<PlotTablePrm> & tablePrmList);
  void writePlotWaveFile();
  void clearGraphData(void);
  void drawMorePoints(void);//框选显示更多细节
  void setListViewAxisNum(int axisCount);
  QStringList tableCurveNameList();//获得列表单的所有曲线的名字
  void saveCurve(QString &fileName, QVector<quint16> &curveIndexVector);
  void addCurve(TableParameters &pars);
  void initialServoStatus(quint16 axixCount);
  void updateOffsetAddrWhenAxisNumberChanged(int axisIndex,int row);
  void addTableMenuAction(quint16 axisCount);
  void clearTableMenuAction();
  void enableUi(bool value);
  void changeUiWhenOpenClicked(bool checked);
  void setCurveTableWidgetOnOffUi(int row, bool isON);
  void evaluateCurveSamplingScript(void);
//  void updataUserCurveTreeList();//根据实际配置版本，更新用户曲线多轴模板地址
  //tab1 ---------------
  void recoverStackWidgetUiData(int index, int axis);
  void setServoOnUi(bool on);

  //add curve ui-------------
  void clearTreeWidgetList(QList<QTreeWidget *> &treeList);
  void changeUsrTreeBySrcTree(QList<QTreeWidget *> &m_usrTreeList, const QTreeWidget *srcAllTree);
  bool drawCurveNumberIsBigger(int compareNum, int &totalCurveNum);
  double readRAMValue(int axisIndex, quint16 offsetAddr, quint16 baseAddr, int bytsNum);//用于读常量系数

  //tab2  ui------------
  void setControlSourceIndexUi(int index);
  void setMotionModeIndexUi(int axisIndex, MotionStatus::MotionMode motionMode);
  void setBtnMotionRunEnable(MotionStatus::MotionMode motionMode, MotionStatus::ControlSource ctlSrcIndex);


private:
  typedef enum{
    TABWIDGET_INDEX_MODE,
    TABWIDGET_INDEX_MOTION,
    TABWIDGET_INDEX_COUNT
  }TabWidgetIndex;

  Ui::PlotWave *ui;
  QList<PlotTablePrm> m_tablePlotPrmList;//表格曲线的row参数，所有的曲线都在这个list里
  ViewSetting m_viewSetting;//窗口参数
  quint16 m_currentViewIndex;
  QVector<QColor> m_colorDefault;//默认颜色,最多8条曲线
  UserConfig *mp_userConfig;//主窗口的用户信息指针，主要是为了设置plot相关的曲线
  MainWindow *mp_mainWindow;
  QTreeWidget *mp_treeAll;//从主窗口中获得轴的树形结构
  DlgSetXYRange *m_dlgSetting;
  DlgSelectCurve *m_dlgSelectCurve;
  BoxZoom *m_boxZoom;
  MeaCursorX *m_cursorX;
  MeaCursorY *m_cursorY;
  MeasureTag *m_tag;
  ThreadSample *m_threadSample;
  bool m_isZoomFirst;
  bool m_isEnterSubmit;
  DftWidget *m_dftWidget;
  QList<ServoStatus> m_servoStatusList;
  QMenu *m_popuMenuTable;
  QList<MotionStatus*> m_motionStatusPtrList;//用于记录tab2状态及更改控制源；换版本时要销毁原来的 重新生成新对象
  QList<QProgressBar *>m_progressBarList;
  ServoGeneralCmd *m_generalCmd;//用于通用指令；程序新建时生成，换软件版本，换通信端口时要更新

  QTreeWidget *m_curveSettingTree;//记录每一个退出时的曲线控制数据
  QTreeWidget *m_usrCurveTreeTemplate;//单个轴的用户曲线模板
  //多个轴的用户曲线信息，由单个轴的生成，然后根据实际的树改变相关地址
  //(使用条件 1:双击增加时,索引其信息 2：右键时索引信息)
  //随配置版本改变而改变
  QList<QTreeWidget *>m_usrCurveTreeList;
  UsrCurveTreeManager *m_usrCurveTreeManager;

  RollBoxWidget *m_rollBoxUi;
  QStringList m_servoModeNameList;


};

#endif // PLOTWAVE_H
