#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "functioninc.h"
#include "NewConfigDialog/newconfigdialog.h"
#include "FlashUpdate/flashupdatewidget.h"
#include "MAction/maction.h"
#include "AxisClone/axisclone.h"
#include "plotwave.h"
#include "moduleio.h"
#include "ui_status.h"
#include "xmlbuilder.h"
#include "BaseReturn_def.h"
#include "servocontrol.h"
#include "AxisClone/axisfileclone.h"
#include "DownloadProgram/downloaddialog.h"
#include "NormalizeTree/normalizetreeform.h"
#include "FpgaDialogSetting/fpgadialogsetting.h"
#include "FpgaDialogSetting/fpgadialogsettingrnnet.h"
#include "globaldef.h"
#include "./NavigationConfig/UserConfigProxyClass/qmluserconfigproxy.h"
#include "./NavigationConfig/DataBase/ConnectDataBase/connectdatabase.h"
#include "./NavigationConfig/DataBase/SqltableModel/sqltablemodel.h"
#include "./NavigationConfig/ControlName/controlname.h"
#include "./Uboot/netconfig.h"
#include "./Uboot/ubootdialog.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QWidgetAction>
#include <QSplashScreen>
#include <QTimer>
#include <QFormBuilder>
#include <QQuickView>
#include <QQmlContext>
#include <QQuickItem>
#define TEST_DEBUG 1
#define FILENAME_MODULEIO "PrmFuncIO"
#define FILENAME_FUNCCMD "PrmFuncCmd"
#define FILENAME_FLASHALL "FlashPrm_AllAxis"
#define FILENAME_RAMALL "RamPrm_AllAxis"
#define FILENAME_FUNCEXT "PrmFuncExtension"
#define SDT_VERSION "1.1.1"

QString MainWindow::g_lastFilePath="./";
int MainWindow::m_progessValue=0;
int MainWindow::m_dspNum=2;
int MainWindow::m_step=0;

enum COL_NAVIGATION_TREE_INDEX
{
  COL_NAVIGATION_TREE_NAME=0,
  COL_NAVIGATION_TREE_CLASSNAME,
  COL_NAVIGATION_TREE_PRMFILE,
  COL_NAVIGATION_TREE_UIINDEX,
  COL_NAVIGATION_TREE_GLOBAL,

  COL_NAVIGATION_TREE_COUNT
};
enum GLOBAL_UI_INDEX
{
  GLOBAL_UI_PLOTVIEW=0,
  GLOBAL_UI_IOMODULE,

  GLOBAL_UI_COUNT
};
enum UI_TAB_TYPE
{
  UI_TAB_TYPE_AXIS=0,
  UI_TAB_TYPE_GLOBAL
};

