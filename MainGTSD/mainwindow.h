#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QMessageBox>

#include "ServoDriverComDll.h"
#include "xmlbuilder.h"

namespace Ui {
class MainWindow;
class status;
}
class QAction;
class QMenu;
class NewConfigDialog;
class FlashUpdateWidget;
class PlotWave;
class ModuleIO;
class XmlBuilder;
class QTreeWidget;
class MAction;
class QSplashScreen;
class AxisClone;
class QTimer;
class FpgaDialogSetting;
class QQuickView;
class QmlUserConfigProxy;
class ConnectDataBase;
class FpgaDialogSettingRnNet;
class AbstractFuncWidget;

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QSplashScreen *screen,QWidget *parent = 0);
  ~MainWindow();

  UserConfig* getUserConfig(void){ return mp_userConfig;}
  QTreeWidget *getFlashAllAxisTree(void){return mp_flashAllTreeWidget;}
  QTreeWidget *getRamAllAxisTree(void){return mp_ramAllTreeWidget;}
  QTreeWidget *getFunctionCmdTree(void){return mp_functionCmdTreeWidget;}
  QTreeWidget *getFunctionExtensionTree(void){return mp_funcExtension;}

  QMap<QString,QVariant> * getModuleShareMapData(void){return &m_moduleShareData;}
  bool getComOpenState(void){return m_isOpenCom;}
  void stopTimer(){m_timer->stop();}
  void startTimer(){m_timer->start();}

  static QString g_lastFilePath;

public slots:
  void onWarningMessageChanged(QString &msg);
  void onClearWarning(void);

signals:
  void updateProgressBar(int value);//这个用于显示新建方案时的进度条
  void updateTreeWidgetValue();//连接到子窗口中的更新树updateTreeWidgetValuefromServo() no necessary to use now
  void startUpMessage(QString message,int textAlign,QColor clr);
  void writeFuncTreetoServoRam();
  void actionReadFuncValueRam();
  void writeFuncTreetoServoFlash();
  void timeOut(int axis);//用于更新状态显示
  void stopThreadSampling();//用于当断线时，关闭采样线程


private slots:
  void onTreeWidgetItemClicked(QTreeWidgetItem *item,int column);
  void onComboBoxAxisSelectClicked(int index);
  //----------菜单响应slots---------------
  void onActionConfigNewClicked(void);
  void onActionConfigOpenClicked(void);
  void onActionConfigSaveClicked(void);
  void onActionConfigSaveAsClicked(void);
  void onActionConfigHistoryClicked(void);
  void onActionConfigExitClicked(void);
  //connect
  void onActionConnectClicked(void);
  void onActionDisConnectClicked(void);
  //view
  void onActionViewNavigation(void);
  void onActionViewPlotCurve(void);
  void onActionViewFullScreen(void);
  //tool
  void onActionToolXmlUpdateClicked(void);
  void onActionFile2ServoClicked(void);
  void onActionServo2FileClicked(void);
  void onActionAxisCloneClicked(void);
  void onActionAxisFileCloneClicked(void);
  void onActionFuncConfigClicked(void);
  void onActionFuncSaveClicked(void);
  void onActionProgramDownloadClicked(void);
  void onActionResetDSPClicked(void);
  void onActionNormalizeTreeClicked();
  void onActionFPGAConfigurationClicked();
  void onActionRestoreFactorySettingClicked(void);
  //help
  void onActionAboutConfigClicked();
  void onActionAboutSDTClicked();
  //-----------新建配置--------------
  void onNewConfigurationActived(UserConfig *config);//槽连接到新建窗口发射的信号,响应新建配置

  void onPlotWaveFloatingShow(bool isfloat);//响应绘图窗口的+-放大缩小信号

  //------响应断线--------
  void onComDisconnected(COM_ERROR error);
  //------时间到发射更新信号到Qml--------
  void onTimeOut();
  //有报警信息
  void onAlmError();
  void onAlmClearFinish();

  //-----打开配置向导-----
  void onBtnConfigGuideClicked();

  //QML slot
  void onQmlNavigationClose();
  void onQmlNavigationServoConfigUpdate();

  //烧写参数时进度条处理
  void onXmlPrmToServo(int axis,int value);

