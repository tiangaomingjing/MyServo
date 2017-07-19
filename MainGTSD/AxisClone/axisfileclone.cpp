#include "axisfileclone.h"
#include "ui_axisfileclone.h"
#include "axisitem.h"
#include "arrow.h"
#include "axisbackground.h"
#include "globaldef.h"
#include "mainwindow.h"
#include "xmlbuilder.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsWidget>
#include <QGraphicsProxyWidget>
#include <QGraphicsWidget>
#include <QDebug>
#include <QtMath>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QFileDialog>
#define BACKGROUND_WIDTH 200
#define BACKGROUND_PERHEIGHT 75

AxisFileClone::AxisFileClone(MainWindow *mainwindow, QWidget *parent) : QWidget(parent),
  ui(new Ui::AxisFileClone),
  mp_mainWindow(mainwindow),
  m_srcTreeWidget(NULL),
  m_dstTreeWidget(NULL)
{
  ui->setupUi(this);
  setMyStyle();
  setAttribute(Qt::WA_DeleteOnClose,true);//关闭时销毁窗口实例
  //没有最大化，最小化
  setWindowFlags(windowFlags()& ~Qt::WindowMaximizeButtonHint& ~Qt::WindowMinimizeButtonHint);
  ui->btn_apply->setText(tr("apply"));
  ui->btn_apply->setEnabled(false);
  ui->btn_exit->setText(tr("exit"));
  ui->btn_open->setText(tr("open"));
  ui->btn_open->setToolTip(tr("open source file..."));
  ui->progressBar->hide();
  m_scene=new QGraphicsScene();
//  m_scene->setSceneRect(-40,-30,600,600);

  m_groupSrc=new QButtonGroup(this);
  m_groupDst=new QButtonGroup(this);
  int axisnum=mp_mainWindow->getUserConfig()->model.axisCount;
//  int axisnum=4;
  AxisItem *axisitem;
  for(int i=0;i<axisnum;i++)
  {
    axisitem=new AxisItem;
    axisitem->button()->setText(tr("axis_src_%1").arg(i));
    axisitem->button()->setIconSize(QSize(48,48));
    axisitem->button()->setIcon(QIcon(ICON_FILE_PATH+ICON_MOTOR));
    m_axisItemSourceList.append(axisitem);
    m_groupSrc->addButton(axisitem->button(),i);
    m_scene->addItem(axisitem);
    axisitem->setPos(0,i*65);

    axisitem=new AxisItem;
    axisitem->button()->setText(tr("axis_dst_%1").arg(i));
    axisitem->button()->setIconSize(QSize(48,48));
    axisitem->button()->setIcon(QIcon(ICON_FILE_PATH+ICON_MOTOR));
    m_axisItemDestinationList.append(axisitem);
    m_groupDst->addButton(axisitem->button(),i);
    m_scene->addItem(axisitem);
    axisitem->setPos(300,i*65);
  }

  m_groupDst->setExclusive(true);
  m_groupSrc->setExclusive(true);

  QSizeF size;
  size.setWidth(BACKGROUND_WIDTH);
  size.setHeight(axisnum*BACKGROUND_PERHEIGHT);
  m_abackground=new AxisBackground(0);
  m_abackground->setPos(-40,-25);
  m_abackground->setSize(size);
  m_scene->addItem(m_abackground);
  m_abackground2=new AxisBackground(0);
  m_scene->addItem(m_abackground2);
  m_abackground2->setPos(260,-25);
  m_abackground2->setSize(size);

  ui->graphicsView->setScene(m_scene);

  connect(m_groupDst,SIGNAL(buttonClicked(int)),this,SLOT(onGroupDstClicked(int)));
  connect(m_groupSrc,SIGNAL(buttonClicked(int)),this,SLOT(onGroupSrcClicked(int)));
  connect(ui->btn_exit,SIGNAL(clicked(bool)),this,SLOT(onBtnExitClicked()));
  connect(ui->btn_apply,SIGNAL(clicked(bool)),this,SLOT(onBtnApplyClicked()));
  connect(ui->btn_open,SIGNAL(clicked(bool)),this,SLOT(onBtnOpenClicked()));

}
AxisFileClone::~AxisFileClone()
{
  delete m_abackground;
  delete m_abackground2;
  AxisItem *aitem;
  for(int i=0;i<m_axisItemSourceList.count();i++)
  {
    aitem=m_axisItemSourceList[i];
    delete aitem;
  }
  m_axisItemSourceList.clear();
  for(int i=0;i<m_axisItemDestinationList.count();i++)
  {
    aitem=m_axisItemDestinationList[i];
    delete aitem;
  }
  m_axisItemDestinationList.clear();

  Arrow *arrow;
  for(int i=0;i<m_arrowList.count();i++)
  {
    arrow=m_arrowList[i];
    delete arrow;
  }
  m_arrowList.clear();
  if(m_dstTreeWidget!=NULL)
  {
    m_dstTreeWidget->clear();
    delete m_dstTreeWidget;
  }

  if(m_srcTreeWidget!=NULL)
  {
    m_srcTreeWidget->clear();
    delete m_srcTreeWidget;
  }

  qDebug()<<"axisfileclone release ->";
}
void AxisFileClone::onGroupSrcClicked(int id)
{
  qDebug()<<"id"<<id;
  m_srcItem=m_axisItemSourceList.at(id);
}
void AxisFileClone::onGroupDstClicked(int id)
{
  m_dstItem=m_axisItemDestinationList.at(id);
  if(m_dstItem->arrows().count()>0)
    return;
  Arrow *arrow=new Arrow(m_srcItem,m_dstItem);
  connect(arrow,SIGNAL(mouseDoubleClicked()),this,SLOT(onArrowDoubleClicked()));
  m_scene->addItem(arrow);
  m_arrowList.append(arrow);
  qDebug()<<"id"<<id;
}
void AxisFileClone::onArrowDoubleClicked()
{
  QObject *obj=sender();
  Arrow *arrow=static_cast<Arrow *>(obj);
  arrow->startItem()->removeArrow(arrow);
  arrow->endItem()->removeArrow(arrow);
  disconnect(arrow,SIGNAL(mouseDoubleClicked()),this,SLOT(onArrowDoubleClicked()));
  m_scene->removeItem(arrow);
  m_arrowList.removeOne(arrow);
  delete arrow;
}