MainWindow::MainWindow(QSplashScreen *screen,QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  uiStatus(new Ui::status),
  m_newConfigDialog(NULL),
  m_itemSize(0),
  m_flashUpdateWidget(NULL),
  mp_flashAllTreeWidget(NULL),
  mp_ramAllTreeWidget(NULL),
  mp_functionCmdTreeWidget(NULL),
  mp_funcExtension(NULL),
  m_isOpenCom(false),
  m_fpgaDialogSetting(NULL),
  m_fpgaDialogSettingRnNet(NULL),
  m_quickView(NULL),
  m_userConfigProxyQml(NULL)
{
  m_moduleShareData.clear();
  ui->setupUi(this);
  ui->progressBar->hide();
  RegisterFunction::registerAll();
  connect(this,SIGNAL(startUpMessage(QString,int,QColor)),screen,SLOT(showMessage(QString,int,QColor)));

  //定义周期发指令到伺服，更新状态显示
  m_timer=new QTimer(this);
  m_timer->setInterval(300);
  connect(m_timer,SIGNAL(timeout()),this,SLOT(onTimeOut()));

  initialUi();
  createActions();
  createMenus();
  createToolBars();
  createStatusUi();
  createConnections();

  m_plotWave=new PlotWave(this);
  ui->verticalLayout_plot->addWidget(m_plotWave);
  connect(m_plotWave,SIGNAL(showMax(bool)),this,SLOT(onPlotWaveFloatingShow(bool)));
  connect(this,SIGNAL(stopThreadSampling()),m_plotWave,SLOT(onStopThreadSampling()));
  ui->dock_wave->hide();
  uiStatus->btn_warring->hide();

  showMaximized();//setWindowState(Qt::WindowMaximized);是一样的功能

  QString dataBaseFile=RESOURCE_FILE_PATH+"DataBase/"+DB_MotorInfo;
//  qDebug()<<"dataBaseFile"<<dataBaseFile;
  m_dataBase=new ConnectDataBase(this);
  m_dataBase->doConnectBase(dataBaseFile);

  this->setWindowTitle(tr("Servo Driver Technology"));
//  QString str=tr("current config:typeId %1 \n typeName %2 \n comId %3 \n comName %4 \n comRnStation %5").arg(mp_userConfig->typeId).arg(mp_userConfig->typeName).arg(mp_userConfig->com.id).arg(mp_userConfig->com.comName).arg(mp_userConfig->com.rnStation);
//  qDebug()<<str;
  m_actConfigExit->setVisible(false);
  ui->btn_navigation->setVisible(false);
}

MainWindow::~MainWindow()
{
  QString str=CUSTOM_USER_FILE_PATH+mp_userConfig->model.modelName+tr("config_%1%2").arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss")).arg(".xml");
  m_xml->saveUserConfigXml(str,mp_fileRecent,mp_userConfig);


  if(m_flashUpdateWidget!=NULL)
  {
    m_flashUpdateWidget->close();
    delete m_flashUpdateWidget;
  }
  if(m_newConfigDialog!=NULL)
  {
    m_newConfigDialog->close();
    delete m_newConfigDialog;
  }
  if(m_fpgaDialogSetting!=NULL)
  {
    m_fpgaDialogSetting->close();
    delete m_fpgaDialogSetting;
  }
  if(m_fpgaDialogSettingRnNet!=NULL)
  {
    m_fpgaDialogSettingRnNet->close();
    delete m_fpgaDialogSettingRnNet;
  }
  if(m_quickView!=NULL)
  {
    m_quickView->close();
    delete m_quickView;
  }
  //释放树
  if(mp_flashAllTreeWidget!=NULL)
  {
    mp_flashAllTreeWidget->clear();
    delete mp_flashAllTreeWidget;
    mp_flashAllTreeWidget=NULL;
  }
  if(mp_ramAllTreeWidget!=NULL)
  {
    mp_ramAllTreeWidget->clear();
    delete mp_ramAllTreeWidget;
    mp_ramAllTreeWidget=NULL;
  }
  if(mp_funcExtension!=NULL)
  {
    mp_funcExtension->clear();
    delete mp_funcExtension;
    mp_funcExtension=NULL;
  }
  if(mp_functionCmdTreeWidget!=NULL)
  {
    mp_functionCmdTreeWidget->clear();
    delete mp_functionCmdTreeWidget;
    mp_functionCmdTreeWidget=NULL;
  }
  qDebug()<<"manwidow release ->";
  delete ui;
  delete uiStatus;
}

void MainWindow::onWarningMessageChanged(QString &msg)
{
  uiStatus->btn_warring->show();
  uiStatus->warningMessge->show();
  uiStatus->warningMessge->setText(msg);
}

void MainWindow::onClearWarning()
{
  uiStatus->btn_warring->hide();
  uiStatus->warningMessge->hide();
}


void MainWindow::setConfigSaveEnableStatus(AbstractFuncWidget *absWidget)
{
  if(absWidget->isWrite2Ram())
    m_actFuncConfig->setEnabled(true);
  else
    m_actFuncConfig->setEnabled(false);

  if(absWidget->isWrite2Flash())
    m_actFuncSave->setEnabled(true);
  else
    m_actFuncSave->setEnabled(false);
}
//!----------------------------slots-----------------------------
void MainWindow::onTreeWidgetItemClicked(QTreeWidgetItem *item, int column)
{
  Q_UNUSED(column)
  /**
   * 1 切换页面
   * 2 发送更新表格（从Ram中）信号到每一个窗口，除了高级功能
   **/
  AbstractFuncWidget *absWidget;
  if(!(item->childCount()>0))//如果是叶子节点
  {
    //判断是不是Global全局节点操作:0:不是 1是
    int isGlobal=0;
    isGlobal=item->text(COL_NAVIGATION_TREE_GLOBAL).toInt();
//    qDebug()<<"global:"<<isGlobal;
    if(isGlobal==UI_TAB_TYPE_AXIS)
    {
      int index;
      index=m_itemSize*ui->combo_axis->currentIndex()+item->text(COL_NAVIGATION_TREE_UIINDEX).toInt();

      ui->stackedWidget->setCurrentIndex(index);
//      qDebug()<<"item:"<<index<<"classname"<<ui->stackedWidget->currentWidget()->objectName()<<"combo"<<ui->combo_axis->currentIndex()<<column;

      //设置全部窗口未激活(用于在激活时执行什么，失去激活时又执行什么)
      //在位置环、速度环激活时重绘窗口，未激活不做处理
      //在编码器时激活时开始定时器，未激活时也就是切换到其它时就关闭更新的定时器

      for(int i=0;i<ui->stackedWidget->count();i++)
      {
        absWidget=static_cast<AbstractFuncWidget *>(ui->stackedWidget->widget(i));
        absWidget->setActiveNow(false);
      }

      absWidget=static_cast<AbstractFuncWidget *>(ui->stackedWidget->currentWidget());

      //设置config save 状态
      setConfigSaveEnableStatus(absWidget);

      absWidget->setTreeWidgetHeaderLables();//更新一下轴号的显示
      absWidget->setActiveNow(true);
      if(m_isOpenCom)
        absWidget->updateUiWhenNavigationTreeClicked();
    }
    else
    {
      //全局点击响应
      int globalUiIndex=0;
      globalUiIndex=item->text(COL_NAVIGATION_TREE_CLASSNAME).toInt();
      qDebug()<<"click item name:"<<item->text(COL_NAVIGATION_TREE_NAME);
      qDebug()<<"globalUiIndex "<<globalUiIndex;
      int stackeIndex=ui->stackedWidget->count()-(GLOBAL_UI_COUNT-1);//因为画图没有加入来
      switch (globalUiIndex)
      {
      case GLOBAL_UI_PLOTVIEW:
        if(ui->dock_wave->isHidden())
          ui->dock_wave->show();
        break;
      case GLOBAL_UI_IOMODULE:
        for(int i=0;i<ui->stackedWidget->count();i++)
        {
          absWidget=static_cast<AbstractFuncWidget *>(ui->stackedWidget->widget(i));
          absWidget->setActiveNow(false);
        }
        ui->stackedWidget->setCurrentIndex(stackeIndex+globalUiIndex-1);
        absWidget=static_cast<AbstractFuncWidget *>(ui->stackedWidget->currentWidget());
        //设置config save 状态
        setConfigSaveEnableStatus(absWidget);
        absWidget->setActiveNow(true);
        break;
      }
    }
  }
}
void MainWindow::onComboBoxAxisSelectClicked(int index)
{
  bool isHide=ui->dock_wave->isHidden();
  qDebug()<<"ui->treeWidget->currentItem():"<<ui->treeWidget->currentItem()->text(0);
  onTreeWidgetItemClicked(ui->treeWidget->currentItem(),0);
  if(isHide)
    ui->dock_wave->hide();
  qDebug()<<index;
}

void MainWindow::onActionConfigNewClicked()
{
  if(m_newConfigDialog==NULL)
  {
    m_newConfigDialog=new NewConfigDialog(mp_srcAllConfig,0);
    connect(m_newConfigDialog,SIGNAL(newConfig(UserConfig*)),this,SLOT(onNewConfigurationActived(UserConfig*)));
  }
  m_newConfigDialog->show();
}

void MainWindow::onActionConfigOpenClicked()
{
  QString path = QFileDialog::getOpenFileName(this, tr("Open userconfig"), ".", tr("Xml Files( *.xml)"));
  if(!path.isNull())
  {
    if(m_xml->parseUserConfig(path))//读取文件成功加载，新建界面
    {
      //关闭刷新时钟
      if(m_timer->isActive())
        m_timer->stop();

      mp_userConfig=m_xml->getUserConfig();
      m_plotWave->onClearPlotWave();
      ui->dock_wave->hide();
      updateUiByUserConfig(mp_userConfig,mp_srcAllConfig);

      //更新每一个页面的参数
      if(m_isOpenCom)
      {
        m_timer->start();
        for(int i=0;i<ui->stackedWidget->count();i++)
        {
          AbstractFuncWidget *absWidget;
          absWidget=static_cast<AbstractFuncWidget *>(ui->stackedWidget->widget(i));
          absWidget->onActionReadFuncValueFromRam();
        }
      }
    }
    else
      QMessageBox::information(0,tr("xml Error"),tr("configXml file error"));
  }
}

void MainWindow::onActionConfigSaveClicked()
{
  QString saveFileName;
  QString strDir=CUSTOM_USER_FILE_PATH+mp_userConfig->model.modelName+\
          tr("config_%1%2").arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss")).arg(".xml");
  saveFileName = QFileDialog::getSaveFileName(this,tr("SaveXML"), strDir,tr("userconfig (*.xml)"));

  if (!saveFileName.isNull())//空表示点的取消按钮
  {
    m_xml->saveUserConfigXml(saveFileName,mp_fileRecent,mp_userConfig);
  }
}

void MainWindow::onActionConfigSaveAsClicked()
{
  onActionConfigSaveClicked();
}
void MainWindow::onActionConfigHistoryClicked()
{
  QAction *act=static_cast<QAction *>(sender());
  qDebug()<<act->text();
  if(m_xml->parseUserConfig(act->text()))//读取文件成功加载，新建界面
  {
    mp_userConfig=m_xml->getUserConfig();
    m_plotWave->onClearPlotWave();
    ui->dock_wave->hide();
    updateUiByUserConfig(mp_userConfig,mp_srcAllConfig);
    ui->dock_wave->show();
  }
  else
    QMessageBox::information(0,tr("xml Error"),tr("configXml file does not exist"));
}

void MainWindow::onActionConfigExitClicked()
{

}

void MainWindow::onActionConnectClicked()
{
  COM_ERROR error=COM_OK;
//  m_isOpenCom=false;
//  ui->progressBar->setVisible(true);
//  enableAllUi(false);
//  error=static_cast<COM_ERROR>(GTSD_CMD_Open(updateProgessBarWhenConnectClicked,(void*)ui->progressBar,static_cast<COM_TYPE>(mp_userConfig->com.id)));
//  if(error!=COM_OK)
//  {
//    QMap<COM_ERROR ,QString>warningMap;
//    warningMap.insert(COM_ARM_OUT_TIME,tr("ARM OUT OFF TIME !"));
//    warningMap.insert(COM_OK,tr("COM NET OK !"));
//    warningMap.insert(COM_NET_ERROR,tr("COM NET ERROR !"));
//    warningMap.insert(COM_NOT_FINISH,tr("COM NOT FINISH !"));
//    QString eMsg;
//    if(warningMap.contains(error))
//      eMsg=warningMap.value(error);
//    else
//      eMsg=tr("FPGA error or disconnect");
//    QMessageBox::information(0,tr("connect"),tr("com connect error:%1").arg(eMsg));
//    m_actConnect->setChecked(false);
//    m_actDisConnect->setChecked(true);
//    m_isOpenCom=false;
//    error=static_cast<COM_ERROR>(GTSD_CMD_Close(static_cast<COM_TYPE>(mp_userConfig->com.id)));
//    ui->progressBar->setVisible(false);
//    enableAllUi(true);
//    return;
//  }
//  //判断是不是千兆网络，提示相关信息后返回
//  short netCarMsg=GTSD_CMD_GetNetCardMsg();
//  QMap<int ,QString>netCarInfoMap;
//  netCarInfoMap.insert(0,tr("1000M ETH"));
//  netCarInfoMap.insert(1,tr("FUNCTION ADDRESS ERROR"));
//  netCarInfoMap.insert(2,tr("NOT 1000M ETH"));
//  netCarInfoMap.insert(3,tr("NO ETH"));
//  if(netCarMsg!=0){
//    QMessageBox::information(0,tr("net error"),tr("com net error information:%1").arg(netCarInfoMap.value(netCarMsg)));
//    m_actConnect->setChecked(false);
//    m_actDisConnect->setChecked(true);
//    m_isOpenCom=false;
//    error=static_cast<COM_ERROR>(GTSD_CMD_Close(static_cast<COM_TYPE>(mp_userConfig->com.id)));
//    ui->progressBar->setVisible(false);
//    enableAllUi(true);
//    return;
//  }
//  m_isOpenCom=true;

  m_isOpenCom=openNetCom();
  if(m_isOpenCom==false)
    return;
  //查询一下所连接的固件版本，与当前设置的版本对比
  //提示用户版本信息
  //当version=0时，说明是uboot程序


  quint16 version=0xffff;
  ServoControl::readDeviceVersion(0,version,(quint16)mp_userConfig->com.id,mp_userConfig->com.rnStation);
  qDebug()<<"Firmware version:"<<version;

  if((version!=0)&&(version!=0xffff))
  {
    QString verStr=QString::number(version);
    QString currentVersion=mp_userConfig->model.version.at(0);

    if(currentVersion.contains(verStr))
    {
      //版本的一致
      qDebug()<<"compare";
    }
    else
    {
      //版本的不一致
      qDebug()<<"not compare";
      QMessageBox::StandardButton rb = QMessageBox::question(this, "Warring",
                                                             tr("current version:%1\n"
                                                                "device version :%2\n"
                                                                "the current version which you select is not matching\n"
                                                                "do you still want to use version:%3?").arg(currentVersion).arg("V"+verStr).arg(currentVersion),
                                                             QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
      if (rb == QMessageBox::No)
      {
        closeNetCom();
        return ;
      }
    }
    //更新每一个页面的参数
    if(m_isOpenCom)
    {
      qDebug()<<"connect return value:"<<error;
      uiStatus->btn_connect->setToolTip(tr("com ok"));
      uiStatus->btn_connect->setIcon(QIcon(ICON_FILE_PATH+ICON_STATUS_CONNECT));
      for(int i=0;i<ui->stackedWidget->count();i++)
      {
        AbstractFuncWidget *absWidget;
        absWidget=static_cast<AbstractFuncWidget *>(ui->stackedWidget->widget(i));
        absWidget->onActionReadFuncValueFromRam();
      }

      uiStatus->btn_warring->hide();
      onTreeWidgetItemClicked(ui->treeWidget->currentItem(),0);
      uiStatus->warningMessge->setText(tr(""));

//      //读伺服状态，更新到绘图中的控件
//      QList<ServoStatus>statusList;
//      ServoStatus sta;
//      for(int i=0;i<mp_userConfig->model.axisCount;i++)
//      {
//        sta.currentModeIndex=ServoControl::servoCurrentUserTaskMode(i,(quint16)mp_userConfig->com.id,mp_userConfig->com.rnStation);
//        sta.isOn=ServoControl::checkServoIsReady(i,(quint16)mp_userConfig->com.id,mp_userConfig->com.rnStation);//读是否上伺服
//        statusList.append(sta);
//      }
      m_plotWave->updateCurrentServoStatus();
      enableAllUi(true);

      //开启定时器定时检查状态及断线情况
      qDebug()<<"timer start";
      m_timer->start();
    }

  }
  else if(version==0){
    //当前版本号=0，说明是处于uboot模式
    uiStatus->btn_connect->setToolTip(tr("com ok"));
    uiStatus->btn_connect->setIcon(QIcon(ICON_FILE_PATH+ICON_STATUS_CONNECT));
    uiStatus->warningMessge->setText(tr("you are working in the uboot mode ,after reboot system,please power off and change boot-switch"));
    //设置uboot ui mode
    enableAllUi(true);
    setUbootModeUi(true);
  }
  else{
    //当前版本号=0xffff
    //DSP没有响应
    uiStatus->btn_connect->setToolTip(tr("com ok"));
    uiStatus->btn_connect->setIcon(QIcon(ICON_FILE_PATH+ICON_STATUS_CONNECT));
    uiStatus->warningMessge->setText(tr("DSP respond error!"));
    QMessageBox::information(0,tr("connect"),tr("DSP respond error!"));
    error=static_cast<COM_ERROR>(GTSD_CMD_Close(static_cast<COM_TYPE>(mp_userConfig->com.id)));
    m_actConnect->setChecked(false);
    m_actDisConnect->setChecked(true);
    setComConnectStatus(false);
    enableAllUi(true);
  }
  ui->progressBar->setVisible(false);
  m_actConfigNew->setEnabled(false);//禁止新建版本
  m_actConfigNew->setEnabled(false);
  m_actConfigOpen->setEnabled(false);
  m_actConfigSave->setEnabled(false);
  m_actConfigSaveAs->setEnabled(false);
  m_menuConfigRecent->setEnabled(false);
}

void MainWindow::onActionDisConnectClicked()
{
  m_timer->stop();
  //设置每一个窗口非激活状态
  AbstractFuncWidget *absWidget;
  for(int i=0;i<ui->stackedWidget->count();i++)
  {
    absWidget=static_cast<AbstractFuncWidget *>(ui->stackedWidget->widget(i));
    absWidget->setActiveNow(false);
//        qDebug()<<absWidget->objectName();
  }
  m_plotWave->resetPlotWarve();
  DownloadDialog::delayms(500);

  COM_ERROR error;
  error=static_cast<COM_ERROR>(GTSD_CMD_Close(static_cast<COM_TYPE>(mp_userConfig->com.id)));
  if(error!=COM_OK)
  {
//    m_timer->start();
    error=static_cast<COM_ERROR>(GTSD_CMD_Close(static_cast<COM_TYPE>(mp_userConfig->com.id)));
//    QMessageBox::information(0,tr("connect"),tr("com connect error:%1").arg(error));
//    return;
  }
//  qDebug()<<"connect return value:"<<error;
  uiStatus->btn_connect->setToolTip(tr("com error"));
  uiStatus->btn_connect->setIcon(QIcon(ICON_FILE_PATH+ICON_STATUS_DISCONNECT));
  m_isOpenCom=false;
  uiStatus->btn_warring->hide();

  m_actDisConnect->setChecked(true);
  m_actConnect->setChecked(false);

  setUbootModeUi(false);
  m_actConfigNew->setEnabled(true);//开放新建版本
  m_actConfigOpen->setEnabled(true);
  m_actConfigSave->setEnabled(true);
  m_actConfigSaveAs->setEnabled(true);
  m_menuConfigRecent->setEnabled(true);
}
void MainWindow::onActionViewNavigation(void)
{

  if(ui->dock_navigation->isHidden())
    ui->dock_navigation->show();
  else
    ui->dock_navigation->hide();

}

void MainWindow::onActionViewPlotCurve(void)
{
  if(ui->dock_wave->isHidden())
    ui->dock_wave->show();
  else
    ui->dock_wave->hide();
}

void MainWindow::onActionViewFullScreen()
{
  if(this->isFullScreen())
  {
    m_actFullScreen->setIcon(QIcon(ICON_FILE_PATH+ICON_MENU_FULLSCREEN));
    showMaximized();
  }
  else
  {
    m_actFullScreen->setIcon(QIcon(ICON_FILE_PATH+ICON_MENU_SMALLSCREEN));
    showFullScreen();
  }
}

void MainWindow::onActionToolXmlUpdateClicked()
{
  if(m_flashUpdateWidget==NULL)
    m_flashUpdateWidget=new FlashUpdateWidget(0);
  m_flashUpdateWidget->show();
}

void MainWindow::onActionFile2ServoClicked()
{
//  qDebug()<<"file to servo";

  if(!m_isOpenCom)
  {
    QMessageBox::information(0,tr("Warring"),tr("please open the com first !"));
    return ;
  }

  stopTimer();
  QString filename;
  filename = QFileDialog::getOpenFileName(this, tr("Open XML File"), MainWindow::g_lastFilePath, tr("XML Files(*.xml)"));
  if (filename.isNull())
  {
    //QMessageBox::information(NULL, tr("Path"), tr("You didn't select any files."));
    return;
  }
  QFileInfo fileInfo;
  fileInfo.setFile(filename);
  MainWindow::g_lastFilePath=fileInfo.filePath()+"/";

  ui->progressBar->show();
  ui->progressBar->setValue(0);
  QTreeWidget *tree=XmlBuilder::readTreeWidgetFromFile(filename);
  ServoControl sctl;
//  ui->progressBar->setValue(50);
//  ServoControl::writeServoFlashByAllAxisTree(tree,(COM_TYPE)mp_userConfig->com.id,mp_userConfig->com.rnStation);
//  ui->progressBar->setValue(100);
  connect(&sctl,SIGNAL(progressValue(int ,int)),this,SLOT(onXmlPrmToServo(int,int)));
  sctl.writeServoFlashByAllAxisTree(tree,(COM_TYPE)mp_userConfig->com.id,mp_userConfig->com.rnStation);
  ui->progressBar->hide();
  tree->clear();
  delete tree;
  startTimer();
  AbstractFuncWidget *absWidget;
  absWidget=static_cast<AbstractFuncWidget *>(ui->stackedWidget->currentWidget());
  absWidget->updateUiWhenNavigationTreeClicked();
}

void MainWindow::onActionServo2FileClicked()
{
  if(!m_isOpenCom)
  {
    QMessageBox::information(0,tr("Warring"),tr("please open the com first !"));
    return ;
  }
  stopTimer();
  QString fileNameXml;
  QString fileNameDefaultQString =MainWindow::g_lastFilePath+"Prm_"+mp_userConfig->model.version.at(0) +"_"+ QDateTime::currentDateTime().toString("yyyyMMdd");//默认文件名
  fileNameXml = QFileDialog::getSaveFileName(this, tr("Save XML"), fileNameDefaultQString, tr("xml file (*.xml)"));
  if (fileNameXml.isNull()) return;

  qDebug()<<"servo to file";
  QFileInfo fileInfo;
  fileInfo.setFile(fileNameXml);
  MainWindow::g_lastFilePath=fileInfo.filePath()+"/";

  bool waveIsVisible=false;
  ui->progressBar->show();
  ui->progressBar->setValue(40);
  if(ui->dock_wave->isVisible())
  {
    ui->dock_wave->hide();
    waveIsVisible=true;
  }

  ServoControl::updateAllFlashTreeWidget(mp_flashAllTreeWidget,(COM_TYPE)mp_userConfig->com.id,mp_userConfig->com.rnStation);
//  QWidget *mwidget=new QWidget();
//  QHBoxLayout *hLayout=new QHBoxLayout(mwidget);
//  hLayout->addWidget(mp_flashAllTreeWidget);
//  mwidget->show();
  ui->progressBar->setValue(80);
  XmlBuilder::saveFlashAllAxisTreeFile(fileNameXml,mp_flashAllTreeWidget);
  ui->progressBar->setValue(100);
  ui->progressBar->hide();
  if(waveIsVisible)
    ui->dock_wave->show();
  startTimer();
}

void MainWindow::onActionAxisCloneClicked()
{
  AxisClone *axis=AxisClone::getAxisCloneInstance(this,0);//单例模式
  axis->show();
}

void MainWindow::onActionAxisFileCloneClicked()
{
  AxisFileClone *fileCopy=new AxisFileClone(this,0);
  fileCopy->show();
}

void MainWindow::onActionFuncConfigClicked(void)
{
  if(m_isOpenCom==false)
  {
    QMessageBox::information(0,tr("connect"),tr("please open com first !"));
    return;
  }
  AbstractFuncWidget *absWidget;
  absWidget=static_cast<AbstractFuncWidget *>(ui->stackedWidget->currentWidget());
  qDebug()<<absWidget->objectName();
  absWidget->onWriteFuncTreetoServoRam();
}

void MainWindow::onActionFuncSaveClicked(void)
{
  if(m_isOpenCom==false)
  {
    QMessageBox::information(0,tr("connect"),tr("please open com first !"));
    return;
  }
  AbstractFuncWidget *absWidget;
  absWidget=static_cast<AbstractFuncWidget *>(ui->stackedWidget->currentWidget());
  absWidget->onWriteFuncTreetoServoFlash();
}
void MainWindow::onActionProgramDownloadClicked()
{
  DownloadDialog dialog(this,0);
  dialog.exec();
}

void MainWindow::onActionResetDSPClicked()
{
  if(!m_isOpenCom)
  {
    QMessageBox::information(0,tr("Warring"),tr("please open the com first !"));
    return ;
  }
//  QMessageBox::StandardButton rb=QMessageBox::question(this,"Warring","Do you want to reset servo ?",QMessageBox::Yes|QMessageBox::No,QMessageBox::No);
//  if (rb==QMessageBox::No)
//  {
//    return;
//  }

  qint16 result=0;
  com_type comtype=(com_type)mp_userConfig->com.id;
  QTreeWidget *tree=mp_funcExtension;
  QTreeWidgetItem *item;
  QTreeWidgetItem*itemChild;
  bool plotIsShow=false;

  stopTimer();
  enableAllUi(false);
  //失效每一个窗口
  AbstractFuncWidget *absWidget;
  for(int i=0;i<ui->stackedWidget->count();i++)
  {
    absWidget=static_cast<AbstractFuncWidget *>(ui->stackedWidget->widget(i));
    absWidget->setActiveNow(false);
  }

  if(ui->dock_wave->isVisible())
  {
    plotIsShow=true;
    ui->dock_wave->hide();
  }
  ui->progressBar->show();
  ui->progressBar->setValue(5);

  //检查伺服有没有开，如果开了则提示信息并返回
  QString msg="null";
  for(int i=0;i<mp_userConfig->model.axisCount;i++)
  {
    bool ret=ServoControl::checkServoIsReady(i,comtype,mp_userConfig->com.rnStation);
    if(ret)
      msg.append(tr("axis_%1 servo is on \n").arg(i));
  }
  if(msg!="null")
  {
    QMessageBox::information(0,tr("Warring"),tr("refuse to reset :%1").arg(msg));
    return ;
  }

  //找到dspNum dspAxis
  int dspNum;
  int dspAxis;
  for(int i=0;i<tree->topLevelItemCount();i++)//在PrmFuncExtension.xml中找到fpga结点
  {
    item=tree->topLevelItem(i);
    if(DEVICE_DESCRIBE==item->text(COL_FUNC_EXTENSION_NAME))
    {
      for(int j=0;j<item->childCount();j++)
      {
        itemChild=item->child(j);
        if(DSP_NAME==itemChild->text(COL_FUNC_EXTENSION_NAME))
        {
          dspNum=itemChild->text(COL_FUNC_EXTENSION_PARAMETER).toInt();
          dspAxis=itemChild->child(0)->text(COL_FUNC_EXTENSION_PARAMETER).toInt();
          break;
        }
      }
      break;
    }
  }

  ui->progressBar->setValue(10);
  int value=10;
  double temp=90.0/(dspNum*7.0);
  int valueInc=(int)temp;
  qDebug()<<"valueInc "<<value;
  bool flagSuccess=true;
  //下发软复位指令
  for(int i=0;i<dspNum;i++)
  {
    bool flagFinish=false;

    quint16 errorCount=0;
    value+=valueInc;qDebug()<<"valueInc "<<value;
    result=GTSD_CMD_ResetSystem(i*dspAxis,comtype,mp_userConfig->com.rnStation);
    ui->progressBar->setValue(value);
    if(result!=0)//没有成功下发指令
    {
      ui->progressBar->setValue(0);
      ui->progressBar->hide();
      flagSuccess=false;
      break;
    }

    DownloadDialog::delayms(500);
    value+=valueInc;qDebug()<<"valueInc "<<value;
    ui->progressBar->setValue(value);
    DownloadDialog::delayms(500);
    value+=valueInc;qDebug()<<"valueInc "<<value;
    ui->progressBar->setValue(value);
    DownloadDialog::delayms(500);
    value+=valueInc;qDebug()<<"valueInc "<<value;
    ui->progressBar->setValue(value);
    DownloadDialog::delayms(500);
    value+=valueInc;qDebug()<<"valueInc "<<value;
    ui->progressBar->setValue(value);
    DownloadDialog::delayms(500);
    value+=valueInc;qDebug()<<"valueInc "<<value;
    qDebug()<<"GTSD_CMD_CheckResetFinish "<<value;
    ui->progressBar->setValue(value);
    do
    {
      result=GTSD_CMD_CheckResetFinish(i*dspAxis, flagFinish, comtype,mp_userConfig->com.rnStation);
      DownloadDialog::delayms(10);
      if(result!=0)//指令下发没有错
      {
        ui->progressBar->setValue(0);
        ui->progressBar->hide();
        flagSuccess=false;
        break;
      }
      errorCount++;
      if(errorCount>=100)//等待超时
      {
        flagSuccess=false;
        break;
      }
      qDebug()<<"reset dsp";
    }while(!flagFinish);
    value+=valueInc;
    ui->progressBar->setValue(value);
  }
  //软复位成功
  if(flagSuccess)
  {
    value+=valueInc;
    ui->progressBar->setValue(value);
    ui->progressBar->setValue(100);
//    QMessageBox::information(0,tr("complete"),tr("reset system success!"));
    ui->progressBar->hide();
    if(plotIsShow)
      ui->dock_wave->show();
    startTimer();
    //清一下报警
    onClearWarning();
    //更新一下画图当前状态
    m_plotWave->updateCurrentServoStatus();

  }
  else
  {
    QMessageBox::warning(0,tr("Error"),tr("ERROR\nreset system error!"));
  }
  enableAllUi(true);
  //使当前的窗口生效
  absWidget=static_cast<AbstractFuncWidget *>(ui->stackedWidget->currentWidget());
  absWidget->setActiveNow(true);
}

void MainWindow::onActionNormalizeTreeClicked()
{
  NormalizeTreeForm treeNormalizeForm(0);
  treeNormalizeForm.exec();
}
void MainWindow::onActionFPGAConfigurationClicked()
{
  if(!m_isOpenCom)
  {
    QMessageBox::information(0,tr("Warring"),tr("please open the com first !"));
    return ;
  }
  //先通过xml获得树对象
  QFormBuilder formBuilder;
  QString version=mp_userConfig->model.version.at(0);
  QString modelName=mp_userConfig->model.modelName;
  QString typeName=mp_userConfig->typeName;
  QString fileName;
  if(mp_userConfig->com.id==GTSD_COM_TYPE_NET)//PcDebug
  {
    fileName=SYSCONFIG_FILE_PATH+typeName+"/"+modelName+"/"+version+"/"+FPGA_SETTING_NAME_PCDEBUG;
    if(m_fpgaDialogSetting==NULL)
    {
      QFile file(fileName);
      if(!file.open(QFile::ReadOnly))
      {
        QMessageBox::information(0,tr("file open error"),tr("open file error:%1").arg(fileName));
        return;
      }
      QWidget *widget=NULL;
      widget=formBuilder.load(&file,0);
      if(widget==NULL)
      {
        QMessageBox::information(0,tr("file load error"),tr("load file error:%1").arg(fileName));
        return;
      }
      file.close();
      QTreeWidget *tree=widget->findChild<QTreeWidget*>("treeWidget");
      m_fpgaDialogSetting=new FpgaDialogSetting(tree,this);
      delete widget;
      widget=NULL;
    }
    m_fpgaDialogSetting->show();
  }
  else if(mp_userConfig->com.id==GTSD_COM_TYPE_RNNET)//等环网
  {
    fileName=SYSCONFIG_FILE_PATH+typeName+"/"+modelName+"/"+version+"/"+FPGA_SETTING_NAME_RNNET;
    if(m_fpgaDialogSettingRnNet==NULL)
    {
      QFile file(fileName);
      if(!file.open(QFile::ReadOnly))
      {
        QMessageBox::information(0,tr("file open error"),tr("open file error:%1").arg(fileName));
        return;
      }
      QWidget *widget=NULL;
      widget=formBuilder.load(&file,0);
      if(widget==NULL)
      {
        QMessageBox::information(0,tr("file load error"),tr("load file error:%1").arg(fileName));
        return;
      }
      file.close();
      QTreeWidget *tree=widget->findChild<QTreeWidget*>("treeWidget");
      m_fpgaDialogSettingRnNet=new FpgaDialogSettingRnNet(tree,this);
      delete widget;
      widget=NULL;
    }
    m_fpgaDialogSettingRnNet->show();
  }
}
void MainWindow::onActionRestoreFactorySettingClicked()
{
//  qDebug()<<"onActionRestoreFactorySettingClicked";
//  enableAllUi(false);
  setUbootModeUi(true);
  uiStatus->btn_warring->hide();
  ui->progressBar->setVisible(true);
  ui->progressBar->setValue(2);
  uiStatus->warningMessge->setText(tr("uboot starting......"));
  DownloadDialog::delayms(20);
  //读取json默认配置文件
  NetConfig net;
  QString filePath=RESOURCE_FILE_PATH+"Uboot/NetConfig.json";
  net.initialDataFromJsonFile(filePath);
  QString outPath=RESOURCE_FILE_PATH+"Uboot/ServoUboot.out";
  QString ldrPath=RESOURCE_FILE_PATH+"Uboot/ServoUboot.ldr";

  int netId=net.netId();
  qint16 netRnStation=net.rnStation();

  uiStatus->warningMessge->setText(tr("servo connecting......"));
  DownloadDialog::delayms(20);
  //close net
  if(m_isOpenCom)
    onActionDisConnectClicked();
  //open net
  COM_ERROR error;
  error=static_cast<COM_ERROR>(GTSD_CMD_Open(updateProgessBarWhenConnectClicked,(void*)ui->progressBar,netId));
  if(error!=COM_OK)
  {
    QMessageBox::information(0,tr("connect"),tr("FPGA comunication connect error:%1").arg(error));
    error=static_cast<COM_ERROR>(GTSD_CMD_Close(static_cast<COM_TYPE>(netId)));
//    enableAllUi(true);
    setUbootModeUi(false);
    ui->progressBar->setVisible(false);
    return;
  }
  qDebug()<<"open com";
  setComConnectStatus(true);
  QString warnnigMessage;
  uiStatus->warningMessge->setText(tr("servo ubooting......"));
  if(0==GTSD_CMD_Hex2Ldr(outPath.toStdWString(),ldrPath.toStdWString(),netId,netRnStation))//转化文件成功
  {
    //从树中先找到 device -> 1 dsp  2 dspAxis
    QTreeWidget *tree=mp_funcExtension;
    QTreeWidgetItem *item;
    QTreeWidgetItem*itemChild;
    int dspNum;
    int dspAxis;
    int result=0;
    for(int i=0;i<tree->topLevelItemCount();i++)//在PrmFuncExtension.xml中找到dsp结点
    {
      item=tree->topLevelItem(i);
      if(DEVICE_DESCRIBE==item->text(COL_FUNC_EXTENSION_NAME))
      {
        for(int j=0;j<item->childCount();j++)
        {
          itemChild=item->child(j);
          if(DSP_NAME==itemChild->text(COL_FUNC_EXTENSION_NAME))
          {
            dspNum=itemChild->text(COL_FUNC_EXTENSION_PARAMETER).toInt();
            m_dspNum=dspNum;
            dspAxis=itemChild->child(0)->text(COL_FUNC_EXTENSION_PARAMETER).toInt();
            break;
          }
        }
        break;
      }
    }

    //烧写.out文件
    qDebug()<<"GTSD_CMD_Hex2Ldr";
    bool ubootOk=true;
    for(int i=0;i<m_dspNum;i++)
    {
      m_step=i;
      qDebug()<<"GTSD_CMD_ProcessorUartBootHandler < "<<i;
      //开始uboot
//      qDebug()<<"ldr file"<<ldrPath.toStdWString();
      if(0!=GTSD_CMD_ProcessorUartBootHandler(i*2,ldrPath.toStdWString(),\
                                              net.baudRate(),net.ubootCmd(),\
                                              net.ubootInputKey().toStdString(),\
                                              updateProgessBarWhenRestoreClicked,\
                                              ui->progressBar,netId,netRnStation))
      {
        warnnigMessage=tr("Warring :Causes of exceptions maybe  1.Boot Switch 2.FPGA FirmWare 3.Boot File ");
        uiStatus->warningMessge->setText(warnnigMessage);
        ubootOk=false;
        break;
      }
      else{
        warnnigMessage=tr("uboot complete successfully ,please power off to restart servo and change the boot-switch......");
      }
    }
    //如果烧写.out成功 弹出hex.xml选择对话框
    if(ubootOk)
    {
      ui->progressBar->setValue(0);
      UbootDialog uDialog(this);
      int ret=uDialog.exec();
      if(ret==QDialog::Accepted)//点击了应用
      {
        QString hexPath=uDialog.hexFilePath();
        QString xmlPath=uDialog.xmlFilePath();
        qDebug()<<"hexPath:"<<hexPath;

        //下载默认的.hex文件
        uiStatus->warningMessge->setText(tr("download hex-file to servo flash......"));
        DownloadDialog::delayms(20);

        bool isHexDownload=false;
        for(int i=0;i<dspNum;i++)
        {
          qDebug()<<"GTSD_CMD_ProcessorFlashHandler step:"<<i;
          m_step=i;
          result=GTSD_CMD_ProcessorFlashHandler(i*dspAxis,hexPath.toStdWString(),updateProgessBarWhenRestoreClicked,(void*)ui->progressBar,netId,netRnStation);
          qDebug()<<"result "<<result;
          isHexDownload=true;
          if(result!=0)//hex没有烧写成功
          {
            isHexDownload=false;
            QMessageBox::information(0,tr("download"),tr("download hex-file to servo not complete"));
            uiStatus->warningMessge->setText(tr("error:download hex-file to servo not complete !"));
            break;
          }
        }

        if(isHexDownload)//hex文件下载成功
        {
          ServoControl scontrol;
          connect(&scontrol,SIGNAL(progressValue(int,int)),this,SLOT(onXmlPrmToServo(int,int)));
          uiStatus->warningMessge->setText(tr("download hex-file to servo complete !"));
          DownloadDialog::delayms(20);

          //烧写驱动器参数
          uiStatus->warningMessge->setText(tr("download xml parameters file to servo ......"));
          DownloadDialog::delayms(20);
          ui->progressBar->setValue(10);
          QTreeWidget *treePrm=XmlBuilder::readTreeWidgetFromFile(xmlPath);
          DownloadDialog::delayms(20);
          ui->progressBar->setValue(50);
          scontrol.writeServoFlashByAllAxisTree(treePrm,(COM_TYPE)netId,netRnStation);
//          ServoControl::writeServoFlashByAllAxisTree(treePrm,(COM_TYPE)netId,netRnStation);
          qDebug()<<"write file to servo complete";
          ui->progressBar->setValue(60);
          DownloadDialog::delayms(20);
          ui->progressBar->setValue(70);
          ui->progressBar->hide();
          treePrm->clear();
          delete treePrm;
          uiStatus->warningMessge->setText(tr("download xml parameters file to servo complete successfully!"));
          ui->progressBar->setValue(80);
          DownloadDialog::delayms(20);
          warnnigMessage=tr("uboot complete successfully ,please power off to restart servo and change the boot-switch......");
          uiStatus->warningMessge->setText(warnnigMessage);
          ui->progressBar->setValue(100);
          DownloadDialog::delayms(20);

          m_progessValue=0;
          m_step=0;
          ui->progressBar->setVisible(false);
          ui->progressBar->setValue(0);
          qDebug()<<warnnigMessage;

          error=static_cast<COM_ERROR>(GTSD_CMD_Close(static_cast<COM_TYPE>(netId)));
          uiStatus->btn_warring->hide();
          setComConnectStatus(false);
        //  enableAllUi(true);
          setUbootModeUi(false);
        }
        else
        {
          qDebug()<<"hex file download error !";
        }
      }
      else//点击了取消
      {
        //显示提示信息
        warnnigMessage=tr("information:uboot donot finish ! you need to do: 1 do not poweroff . 2 download DSP (.hex) file  3 download parameters(.xml) file ");
        uiStatus->warningMessge->setText(warnnigMessage);
      }

    }
  }
  else
  {
    warnnigMessage=tr("Warring : convert .out file to .ldr faild !");
    uiStatus->warningMessge->setText(warnnigMessage);
  }
}

void MainWindow::onActionAboutConfigClicked()
{
  QMessageBox mess;
  mess.setIcon(QMessageBox::NoIcon);
  QString info;
  if(m_isOpenCom)//已经连接
  {
    quint16 pVersion;
    quint16 fVersion;
    QString hexFVersion;
    ServoControl::readDeviceVersion(0,pVersion,(quint16)mp_userConfig->com.id,mp_userConfig->com.rnStation);
    ServoControl::readDeviceFirmwareVersion(0,fVersion,(quint16)mp_userConfig->com.id,mp_userConfig->com.rnStation);
    hexFVersion=QString::asprintf("%#04X",fVersion);
    info=tr("SDT setting info:\nmodel:%1\nversion:%2\naxisCount:%3\ncomName:%4"
                          "\n\nDevice info:\nprocessor version:%5\nfirmware version:%6\n")
                              .arg(mp_userConfig->model.modelName)
                              .arg(mp_userConfig->model.version.at(0))
                              .arg(mp_userConfig->model.axisCount)
                              .arg(mp_userConfig->com.comName)
                              .arg(pVersion)
                              .arg(hexFVersion);
  }

  else//还示连接
  {
    bool openOk=false;
    openOk=openNetCom();
    if(openOk)
    {
      quint16 pVersion;
      quint16 fVersion;
      QString hexFVersion;
      ServoControl::readDeviceVersion(0,pVersion,(quint16)mp_userConfig->com.id,mp_userConfig->com.rnStation);
      ServoControl::readDeviceFirmwareVersion(0,fVersion,(quint16)mp_userConfig->com.id,mp_userConfig->com.rnStation);
      hexFVersion=QString::asprintf("%#04X",fVersion);
      info=tr("SDT setting info:\n model:%1\nversion:%2\naxisCount:%3\ncomName:%4"
                            "\n\nDevice info:\nprocessor version:%5\nfirmware version:%6\n")
                                .arg(mp_userConfig->model.modelName)
                                .arg(mp_userConfig->model.version.at(0))
                                .arg(mp_userConfig->model.axisCount)
                                .arg(mp_userConfig->com.comName)
                                .arg(pVersion)
                                .arg(hexFVersion);
    }
    else
    {
      info=tr("SDT setting info:\n model:%1\nversion:%2\naxisCount:%3\ncomName:%4"
                            "\n\n")
                                .arg(mp_userConfig->model.modelName)
                                .arg(mp_userConfig->model.version.at(0))
                                .arg(mp_userConfig->model.axisCount)
                                .arg(mp_userConfig->com.comName);
    }
    closeNetCom();
  }
  mess.setText(info);
  mess.setIconPixmap(QPixmap(ICON_FILE_PATH+ICON_SERVO_DEVICE));
  mess.exec();


}

void MainWindow::onActionAboutSDTClicked()
{
  QMessageBox mess;
  mess.setIcon(QMessageBox::NoIcon);
  QString info;
  info=tr("Version:%1\n\nget more help from:"
          "\nhttp://www.googoltech.com").arg(SDT_VERSION);
  mess.setText(info);
  mess.setIconPixmap(QPixmap(ICON_FILE_PATH+ICON_SDT_LOGO));
  mess.exec();
}

void MainWindow::onNewConfigurationActived(UserConfig *config)
{
  enableAllUi(false);
  mp_userConfig=config;
  qDebug()<<mp_userConfig->com.comName;
  qDebug()<<mp_userConfig->typeName;
  qDebug()<<mp_userConfig->model.modelName;
  qDebug()<<mp_userConfig->model.version.at(0);
//  //更新曲线视图
//  m_plotWave->onClearPlotWave();
//  ui->dock_wave->hide();
  //关闭刷新时钟，在devicestatus中就不会更新界面了
  if(m_timer->isActive())
    m_timer->stop();

  //使每个页面失效，在CfgEncoder中就不会再更新界面
  AbstractFuncWidget *absWidget;
  for(int i=0;i<ui->stackedWidget->count();i++)
  {
    absWidget=static_cast<AbstractFuncWidget *>(ui->stackedWidget->widget(i));
    absWidget->setActiveNow(false);
  }

  qDebug()<<"absWidget->setActiveNow";
  //界面清0
  updateUiByUserConfig(mp_userConfig,mp_srcAllConfig);
  qDebug()<<"updateUiByUserConfig";

  //更新曲线视图
  m_plotWave->onClearPlotWave();
  qDebug()<<"onClearPlotWave";
  ui->dock_wave->hide();

  //更新界面参数
  if(m_isOpenCom)
  {
    //更新每一个页面的参数
    for(int i=0;i<ui->stackedWidget->count();i++)
    {
      AbstractFuncWidget *absWidget;
      absWidget=static_cast<AbstractFuncWidget *>(ui->stackedWidget->widget(i));
      absWidget->onActionReadFuncValueFromRam();
      absWidget->setActiveNow(false);
    }
    m_timer->start();
//    absWidget=static_cast<AbstractFuncWidget *>(ui->stackedWidget->currentWidget());
//    absWidget->setActiveNow(true);
  }
  enableAllUi(true);
}

void MainWindow::onPlotWaveFloatingShow(bool isfloat)
{
  if(isfloat)
  {
    ui->dock_wave->setFloating(true);
    ui->dock_wave->setWindowState(Qt::WindowMaximized);

    qDebug()<<"floating plotwave";
  }
  else
  {
    qDebug()<<"not floating plotwave";
    ui->dock_wave->setFloating(true);
    ui->dock_wave->resize(ui->dock_wave->sizeHint());
  }

}

void MainWindow::onComDisconnected(COM_ERROR error)//断线错误检查
{
  emit stopThreadSampling();
  onActionDisConnectClicked();
  setComConnectStatus(false);
  m_actDisConnect->setChecked(true);
  uiStatus->btn_warring->show();
  QMap<COM_ERROR ,QString>warningMap;
  warningMap.insert(COM_ARM_OUT_TIME,tr("ARM OUT OFF TIME !"));
  warningMap.insert(COM_OK,tr("COM NET OK !"));
  warningMap.insert(COM_NET_ERROR,tr("COM NET ERROR !"));
  warningMap.insert(COM_NOT_FINISH,tr("COM NOT FINISH !"));
  uiStatus->warningMessge->setText(warningMap.value(error));
  qDebug()<<"timer stop";
  m_timer->stop();
  qDebug()<<tr("***************************com disconnect*********************");
  qDebug()<<tr("\r\n error type:%1" ).arg(warningMap.value(error));
  //后面加上错误信息显示
}
void MainWindow::onTimeOut()
{
  static int axisIndex=0;
  emit timeOut(axisIndex);
  axisIndex++;
  if(axisIndex>=mp_userConfig->model.axisCount)
    axisIndex=0;
}

void MainWindow::onAlmError()
{
  uiStatus->btn_warring->show();
//  qDebug()<<">>>>>>>>>>>>>>>>> alm error <<<<<<<<<<<<<<<<";
}
void MainWindow::onAlmClearFinish()
{
  uiStatus->btn_warring->hide();
}

void MainWindow::onBtnConfigGuideClicked()
{
  if(m_isOpenCom)
    onActionDisConnectClicked();//先关一下连接，即使没有连接上也没有关系
  // @uri QmlGlobalClass
  qmlRegisterType<QmlUserConfigProxy>("QmlGlobalClass", 1, 0, "QmlUserConfigProxy");
  qmlRegisterType<QmlConfigSetting>("QmlGlobalClass", 1, 0, "QmlConfigSetting");
  qmlRegisterType<SqlTableModel>("QmlGlobalClass", 1, 0, "SqlTableModel");
  qmlRegisterType<ControlName>("QmlGlobalClass", 1, 0, "ControlName");

  m_quickView=new QQuickView();

  //---------------------向qml中暴露成员--------------------------------
  QString sysConfigFilePath=SYSCONFIG_FILE_PATH;
  m_userConfigProxyQml=new QmlUserConfigProxy;
  m_userConfigProxyQml->setSourceUserConfig(mp_userConfig);
  m_quickView->rootContext()->setContextProperty("quickViewWindowSrc",m_quickView);
  m_quickView->rootContext()->setContextProperty("userConfigSrc",m_userConfigProxyQml);
  m_quickView->rootContext()->setContextProperty("mainWindowSrc",this);
  m_quickView->rootContext()->setContextProperty("sysConfigFilePathSrc",sysConfigFilePath);

  //----------------加载qml--------------------
  m_quickView->setResizeMode(QQuickView::SizeViewToRootObject);
//  QString typeName=mp_userConfig->typeName;
//  QString modelName=mp_userConfig->model.modelName;
//  QString version=mp_userConfig->model.version.at(0);
//  QString path=SYSCONFIG_FILE_PATH+typeName+"/"+modelName+"/NavigationUI/main.qml";
//  QString path=SYSCONFIG_FILE_PATH+typeName+"/NavigationUI/main.qml";
  QString path=SYSCONFIG_FILE_PATH+"/NavigationUI/main.qml";
  m_quickView->setSource(QUrl::fromLocalFile(path));

  m_quickView->setFlags(Qt::Window|Qt::FramelessWindowHint);
  m_quickView->setColor(QColor(Qt::transparent));

  //连接qml信号
  QQuickItem *rootObject=m_quickView->rootObject();
  QObject::connect(rootObject,SIGNAL(close()),this,SLOT(onQmlNavigationClose()));
  QObject::connect(rootObject,SIGNAL(finishAll()),this,SLOT(onQmlNavigationServoConfigUpdate()));
//  QObject *connectionsConfig=rootObject->findChild<QObject*>("ConnectionsConfig");
//  if(connectionsConfig!=NULL)
//    QObject::connect(connectionsConfig,SIGNAL(mainWindowUpdateUserConfig()),this,SLOT(onQmlNavigationServoConfigUpdate()));

  m_quickView->show();
  this->showMinimized();
}

void MainWindow::onQmlNavigationClose()
{
  m_quickView->deleteLater();
  m_quickView=NULL;
  qDebug()<<"quick view delete";
  this->showMaximized();
  m_userConfigProxyQml->deleteLater();
  m_userConfigProxyQml=NULL;
}
void MainWindow::onQmlNavigationServoConfigUpdate()
{
  QObject *obj=qobject_cast<QObject *> (sender());
  qDebug()<<"onQmlNavigationServoConfigUpdate:"<<obj->objectName();
  m_quickView->hide();
  this->showMaximized();

  enableAllUi(false);
  qApp->processEvents();//给其它事件响应的机会
  qDebug()<<mp_userConfig->com.comName;
  qDebug()<<mp_userConfig->typeName;
  qDebug()<<mp_userConfig->model.modelName;
  qDebug()<<mp_userConfig->model.version.at(0);
  //更新曲线视图
  m_plotWave->onClearPlotWave();
  ui->dock_wave->hide();
  //关闭刷新时钟
  if(m_timer->isActive())
    m_timer->stop();
  //界面清0
  updateUiByUserConfig(mp_userConfig,mp_srcAllConfig);

  //更新界面参数
//  if(m_isOpenCom)
//  {
//    m_timer->start();
//    //更新每一个页面的参数
//    for(int i=0;i<ui->stackedWidget->count();i++)
//    {
//      AbstractFuncWidget *absWidget;
//      absWidget=static_cast<AbstractFuncWidget *>(ui->stackedWidget->widget(i));
//      absWidget->onActionReadFuncValueFromRam();
//    }
//  }
  enableAllUi(true);
  onActionConnectClicked();
  m_actConnect->setChecked(true);
  m_actDisConnect->setChecked(false);
  onQmlNavigationClose();
}
void MainWindow::onXmlPrmToServo(int axis, int value)
{
  ui->progressBar->setValue(value);
  if(value%20==0)
  {
    ui->statusBar->showMessage(tr("axis:%1   writing xml parameters to servo....").arg(axis));
    qApp->processEvents();
  }
  if((axis==(mp_userConfig->model.axisCount-1))&&value>90)
    ui->statusBar->showMessage(tr("write xml parameters to servo successfully !"),1000);

}

//-------------protected function-------------------------
void MainWindow::keyPressEvent(QKeyEvent *keyEvent)
{
  if(keyEvent->key()==Qt::Key_Escape)
    onActionViewFullScreen();
  QWidget::keyPressEvent(keyEvent);
}

//----------------------------private function----------------
void MainWindow::createMenus(void)
{
  //--------------config menu---------------------------
  m_menuConfig=menuBar()->addMenu(tr("Config(&C)"));
  m_menuConfig->addAction(m_actConfigNew);
  m_menuConfig->addAction(m_actConfigOpen);
  m_menuConfig->addSeparator();
  m_menuConfig->addAction(m_actConfigSave);
  m_menuConfig->addAction(m_actConfigSaveAs);
  m_menuConfig->addSeparator();
  m_menuConfigRecent=new QMenu(tr("recentFiles"));
  m_menuConfigRecent->setIcon(QIcon(ICON_FILE_PATH+ICON_MENU_HISTORY_FILE));
  m_menuConfig->addMenu(m_menuConfigRecent);
  m_menuConfigRecent->addActions(m_actConfigHistory);
  m_menuConfig->addSeparator();
  m_menuConfig->addAction(m_actConfigExit);
  //---------------view menu------------------
  m_menuView=menuBar()->addMenu(tr("View(&V)"));
  m_menuView->addAction(m_actNavigation);
  m_menuView->addAction(m_actPlotCurve);
  m_menuView->addAction(m_actFullScreen);
  //--------------tool menu-------------------
  m_menuTool=menuBar()->addMenu(tr("Tool(&T)"));
  m_menuTool->addAction(m_actXmUpdate);

  m_actFileservo=new QAction(QIcon(ICON_FILE_PATH+ICON_MENU_FILE2SERVO),tr("File2Servo"),this);
  connect(m_actFileservo,SIGNAL(triggered(bool)),this,SLOT(onActionFile2ServoClicked()));
  m_actServofile=new QAction(QIcon(ICON_FILE_PATH+ICON_MENU_SERVO2FILE),tr("Servo2File"),this);
  connect(m_actServofile,SIGNAL(triggered(bool)),this,SLOT(onActionServo2FileClicked()));
  m_menuTool->addAction(m_actFileservo);
  m_menuTool->addAction(m_actServofile);
  m_menuTool->addAction(m_actFPGAControl);
  m_menuTool->addAction(m_actAxisClone);
  m_menuTool->addAction(m_actAxisFileClone);
  m_menuTool->addSeparator();
  m_menuTool->addAction(m_actFuncConfig);
  m_menuTool->addAction(m_actFuncSave);
  m_menuTool->addAction(m_actNormalizeTree);
  QMenu *menuDsp=new QMenu(tr("DSPUpdate"),this);
  m_menuTool->addMenu(menuDsp);
  menuDsp->addAction(m_actProgramUpdate);
  menuDsp->addAction(m_actResetServo);
  menuDsp->addAction(m_actRestoreFactorySetting);
  //--------------help menu-------------------
  m_menuHelp=menuBar()->addMenu(tr("Help(&H)"));
  m_menuHelp->addAction(m_actAboutConfig);
  m_menuHelp->addSeparator();
  m_menuHelp->addAction(m_actAboutSDT);
}

void MainWindow::createActions(void)
{
  //---------------------------config action-------------------------------------------
  m_actConfigNew=new QAction(QIcon(ICON_FILE_PATH+ICON_MENU_NEW),tr("&New"),this);
  m_actConfigNew->setToolTip(tr("create a new configration file"));
  m_actConfigNew->setStatusTip(tr("create a new configration file"));
  connect(m_actConfigNew,SIGNAL(triggered(bool)),this,SLOT(onActionConfigNewClicked()));

  m_actConfigOpen=new QAction(QIcon(ICON_FILE_PATH+ICON_MENU_OPEN),tr("&Open"),this);
  m_actConfigOpen->setToolTip(tr("open a new configration file"));
  m_actConfigOpen->setStatusTip(tr("open a new configration file"));
  connect(m_actConfigOpen,SIGNAL(triggered(bool)),this,SLOT(onActionConfigOpenClicked()));

  m_actConfigSave=new QAction(QIcon(ICON_FILE_PATH+ICON_MENU_SAVE),tr("&Save"),this);
  m_actConfigSave->setToolTip(tr("save a configration file"));
  m_actConfigSave->setStatusTip(tr("save a configration file"));
  connect(m_actConfigSave,SIGNAL(triggered(bool)),this,SLOT(onActionConfigSaveClicked()));

  m_actConfigSaveAs=new QAction(QIcon(ICON_FILE_PATH+ICON_MENU_SAVEAS),tr("&SaveAs"),this);
  m_actConfigSaveAs->setToolTip(tr("save another configration file"));
  m_actConfigSaveAs->setStatusTip(tr("save another configration file"));
  connect(m_actConfigSaveAs,SIGNAL(triggered(bool)),this,SLOT(onActionConfigSaveAsClicked()));

  m_actConfigExit=new QAction(QIcon(ICON_FILE_PATH+ICON_MENU_EXIT),tr("&Exit"),this);
  m_actConfigExit->setToolTip(tr("exit the application"));
  m_actConfigExit->setStatusTip(tr("exit the application"));
  connect(m_actConfigExit,SIGNAL(triggered(bool)),this,SLOT(onActionConfigExitClicked()));

  //-------------connect--disconnect----------------------
  m_actConnect=new QAction(QIcon(ICON_FILE_PATH+ICON_MENU_CONNECT),tr("&Connect"),this);
  m_actConnect->setToolTip(tr("connect the servo"));
  m_actConnect->setStatusTip(tr("connect the servo"));
  m_actConnect->setCheckable(true);
  m_actConnect->setChecked(false);
  connect(m_actConnect,SIGNAL(triggered(bool)),this,SLOT(onActionConnectClicked()));

  m_actDisConnect=new QAction(QIcon(ICON_FILE_PATH+ICON_MENU_DISCONNECT),tr("&Disconnect"),this);
  m_actDisConnect->setToolTip(tr("disconnect the servo"));
  m_actDisConnect->setStatusTip(tr("disconnect the servo"));
  m_actDisConnect->setCheckable(true);
  m_actDisConnect->setChecked(true);
  connect(m_actDisConnect,SIGNAL(triggered(bool)),this,SLOT(onActionDisConnectClicked()));

  QActionGroup *agroup=new QActionGroup(this);
  agroup->addAction(m_actConnect);
  agroup->addAction(m_actDisConnect);

  createHistoryAction();

  //-----------------------------view action--------------------------------------------
  m_actNavigation=new QAction(QIcon(ICON_FILE_PATH+ICON_MENU_TREEVIEW),tr("&Navigation"),this);
  m_actNavigation->setToolTip(tr("show or hide navigation tree"));
  m_actNavigation->setStatusTip(tr("show or hide navigation tree"));
  connect(m_actNavigation,SIGNAL(triggered(bool)),this,SLOT(onActionViewNavigation()));

  m_actPlotCurve=new QAction(QIcon(ICON_FILE_PATH+ICON_MENU_PLOTVIEW),tr("&Plot"),this);
  m_actPlotCurve->setToolTip(tr("show or hide plot view"));
  m_actPlotCurve->setStatusTip(tr("show or hide plot view"));
  connect(m_actPlotCurve,SIGNAL(triggered(bool)),this,SLOT(onActionViewPlotCurve()));

  m_actFullScreen=new QAction(QIcon(ICON_FILE_PATH+ICON_MENU_SMALLSCREEN),tr("&Full"),this);
  m_actFullScreen->setToolTip(tr("show or hide full view"));
  m_actFullScreen->setStatusTip(tr("show or hide full view"));
  connect(m_actFullScreen,SIGNAL(triggered(bool)),this,SLOT(onActionViewFullScreen()));

  //------------------------------tool action------------------------------------
  m_actXmUpdate=new QAction(QIcon(ICON_FILE_PATH+ICON_MENU_XMLUPDATE),tr("xmlupdate"),this);
  m_actXmUpdate->setToolTip(tr("update xml file"));
  m_actXmUpdate->setStatusTip(tr("update xml file"));
  connect(m_actXmUpdate,SIGNAL(triggered(bool)),this,SLOT(onActionToolXmlUpdateClicked()));

  m_actFile2Servo = new MAction(this,QIcon(ICON_FILE_PATH+ICON_MENU_FILE2SERVO));
  m_actFile2Servo->setToolTip(tr("file update to servo"));
  m_actFile2Servo->setStatusTip(tr("file update to servo"));
  connect(m_actFile2Servo,SIGNAL(triggered(bool)),this,SLOT(onActionFile2ServoClicked()));

  m_actServo2File = new MAction(this,QIcon(ICON_FILE_PATH+ICON_MENU_SERVO2FILE));
  m_actServo2File->setToolTip(tr("save sevo file"));
  m_actServo2File->setStatusTip(tr("save sevo file"));
  connect(m_actServo2File,SIGNAL(triggered(bool)),this,SLOT(onActionServo2FileClicked()));

  m_actAxisClone=new QAction(this);
  m_actAxisClone->setText(tr("clone"));
  m_actAxisClone->setToolTip(tr("clone one axis'parameters to another"));
  m_actAxisClone->setStatusTip(tr("clone one axis'parameters to another"));
  m_actAxisClone->setIcon(QIcon(ICON_FILE_PATH+ICON_CLONE));
  connect(m_actAxisClone,SIGNAL(triggered(bool)),this,SLOT(onActionAxisCloneClicked()));

  m_actAxisFileClone=new QAction(this);
  m_actAxisFileClone->setText(tr("file copy"));
  m_actAxisFileClone->setToolTip(tr("copy one axis'parameters to another and save file"));
  m_actAxisFileClone->setStatusTip(tr("copy one axis'parameters to another and save file"));
  m_actAxisFileClone->setIcon(QIcon(ICON_FILE_PATH+ICON_FILE_CLONE));
  connect(m_actAxisFileClone,SIGNAL(triggered(bool)),this,SLOT(onActionAxisFileCloneClicked()));

  m_actFuncConfig=new QAction(this);
  m_actFuncConfig->setEnabled(false);
  m_actFuncConfig->setText(tr("Config"));
  m_actFuncConfig->setToolTip(tr("Config"));
  m_actFuncConfig->setStatusTip(tr("set parameters to func_RAM"));
  m_actFuncConfig->setIcon(QIcon(ICON_FILE_PATH+ICON_FUNCCONFIG));
  connect(m_actFuncConfig,SIGNAL(triggered(bool)),this,SLOT(onActionFuncConfigClicked()));

  m_actFuncSave=new QAction(this);
  m_actFuncSave->setText(tr("Save"));
  m_actFuncSave->setIcon(QIcon(ICON_FILE_PATH+ICON_FUNCSAVE));
  m_actFuncSave->setToolTip(tr("Save FLASH"));
  m_actFuncSave->setStatusTip(tr("download parameters to func_FLASH"));
  connect(m_actFuncSave,SIGNAL(triggered(bool)),this,SLOT(onActionFuncSaveClicked()));

  m_actFPGAControl=new QAction(this);
  m_actFPGAControl->setText(tr("FPGA"));
  m_actFPGAControl->setIcon(QIcon(ICON_FILE_PATH+ICON_FPGA_CONFIG));
  m_actFPGAControl->setToolTip(tr("FPGA configuration"));
  m_actFPGAControl->setStatusTip(tr("FPGA configuration"));
  connect(m_actFPGAControl,SIGNAL(triggered(bool)),this,SLOT(onActionFPGAConfigurationClicked()));


  //程序烧写
  m_actProgramUpdate=new QAction(this);
  m_actProgramUpdate->setText(tr("Download"));
  m_actProgramUpdate->setIcon(QIcon(ICON_FILE_PATH+ICON_MENU_DOWNLOAD));
  m_actProgramUpdate->setToolTip(tr("Download program"));
  m_actProgramUpdate->setStatusTip(tr("Download program"));
  connect(m_actProgramUpdate,SIGNAL(triggered(bool)),this,SLOT(onActionProgramDownloadClicked()));

  m_actResetServo=new QAction(this);
  m_actResetServo->setText(tr("RESETDSP"));
  m_actResetServo->setIcon(QIcon(ICON_FILE_PATH+ICON_MENU_RESETSERVO));
  m_actResetServo->setToolTip(tr("Reset DSP"));
  m_actResetServo->setStatusTip(tr("Reset DSP"));
  connect(m_actResetServo,SIGNAL(triggered(bool)),this,SLOT(onActionResetDSPClicked()));

  m_actRestoreFactorySetting=new QAction(this);
  m_actRestoreFactorySetting->setText(tr("RestoreSetting"));
  m_actRestoreFactorySetting->setIcon(QIcon(ICON_FILE_PATH+ICON_MENU_RESTORESETTING));
  m_actRestoreFactorySetting->setToolTip(tr("Restore Factory setting"));
  m_actRestoreFactorySetting->setStatusTip(tr("Restore Factory setting"));
  connect(m_actRestoreFactorySetting,SIGNAL(triggered(bool)),this,SLOT(onActionRestoreFactorySettingClicked()));

  m_actNormalizeTree=new QAction(this);
  m_actNormalizeTree->setText(tr("NormalizeTree"));
//  m_actNormalizeTree->setIcon(QIcon(ICON_FILE_PATH+ICON_MENU_RESETSERVO));
  m_actNormalizeTree->setToolTip(tr("Normalize Tree Content"));
  m_actNormalizeTree->setStatusTip(tr("Normalize Tree Content"));
  connect(m_actNormalizeTree,SIGNAL(triggered(bool)),this,SLOT(onActionNormalizeTreeClicked()));


  //help action
  m_actAboutConfig=new QAction(this);
  m_actAboutConfig->setText(tr("AboutConfiguration"));
//  m_actNormalizeTree->setIcon(QIcon(ICON_FILE_PATH+ICON_MENU_RESETSERVO));
  m_actAboutConfig->setToolTip(tr("About Current Configuration"));
  m_actAboutConfig->setStatusTip(tr("About Current Configuration"));
  connect(m_actAboutConfig,SIGNAL(triggered(bool)),this,SLOT(onActionAboutConfigClicked()));

  m_actAboutSDT=new QAction(this);
  m_actAboutSDT->setText(tr("AboutSDT"));
//  m_actNormalizeTree->setIcon(QIcon(ICON_FILE_PATH+ICON_MENU_RESETSERVO));
  m_actAboutSDT->setToolTip(tr("About SDT version"));
  m_actAboutSDT->setStatusTip(tr("About SDT version"));
  connect(m_actAboutSDT,SIGNAL(triggered(bool)),this,SLOT(onActionAboutSDTClicked()));
}

void MainWindow::createToolBars(void)
{
  //----------config toolbar---------------
  ui->mainToolBar->setWindowTitle("Main");
  ui->mainToolBar->addAction(m_actConnect);
  ui->mainToolBar->addAction(m_actDisConnect);
  ui->mainToolBar->addSeparator();
  ui->mainToolBar->addAction(m_actConfigNew);
  //ui->mainToolBar->addAction(m_actConfigOpen);
  ui->mainToolBar->addSeparator();
  //ui->mainToolBar->addAction(m_actConfigSave);
  //ui->mainToolBar->addAction(m_actConfigSaveAs);
  //ui->mainToolBar->addSeparator();
  //ui->mainToolBar->addAction(m_actConfigExit);
  //---------view toolbar--------------------
  m_toolBarView=addToolBar(tr("ViewControl"));
  m_toolBarView->addAction(m_actNavigation);
  m_toolBarView->addAction(m_actPlotCurve);
  m_toolBarView->addSeparator();
  m_toolBarView->addAction(m_actFullScreen);
  //----------tool toolbar-------------------
  m_toolBarTool=addToolBar(tr("Tool"));
  m_toolBarTool->addAction(m_actFile2Servo);
  m_toolBarTool->addAction(m_actXmUpdate);
  m_toolBarTool->addAction(m_actServo2File);
  m_toolBarTool->addAction(m_actFPGAControl);
  m_toolBarTool->addAction(m_actAxisClone);
  m_toolBarTool->addAction(m_actAxisFileClone);
  m_toolBarTool->addSeparator();
  m_toolBarTool->addAction(m_actFuncConfig);
  m_toolBarTool->addAction(m_actFuncSave);

}

void MainWindow::createConnections()
{
  connect(ui->treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(onTreeWidgetItemClicked(QTreeWidgetItem*,int)));
  connect(ui->combo_axis,SIGNAL(currentIndexChanged(int)),this,SLOT(onComboBoxAxisSelectClicked(int)));
  connect(this,SIGNAL(updateProgressBar(int)),ui->progressBar,SLOT(setValue(int)));

}

void MainWindow::createStatusUi()
{
  QWidget *w=new QWidget(ui->statusBar);
  uiStatus->setupUi(w);
  uiStatus->btn_connect->setIcon(QIcon(ICON_FILE_PATH+ICON_STATUS_DISCONNECT));
  uiStatus->btn_connect->setText("neterror");
  uiStatus->btn_connect->setToolTip(tr("com error"));

  uiStatus->btn_warring->setIcon(QIcon(ICON_FILE_PATH+ICON_STATUS_SERVO_ERROR));
  uiStatus->btn_warring->setText(tr("ERROR"));
  uiStatus->btn_warring->setToolTip(tr("servo_error"));

  ui->statusBar->addPermanentWidget(w,0);
}

void MainWindow::initialUi()
{
  m_xml=new XmlBuilder(this);
  m_xml->parseSysConfig(SYSCONFIG_START_FILE_PATH);
  m_xml->parseFileRecent(CUSTOM_START_FILE_PATH);
//      qDebug()<<SYSCONFIG_START_FILE_PATH;
  mp_fileRecent=m_xml->getFileRecentInfo();
  mp_srcAllConfig=m_xml->getSysConfig();//获得所有的树信息在这里

  QString filePath;
  //--------------------------第一次打开软件加载默认路径--------------------------------
  if(mp_fileRecent->firstBit)
  {
//    qDebug()<<"bit true";
    filePath=CUSTOM_DEFAULT_FILE_PATH;
  }
  //---------------------------读取最近文件---------------------------
  else
  {
//    qDebug()<<"bit false";
    filePath=mp_fileRecent->filePathList.at(0);
  }
  if(m_xml->parseUserConfig(filePath))//读取用户配置文件成功加载，新建界面
  {
    mp_userConfig=m_xml->getUserConfig();
    updateUiByUserConfig(mp_userConfig,mp_srcAllConfig);
  }
  else//加载失败后新建配置
  {

  }


  QString style1="QPushButton{border:2px groove gray;border-radius:10px;padding:2px 4px;background-color:lightgreen;min-height:20;}\
                  QPushButton:hover{background-color:lightgray}\
                  QPushButton:pressed{background-color:gray}";
  ui->btn_navigation->setStyleSheet(style1);
  ui->btn_navigation->setText(tr("config guide"));
  ui->btn_navigation->setIcon(QIcon(ICON_FILE_PATH+ICON_CONFIG_WIZARD));
  connect(ui->btn_navigation,SIGNAL(clicked(bool)),this,SLOT(onBtnConfigGuideClicked()));

//  QString style2="QDockWidget{background-color:lightgray}";
  QString style3="QMainWindow::separator {background: lightgray; width: 1px;  height: 1px; }";//为分割器增加分割线
  qApp->setStyleSheet(style3);
  style3="QStatusBar {background-color:lightgray;border: 1px solid darkgray;}";
  ui->statusBar->setStyleSheet(style3);
//  style3="QMenuBar {background-color: lightgray;}";
//  ui->menuBar->setStyleSheet(style3);

  ui->dock_navigation->setWindowTitle(tr("navigation tree"));
  ui->dock_wave->setWindowTitle(tr("plot wave"));

//  QString ss=QString(SYSCONFIG_FILE_PATH+mp_userConfig->model.modelName+"/"+mp_userConfig->model.version.at(0)+"/FlashPrm_AllAxis.xml");
//  mp_flashAllTreeWidget=m_xml->readTreeWidgetFromFile(ss);
//  QWidget *mwidget=new QWidget();
//  QHBoxLayout *hLayout=new QHBoxLayout(mwidget);
//  hLayout->addWidget(mp_flashAllTreeWidget);
//  mwidget->show();
}

void MainWindow::updateUiByUserConfig(UserConfig *theconfig, SysConfig *srcConfig)
{
  //界面先清0
  ui->progressBar->show();
  ui->progressBar->setValue(10);
  QWidget *stackWidget=NULL;
  while(ui->stackedWidget->count()>0)//删除多余的widget
  {
    stackWidget=ui->stackedWidget->currentWidget();
    ui->stackedWidget->removeWidget(stackWidget);
    delete stackWidget;
  }
  ui->treeWidget->clear();
  disconnect(ui->combo_axis,SIGNAL(currentIndexChanged(int)),this,SLOT(onComboBoxAxisSelectClicked(int)));
  ui->combo_axis->clear();

  emit updateProgressBar(10);

  //初始化combobox
  QString strmodel=theconfig->model.modelName;
  for(int i=0;i<theconfig->model.axisCount;i++)
  {
    ui->combo_axis->addItem(QIcon(ICON_FILE_PATH+ICON_MOTOR),strmodel+tr("_S%1").arg(i));
  }
  ui->progressBar->setValue(12);
  DownloadDialog::delayms(20);
  //找到树结构
  for(int i=0;i<srcConfig->sysTypeList.size();i++)
  {
    if(srcConfig->sysTypeList.at(i).typeId==theconfig->typeId)
    {
//      qDebug()<<i;
      theconfig->treeItem=srcConfig->sysTypeList.at(i).treeItem;//用户指向该树
    }
  }


//  ui->treeWidget->setColumnCount(4);
  ui->progressBar->setValue(15);
  DownloadDialog::delayms(20);

  //初始化树ui
  for(int i=0;i<theconfig->treeItem->childCount();i++)
  {
//    qDebug()<<theconfig->treeItem->child(i)->text(0);
    QTreeWidgetItem *item=theconfig->treeItem->child(i)->clone();
    item->setIcon(0,QIcon(ICON_FILE_PATH+ICON_CONFIGTREE));
    ui->treeWidget->addTopLevelItem(item);
  }

  ui->progressBar->setValue(20);
  DownloadDialog::delayms(20);

  int index2=0;
  QString classname;

//  RegisterFunction::registerAll();
  AbstractFuncWidget *absWidget=NULL;
  QWidget *widget=NULL;
  QString configName;
  QString fileName;

  configName=SYSCONFIG_FILE_PATH+theconfig->typeName+"/"+theconfig->model.modelName+"/"+theconfig->model.version.at(0)+"/";

  //cmd命令控制树建立
  if(mp_functionCmdTreeWidget!=NULL)
  {
    mp_functionCmdTreeWidget->clear();
    delete mp_functionCmdTreeWidget;
    mp_functionCmdTreeWidget=NULL;
  }
  fileName=QString(FILENAME_FUNCCMD)+".xml";
  mp_functionCmdTreeWidget=m_xml->readTreeWidgetFromFile(QString(configName+fileName));
//  qDebug()<<"mp_functionCmdTreeWidget  update";

  //根据名称新建类
  double valueinc=20;
  double incStep=60/(theconfig->model.axisCount*ui->treeWidget->topLevelItemCount());
  for(int k=0;k<theconfig->model.axisCount;k++)
  {
    //k代表轴号
    int index=0;
    m_itemSize=0;
    for(int i=0;i<ui->treeWidget->topLevelItemCount();i++)
    {
      valueinc+=incStep;
      emit updateProgressBar(valueinc);
//      qDebug()<<"topLevelItemCount: "<<i;
      for(int j=0;j<ui->treeWidget->topLevelItem(i)->childCount();j++)
      {
        classname=ui->treeWidget->topLevelItem(i)->child(j)->text(COL_NAVIGATION_TREE_CLASSNAME);
        fileName=ui->treeWidget->topLevelItem(i)->child(j)->text(COL_NAVIGATION_TREE_PRMFILE);
        widget= Factory::createObject(classname.toLatin1(),0);
        if(widget!=NULL)
        {
          absWidget=static_cast<AbstractFuncWidget *>(widget);
          absWidget->readTreeXMLFile(this,QString(configName+fileName),k);
          ui->stackedWidget->addWidget(absWidget);
//          qDebug()<<"add widget :"<<absWidget->objectName();
          updateStartUpMessage(fileName);
          qApp->processEvents();//给其它事件响应的机会
        }
        ui->treeWidget->topLevelItem(i)->child(j)->setText(COL_NAVIGATION_TREE_UIINDEX,QString::number(index));
        ui->treeWidget->topLevelItem(i)->child(j)->setText(COL_NAVIGATION_TREE_GLOBAL,QString::number(UI_TAB_TYPE_AXIS));
        ui->treeWidget->topLevelItem(i)->child(j)->setIcon(COL_NAVIGATION_TREE_NAME,QIcon(ICON_FILE_PATH+ICON_CHILD));
        index++;
        m_itemSize++;
        index2++;
//        qDebug()<<"child classname"<<classname<<"index  "<<index<<"index2: "<<index2;
      }
    }
  }
  updateStartUpMessage(tr("initial ui......"));
//  qDebug()<<"finish the dynamic instance....";
  ui->stackedWidget->setCurrentIndex(0);
  ui->treeWidget->setCurrentItem(ui->treeWidget->topLevelItem(0)->child(0));
//  qDebug()<<"stackewidget count:"<<ui->stackedWidget->count();

  //-----------------增加额外的导航树结构----------------------------
  //---------------Global树定义-----------------------------
  //***Global
  //---***PlotAnalyses 0代表绘图
  //---***????         1预留

  //专家分析 ->示波图
  QTreeWidgetItem *itemGlobal=new QTreeWidgetItem(ui->treeWidget->invisibleRootItem());
  itemGlobal->setText(COL_NAVIGATION_TREE_NAME,tr("Global"));
  itemGlobal->setIcon(0,QIcon(ICON_FILE_PATH+ICON_TREE_GLOBAL_ANALYSIS));
  QTreeWidgetItem *itemGlobalChild=new QTreeWidgetItem(itemGlobal);
  itemGlobalChild->setText(COL_NAVIGATION_TREE_NAME,tr("PlotAnalyses"));
  itemGlobalChild->setText(COL_NAVIGATION_TREE_CLASSNAME,QString::number(GLOBAL_UI_PLOTVIEW));
  itemGlobalChild->setText(COL_NAVIGATION_TREE_GLOBAL,QString::number(UI_TAB_TYPE_GLOBAL));//设置是全局标志位
  itemGlobalChild->setIcon(0,QIcon(ICON_FILE_PATH+ICON_MENU_PLOTVIEW));
  //通用 ->IO
  itemGlobal=new QTreeWidgetItem(ui->treeWidget->invisibleRootItem());
  itemGlobal->setText(COL_NAVIGATION_TREE_NAME,tr("General"));
  itemGlobal->setIcon(0,QIcon(ICON_FILE_PATH+ICON_TREE_GLOBAL));
  itemGlobalChild=new QTreeWidgetItem(itemGlobal);
  itemGlobalChild->setText(COL_NAVIGATION_TREE_NAME,tr("IOModule"));
  itemGlobalChild->setText(COL_NAVIGATION_TREE_CLASSNAME,QString::number(GLOBAL_UI_IOMODULE));
  itemGlobalChild->setText(COL_NAVIGATION_TREE_GLOBAL,QString::number(UI_TAB_TYPE_GLOBAL));//设置是全局标志位
  itemGlobalChild->setIcon(0,QIcon(ICON_FILE_PATH+ICON_TREE_GLOBAL_IO));
  ModuleIO *ioWidget=new ModuleIO(0);//这个已经在进来时释放空间了，所以不用释放
  fileName=FILENAME_MODULEIO;
  ioWidget->readTreeXMLFile(this,configName+fileName,0);
//  ioWidget->show();
  ui->stackedWidget->addWidget(ioWidget);

  ui->treeWidget->resizeColumnToContents(1);
  ui->treeWidget->resizeColumnToContents(2);
  ui->treeWidget->setHeaderHidden(true);
  ui->treeWidget->hideColumn(COL_NAVIGATION_TREE_CLASSNAME);
  ui->treeWidget->hideColumn(COL_NAVIGATION_TREE_PRMFILE);
  ui->treeWidget->hideColumn(COL_NAVIGATION_TREE_UIINDEX);
  ui->treeWidget->hideColumn(COL_NAVIGATION_TREE_GLOBAL);

  ui->treeWidget->expandAll();
  ui->treeWidget->resizeColumnToContents(0);
  //-----------------增加额外的导航树结构-------------------------end

  connect(ui->combo_axis,SIGNAL(currentIndexChanged(int)),this,SLOT(onComboBoxAxisSelectClicked(int)));
  ui->progressBar->hide();

  updateStartUpMessage(tr("clear the tree data......"));
  //更新所有轴的树结构
  if(mp_flashAllTreeWidget!=NULL)
  {
    mp_flashAllTreeWidget->clear();
    delete mp_flashAllTreeWidget;
    mp_flashAllTreeWidget=NULL;
  }
  if(mp_ramAllTreeWidget!=NULL)
  {
    mp_ramAllTreeWidget->clear();
    delete mp_ramAllTreeWidget;
    mp_ramAllTreeWidget=NULL;
  }

  if(mp_funcExtension!=NULL)
  {
    mp_funcExtension->clear();
    delete mp_funcExtension;
    mp_funcExtension=NULL;
  }

  fileName=QString(FILENAME_FLASHALL)+".xml";
  updateStartUpMessage(fileName);
  mp_flashAllTreeWidget=m_xml->readTreeWidgetFromFile(QString(configName+fileName));
//  qDebug()<<"flash tree update";
  emit updateProgressBar(80);
  DownloadDialog::delayms(20);
//  m_xml->ramFlashTreeWidgetNormalization(mp_flashAllTreeWidget);
  fileName=QString(FILENAME_RAMALL)+".xml";
  updateStartUpMessage(fileName);
  mp_ramAllTreeWidget=m_xml->readTreeWidgetFromFile(QString(configName+fileName));
//  qDebug()<<"mp_ramAllTreeWidget  update";
  emit updateProgressBar(85);
  DownloadDialog::delayms(20);
//  qDebug()<<"updateProgressBar  update 85";


  emit updateProgressBar(90);
  DownloadDialog::delayms(20);

  fileName=QString(FILENAME_FUNCEXT)+".xml";
  updateStartUpMessage(fileName);
  mp_funcExtension=m_xml->readTreeWidgetFromFile(QString(configName+fileName));

  //更新模块共享数据区
  m_moduleShareData.clear();
  QTreeWidgetItem *treeExtensionItem,*treeExtensionItemChild;
  QString key;
  int value;
  treeExtensionItem=mp_funcExtension->topLevelItem(ROW_FUNC_EXT_INDEX_ADVCONTROLPRM);
  treeExtensionItemChild=treeExtensionItem->child(ROW_ADVCTLPRM_INDEX_SERDRV);
  key=treeExtensionItemChild->text(COL_FUNC_EXTENSION_NAME);
  value=treeExtensionItemChild->text(COL_FUNC_EXTENSION_PARAMETER).toInt();
  m_moduleShareData.insert(key,value);
  treeExtensionItemChild=treeExtensionItem->child(ROW_ADVCTLPRM_INDEX_AUX);
  key=treeExtensionItemChild->text(COL_FUNC_EXTENSION_NAME);
  value=treeExtensionItemChild->text(COL_FUNC_EXTENSION_PARAMETER).toInt();
  m_moduleShareData.insert(key,value);

//  for(int i=0;i<mp_funcExtension->topLevelItemCount();i++)
//  {
//    treeExtensionItem=mp_funcExtension->topLevelItem(i);
//    if(treeExtensionItem->text(COL_FUNC_EXTENSION_NAME).contains(EXTENSION_ADVCONTROLPRM))
//    {
//      for(int j=0;j<treeExtensionItem->childCount();j++)
//      {
//        treeExtensionItemChild=treeExtensionItem->child(j);
//        int value=treeExtensionItemChild->text(COL_FUNC_EXTENSION_PARAMETER).toInt();
//        if(treeExtensionItemChild->text(COL_FUNC_EXTENSION_NAME).contains(EXTENSION_ADVCONTROLPRM_GSERVDRV))
//        {
//          m_moduleShareData.insert(EXTENSION_ADVCONTROLPRM_GSERVDRV,value);
//        }
//        else if(treeExtensionItemChild->text(COL_FUNC_EXTENSION_NAME).contains(EXTENSION_ADVCONTROLPRM_GAUXFUNC))
//        {
//          m_moduleShareData.insert(EXTENSION_ADVCONTROLPRM_GAUXFUNC,value);
//        }
//      }
//      break;
//    }
//  }
#if TEST_DEBUG
  QMapIterator<QString ,QVariant> imap(m_moduleShareData);
  while(imap.hasNext()){
    imap.next();
    qDebug()<<tr("Key:%1 Value:%2").arg(imap.key()).arg(imap.value().toInt());
  }
#endif

//  qDebug()<<"mp_funcExtension  update";
//  m_xml->ramFlashTreeWidgetNormalization(mp_ramAllTreeWidget);
//  QWidget *mwidget=new QWidget();
//  QHBoxLayout *hLayout=new QHBoxLayout(mwidget);
//  hLayout->addWidget(mp_functionCmdTreeWidget);
//  mwidget->show();
//  QWidget *mwidget2=new QWidget();
//  QHBoxLayout *hLayout2=new QHBoxLayout(mwidget2);
//  hLayout2->addWidget(mp_ramAllTreeWidget);
//  mwidget2->show();


  //因为RnNet与PcDebug的FPGA不一样，所以要从新更新
  if(m_fpgaDialogSetting!=NULL)
  {
    m_fpgaDialogSetting->close();
    delete m_fpgaDialogSetting;
    m_fpgaDialogSetting=NULL;
  }
  if(m_fpgaDialogSettingRnNet!=NULL)
  {
    m_fpgaDialogSettingRnNet->close();
    delete m_fpgaDialogSettingRnNet;
    m_fpgaDialogSettingRnNet=NULL;
  }

//  qDebug()<<"finish update";
  emit updateProgressBar(100);
  DownloadDialog::delayms(20);
  ui->progressBar->hide();
  updateStartUpMessage(tr(">>finish update"));
}

void MainWindow::createHistoryAction()
{
  QString filePath;
  QAction *action;
  foreach (filePath, mp_fileRecent->filePathList)
  {
    qDebug()<<filePath;
    action=new QAction(this);
    action->setIcon(QIcon(ICON_FILE_PATH+ICON_MENU_HISTORY_FILE));
    action->setText(filePath);
    action->setToolTip(tr("open file %1").arg(filePath));
    m_actConfigHistory.append(action);
    connect(action,SIGNAL(triggered(bool)),this,SLOT(onActionConfigHistoryClicked()));
  }
}
void MainWindow::updateStartUpMessage(QString message)
{
  static QString strWellcom=tr(">> GOOGOLTECH Servo Debug Assistant \n");
  static int count=0;
  count++;
  if(count>24)
  {
    strWellcom.clear();
    count=0;
  }
  strWellcom.append(">> Loading file:"+message+"......\n");
  emit startUpMessage(strWellcom, Qt::AlignLeft | Qt::AlignBottom, Qt::white);
}
void MainWindow::closeEvent(QCloseEvent *event)
{
  QMessageBox::StandardButton rb = QMessageBox::question(this, tr("Warring"), tr("Do you want to close?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
  if (rb == QMessageBox::Yes)
  {
    if(m_isOpenCom)
    {
      if(m_timer->isActive()) m_timer->stop();
      GTSD_CMD_Close(static_cast<COM_TYPE>(mp_userConfig->com.id));
    }
    for (int a = 0; a < 100; a++)
    {
      int i = 65536;
      while (i--);
    }
    //非激活每一个窗口
    AbstractFuncWidget *absWidget;
    for(int i=0;i<ui->stackedWidget->count();i++)
    {
      absWidget=static_cast<AbstractFuncWidget *>(ui->stackedWidget->widget(i));
      absWidget->setActiveNow(false);
  //        qDebug()<<absWidget->objectName();
    }
    event->accept();
    deleteLater();

  }
  else
  {
    event->ignore();
  }
}
void MainWindow::enableAllUi(bool state)
{
  m_toolBarView->setEnabled(state);
  m_toolBarTool->setEnabled(state);
//  ui->treeWidget->setEnabled(state);
  ui->dock_navigation->setEnabled(state);
  ui->mainToolBar->setEnabled(state);
  ui->stackedWidget->currentWidget()->setEnabled(state);
  ui->dock_wave->setEnabled(state);
}
void MainWindow::setUbootModeUi(bool sta)
{
  bool state=!sta;
  m_toolBarView->setEnabled(state);
  ui->dock_navigation->setEnabled(state);
  ui->stackedWidget->currentWidget()->setEnabled(state);

  m_actConfigNew->setEnabled(state);
  m_actConfigOpen->setEnabled(state);
  m_actConfigSave->setEnabled(state);
  m_actConfigSaveAs->setEnabled(state);
  m_actConfigExit->setEnabled(state);

  //--------tool action--------
  m_actXmUpdate->setEnabled(state);
  m_actServo2File->setEnabled(state);
  m_actAxisClone->setEnabled(state);
  m_actAxisFileClone->setEnabled(state);
  m_actFPGAControl->setEnabled(state);
  m_actFuncConfig->setEnabled(state);
  m_actFuncSave->setEnabled(state);

  //程序烧写
  m_actResetServo->setEnabled(state);
  m_actNormalizeTree->setEnabled(state);

  m_menuHelp->setEnabled(state);
  m_menuConfigRecent->setEnabled(state);
  m_menuConfig->setEnabled(state);
  m_menuView->setEnabled(state);
  m_actServofile->setEnabled(state);
}

void MainWindow::setComConnectStatus(bool isConnected)
{
  if(isConnected)
  {
    uiStatus->btn_connect->setToolTip(tr("com ok"));
    uiStatus->btn_connect->setIcon(QIcon(ICON_FILE_PATH+ICON_STATUS_CONNECT));
    m_isOpenCom=true;
  }
  else
  {
    uiStatus->btn_connect->setToolTip(tr("com error"));
    uiStatus->btn_connect->setIcon(QIcon(ICON_FILE_PATH+ICON_STATUS_DISCONNECT));
    m_isOpenCom=false;
  }
}
void MainWindow::updateProgessBarWhenRestoreClicked(void *arg, qint16 *value)
{
  QProgressBar *bar=static_cast<QProgressBar *>(arg);
  m_progessValue=(100/m_dspNum)*m_step+(*value)/m_dspNum;
  bar->setValue(m_progessValue);
}
void MainWindow::updateProgessBarWhenConnectClicked(void *arg, qint16 *value)
{
  QProgressBar *bar=static_cast<QProgressBar *>(arg);
  bar->setValue(*value);
  qDebug()<<"value "<<*value;
}
bool MainWindow::openNetCom()
{
  COM_ERROR error=COM_OK;
  bool isOpenCom=false;
  ui->progressBar->setVisible(true);
  enableAllUi(false);
  error=static_cast<COM_ERROR>(GTSD_CMD_Open(updateProgessBarWhenConnectClicked,(void*)ui->progressBar,static_cast<COM_TYPE>(mp_userConfig->com.id)));
  if(error!=COM_OK)
  {
    QMap<COM_ERROR ,QString>warningMap;
    warningMap.insert(COM_ARM_OUT_TIME,tr("ARM OUT OFF TIME !"));
    warningMap.insert(COM_OK,tr("COM NET OK !"));
    warningMap.insert(COM_NET_ERROR,tr("COM NET ERROR !"));
    warningMap.insert(COM_NOT_FINISH,tr("COM NOT FINISH !"));
    QString eMsg;
    if(warningMap.contains(error))
      eMsg=warningMap.value(error);
    else
      eMsg=tr("FPGA error or disconnect");
    QMessageBox::information(0,tr("connect"),tr("com connect error:%1").arg(eMsg));
    m_actConnect->setChecked(false);
    m_actDisConnect->setChecked(true);
    isOpenCom=false;
    error=static_cast<COM_ERROR>(GTSD_CMD_Close(static_cast<COM_TYPE>(mp_userConfig->com.id)));
    ui->progressBar->setVisible(false);
    enableAllUi(true);
    return isOpenCom;
  }
  //判断是不是千兆网络，提示相关信息后返回
  short netCarMsg=GTSD_CMD_GetNetCardMsg();
  QMap<int ,QString>netCarInfoMap;
  netCarInfoMap.insert(0,tr("1000M ETH"));
  netCarInfoMap.insert(1,tr("FUNCTION ADDRESS ERROR"));
  netCarInfoMap.insert(2,tr("NOT 1000M ETH"));
  netCarInfoMap.insert(3,tr("NO ETH"));
  if(netCarMsg!=0){
    QMessageBox::information(0,tr("net error"),tr("com net error information:%1").arg(netCarInfoMap.value(netCarMsg)));
    m_actConnect->setChecked(false);
    m_actDisConnect->setChecked(true);
    isOpenCom=false;
    error=static_cast<COM_ERROR>(GTSD_CMD_Close(static_cast<COM_TYPE>(mp_userConfig->com.id)));
    ui->progressBar->setVisible(false);
    enableAllUi(true);
    return isOpenCom;
  }
  isOpenCom=true;
  return isOpenCom;
}
void MainWindow::closeNetCom()
{
  m_actConnect->setChecked(false);
  m_actDisConnect->setChecked(true);
  m_isOpenCom=false;
  GTSD_CMD_Close(static_cast<COM_TYPE>(mp_userConfig->com.id));
  ui->progressBar->setVisible(false);
  enableAllUi(true);
}
