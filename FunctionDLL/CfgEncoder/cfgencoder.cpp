#include "cfgencoder.h"
#include "ui_CfgEncoder.h"
#include "MainGTSD/mainwindow.h"

#include <QQuickWidget>
#include <QQmlContext>


CfgEncoder::CfgEncoder(QWidget *parent):
  AbstractFuncWidget(parent),
  ui(new Ui::CfgEncoder())
{
  ui->setupUi(this);
}
CfgEncoder::~CfgEncoder()
{
  qDebug()<<"CfgEncoder release ->";
}
bool CfgEncoder::getComConnectSatus()
{
  return mp_mainWindow->getComOpenState();
}

//!------------------private function-----------------------------
QTreeWidget* CfgEncoder::getUiTree()
{
  QStringList headerlist;
  headerlist<<tr("name")<<tr("ID")<<tr("value")<<tr("type")\
              <<tr("axisnum")<<tr("parent")<<tr("unit")<<tr("introduction")\
              <<tr("deaultvalue")<<tr("uplimit")<<tr("downlimit")<<tr("funcnum");
  ui->treeWidget->setHeaderLabels(headerlist);
  ui->treeWidget->setColumnCount(COLUMN_SIZE);
  return ui->treeWidget;
}
QStackedWidget* CfgEncoder::getUiStackedWidget()
{
  return ui->stackedWidget;
}
void CfgEncoder::createUiByQml()
{
  QString version=mp_mainWindow->getUserConfig()->model.version.at(0);
  QString modelName=mp_mainWindow->getUserConfig()->model.modelName;
  QString typeName=mp_mainWindow->getUserConfig()->typeName;
  m_qmlFilePath=SYSCONFIG_FILE_PATH+typeName+"/"+modelName+"/"+version+"/ui/"+objectName()+".qml";
//  qDebug()<<tr("******************qml path:%1****************\n*****icon path %2********").arg(m_qmlFilePath).arg(m_qmlIconFilePath);

  QString cmdFilePath=SYSCONFIG_FILE_PATH+typeName+"/"+modelName+"/"+version+"/PrmFuncCmd.xml";
  QString advCmdFilePath=SYSCONFIG_FILE_PATH+typeName+"/"+modelName+"/"+version+"/AdvanceFastCommand.xml";

  QQuickWidget *qwidget=new QQuickWidget(this);
  qwidget->setMinimumSize(600,560);
  qwidget->rootContext()->setContextProperty("cmdFilePathSrc",cmdFilePath);
  qwidget->rootContext()->setContextProperty("advCmdFilePathSrc",advCmdFilePath);
  qwidget->rootContext()->setContextProperty("driveEncoder",this);
  qwidget->rootContext()->setContextProperty("axisIndexSrc",m_axisNumber);
  qwidget->rootContext()->setContextProperty("treeSource",ui->treeWidget);
  qwidget->rootContext()->setContextProperty("comTypeSrc",mp_mainWindow->getUserConfig()->com.id);

  qwidget->resize(600,560);
  qwidget->setResizeMode(QQuickWidget::SizeRootObjectToView );
  qwidget->setSource(QUrl::fromLocalFile(m_qmlFilePath));
//  qwidget->show();
  ui->qmlHboxLayout->addWidget(qwidget);
}
void CfgEncoder::setActiveNow(bool actived)
{
  if(mp_mainWindow->getComOpenState())
    emit activeNow(actived);
  else
    emit activeNow(false);
}
