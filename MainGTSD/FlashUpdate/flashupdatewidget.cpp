#include "flashupdatewidget.h"
#include "ui_flashupdatewidget.h"
#include "xmlbuilder.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QXmlStreamReader>
#include <QDebug>
#include <QTime>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include<QAbstractScrollArea>
#include <QScrollBar>
#include <QAbstractSlider>


FlashUpdateWidget::FlashUpdateWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::FlashUpdateWidget),
  m_lastFilePathName("../")
{
  ui->setupUi(this);
  m_flagNewOld=0x00;
  m_totalItemNumber=0;

  ui->progressBar->hide();
  ui->treeNew->setAcceptDrops(true);
  ui->treeOld->setAcceptDrops(true);
  connect(ui->btnExit,SIGNAL(clicked(bool)),this,SLOT(onBtnExitClicked()));
  connect(ui->btnOld,SIGNAL(clicked(bool)),this,SLOT(onBtnOldClicked()));
  connect(ui->btnNew,SIGNAL(clicked(bool)),this,SLOT(onBtnNewClicked()));
  connect(ui->btnUpdate,SIGNAL(clicked(bool)),this,SLOT(onBtnUpdateXML()));
  connect(ui->btnCompare,SIGNAL(clicked(bool)),this,SLOT(onBtnCompareClicked()));
  connect(ui->treeNew,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(onTreeNewDoubleClicked(QTreeWidgetItem*,int)));
  connect(ui->checkBox_sync,SIGNAL(clicked(bool)),this,SLOT(onCheckedBoxSyncClicked(bool)));

  ui->treeNew->installEventFilter(this);
  ui->treeOld->installEventFilter(this);

  setTreeViewMoveSync(true);
}

FlashUpdateWidget::~FlashUpdateWidget()
{
  delete ui;
}

