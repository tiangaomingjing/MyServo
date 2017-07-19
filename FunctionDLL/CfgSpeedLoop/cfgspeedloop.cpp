#include "cfgspeedloop.h"
#include "ui_CfgSpeedLoop.h"
#include "MainGTSD/mainwindow.h"

#include <QQuickWidget>
#include <QQmlContext>

CfgSpeedLoop::CfgSpeedLoop(QWidget *parent):
  AbstractFuncWidget(parent),
  ui(new Ui::CfgSpeedLoop())
{
  ui->setupUi(this);
  ui->stackedWidget->setCurrentIndex(0);
}
CfgSpeedLoop::~CfgSpeedLoop()
{
  qDebug()<<"CfgSpeedLoop release ->";
}

//!------------------private function-----------------------------

QTreeWidget* CfgSpeedLoop::getUiTree()
{
  QStringList headerlist;
  headerlist<<tr("name")<<tr("ID")<<tr("value")<<tr("type")\
              <<tr("axisnum")<<tr("parent")<<tr("unit")<<tr("introduction")\
              <<tr("deaultvalue")<<tr("uplimit")<<tr("downlimit")<<tr("funcnum");
  ui->treeWidget->setHeaderLabels(headerlist);
  ui->treeWidget->setColumnCount(COLUMN_SIZE);
  return ui->treeWidget;
}
QStackedWidget* CfgSpeedLoop::getUiStackedWidget()
{
  return ui->stackedWidget;
}

void CfgSpeedLoop::setActionReadFuncValueFromFlashEnable()
{
  m_actReadFuncValueFromFlash->setEnabled(true);
}
void CfgSpeedLoop::createUiByQml()
{
  QString version=mp_mainWindow->getUserConfig()->model.version.at(0);
  QString modelName=mp_mainWindow->getUserConfig()->model.modelName;
  QString typeName=mp_mainWindow->getUserConfig()->typeName;
  m_qmlFilePath=SYSCONFIG_FILE_PATH+typeName+"/"+modelName+"/"+version+"/ui/"+objectName()+".qml";
//  qDebug()<<tr("******************qml path:%1****************\n*****icon path %2********").arg(m_qmlFilePath).arg(m_qmlIconFilePath);

  QQuickWidget *qwidget=new QQuickWidget(this);
  qwidget->setMinimumSize(1000,560);
//  qwidget->rootContext()->setContextProperty("iconPath",m_qmlIconFilePath);
  qwidget->rootContext()->setContextProperty("driveSpeedLoop",this);
  qwidget->rootContext()->setContextProperty("axisIndex",m_axisNumber);
  qwidget->rootContext()->setContextProperty("treeSource",ui->treeWidget);

  qwidget->resize(1000,560);
  qwidget->setResizeMode(QQuickWidget::SizeRootObjectToView );
  qwidget->setSource(QUrl::fromLocalFile(m_qmlFilePath));
//  qwidget->show();
  ui->qmlHboxLayout->addWidget(qwidget);
}
void CfgSpeedLoop::setActiveNow(bool actived)
{
  if(actived)
    emit activeNow();
}
