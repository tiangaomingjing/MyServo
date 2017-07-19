#include "downloaddialog.h"
#include "ui_downloaddialog.h"
#include "mainwindow.h"
#include "ServoDriverComDll.h"
#include "globaldef.h"
#include "xmlbuilder.h"
#include "servocontrol.h"

#include <QFileDialog>
#include <QTimer>


int DownloadDialog::m_dspProgressValue=0;
int DownloadDialog::m_fpgaProgressValue=0;
int DownloadDialog::m_dspNum=0;
int DownloadDialog::m_fpgaNum=0;
int DownloadDialog::m_currentStep=0;

DownloadDialog::DownloadDialog(MainWindow *mainwindow, QWidget *parent) :
  QDialog(parent),
  ui(new Ui::DownloadDialog),
  mp_mainwindow(mainwindow)
{
  ui->setupUi(this);
  ui->progressBar->setValue(0);
  ui->progressBar->hide();
  ui->btn_reset->hide();
  ui->btn_exit->setText(tr("exit"));
  ui->btn_reset->setText(tr("DSP Reset"));
  setWindowTitle(tr("DownLoad Dialog"));
  ui->label_info->setText(tr(" "));

  connect(ui->btn_dspOpen,SIGNAL(clicked(bool)),this,SLOT(onBtnDspFileOpenClicked()));
  connect(ui->btn_fpgaOpen,SIGNAL(clicked(bool)),this,SLOT(onBtnFpgaFileOpenClicked()));
  connect(ui->btn_dspload,SIGNAL(clicked(bool)),this,SLOT(onBtnDspDownloadClicked()));
  connect(ui->btn_fpgaload,SIGNAL(clicked(bool)),this,SLOT(onBtnFpgaDownloadClicked()));
  connect(ui->btn_exit,SIGNAL(clicked(bool)),this,SLOT(onBtnExitClicked()));
  connect(ui->btn_reset,SIGNAL(clicked(bool)),this,SLOT(onBtnResetDSPClicked()));
}

DownloadDialog::~DownloadDialog()
{
  delete ui;
}

void DownloadDialog::onBtnDspFileOpenClicked()
{
  QString path = QFileDialog::getOpenFileName(this, tr("Open File"), MainWindow::g_lastFilePath, tr("Hex Files( *.hex);;All Files( *.*)"));
  if(path.isNull())
    return;
  QFileInfo fileInfo;
  fileInfo.setFile(path);
  MainWindow::g_lastFilePath=fileInfo.filePath()+"/";
  ui->lineEdit_dsp->setText(path);
}

void DownloadDialog::onBtnFpgaFileOpenClicked()
{
  QString path = QFileDialog::getOpenFileName(this, tr("Open File"), MainWindow::g_lastFilePath, tr("Rpd Files( *.rpd);;All Files( *.*)"));
  if(path.isNull())
    return;
  QFileInfo fileInfo;
  fileInfo.setFile(path);
  MainWindow::g_lastFilePath=fileInfo.filePath()+"/";
  ui->lineEdit_fpga->setText(path);
}

void DownloadDialog::onBtnDspDownloadClicked()
{
  if(!mp_mainwindow->getComOpenState())
  {
    QMessageBox::information(0,tr("Warring"),tr("please open the com first !"));
    return ;
  }
  QString fileName=ui->lineEdit_dsp->text();
  if(fileName.isEmpty())
    return;

  mp_mainwindow->stopTimer();
  enableUi(false);
  ui->label_info->setText(tr("DSP loading......"));
  ui->progressBar->show();
  ui->progressBar->setValue(0);
  m_dspNum=0;
  m_dspProgressValue=0;
  m_currentStep=0;
  com_type comtype=(com_type)mp_mainwindow->getUserConfig()->com.id;
  short comStation=mp_mainwindow->getUserConfig()->com.rnStation;

  QTreeWidget *tree=mp_mainwindow->getFunctionExtensionTree();
  QTreeWidgetItem *item;
  QTreeWidgetItem*itemChild;
  int dspNum;
  int dspAxis;
  int result;
  for(int i=0;i<tree->topLevelItemCount();i++)//在PrmFuncExtension.xml中找到fpga结点
  {
    item=tree->topLevelItem(i);
    if(DEVICE_DESCRIBE==item->text(COL_FUNC_EXTENSION_NAME))
    {
      for(int j=0;j<item->childCount();j++)
      {
        itemChild=item->child(j);
        if(DSP_NAME==itemChild->text(COL_FUNC_EXTENSION_NAME))
        {
          dspNum=itemChild->text(COL_FUNC_EXTENSION_PARAMETER).toInt();
          m_dspNum=dspNum;
          dspAxis=itemChild->child(0)->text(COL_FUNC_EXTENSION_PARAMETER).toInt();
          break;
        }
      }
      break;
    }
  }

  bool dwnOK=true;
  QString msg=tr("file download success!");
  for(int i=0;i<dspNum;i++)
  {
    m_currentStep=i;
    result=GTSD_CMD_ProcessorFlashHandler(i*dspAxis,fileName.toStdWString(),updateProgressValueDSP,(void*)ui->progressBar,(int16)comtype,comStation);
    if(result!=0)
    {
      dwnOK=false;
      break;
    }
//    delayms(1500);
  }
  if(dwnOK)
  {
    ui->progressBar->setValue(100);
    ui->progressBar->hide();
    ui->label_info->setText(tr("DSP Download successfully"));
  }
  else
  {
    msg=tr("ERROR\nfile download unsuccessful !");
    QMessageBox::information(0,tr("complete"),msg);
  }

  enableUi(true);
  mp_mainwindow->startTimer();
}

