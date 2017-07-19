#include "cfgpositionloop.h"
#include "ui_CfgPositionLoop.h"
#include "MainGTSD/mainwindow.h"

#include <QQuickWidget>
#include <QQmlContext>

CfgPositionLoop::CfgPositionLoop(QWidget *parent):
  AbstractFuncWidget(parent),
  ui(new Ui::CfgPositionLoop())
{
  ui->setupUi(this);
  ui->stackedWidget->setCurrentIndex(0);
}
CfgPositionLoop::~CfgPositionLoop()
{
  qDebug()<<"CfgPositionLoop release ->";
}
//!------------------private function-----------------------------

QTreeWidget* CfgPositionLoop::getUiTree()
{
  QStringList headerlist;
  headerlist<<tr("name")<<tr("ID")<<tr("value")<<tr("type")\
              <<tr("axisnum")<<tr("parent")<<tr("unit")<<tr("introduction")\
              <<tr("deaultvalue")<<tr("uplimit")<<tr("downlimit")<<tr("funcnum");
  ui->treeWidget->setHeaderLabels(headerlist);
  ui->treeWidget->setColumnCount(COLUMN_SIZE);
  return ui->treeWidget;
}
QStackedWidget* CfgPositionLoop::getUiStackedWidget()
{
  return ui->stackedWidget;
}

void CfgPositionLoop::setActionReadFuncValueFromFlashEnable()
{
  m_actReadFuncValueFromFlash->setEnabled(true);
}
void CfgPositionLoop::createUiByQml()
{
  QString version=mp_mainWindow->getUserConfig()->model.version.at(0);
  QString modelName=mp_mainWindow->getUserConfig()->model.modelName;
  QString typeName=mp_mainWindow->getUserConfig()->typeName;
  m_qmlFilePath=SYSCONFIG_FILE_PATH+typeName+"/"+modelName+"/"+version+"/ui/"+objectName()+".qml";
//  qDebug()<<tr("******************qml path:%1****************\n*****icon path %2********").arg(m_qmlFilePath).arg(m_qmlIconFilePath);

  QQuickWidget *qwidget=new QQuickWidget(this);
  qwidget->setMinimumSize(1000,560);
//  qwidget->rootContext()->setContextProperty("iconPath",m_qmlIconFilePath);
  qwidget->rootContext()->setContextProperty("drivePositionLoop",this);
  qwidget->rootContext()->setContextProperty("axisIndex",m_axisNumber);
  qwidget->rootContext()->setContextProperty("treeSource",ui->treeWidget);

  qwidget->resize(1000,560);
  qwidget->setResizeMode(QQuickWidget::SizeRootObjectToView );
  qwidget->setSource(QUrl::fromLocalFile(m_qmlFilePath));
//  qwidget->show();
  ui->qmlHboxLayout->addWidget(qwidget);
}
void CfgPositionLoop::setActiveNow(bool actived)
{
  if(actived)
    emit activeNow();
}
