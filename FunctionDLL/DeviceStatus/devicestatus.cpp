#include "devicestatus.h"
#include "ui_devicestatus.h"

#include "servocontrol.h"
#include "MainGTSD/mainwindow.h"
#include "qmlregisterincludes.h"

#include <QMessageBox>
#define ALMFLAG_NAME "gSevDrv.sev_obj.cur.pro.alm_flag"
#include "MainGTSD/mainwindow.h"

#include <QQuickWidget>
#include <QQmlContext>

//typedef enum tree_item :quint16
//{
//  TREE_ITEM_CURRSTATE=0,
//  TREE_ITEM_VDC,
//  TREE_ITEM_ALMFLAG,
//  TREE_ITEM_ALMCODE_ALL,
//  TREE_ITEM_CMDID,
//  TREE_ITEM_CURRID,
//  TREE_ITEM_USRMODE
//}TREE_ITEM;

DeviceStatus::DeviceStatus(QWidget *parent):
  AbstractFuncWidget(parent),
  ui(new Ui::DeviceStatus())
{
  ui->setupUi(this);
  ui->stackedWidget->setCurrentIndex(0);
}
DeviceStatus::~DeviceStatus()
{
  qDebug()<<"DeviceStatus release ->";
}
void DeviceStatus::clearAlarm()
{
  if(!mp_mainWindow->getComOpenState())
  {
    QMessageBox::information(0,tr("connect"),tr("please open com first !"));
    return;
  }
  ServoControl::clearAlarm(m_axisNumber,(COM_TYPE)mp_mainWindow->getUserConfig()->com.id,mp_mainWindow->getUserConfig()->com.rnStation);
  emit this->almClearFinish();
  qDebug()<<"clear alarm";
}

void DeviceStatus::onActionReadFuncValueFromRam()
{
  if(!mp_mainWindow->getComOpenState())
  {
    //QMessageBox::information(0,tr("connect"),tr("please open com first !"));
    return;
  }
  m_highLightInfo.enterFlag=true;
  UserConfig *config=mp_mainWindow->getUserConfig();
  COM_TYPE comtype=(COM_TYPE)config->com.id;
  QTreeWidget *cmdTree=mp_mainWindow->getFunctionCmdTree();

  COM_ERROR comReturn;
  comReturn=ServoControl::updateFunctionValueFromRamAllTree(ui->treeWidget,cmdTree,m_axisNumber,comtype,config->com.rnStation);
  if((comReturn==COM_NET_ERROR)||(comReturn==COM_ARM_OUT_TIME))
  {
    //断线
    emit netError(comReturn);
    return;
  }

  //给.alm_code.all下的位赋值
  //找到报警位
  QTreeWidgetItemIterator it(ui->treeWidget);
  QTreeWidgetItem *item;
  while (*it)
  {
    item=(*it);
    if(item->childCount()>0)//给位赋值
    {
      quint32 value;
      value=static_cast<quint32>(item->text(COL_FUNC_VALUE).toDouble());
      for(int i=0;i<item->childCount();i++)
      {
        int bit=0;
        bit=getBitValue(value,i);
        item->child(i)->setText(COL_FUNC_VALUE,QString::number(bit));
//        qDebug()<<"i:"<<i<<"bit :"<<bit;
      }
    }

    if(ALMFLAG_NAME==item->text(COL_FUNC_NAME))//查询报警总标志位
    {
      quint16 flag=item->text(COL_FUNC_VALUE).toDouble();
//      qDebug()<<"item text"<<item->text(COL_FUNC_NAME)<<"flag :"<<flag;
      if(flag==1)
      {
        //报警
        emit almError();
//        qDebug()<<"emit almError";
      }
    }
    it++;
  }
//  qDebug()<<this->objectName()<<"axisnum:"<<m_axisNumber<<" time out";

}
void DeviceStatus::onMainWindowTimeOut(int axisInex)
{
//  qDebug()<<"update onMainWindowTimeOut"<<m_axisNumber;
//  qDebug()<<"axis index:"<<axisInex;
  if(axisInex==m_axisNumber)
  {
//    qDebug()<<"update index:"<<axisInex;
    onActionReadFuncValueFromRam();
    emit timeOutToQml();
  }
}