protected:
  void keyPressEvent(QKeyEvent *keyEvent);

private:
  void createMenus(void);
  void createActions(void);
  void createToolBars(void);
  void createConnections(void);
  void createStatusUi(void);
  void initialUi(void);
  void updateUiByUserConfig(UserConfig *theconfig, SysConfig *srcConfig);//根据配置更新每一个界面
  void createHistoryAction(void);
  void setConfigSaveEnableStatus(AbstractFuncWidget *absWidget);

  void updateStartUpMessage(QString message);
  void closeEvent(QCloseEvent *event);
  void enableAllUi(bool state);
  void setUbootModeUi(bool sta);

  void setComConnectStatus(bool isConnected);

  static void updateProgessBarWhenRestoreClicked(void *arg,qint16 *value);//只用于点击恢复出厂设置时显示进步条
  static void updateProgessBarWhenConnectClicked(void *arg,qint16 *value);//只用于点击连接时显示进步条
  bool openNetCom(void);
  void closeNetCom(void);



private:
  Ui::MainWindow *ui;
  Ui::status *uiStatus;
  //------配置方案action------
  QAction *m_actConfigNew;
  QAction *m_actConfigOpen;
  QAction *m_actConfigSave;
  QAction *m_actConfigSaveAs;
  QAction *m_actConfigExit;
  QList<QAction *>m_actConfigHistory;

  //-------连接action--------
  QAction *m_actConnect;
  QAction *m_actDisConnect;

  //-------view action--------
  QAction *m_actNavigation;
  QAction *m_actPlotCurve;
  QAction *m_actFullScreen;

  //--------tool action--------
  QAction *m_actXmUpdate;
  MAction *m_actFile2Servo;
  QAction *m_actFileservo;//与m_actFile2Servo是一样的，只不过是在menu中
  MAction *m_actServo2File;
  QAction *m_actServofile;//与m_actServo2File是一样的，只不过是在menu中
  QAction *m_actAxisClone;
  QAction *m_actAxisFileClone;
  QAction *m_actFPGAControl;
  QAction *m_actFuncConfig;
  QAction *m_actFuncSave;
  //程序烧写
  QAction *m_actProgramUpdate;
  QAction *m_actResetServo;
  QAction *m_actRestoreFactorySetting;

  QAction *m_actNormalizeTree;

  //--------help action------
  QAction *m_actAboutConfig;
  QAction *m_actAboutSDT;

  //--------菜单--------------
  QMenu *m_menuConfig;
  QMenu *m_menuConfigRecent;
  QMenu *m_menuView;
  QMenu *m_menuTool;
  QMenu *m_menuHelp;
  //--------toolbar-----------
  QToolBar *m_toolBarView;
  QToolBar *m_toolBarTool;

  XmlBuilder *m_xml;
  UserConfig *mp_userConfig;//用户自己的信息
  FileRecent *mp_fileRecent;//文件列表信息
  SysConfig  *mp_srcAllConfig;//读取的原始xml树文件指针 所有的树信息
  int m_itemSize;//总共的树形配置项

  //相关设置对话框
  NewConfigDialog *m_newConfigDialog;
  FlashUpdateWidget *m_flashUpdateWidget;
  PlotWave *m_plotWave;
  AxisClone *m_axisCloneWidget;
  FpgaDialogSetting *m_fpgaDialogSetting;
  FpgaDialogSettingRnNet *m_fpgaDialogSettingRnNet;

  QTreeWidget *mp_flashAllTreeWidget;
  QTreeWidget *mp_ramAllTreeWidget;
  QTreeWidget *mp_functionCmdTreeWidget;
  QTreeWidget *mp_funcExtension;
  bool m_isOpenCom;//是否打开通信
  QTimer *m_timer;

  //配置向导QML UI
  QQuickView *m_quickView;
  QmlUserConfigProxy *m_userConfigProxyQml;
  ConnectDataBase* m_dataBase;

  static int m_progessValue;//只用于点击恢复出厂设置时显示进步条
  static int m_dspNum;
  static int m_step;

  QMap<QString,QVariant> m_moduleShareData;//多个dll模块中的共享数据

};
#endif // MAINWINDOW_H
