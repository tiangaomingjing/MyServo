#include "cfgmotor.h"
#include "ui_cfgmotor.h"
#include "MainGTSD/mainwindow.h"

#include <QQuickWidget>
#include <QQmlContext>


CfgMotor::CfgMotor(QWidget *parent):
  AbstractFuncWidget(parent),
  ui(new Ui::CfgMotor())
{
  ui->setupUi(this);
  ui->stackedWidget->setCurrentIndex(0);
}
CfgMotor::~CfgMotor()
{
  qDebug()<<"CfgMotor release ->";
}
void CfgMotor::updateUiWhenNavigationTreeClicked()
{
  onActionReadFuncValueFromFlash();
}
void CfgMotor::onWriteFuncTreetoServoFlash()
{
  ImaxExtensionPrm imaxPrm;
  ImaxExtensionPrmGain gainInfo;

  QTreeWidgetItem *imaxTreeItem;
  QTreeWidgetItem *imaxInfoItem;
  QTreeWidgetItem *gainInfoItem;
  AbstractFuncWidget::onWriteFuncTreetoServoFlash();
//  mp_mainWindow->clearWarning();
  emit clearWarning();
  imaxTreeItem=mp_mainWindow->getFunctionExtensionTree()\
          ->topLevelItem(ROW_FUNC_EXT_INDEX_MODULEPRM)\
          ->child(ROW_MODULEPRM_INDEX_MOTOR)\
          ->child(m_axisNumber);
  qDebug()<<"imaxTree: "<<imaxTreeItem->text(COL_FUNC_EXTENSION_NAME);
  imaxInfoItem=imaxTreeItem->child(ROW_MOTOR_AXIS_INDEX_IMAXINFO);
  gainInfoItem=imaxTreeItem->child(ROW_MOTOR_AXIS_INDEX_GAININFO);
  imaxPrm.imaxInfo.name=imaxInfoItem->child(ROW_IMAXINFO_INDEX_NAME)->text(COL_FUNC_EXTENSION_PARAMETER);
  imaxPrm.imaxInfo.type=imaxInfoItem->child(ROW_IMAXINFO_INDEX_TYPE)->text(COL_FUNC_EXTENSION_PARAMETER);
  imaxPrm.imaxInfo.offsetAddr=imaxInfoItem->child(ROW_IMAXINFO_INDEX_OFFSETADDR)->text(COL_FUNC_EXTENSION_PARAMETER).toUInt();

  for(int i=0;i<gainInfoItem->childCount();i++)
  {
    QTreeWidgetItem *item=gainInfoItem->child(i);
    qDebug()<<"child name: "<<item->text(COL_FUNC_EXTENSION_NAME);
    gainInfo.gain=item->text(COL_FUNC_EXTENSION_PARAMETER).toDouble();
    gainInfo.writeFlashName=item->child(0)->child(ROW_GAINWRFLASH_INDEX_NAME)->text(COL_FUNC_EXTENSION_PARAMETER);
    gainInfo.writeFlashType=item->child(0)->child(ROW_GAINWRFLASH_INDEX_TYPE)->text(COL_FUNC_EXTENSION_PARAMETER);
    gainInfo.offsetAddr=item->child(0)->child(ROW_GAINWRFLASH_INDEX_OFFSETADDR)->text(COL_FUNC_EXTENSION_PARAMETER).toUInt();
    imaxPrm.gainInfoList.append(gainInfo);
  }

  qDebug()<<"ImaxInfo: "<<" name:"<<imaxPrm.imaxInfo.name<<" type:"<<imaxPrm.imaxInfo.type<<" offset:"<<imaxPrm.imaxInfo.offsetAddr;
  foreach (gainInfo, imaxPrm.gainInfoList) {
    qDebug()<<"list in imaxExtPrmList: "<<gainInfo.writeFlashName<<" gain:"<<gainInfo.gain<<" type:"<<gainInfo.writeFlashType<<" offsetAddr:"<<gainInfo.offsetAddr;
  }
  //读Imax的原始值
  Uint16 imaxValue;
  int16 ret=0;
  UserConfig *config=mp_mainWindow->getUserConfig();
  ret=GTSD_CMD_Fram_Read16BitByAdr(m_axisNumber, (int16) imaxPrm.imaxInfo.offsetAddr, (int16*) &imaxValue, config->com.id, config->com.rnStation);
  if(ret!=0)
     ret=GTSD_CMD_Fram_Read16BitByAdr(m_axisNumber, (int16) imaxPrm.imaxInfo.offsetAddr, (int16*) &imaxValue, config->com.id, config->com.rnStation);
  qDebug()<<"imax value:"<<imaxValue;
  foreach (gainInfo, imaxPrm.gainInfoList)
  {
    double k=gainInfo.gain/imaxValue;
    if(k>32767)
    {
      k=32767;
//      mp_mainWindow->setWarningMessage(tr("Imax is too small!"));
      emit warningMessageChanged(tr("Imax is too small!"));
    }
    int16 value=(int16)k;
    ret=GTSD_CMD_Fram_Write16BitByAdr(m_axisNumber,gainInfo.offsetAddr,value,config->com.id, config->com.rnStation);
    if(ret!=0)
      ret=GTSD_CMD_Fram_Write16BitByAdr(m_axisNumber,gainInfo.offsetAddr,value,config->com.id, config->com.rnStation);
  }
}