//!------------------private function-----------------------------

QTreeWidget* DeviceStatus::getUiTree()
{
  QStringList headerlist;
  headerlist<<tr("name")<<tr("ID")<<tr("value")<<tr("type")\
              <<tr("axisnum")<<tr("parent")<<tr("unit")<<tr("introduction")\
              <<tr("deaultvalue")<<tr("uplimit")<<tr("downlimit")<<tr("funcnum");
  ui->treeWidget->setHeaderLabels(headerlist);
  ui->treeWidget->setColumnCount(COLUMN_SIZE);
  return ui->treeWidget;
}
QStackedWidget* DeviceStatus::getUiStackedWidget()
{
  return ui->stackedWidget;
}

void DeviceStatus::setActionReadFuncValueFromFlashEnable()
{
  m_actReadFuncValueFromFlash->setEnabled(false);
}
void DeviceStatus::createUiByQml()
{
  QString version=mp_mainWindow->getUserConfig()->model.version.at(0);
  QString modelName=mp_mainWindow->getUserConfig()->model.modelName;
  QString typeName=mp_mainWindow->getUserConfig()->typeName;
  m_qmlFilePath=SYSCONFIG_FILE_PATH+typeName+"/"+modelName+"/"+version+"/ui/"+objectName()+".qml";
//  qDebug()<<tr("******************qml path:%1****************\n*****icon path %2********").arg(m_qmlFilePath).arg(m_qmlIconFilePath);

  //expose c++ objet instance to qml
//  m_qmlEngine=new QQmlEngine(this);
//  m_qmlEngine->rootContext()->setContextProperty("mainPage",ui->mainPage);
//  m_qmlEngine->rootContext()->setContextProperty("mainWindowDevice",this);
//  m_qmlEngine->rootContext()->setContextProperty("treeSource",ui->treeWidget);
//  m_qmlEngine->rootContext()->setContextProperty("iconPath",m_qmlIconFilePath);

//  QQmlComponent component(m_qmlEngine,QUrl::fromLocalFile(m_qmlFilePath));
//  QObject *obj=component.create();
//  qDebug()<<tr("**************qml errors:")<<component.errors();
//  Q_UNUSED(obj);

  QQuickWidget *qwidget=new QQuickWidget(this);
  qwidget->setMinimumSize(600,560);
  qwidget->rootContext()->setContextProperty("iconFilePath",m_qmlIconFilePath);
  qwidget->rootContext()->setContextProperty("driverStatus",this);
  qwidget->rootContext()->setContextProperty("axisIndex",m_axisNumber);
  qwidget->rootContext()->setContextProperty("treeSource",ui->treeWidget);

  qwidget->resize(600,560);
  qwidget->setResizeMode(QQuickWidget::SizeRootObjectToView );
  qwidget->setSource(QUrl::fromLocalFile(m_qmlFilePath));
//  qwidget->show();
  ui->qmlHboxLayout->addWidget(qwidget);
}

void DeviceStatus::connectionSignalSlotHandler()
{
  connect(mp_mainWindow,SIGNAL(timeOut(int)),this,SLOT(onMainWindowTimeOut(int)));
  connect(this,SIGNAL(netError(COM_ERROR)),mp_mainWindow,SLOT(onComDisconnected(COM_ERROR)));
  connect(this,SIGNAL(almError()),mp_mainWindow,SLOT(onAlmError()));
  connect(this,SIGNAL(almClearFinish()),mp_mainWindow,SLOT(onAlmClearFinish()));
}

int DeviceStatus::getBitValue(quint32 value, quint32 index)
{
  int ret;
  ret=((value&(1<<index))==0)?0:1;
  return ret;
}