void AxisFileClone::onBtnExitClicked()
{
  this->close();
}
void AxisFileClone::onBtnOpenClicked()
{
  QString filePath = QFileDialog::getOpenFileName(this, tr("Open xmlfile"), ".", tr("Xml Files( *.xml)"));
  if(!filePath.isNull())
  {
    ui->progressBar->show();
    ui->progressBar->setValue(10);
    if(m_srcTreeWidget!=NULL)
    {
      m_srcTreeWidget->clear();
      delete m_srcTreeWidget;
    }
    ui->progressBar->setValue(50);
    m_srcTreeWidget=XmlBuilder::readTreeWidgetFromFile(filePath);
    ui->progressBar->setValue(100);
    ui->progressBar->hide();
    ui->btn_apply->setEnabled(true);
//    QWidget *mwidget=new QWidget();
//    QHBoxLayout *hLayout=new QHBoxLayout(mwidget);
//    hLayout->addWidget(m_srcTreeWidget);
//    mwidget->show();
  }
}

void AxisFileClone::onBtnApplyClicked()
{
  QString saveFileName;
  QString strDir=tr("./axisData_%1%2").arg(QDateTime::currentDateTime().toString("yyyyMMdd")).arg(".xml");
  saveFileName = QFileDialog::getSaveFileName(this,tr("SaveXML"), strDir,tr("axisData (*.xml)"));
  if (saveFileName.isNull())//空表示点的取消按钮
  {
    return;
  }
  ui->progressBar->show();
  ui->progressBar->setValue(0);

  QTreeWidgetItem *treeItem=NULL;
  QTreeWidget *singleSrcAxisTree=NULL;
  QTreeWidget *singleDstAxisTree=NULL;
  QList<QTreeWidget *>resultTreeList;
  int axisCount=0;
  //先分成四个轴的单个数据
  axisCount=m_srcTreeWidget->topLevelItemCount();
  for(int i=0;i<axisCount;i++)
  {
    singleDstAxisTree=new QTreeWidget;
    treeItem=m_srcTreeWidget->topLevelItem(i)->clone();
    singleDstAxisTree->addTopLevelItem(treeItem);
    resultTreeList.append(singleDstAxisTree);
  }
  ui->progressBar->setValue(5);
  //遍历左边一组轴，再遍历每个轴的arrow,再从右边找到有没有与左边arrow是一样的
  int srcIndex,dstIndex;
  srcIndex=dstIndex=0;
  int value=5;
  bool hasArrow=false;
  foreach (AxisItem *item, m_axisItemSourceList)
  {
    value+=90/m_srcTreeWidget->topLevelItemCount();
    foreach (Arrow *arrow, item->arrows())
    {
      dstIndex=0;
      foreach (AxisItem *itemDst, m_axisItemDestinationList)
      {
        if(itemDst->arrows().count()>0)
        {
          if(arrow==itemDst->arrows().at(0))
          {
            hasArrow=true;
            //获得当前左边轴的树
            singleSrcAxisTree=new QTreeWidget;
            treeItem=m_srcTreeWidget->topLevelItem(srcIndex)->clone();
            singleSrcAxisTree->addTopLevelItem(treeItem);
            //获得当前右边轴的树
            singleDstAxisTree=resultTreeList.at(dstIndex);
            //用左边的树的值改变右边的树
            XmlBuilder::setTreeValueFromTree(singleDstAxisTree,singleSrcAxisTree);
            singleSrcAxisTree->clear();
            delete singleSrcAxisTree;
//            delete treeItem;
            qDebug()<<tr("arrow srcIndex:%1 dstIndex:%2").arg(srcIndex).arg(dstIndex);
            break;
          }
        }
        dstIndex++;
      }
    }
    ui->progressBar->setValue(value);
    srcIndex++;
    qApp->processEvents();
  }
  //逐个树放到目标树中去
  if(hasArrow)
  {
    m_dstTreeWidget=new QTreeWidget;
    for(int i=0;i<axisCount;i++)
    {
      treeItem=resultTreeList[i]->topLevelItem(0)->clone();
      m_dstTreeWidget->addTopLevelItem(treeItem);
    }
//        QWidget *mwidget=new QWidget();
//        QHBoxLayout *hLayout=new QHBoxLayout(mwidget);
//        hLayout->addWidget(m_dstTreeWidget);
//        mwidget->show();

    //清空原来list的树内存
    for(int i=0;i<axisCount;i++)
    {
      singleDstAxisTree=resultTreeList.at(i);
      singleDstAxisTree->clear();
      delete singleDstAxisTree;
    }
    resultTreeList.clear();

    //保存树文件

    XmlBuilder::saveFlashAllAxisTreeFile(saveFileName,m_dstTreeWidget);
    m_dstTreeWidget->clear();
    delete m_dstTreeWidget;
    m_dstTreeWidget=NULL;

  }
  ui->progressBar->hide();
}

void AxisFileClone::setMyStyle()
{
  QString str;
  str="QProgressBar::chunk \
    {\
        background-color: #CD96CD;\
        width: 10px;\
        margin: 0.5px;\
    }\
    QProgressBar {\
    border: 2px solid grey;\
    border-radius: 5px;\
    }";
  ui->progressBar->setStyleSheet(str);
}