//!------------------------------------private slots----------------------------------------
void FlashUpdateWidget::onBtnExitClicked()
{
  ui->progressBar->setValue(0);
  ui->progressBar->show();
  if(m_flagNewOld&FLAG_NEW)
    saveXMLFile(ui->treeNew);
  ui->progressBar->hide();
  this->hide();
  //this->close();
}
void FlashUpdateWidget::onBtnOldClicked()
{
  QString filename;
  filename = QFileDialog::getOpenFileName(this, tr("Open XML File"), m_lastFilePathName, tr("XML Files(*.xml)"));
  if (filename.isEmpty())
  {
    return ;
  }
  QFileInfo fileInfo;
  fileInfo.setFile(filename);
  m_lastFilePathName=fileInfo.filePath()+"/";
  ui->label_oldVersion->setText(fileInfo.baseName());

  ui->treeOld->clear();
  m_totalItemNumber=0;
  if(loadTree(filename,ui->treeOld))
  {
    m_flagNewOld|=FLAG_OLD;
    ui->treeOld->expandAll();
    ui->treeOld->resizeColumnToContents(COL_NAME_INDEX);
    ui->treeOld->resizeColumnToContents(COL_VALUE_INDEX);
    QTreeWidgetItemIterator it(ui->treeOld);
    while(*it)
    {
      m_totalItemNumber++;//统计有多少行
      it++;
    }
  }
}
void FlashUpdateWidget::onBtnNewClicked()
{
  QString filename;
  filename = QFileDialog::getOpenFileName(this, tr("Open XML File"), m_lastFilePathName, tr("XML Files(*.xml)"));
  if (filename.length() == 0)
  {
//    QMessageBox::information(NULL, tr("Path"), tr("You didn't select any files."));
    return ;
  }
  QFileInfo fileInfo;
  fileInfo.setFile(filename);
  m_lastFilePathName=fileInfo.filePath()+"/";
  ui->label_newVersion->setText(fileInfo.baseName());

  ui->treeNew->clear();
  if(loadTree(filename,ui->treeNew))
  {
    m_flagNewOld|=FLAG_NEW;
    ui->treeNew->expandAll();
    ui->treeNew->resizeColumnToContents(COL_NAME_INDEX);
    ui->treeNew->resizeColumnToContents(COL_VALUE_INDEX);
  }
}
void FlashUpdateWidget::onBtnUpdateXML()
{
  switch(m_flagNewOld)
  {
  case FLAG_OLD:QMessageBox::information(NULL, tr("Select Error"), tr("select your new xml file."));
    break;
  case FLAG_NEW:QMessageBox::information(NULL, tr("Select Error"), tr("select your old xml file."));
    break;
  case FLAG_NEW|FLAG_OLD:
  {
    ui->progressBar->show();
    this->update();
    //将每一个轴的树分配到一个list容器中
    QList<QTreeWidget*> old_treeWidgetList;
    QList<QTreeWidget*> new_treeWidgetList;
    for(int i=0;i<ui->treeOld->topLevelItemCount();i++)
    {
      QTreeWidget *temp=new QTreeWidget();
      temp->setColumnCount(COL_TOTAL);
      QTreeWidgetItem *item;
      item=ui->treeOld->topLevelItem(i)->clone();
      temp->addTopLevelItem(item);
      old_treeWidgetList.append(temp);
    }
    ui->progressBar->setValue(5);
    for(int i=0;i<ui->treeNew->topLevelItemCount();i++)
    {
      QTreeWidget *temp=new QTreeWidget();
      temp->setColumnCount(COL_TOTAL);
      QTreeWidgetItem *item;
      item=ui->treeNew->topLevelItem(i)->clone();
      temp->addTopLevelItem(item);
      new_treeWidgetList.append(temp);
    }
    ui->progressBar->setValue(10);
    int axisCount=ui->treeOld->topLevelItemCount();
    double incValue=(80-10)/(axisCount);
    double sumPrgValue=10;
    long count=0;
    QList<QTreeWidget*>::iterator old_treeIt=old_treeWidgetList.begin();
    QList<QTreeWidget*>::iterator new_treeIt=new_treeWidgetList.begin();
    while(old_treeIt!=old_treeWidgetList.end())
    {
      QTreeWidgetItemIterator itOld(*old_treeIt);
      QString oldName;
      QString newName;
      QString oldValue;
      QString newValue;

      sumPrgValue+=incValue;
      qDebug()<<"progress value:"<<sumPrgValue<<" incValue"<<incValue;
      ui->progressBar->setValue(sumPrgValue);
      QCoreApplication::processEvents();

      while (*itOld)
      {
        oldName=(*itOld)->text(COL_NAME_INDEX);
        oldName=oldName.trimmed();
        oldValue=(*itOld)->text(COL_VALUE_INDEX);
        oldValue=oldValue.trimmed();

        QTreeWidgetItemIterator itNew(*new_treeIt);
        while(*itNew)
        {
          newName=(*itNew)->text(COL_NAME_INDEX);
          newName=newName.trimmed();
          newValue=(*itNew)->text(COL_VALUE_INDEX);
          newValue=newValue.trimmed();

          if(0==oldName.compare(newName))
          {
            if(0!=oldValue.compare(newValue))
            {
              (*itNew)->setText(COL_VALUE_INDEX,oldValue);
              (*itNew)->setTextColor(COL_VALUE_INDEX,Qt::red);
//              qDebug()<<oldName<<oldName<<"             :"<<count;
            }
            break;
          }
          ++itNew;
        }
        ++itOld;
        count++;
//        qDebug()<<count;
      }
      ++old_treeIt;
      ++new_treeIt;
    }
    ui->treeNew->clear();
    ui->progressBar->setValue(80);

    for(int i=0;i<new_treeWidgetList.size();i++)
    {
      QTreeWidgetItem *top;
      top=new_treeWidgetList.at(i)->topLevelItem(0)->clone();
      ui->treeNew->addTopLevelItem(top);
//      qDebug()<<new_treeWidgetList.at(i)->topLevelItem(0)->text(0);
    }
    ui->progressBar->setValue(100);
    ui->treeNew->expandAll();
    ui->treeNew->resizeColumnToContents(COL_VALUE_INDEX);
    for(int i=0;i<old_treeWidgetList.size();i++)
    {
      old_treeWidgetList[i]->clear();
      new_treeWidgetList[i]->clear();
    }
    old_treeWidgetList.clear();
    new_treeWidgetList.clear();
    ui->progressBar->hide();
  }
    break;
  case 0x00:QMessageBox::information(NULL, tr("Select Error"), tr("select your old and new xml file."));
    break;
  }
}

