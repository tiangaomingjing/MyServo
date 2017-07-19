#include "cfgcurrentloop.h"
#include "ui_CfgCurrentLoop.h"
#include "globaldef.h"
#include "MainGTSD/mainwindow.h"

#include <QQuickWidget>
#include <QQmlContext>

CfgCurrentLoop::CfgCurrentLoop(QWidget *parent):
  AbstractFuncWidget(parent),
  ui(new Ui::CfgCurrentLoop())
{
  ui->setupUi(this);
  ui->stackedWidget->setCurrentIndex(0);
}
CfgCurrentLoop::~CfgCurrentLoop()
{
  qDebug()<<"CfgCurrentLoop release ->";
}

//!------------------private function-----------------------------

QTreeWidget* CfgCurrentLoop::getUiTree()
{
  return ui->treeWidget;
}
QStackedWidget* CfgCurrentLoop::getUiStackedWidget()
{
  return ui->stackedWidget;
}
void CfgCurrentLoop::createUiByQml()
{
  QString version=mp_mainWindow->getUserConfig()->model.version.at(0);
  QString modelName=mp_mainWindow->getUserConfig()->model.modelName;
  QString typeName=mp_mainWindow->getUserConfig()->typeName;
  m_qmlFilePath=SYSCONFIG_FILE_PATH+typeName+"/"+modelName+"/"+version+"/ui/"+objectName()+".qml";
//  qDebug()<<tr("******************qml path:%1****************\n*****icon path %2********").arg(m_qmlFilePath).arg(m_qmlIconFilePath);

  QQuickWidget *qwidget=new QQuickWidget(this);
  qwidget->setMinimumSize(600,560);
  qwidget->rootContext()->setContextProperty("iconPath",m_qmlIconFilePath);
  qwidget->rootContext()->setContextProperty("driveCurrentLoop",this);
  qwidget->rootContext()->setContextProperty("axisIndex",m_axisNumber);
  qwidget->rootContext()->setContextProperty("treeSource",ui->treeWidget);

  qwidget->resize(600,560);
  qwidget->setResizeMode(QQuickWidget::SizeRootObjectToView );
  qwidget->setSource(QUrl::fromLocalFile(m_qmlFilePath));
//  qwidget->show();
  ui->qmlHboxLayout->addWidget(qwidget);
}
void CfgCurrentLoop::setActiveNow(bool actived)
{
  emit activeNow(actived);
}
