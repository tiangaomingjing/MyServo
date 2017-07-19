#include "axisclone.h"
#include "ui_axisclone.h"
#include "mainwindow.h"
#include "servocontrol.h"
#include "xmlbuilder.h"

AxisClone* AxisClone::instance=NULL;
AxisClone::AxisClone(MainWindow *window, QWidget *parent) :
  QWidget(parent),
  mp_mainWindow(window),
  ui(new Ui::AxisClone)
{
  ui->setupUi(this);
  ui->toolButton->setText(tr("clone"));
  ui->label->setText(tr("source"));
  ui->label_2->setText(tr("destination"));
  ui->progressBar->hide();
  QString str;
  str="QListView {\
      show-decoration-selected: 1;\
  } \
  QListView::item:alternate {\
      background: #EEEEEE;\
  }\
  QListView::item:selected {\
      border: 1px solid #6a6ea9;\
  }\
  QListView::item:selected:!active {\
      background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\
                                  stop: 0 #ABAFE5, stop: 1 #8588B2);\
  }\
  QListView::item:selected:active {\
      background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\
                                  stop: 0 #6a6ea9, stop: 1 #888dd9);\
  }\
  QListView::item:hover {\
      background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\
                                  stop: 0 #FAFBFE, stop: 1 #DCDEF1);\
  }";
  ui->listWidget_src->setStyleSheet(str);
  ui->listWidget_dst->setStyleSheet(str);

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

  QStringList stringList;
  for(int i=0;i<mp_mainWindow->getUserConfig()->model.axisCount;i++)
  {
    stringList.append(tr("Axis_%1").arg(i));
  }
  ui->listWidget_src->addItems(stringList);
  ui->listWidget_dst->addItems(stringList);
  connect(ui->toolButton,SIGNAL(clicked(bool)),this,SLOT(onBtnSubmitClicked()));
  ui->listWidget_src->setCurrentRow(0);
  ui->listWidget_dst->setCurrentRow(1);
  ui->listWidget_dst->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

AxisClone::~AxisClone()
{
  delete ui;
  qDebug()<<"axisclone release ->";
}
AxisClone* AxisClone::getAxisCloneInstance(MainWindow *window, QWidget *parent)
{
  if(instance==NULL)
  {
    instance=new AxisClone(window,parent);
  }
  return instance;
}

void AxisClone::onBtnSubmitClicked()
{
  if(!mp_mainWindow->getComOpenState())
  {
    QMessageBox::information(0,tr("Warring"),tr("please open the com first !"));
    return ;
  }
  ui->progressBar->show();
  QTreeWidget *srcTree;
  QTreeWidgetItem *item;
  QTreeWidget *dstTree;
  srcTree=new QTreeWidget();
  int srcRow;
  srcRow=ui->listWidget_src->currentRow();
  item=mp_mainWindow->getFlashAllAxisTree()->topLevelItem(srcRow)->clone();
  srcTree->addTopLevelItem(item);

//  srcTree->setColumnCount(COL_PARENT);
//  srcTree->show();
  ServoControl sControl;
  sControl.updateFlashTreeWidget(srcTree,srcRow,(COM_TYPE)mp_mainWindow->getUserConfig()->com.id,mp_mainWindow->getUserConfig()->com.rnStation);

//  ServoControl::updateFlashTreeWidget(srcTree,srcRow,(COM_TYPE)mp_mainWindow->getUserConfig()->com.id,mp_mainWindow->getUserConfig()->com.rnStation);
  ui->progressBar->setValue(10);
  int value=10;
  for(int i=0;i<ui->listWidget_dst->count();i++)
  {
    value+=90/ui->listWidget_dst->count();
    if(ui->listWidget_dst->item(i)->isSelected())
    {
      dstTree=new QTreeWidget();

      item=mp_mainWindow->getFlashAllAxisTree()->topLevelItem(i)->clone();
      dstTree->addTopLevelItem(item);
      XmlBuilder::setTreeValueFromTree(dstTree,srcTree);
//      dstTree->setColumnCount(COL_PARENT);
//      dstTree->show();
      sControl.writeServoFlashByAxisTree(dstTree,i,(COM_TYPE)mp_mainWindow->getUserConfig()->com.id,mp_mainWindow->getUserConfig()->com.rnStation);
//      ServoControl::writeServoFlashByAxisTree(dstTree,i,(COM_TYPE)mp_mainWindow->getUserConfig()->com.id,mp_mainWindow->getUserConfig()->com.rnStation);
      dstTree->clear();
      delete dstTree;
      dstTree=NULL;
    }
    ui->progressBar->setValue(value);
  }
  srcTree->clear();
  delete srcTree;
  ui->progressBar->hide();
}
void AxisClone::closeEvent(QCloseEvent *event)
{
  Q_UNUSED(event);
  ui->progressBar->hide();
  delete instance;
  instance=NULL;
}