void FlashUpdateWidget::onBtnCompareClicked()
{
  bool hasOld=(ui->treeOld->topLevelItemCount()>0)?true:false;
  bool hasNew=(ui->treeNew->topLevelItemCount()>0)?true:false;
  if((hasOld&&hasNew)==false)
    return;
  ui->progressBar->show();
  this->update();
  //将每一个轴的树分配到一个list容器中
  QList<QTreeWidget*> old_treeWidgetList;
  QList<QTreeWidget*> new_treeWidgetList;
  for(int i=0;i<ui->treeOld->topLevelItemCount();i++)
  {
    QTreeWidget *temp=new QTreeWidget();
    temp->setColumnCount(COL_TOTAL);
    QTreeWidgetItem *item;
    item=ui->treeOld->topLevelItem(i)->clone();
    temp->addTopLevelItem(item);
    old_treeWidgetList.append(temp);
  }
  ui->progressBar->setValue(5);
  for(int i=0;i<ui->treeNew->topLevelItemCount();i++)
  {
    QTreeWidget *temp=new QTreeWidget();
    temp->setColumnCount(COL_TOTAL);
    QTreeWidgetItem *item;
    item=ui->treeNew->topLevelItem(i)->clone();
    temp->addTopLevelItem(item);
    new_treeWidgetList.append(temp);
  }

  //新旧的名字相同时，对比它们的值，如果两个体不一样，则高亮两边
  ui->progressBar->setValue(10);
  int axisCount=ui->treeOld->topLevelItemCount();
  double incValue=(80-10)/(axisCount);
  double sumPrgValue=10;
  long count=0;
  QList<QTreeWidget*>::iterator old_treeIt=old_treeWidgetList.begin();
  QList<QTreeWidget*>::iterator new_treeIt=new_treeWidgetList.begin();
  while(old_treeIt!=old_treeWidgetList.end())
  {
    QTreeWidgetItemIterator itOld(*old_treeIt);
    QString oldName;
    QString newName;
    QString oldValue;
    QString newValue;

    sumPrgValue+=incValue;
    qDebug()<<"progress value:"<<sumPrgValue<<" incValue"<<incValue;
    ui->progressBar->setValue(sumPrgValue);
    QCoreApplication::processEvents();

    while (*itOld)
    {
      oldName=(*itOld)->text(COL_NAME_INDEX);
      oldName=oldName.trimmed();
      oldValue=(*itOld)->text(COL_VALUE_INDEX);
      oldValue=oldValue.trimmed();

      QTreeWidgetItemIterator itNew(*new_treeIt);
      while(*itNew)
      {
        newName=(*itNew)->text(COL_NAME_INDEX);
        newName=newName.trimmed();
        newValue=(*itNew)->text(COL_VALUE_INDEX);
        newValue=newValue.trimmed();

        if(0==oldName.compare(newName))
        {
          if(0!=oldValue.compare(newValue))
          {
            (*itNew)->setTextColor(COL_VALUE_INDEX,Qt::red);
            (*itOld)->setTextColor(COL_VALUE_INDEX,Qt::red);
            if(!((*itOld)->childCount()>0))//叶子节点
            {
              for(int i=0;i<COL_TOTAL;i++)
              {
                (*itOld)->setBackgroundColor(i,QColor(Qt::yellow));
                (*itNew)->setBackgroundColor(i,QColor(Qt::yellow));
              }
            }

          }
          break;
        }
        ++itNew;
      }
      ++itOld;
      count++;
//        qDebug()<<count;
    }
    ++old_treeIt;
    ++new_treeIt;
  }
  //重新更新两边的树
  ui->treeOld->clear();
  ui->progressBar->setValue(80);

  for(int i=0;i<old_treeWidgetList.size();i++)
  {
    QTreeWidgetItem *top;
    top=old_treeWidgetList.at(i)->topLevelItem(0)->clone();
    ui->treeOld->addTopLevelItem(top);
//      qDebug()<<new_treeWidgetList.at(i)->topLevelItem(0)->text(0);
  }

  ui->treeNew->clear();
  ui->progressBar->setValue(90);

  for(int i=0;i<new_treeWidgetList.size();i++)
  {
    QTreeWidgetItem *top;
    top=new_treeWidgetList.at(i)->topLevelItem(0)->clone();
    ui->treeNew->addTopLevelItem(top);
//      qDebug()<<new_treeWidgetList.at(i)->topLevelItem(0)->text(0);
  }
  ui->progressBar->setValue(100);
  ui->treeOld->expandAll();
  ui->treeNew->expandAll();


  for(int i=0;i<old_treeWidgetList.size();i++)
  {
    old_treeWidgetList[i]->clear();
    new_treeWidgetList[i]->clear();
  }
  old_treeWidgetList.clear();
  new_treeWidgetList.clear();
  ui->progressBar->hide();
}