void DownloadDialog::onBtnFpgaDownloadClicked()
{
  if(!mp_mainwindow->getComOpenState())
  {
    QMessageBox::information(0,tr("Warring"),tr("please open the com first !"));
    return ;
  }
  QString fileName=ui->lineEdit_fpga->text();
  qDebug()<<fileName;
  if(fileName.isEmpty())
    return;

  mp_mainwindow->stopTimer();
  enableUi(false);

  ui->progressBar->show();
  ui->progressBar->setValue(0);
  m_fpgaNum=0;
  m_fpgaProgressValue=0;
  m_currentStep=0;
  com_type comtype=(com_type)mp_mainwindow->getUserConfig()->com.id;

  QTreeWidget *tree=mp_mainwindow->getFunctionExtensionTree();
  QTreeWidgetItem *item;
  QTreeWidgetItem*itemChild;
  QString msg=tr("file download success!");
  int result;
  int fpgaNum=1;
  int fpgaAxis;
  for(int i=0;i<tree->topLevelItemCount();i++)//在PrmFuncExtension.xml中找到fpga结点
  {
    item=tree->topLevelItem(i);
    if(DEVICE_DESCRIBE==item->text(COL_FUNC_EXTENSION_NAME))
    {
      for(int j=0;j<item->childCount();j++)
      {
        itemChild=item->child(j);
        if(FPGA_NAME==itemChild->text(COL_FUNC_EXTENSION_NAME))
        {
          fpgaNum=itemChild->text(COL_FUNC_EXTENSION_PARAMETER).toInt();
          m_fpgaNum=fpgaNum;
          fpgaAxis=itemChild->child(0)->text(COL_FUNC_EXTENSION_PARAMETER).toInt();
          break;
        }
      }
      break;
    }
  }
  qDebug()<<"fpga num="<<m_fpgaNum<<" axis="<<fpgaAxis;

  bool downloadOK=true;
  for(int i=0;i<fpgaNum;i++)
  {
    m_currentStep=i;
    result=GTSD_CMD_FirmwareFlashHandler(i*fpgaAxis,fileName.toStdWString(),updateProgressValueFPGA,(void*)(this),(int16)comtype,mp_mainwindow->getUserConfig()->com.rnStation);
    if(result!=0)
    {
      downloadOK=false;
      break;
    }
//    qDebug()<<"result value = "<<result;
  }
  //add result error handler -----

  if(downloadOK)
  {
    ui->progressBar->setValue(100);
    ui->progressBar->hide();
    ui->label_info->setText(tr("FPGA Download successfully"));
  }
  else
  {
    msg=tr("ERROR\nfile download unsuccessful !");
    QMessageBox::information(0,tr("complete"),msg);
  }
  enableUi(true);
  mp_mainwindow->startTimer();
}

void DownloadDialog::onBtnExitClicked()
{
  close();
}

