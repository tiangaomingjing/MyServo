#include "cfgdriverpower.h"
#include "ui_cfgdriverpower.h"
#include "MainGTSD/mainwindow.h"

#include <QAction>
#include <QMessageBox>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQuickWidget>
#include <QQmlContext>
#include <QQuickItem>

CfgDriverPower::CfgDriverPower(QWidget *parent)
  :AbstractFuncWidget(parent),
    ui(new Ui::CfgDriverPower())
{
  ui->setupUi(this);
  ui->stackedWidget->setCurrentIndex(0);
//  connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(onPushButtonClicked()));
}
CfgDriverPower::~CfgDriverPower()
{
  qDebug()<<"CfgDriverPower release ->";
}
//!-------------------slots-------------------------
void CfgDriverPower::onPushButtonClicked()
{
  QMessageBox::information(0,"information","test information");
}
void CfgDriverPower::onQmlValueChanged()
{
  qDebug()<<"qt value change ";
}

QTreeWidget *CfgDriverPower::getUiTree()
{
  QStringList headerlist;
  headerlist<<tr("name")<<tr("ID")<<tr("value")<<tr("type")\
              <<tr("axisnum")<<tr("parent")<<tr("unit")<<tr("introduction")\
              <<tr("deaultvalue")<<tr("uplimit")<<tr("downlimit")<<tr("funcnum");
  ui->treeWidget->setHeaderLabels(headerlist);
  ui->treeWidget->setColumnCount(COLUMN_SIZE);

  return ui->treeWidget;
}

QStackedWidget *CfgDriverPower::getUiStackedWidget()
{
  return ui->stackedWidget;
}
void CfgDriverPower::createUiByQml()
{
  QString version=mp_mainWindow->getUserConfig()->model.version.at(0);
  QString modelName=mp_mainWindow->getUserConfig()->model.modelName;
  QString typeName=mp_mainWindow->getUserConfig()->typeName;
  m_qmlFilePath=SYSCONFIG_FILE_PATH+typeName+"/"+modelName+"/"+version+"/ui/"+objectName()+".qml";
//  qDebug()<<tr("******************qml path:%1****************\n*****icon path %2********").arg(m_qmlFilePath).arg(m_qmlIconFilePath);

  QQuickWidget *qwidget=new QQuickWidget(this);
  qwidget->setMinimumSize(600,560);
  qwidget->rootContext()->setContextProperty("iconPath",m_qmlIconFilePath);
  qwidget->rootContext()->setContextProperty("driverPower",this);
  qwidget->rootContext()->setContextProperty("axisIndex",m_axisNumber);
  qwidget->rootContext()->setContextProperty("treeSource",ui->treeWidget);


  qwidget->resize(600,560);
  qwidget->setResizeMode(QQuickWidget::SizeRootObjectToView );
  qwidget->setSource(QUrl::fromLocalFile(m_qmlFilePath));
//  qwidget->show();
  ui->qmlHboxLayout->addWidget(qwidget);
  qDebug()<<"QML Errors:"<<qwidget->errors();
//  QObject *root=qobject_cast<QObject*> (qwidget->rootObject());
//  if(root)
//  {
//    connect(root,SIGNAL(valueChanged()),this,SLOT(onQmlValueChanged()));
//  }
}