//!-------------------------private function--------------------------
QTreeWidget* CfgMotor::getUiTree()
{
  QStringList headerlist;
  headerlist<<tr("name")<<tr("ID")<<tr("value")<<tr("type")\
              <<tr("axisnum")<<tr("parent")<<tr("unit")<<tr("introduction")\
              <<tr("deaultvalue")<<tr("uplimit")<<tr("downlimit")<<tr("funcnum");
  ui->treeWidget->setHeaderLabels(headerlist);
  ui->treeWidget->setColumnCount(COLUMN_SIZE);
  return ui->treeWidget;
}
QStackedWidget* CfgMotor::getUiStackedWidget()
{
  return ui->stackedWidget;
}

void CfgMotor::setActionReadFuncValueFromFlashEnable()
{
  m_actReadFuncValueFromFlash->setEnabled(true);
}
void CfgMotor::createUiByQml()
{
  QString version=mp_mainWindow->getUserConfig()->model.version.at(0);
  QString modelName=mp_mainWindow->getUserConfig()->model.modelName;
  QString typeName=mp_mainWindow->getUserConfig()->typeName;
  m_qmlFilePath=SYSCONFIG_FILE_PATH+typeName+"/"+modelName+"/"+version+"/ui/"+objectName()+".qml";
  qDebug()<<tr("******************qml path:%1****************\n*****icon path %2********").arg(m_qmlFilePath).arg(m_qmlIconFilePath);

  QQuickWidget *qwidget=new QQuickWidget(this);
  qwidget->setMinimumSize(600,560);
  qwidget->rootContext()->setContextProperty("iconPath",m_qmlIconFilePath);
  qwidget->rootContext()->setContextProperty("driveMotor",this);
  qwidget->rootContext()->setContextProperty("axisIndex",m_axisNumber);
  qwidget->rootContext()->setContextProperty("treeSource",ui->treeWidget);

  qwidget->resize(600,560);
  qwidget->setResizeMode(QQuickWidget::SizeRootObjectToView );
  qwidget->setSource(QUrl::fromLocalFile(m_qmlFilePath));
//  qwidget->show();
  ui->qmlHboxLayout->addWidget(qwidget);
}
void CfgMotor::connectionSignalSlotHandler()
{
  connect(this,SIGNAL(warningMessageChanged(QString&)),mp_mainWindow,SLOT(onWarningMessageChanged(QString&)));
  connect(this,SIGNAL(clearWarning()),mp_mainWindow,SLOT(onClearWarning()));
}