void DownloadDialog::onBtnResetDSPClicked()
{
  if(!mp_mainwindow->getComOpenState())
  {
    QMessageBox::information(0,tr("Warring"),tr("please open the com first !"));
    return ;
  }
  ui->progressBar->show();
  COM_ERROR result;
  com_type comtype=(com_type)mp_mainwindow->getUserConfig()->com.id;
  QTreeWidget *tree=mp_mainwindow->getFunctionExtensionTree();
  QTreeWidgetItem *item;
  QTreeWidgetItem*itemChild;
  ui->progressBar->setValue(5);
  int dspNum;
  int dspAxis;
  for(int i=0;i<tree->topLevelItemCount();i++)//在PrmFuncExtension.xml中找到fpga结点
  {
    item=tree->topLevelItem(i);
    if(DEVICE_DESCRIBE==item->text(COL_FUNC_EXTENSION_NAME))
    {
      for(int j=0;j<item->childCount();j++)
      {
        itemChild=item->child(j);
        if(DSP_NAME==itemChild->text(COL_FUNC_EXTENSION_NAME))
        {
          dspNum=itemChild->text(COL_FUNC_EXTENSION_PARAMETER).toInt();
          dspAxis=itemChild->child(0)->text(COL_FUNC_EXTENSION_PARAMETER).toInt();
          break;
        }
      }
      break;
    }
  }
  ui->progressBar->setValue(10);
  int value=10;
  int valueInc=90/dspNum/7;
  for(int i=0;i<dspNum;i++)
  {
    bool flag_finish=false;
    value+=valueInc;
    result=(COM_ERROR)GTSD_CMD_ResetSystem(i*dspAxis,comtype,mp_mainwindow->getUserConfig()->com.rnStation);
    ui->progressBar->setValue(value);
    if(!ServoControl::checkedCmdReturnValue((COM_ERROR)result))
    {
      ui->progressBar->setValue(0);
      ui->progressBar->hide();
      return;
    }
    do
    {
      delayms(500);
      value+=valueInc;
      ui->progressBar->setValue(value);
      delayms(500);
      value+=valueInc;
      ui->progressBar->setValue(value);
      delayms(500);
      value+=valueInc;
      ui->progressBar->setValue(value);
      delayms(500);
      value+=valueInc;
      ui->progressBar->setValue(value);
      delayms(500);
      value+=valueInc;
      ui->progressBar->setValue(value);
      result=(COM_ERROR)GTSD_CMD_CheckResetFinish(i*dspAxis, flag_finish, comtype,mp_mainwindow->getUserConfig()->com.rnStation);
      if(!ServoControl::checkedCmdReturnValue((COM_ERROR)result))
      {
        ui->progressBar->setValue(0);
        ui->progressBar->hide();
        return;
      }
      qDebug()<<"reset dsp";
    }while(!flag_finish);
    value+=valueInc;
    ui->progressBar->setValue(value);
  }
  value+=valueInc;
  ui->progressBar->setValue(value);
  ui->progressBar->setValue(100);
  QMessageBox::information(0,tr("complete"),tr("reset system success!"));
  ui->progressBar->hide();
}

void DownloadDialog::delayms(quint16 ms)
{
  QTime dieTime = QTime::currentTime().addMSecs(ms);
  while( QTime::currentTime() < dieTime )
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void DownloadDialog::updateProgressValueDSP(void *arg,qint16 *value)
{
    QProgressBar *bar=static_cast<QProgressBar *>(arg);
    m_dspProgressValue=(100/m_dspNum)*m_currentStep+(*value)/m_dspNum;
    bar->setValue(m_dspProgressValue);
    qApp->processEvents();
}

void DownloadDialog::updateProgressValueFPGA(void *arg,qint16 *value)
{
  bool isErase;
  quint16 bitHigh=(quint16)(1<<15);
  isErase=(*value)&bitHigh;
  qint16 v=(*value)&(~bitHigh);
  DownloadDialog *dialog=static_cast<DownloadDialog *>(arg);
  QProgressBar *bar=dialog->ui->progressBar;
  QLabel *label=dialog->ui->label_info;
//  QProgressBar *bar=static_cast<QProgressBar *>(arg);
  m_fpgaProgressValue=(100/m_fpgaNum)*m_currentStep+v/m_fpgaNum;
//  m_fpgaProgressValue=(100/m_fpgaNum)*m_currentStep+(*value)/m_fpgaNum;
  bar->setValue(m_fpgaProgressValue);
  if(isErase)
  {
    label->setText(tr("FPGA Erasing ......"));
  }
  else{
    label->setText(tr("FPGA DownLoad ......"));
  }
  qApp->processEvents();
//  qDebug()<<"v="<<v;
}

void DownloadDialog::enableUi(bool state)
{
  ui->btn_dspload->setEnabled(state);
  ui->btn_dspOpen->setEnabled(state);
  ui->btn_exit->setEnabled(state);
  ui->btn_fpgaload->setEnabled(state);
  ui->btn_fpgaOpen->setEnabled(state);
  ui->btn_reset->setEnabled(state);
}