void FlashUpdateWidget::onTreeNewDoubleClicked(QTreeWidgetItem *item, int column)
{
  if(column==COL_VALUE_INDEX)
  {
    if(ui->checkBox->isChecked())
    {
      if(!(item->childCount()>0))
      {
        item->setFlags(item->flags()|Qt::ItemIsEditable);
      }
      else item->setFlags(item->flags()&(~Qt::ItemIsEditable));
    }
    else
      item->setFlags(item->flags()&(~Qt::ItemIsEditable));
  }

}
void FlashUpdateWidget::onCheckedBoxSyncClicked(bool checked)
{
  setTreeViewMoveSync(checked);
}

//!-----------------------------------private functions--------------------------------------
bool FlashUpdateWidget::loadTree(QString &filename,QTreeWidget *treeWidget)
{
  QTreeWidget *tree=NULL;
  tree=XmlBuilder::readTreeWidgetFromFile(filename);
  if(treeWidget==NULL)
    return false;
  else
  {
    QTreeWidgetItem *item;
    for(int i=0;i<tree->topLevelItemCount();i++)
    {
      item=tree->topLevelItem(i)->clone();
      treeWidget->addTopLevelItem(item);
    }
    item=tree->headerItem()->clone();
    treeWidget->setHeaderItem(item);
    tree->clear();
    delete tree;
    return true;
  }
}

bool FlashUpdateWidget::saveXMLFile(QTreeWidget *tree)
{
  //------------------------------write xml file-----------------------------------------------------
  QString fileNameXml;
  QString fileNameDefaultQString =m_lastFilePathName+"Prm_Vxxx_"+ QDateTime::currentDateTime().toString("yyyyMMdd");//默认文件名
  fileNameXml = QFileDialog::getSaveFileName(this, tr("Save XML"), fileNameDefaultQString, tr("xml file (*.xml)"));
  if (fileNameXml.isNull()) return false;
  ui->progressBar->show();
  ui->progressBar->setValue(50);
  XmlBuilder::saveFlashAllAxisTreeFile(fileNameXml,tree);
  ui->progressBar->setValue(100);
  ui->progressBar->hide();
  return true;
}

bool FlashUpdateWidget::eventFilter(QObject *watchObj, QEvent *event)
{
  if(watchObj==ui->treeNew||watchObj==ui->treeOld)
  {
    if(event->type()==QEvent::DragEnter)
    {
      QDragEnterEvent *eve=static_cast<QDragEnterEvent *>(event);
      if(eve->mimeData()->hasFormat("text/uri-list")) eve->acceptProposedAction();
      return true;
    }
    if(event->type()==QEvent::Drop)
    {
      if(watchObj==ui->treeNew)
        m_flagNewOld|=FLAG_NEW;
      else
        m_flagNewOld|=FLAG_OLD;
      QDropEvent *eve=static_cast<QDropEvent *>(event);
      QList<QUrl> urls=eve->mimeData()->urls();
      if(urls.isEmpty()) return true;
      QString fileName=urls.first().toLocalFile();
      if(!loadTree(fileName,static_cast<QTreeWidget*>(watchObj)))
        QMessageBox::information(0,"urls",fileName);
      return true;
    }

  }
  return QWidget::eventFilter(watchObj,event);
}

void FlashUpdateWidget::delayms(quint16 ms)
{
  QTime dieTime = QTime::currentTime().addMSecs(ms);
  while( QTime::currentTime() < dieTime )
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}
void FlashUpdateWidget::setTreeViewMoveSync(bool sync)
{
  QAbstractScrollArea *area1=ui->treeNew;
  QAbstractScrollArea *area2=ui->treeOld;
  QAbstractSlider *sa1=area1->verticalScrollBar();
  QAbstractSlider *sa2=area2->verticalScrollBar();
  if(sync)
  {
    connect(sa1,SIGNAL(valueChanged(int)),sa2,SLOT(setValue(int)));
    connect(sa2,SIGNAL(valueChanged(int)),sa1,SLOT(setValue(int)));
  }
  else
  {
    disconnect(sa1,SIGNAL(valueChanged(int)),sa2,SLOT(setValue(int)));
    disconnect(sa2,SIGNAL(valueChanged(int)),sa1,SLOT(setValue(int)));
  }
}
