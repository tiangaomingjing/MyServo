#include "plotwave.h"
#include "ui_plotwave.h"
#include "MainGTSD/mainwindow.h"
#include "DlgSetXYRange/DlgSetXYRange.h"
#include "DlgSelectCurve/dlgselectcurve.h"
#include "qcustomplot.h"
#include "plotwave_globaldef.h"
#include "boxzoom.h"
#include "meacursorx.h"
#include "meacursory.h"
#include "measuretag.h"
#include "threadsample.h"
#include "dftwidget.h"
#include "ServoDriverAlgorithmDll/ServoDriverAlgorithmDll.h"
#include "MotionStatus/motionstatus.h"
#include "../ServoGeneralCmd/servogeneralcmd.h"
#include "QtTreeManager/qttreemanager.h"
#include "DialogPickCurve/dialogpickcurve.h"
#include "muParser.h"
#include "muParserDef.h"
#include "DialogPickCurve/usrcurvetreemanager.h"
#include "wheelwidget/rollboxwidget.h"

#include <QXmlAttributes>
#include <QXmlStreamReader>
#include <QFile>
#include <QList>
#include <QtMath>
#include <QTreeWidgetItemIterator>
#include <QInputDialog>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QJSEngine>
#include <QFormBuilder>

#define CURVE_NUM_MAX 16
#define TEST_DEBUG 0

QDataStream &operator<<(QDataStream &out,const TableParameters &par)
{
  out<<par.isDraw<<par.fullName<<par.color<<par.bytes<<\
       par.axisNum<<par.offsetAddr<<par.curveKey<<par.curveValue;
  return out;
}

QDataStream &operator>>(QDataStream &in,TableParameters &par)
{
  in>>par.isDraw>>par.fullName>>par.color>>par.bytes>>\
      par.axisNum>>par.offsetAddr>>par.curveKey>>par.curveValue;
  return in;
}

PlotWave::PlotWave(QWidget *parnet):
  QWidget(parnet),
  ui(new Ui::PlotWave())
{
  ui->setupUi(this);
  mp_mainWindow=static_cast<MainWindow*>(parnet);
  mp_userConfig=NULL;
  m_dlgSetting=NULL;
  m_dlgSelectCurve=NULL;
  m_boxZoom=NULL;
  m_cursorX=NULL;
  m_cursorY=NULL;
  m_tag=NULL;
  m_threadSample=NULL;
  m_dftWidget=NULL;
  m_curveSettingTree=NULL;
  m_usrCurveTreeTemplate=NULL;

  initialUi();

  ui->plotCurve->setMouseTracking(true);
  ui->tableWidget->setMouseTracking(true);
  qRegisterMetaType<CurveData>("CurveData");//信号与槽发送自己定义的数据

  //tab1----------------------------------------------------
  ui->dSpinBox_syncVel->installEventFilter(this);
  ui->dSpinBox_id->installEventFilter(this);
  ui->dSpinBox_iq->installEventFilter(this);
  ui->dSpinBox_profileVel->installEventFilter(this);
  ui->dSpinBox_posadjust->installEventFilter(this);
  ui->dSpinBox_posfollow->installEventFilter(this);
  ui->dSpinBox_ua->installEventFilter(this);
  ui->dSpinBox_ub->installEventFilter(this);
  ui->dSpinBox_uc->installEventFilter(this);
  ui->dSpinBox_ud->installEventFilter(this);
  ui->dSpinBox_uq->installEventFilter(this);
  ui->dSpinBox_closeVel->installEventFilter(this);

  //tab2--------------------------------------------------
  ui->dSBox_ampCircle->installEventFilter(this);
  ui->dSBox_ampNoCircle->installEventFilter(this);
  ui->dSBox_circleCount->installEventFilter(this);
  ui->dSBox_T->installEventFilter(this);
  ui->dSBox_time->installEventFilter(this);

  connect(ui->tableWidget,SIGNAL(cellEntered(int,int)),this,SLOT(onCellEnteredMoreDetail(int,int)));
  connect(ui->tableWidget,SIGNAL(itemClicked(QTableWidgetItem*)),this,SLOT(onTableItemClicked(QTableWidgetItem*)));
  connect(ui->tableWidget,SIGNAL(itemDoubleClicked(QTableWidgetItem*)),this,SLOT(onTableItemDoubleClicked(QTableWidgetItem*)));
//  connect(ui->tableWidget,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(onTableItemChanged(QTableWidgetItem*)));
  connect(ui->tableWidget->horizontalHeader(),SIGNAL(sectionDoubleClicked(int)),this,SLOT(onTableHorizontalHeaderDoubleClicked(int)));

  connect(ui->btn_selectAll,SIGNAL(clicked(bool)),this,SLOT(onBtnSelectAllClicked()));
  connect(ui->btn_hori_measure,SIGNAL(clicked(bool)),this,SLOT(onBtnHorizontalMeasureClicked(bool)));
  connect(ui->btn_ver_measure,SIGNAL(clicked(bool)),this,SLOT(onBtnVerticalMeasureClicked(bool)));
  connect(ui->btn_viewMax,SIGNAL(clicked(bool)),this,SLOT(onBtnViewMaxClicked()));
  connect(ui->btn_viewMin,SIGNAL(clicked(bool)),this,SLOT(onBtnViewMinClicked()));
  connect(ui->btn_view,SIGNAL(clicked(bool)),this,SLOT(onBtnViewSettingClicked()));
  connect(ui->btn_auto,SIGNAL(clicked(bool)),this,SLOT(onBtnAutoScaleClicked(bool)));
  connect(ui->btn_reset,SIGNAL(clicked(bool)),this,SLOT(onBtnResetClicked()));
  connect(ui->btn_remove,SIGNAL(clicked(bool)),this,SLOT(onBtnRemoveClicked()));
  connect(ui->btn_clear,SIGNAL(clicked(bool)),this,SLOT(onBtnClearClicked()));
  connect(ui->btn_prev,SIGNAL(clicked(bool)),this,SLOT(onBtnPrevViewClicked()));
  connect(ui->btn_next,SIGNAL(clicked(bool)),this,SLOT(onBtnNextViewClicked()));
  connect(ui->btn_tag,SIGNAL(clicked(bool)),this,SLOT(onBtnTagClicked(bool)));
  connect(ui->btn_add,SIGNAL(clicked(bool)),this,SLOT(onBtnAddClicked()));
  connect(ui->btn_start,SIGNAL(clicked(bool)),this,SLOT(onBtnStartClicked()));
  connect(ui->btn_servo_switch,SIGNAL(clicked(bool)),this,SLOT(onBtnServoOnClicked(bool)));
  connect(ui->btn_fft,SIGNAL(clicked(bool)),this,SLOT(onBtnFFTClicked()));
  connect(ui->btn_open,SIGNAL(clicked(bool)),this,SLOT(onBtnOpenClicked(bool)));
  connect(ui->btn_save,SIGNAL(clicked(bool)),this,SLOT(onBtnSaveClicked()));
  connect(ui->btn_x_left,SIGNAL(clicked(bool)),this,SLOT(onBtnMeasureXLeftClicked(bool)));
  connect(ui->btn_x_right,SIGNAL(clicked(bool)),this,SLOT(onBtnMeasureXRightClicked(bool)));
  connect(ui->btn_y_down,SIGNAL(clicked(bool)),this,SLOT(onBtnMeasureYDownClicked(bool)));
  connect(ui->btn_y_up,SIGNAL(clicked(bool)),this,SLOT(onBtnMeasureYUpClicked(bool)));

  connect(ui->plotCurve,SIGNAL(mouseMove(QMouseEvent*)),this,SLOT(onPlotMouseMoveEvent(QMouseEvent*)));
  connect(ui->plotCurve,SIGNAL(mousePress(QMouseEvent*)),this,SLOT(onPlotMousePressEvent(QMouseEvent*)));
  connect(ui->plotCurve,SIGNAL(mouseRelease(QMouseEvent*)),this,SLOT(onPlotMouseReleaseEvent(QMouseEvent*)));
  connect(ui->plotCurve,SIGNAL(mouseWheel(QWheelEvent*)),this,SLOT(onPlotMouseWheelEvent(QWheelEvent*)));

  connect(ui->comboBox_plotStyle,SIGNAL(currentIndexChanged(int)),this,SLOT(onPlotStyleChanged(int)));

  //tab1--------------------------------------
  connect(ui->dSpinBox_syncVel,SIGNAL(valueChanged(double)),this,SLOT(onDoubleSpinBoxValueChanged(double)));
  connect(ui->dSpinBox_id,SIGNAL(valueChanged(double)),this,SLOT(onDoubleSpinBoxValueChanged(double)));
  connect(ui->dSpinBox_iq,SIGNAL(valueChanged(double)),this,SLOT(onDoubleSpinBoxValueChanged(double)));
  connect(ui->dSpinBox_profileVel,SIGNAL(valueChanged(double)),this,SLOT(onDoubleSpinBoxValueChanged(double)));
  connect(ui->dSpinBox_posadjust,SIGNAL(valueChanged(double)),this,SLOT(onDoubleSpinBoxValueChanged(double)));
  connect(ui->dSpinBox_posfollow,SIGNAL(valueChanged(double)),this,SLOT(onDoubleSpinBoxValueChanged(double)));
  connect(ui->dSpinBox_ua,SIGNAL(valueChanged(double)),this,SLOT(onDoubleSpinBoxValueChanged(double)));
  connect(ui->dSpinBox_ub,SIGNAL(valueChanged(double)),this,SLOT(onDoubleSpinBoxValueChanged(double)));
  connect(ui->dSpinBox_uc,SIGNAL(valueChanged(double)),this,SLOT(onDoubleSpinBoxValueChanged(double)));
  connect(ui->dSpinBox_ud,SIGNAL(valueChanged(double)),this,SLOT(onDoubleSpinBoxValueChanged(double)));
  connect(ui->dSpinBox_uq,SIGNAL(valueChanged(double)),this,SLOT(onDoubleSpinBoxValueChanged(double)));
  connect(ui->dSpinBox_closeVel,SIGNAL(valueChanged(double)),this,SLOT(onDoubleSpinBoxValueChanged(double)));

  //tab2---------------------------------------------
  connect(ui->dSBox_ampCircle,SIGNAL(valueChanged(double)),this,SLOT(onDoubleSpinBoxValueChanged(double)));
  connect(ui->dSBox_ampNoCircle,SIGNAL(valueChanged(double)),this,SLOT(onDoubleSpinBoxValueChanged(double)));
  connect(ui->dSBox_circleCount,SIGNAL(valueChanged(double)),this,SLOT(onDoubleSpinBoxValueChanged(double)));
  connect(ui->dSBox_T,SIGNAL(valueChanged(double)),this,SLOT(onDoubleSpinBoxValueChanged(double)));
  connect(ui->dSBox_time,SIGNAL(valueChanged(double)),this,SLOT(onDoubleSpinBoxValueChanged(double)));

  //tab1--------------
  connect(ui->listWidget_axis,SIGNAL(currentRowChanged(int)),this,SLOT(onListWidgetAxisClicked(int)));
  connect(ui->listWidget_mode,SIGNAL(currentRowChanged(int)),this,SLOT(onListWidgetModeIndexChanged(int)));

  //tab2---------------
  connect(ui->listWidget_motionSrc_tab2,SIGNAL(currentRowChanged(int)),ui->stackedWidget_motionMode,SLOT(setCurrentIndex(int)));
  connect(ui->listWidget_axis_tab2,SIGNAL(currentRowChanged(int)),this,SLOT(onUpdateMotionTabUi(int)));
  connect(ui->listWidget_motionSrc_tab2,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(onListWidgetMotionSrcTab2ItemClicked(QListWidgetItem*)));
  connect(ui->btn_srcPC,SIGNAL(clicked(bool)),this,SLOT(onBtnControlSourcePCClicked()));
  connect(ui->btn_srcGLink2,SIGNAL(clicked(bool)),this,SLOT(onBtnConrtolSourceGLink2Clicked()));
  connect(ui->checkBox_vclCircleSwitch,SIGNAL(clicked(bool)),this,SLOT(onCheckBoxVelocityClicked(bool)));
  connect(ui->btn_motionRun,SIGNAL(clicked(bool)),this,SLOT(onBtnMotionRunClicked(bool)));


  //fft相关功能
//  connect(ui->tableWidget,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(onTableWidgetCustomContextMenuRequested(QPoint)));
//  connect(m_actFFTSignal,SIGNAL(triggered(bool)),this,SLOT(onContextActionClicked()));
//  connect(m_actFFTSystemInPut,SIGNAL(triggered(bool)),this,SLOT(onContextActionClicked()));
//  connect(m_actFFTSystemOutPut,SIGNAL(triggered(bool)),this,SLOT(onContextActionClicked()));
//  connect(ui->btn_fftOK,SIGNAL(clicked(bool)),this,SLOT(onBtnFFTApplyClicked()));
//  connect(ui->btn_fftExit,SIGNAL(clicked(bool)),this,SLOT(onBtnFFTCancelClicked()));
}
PlotWave::~PlotWave()
{
  qDebug()<<"plotwave release ->";
  writePlotWaveFile();
  if(m_cursorX!=NULL)
    delete m_cursorX;
  if(m_cursorY!=NULL)
    delete m_cursorY;
  if(m_dftWidget!=NULL)
    delete m_dftWidget;
  if(m_dlgSetting!=NULL)
    delete m_dlgSetting;
  delete ui;

  MotionStatus *status;
  for(int i=0;i<m_motionStatusPtrList.count();i++)
  {
    status=m_motionStatusPtrList[i];
    delete status;
  }
  m_motionStatusPtrList.clear();

  if(m_curveSettingTree!=NULL)
  {
    m_curveSettingTree->clear();
    delete m_curveSettingTree;
  }

  delete m_usrCurveTreeManager;

  if(m_rollBoxUi!=NULL)
    delete m_rollBoxUi;

}
void PlotWave::updateTablePlotPrmListConstValueWhenSampling()
{
  QMutableListIterator<PlotTablePrm> itor(m_tablePlotPrmList);
  double cFactorValue=0;
  CurveType curveType;
  int totalCount;
  int varCount;
  int axisNum;
  quint16 offset;
  quint16 base;
  int bytesNum;
  while (itor.hasNext())
  {
    itor.next();
    curveType=itor.value().curveType;
    if(curveType==CURVE_TYPE_SCRIPT)
    {
      totalCount=itor.value().prmFactorList.count();
      varCount=itor.value().varCount;
      axisNum=itor.value().axisNum;
      for(int i=varCount;i<totalCount;i++)
      {
        offset=itor.value().prmFactorList[i].controlPrm.offsetAddr;
        base=itor.value().prmFactorList[i].controlPrm.baseAddr;
        bytesNum=itor.value().prmFactorList[i].controlPrm.bytes;
        cFactorValue=readRAMValue(axisNum,offset,base,bytesNum);
        itor.value().prmFactorList[i].constValue=cFactorValue;
        qDebug()<<"cfactor:"<<cFactorValue;
      }
    }
  }
}

void PlotWave::evaluateCurveSamplingScript()
{
  QMutableListIterator<PlotTablePrm> itor(m_tablePlotPrmList);
  CurveType curveType;
  int totalCount;
  int varCount;
//  qDebug()<<"m_tablePlotPrmList.at(0).data.curveKey.size():"<<m_tablePlotPrmList.at(0).data.curveKey.size();
//  qDebug()<<"m_tablePlotPrmList.at(0).data.curveValue.size():"<<m_tablePlotPrmList.at(0).data.curveValue.size();
//  qDebug()<<"varDatas size:"<<m_tablePlotPrmList.at(0).prmFactorList.at(0).varDatas.curveValue.size();
  while (itor.hasNext())
  {
    itor.next();
    curveType=itor.value().curveType;
    if(curveType==CURVE_TYPE_SCRIPT)
    {
      //构建数学解析器
      totalCount=itor.value().prmFactorList.count();
      varCount=itor.value().varCount;
      double varArray[CURVE_NUM_MAX];
      double *varPtr;
      varPtr=varArray;
      mu::Parser mParser;
      for(int var=0;var<varCount;var++)
      {
        mParser.DefineVar(tr("V%1").arg(var).toStdWString(),varPtr++);
      }
      for(int c=0;c<totalCount-varCount;c++)
      {
        mParser.DefineConst(tr("C%1").arg(c).toStdWString(),itor.value().prmFactorList.at(c+varCount).constValue);
        qDebug()<<"itor.value().prmFactorList.at(c+varCount).constValue = "<<itor.value().prmFactorList.at(c+varCount).constValue;
      }
      mParser.SetExpr(itor.value().equationDescription.toStdWString());
      qDebug()<<"itor.value().equationDescription.toStdWString():"<<QString::fromStdWString(itor.value().equationDescription.toStdWString());

      double jsValueTemp;
//      for(quint32 i=0;i<length;i++)
//      {
//        for(int var=0;var<varCount;var++)
//        {
//          varArray[var]=itor.value().prmFactorList.at(var).varDatas.curveValue.at(i);
//        }
//        //开始解析
//        try{
//          jsValueTemp=mParser.Eval();
//        }
//        catch (mu::Parser::exception_type &e)
//        {
//          qDebug()<<"ParserError: "<<QString::fromStdWString(e.GetMsg());
//        }
//        itor.value().data.curveValue[i]=jsValueTemp;
//      }
      QMutableVectorIterator<double> vItor(itor.value().data.curveValue);
      quint32 index=0;
      double unitFactor;
      unitFactor=itor.value().multiValue;
      while(vItor.hasNext())
      {
        vItor.next();
        for(int var=0;var<varCount;var++)
        {
          varArray[var]=itor.value().prmFactorList.at(var).varDatas.curveValue.at(index);
        }
        //开始解析
        try{
          jsValueTemp=mParser.Eval();
          jsValueTemp*=unitFactor;
        }
        catch (mu::Parser::exception_type &e)
        {
          qDebug()<<"ParserError: "<<QString::fromStdWString(e.GetMsg());
        }
        vItor.setValue(jsValueTemp);
        index++;
      }
    }
  }
}

UserConfig *PlotWave::getUserConfigFromMainWidow()
{
  return mp_mainWindow->getUserConfig();
}

double PlotWave::getSamplingTime()
{
  double t=ui->lineEdit_sampling->text().toDouble();
  return t;
}

void PlotWave::updateCurrentServoStatus()
{
//  if(axisNum<m_servoStatusList.count())
//  {
//    m_servoStatusList[axisNum]=status;
//    //update ui
//    ui->listWidget_axis->setCurrentRow(axisNum);
//  }
  //--------------------------读当前的伺服状态-------------------------------------
  quint16 comid=mp_userConfig->com.id;
  qint16 rnStation=mp_userConfig->com.rnStation;

  for(int i=0;i<m_servoStatusList.count();i++)
  {
    m_servoStatusList[i].currentModeIndex=ServoControl::servoCurrentUserTaskMode(i,(quint16)comid,rnStation);
    m_servoStatusList[i].isOn=ServoControl::checkServoIsReady(i,(quint16)comid,rnStation);//读是否上伺服
//    m_servoStatusList[i].m_closeVel=0;
//    m_servoStatusList[i].m_id=0;
//    m_servoStatusList[i].m_iq=0;
//    m_servoStatusList[i].m_posAdj=0;
//    m_servoStatusList[i].m_posFollow=0;
//    m_servoStatusList[i].m_profileVel=0;
//    m_servoStatusList[i].m_syncVel=0;
//    m_servoStatusList[i].m_ua=0;
//    m_servoStatusList[i].m_ub=0;
//    m_servoStatusList[i].m_uc=0;
//    m_servoStatusList[i].m_ud=0;
//    m_servoStatusList[i].m_uq=0;
  }
  //update ui
  //先关掉连接，以免改变下触发SLOT函数
  disconnect(ui->listWidget_mode,SIGNAL(currentRowChanged(int)),this,SLOT(onListWidgetModeIndexChanged(int)));
  ui->listWidget_mode->setCurrentRow(m_servoStatusList.at(0).currentModeIndex);
  connect(ui->listWidget_mode,SIGNAL(currentRowChanged(int)),this,SLOT(onListWidgetModeIndexChanged(int)));
  //改变滚轮1的状态
  m_rollBoxUi->setButtonChecked(0);
  for(int i=0;i<m_servoStatusList.count();i++)
    m_rollBoxUi->setCurrentIndexAt(m_servoStatusList.at(i).currentModeIndex,i);
  ui->stackedWidget->setCurrentIndex(m_servoStatusList.at(0).currentModeIndex);
  ui->btn_servo_switch->setEnabled(true);
  setServoOnUi(m_servoStatusList.at(0).isOn);

  //------------------读当前的运动控制源-------------------------------
  for(int i=0;i<m_motionStatusPtrList.count();i++)
  {
    qDebug()<<"src:"<<i<<" "<<m_motionStatusPtrList[i]->controlSrc();
    m_motionStatusPtrList[i]->setMotionMode(MotionStatus::MOTION_MODE_NONE);
  }
  ui->listWidget_axis_tab2->setCurrentRow(0);
  ui->listWidget_motionSrc_tab2->setCurrentRow(0);
  ui->btn_motionRun->setEnabled(false);

  MotionStatus::ControlSource ctlSrcIndex=m_motionStatusPtrList[0]->controlSrc();
  setControlSourceIndexUi(ctlSrcIndex);
}

void PlotWave::onClearPlotWave()
{
//  //clear tablewidget
//  ui->tableWidget->setRowCount(0);
//  ui->tableWidget->clearContents();
//  //clear graph curve
//  if(ui->btn_tag->isChecked())
//    onBtnTagClicked(false);
//  int rowcount=ui->plotCurve->graphCount();
//  for(int i=0;i<rowcount;i++)
//    ui->plotCurve->removeGraph(i);

//  //clear m_tablePlotPrmList
//  m_tablePlotPrmList.clear();
  qDebug()<<"step 1";
  int axisCount=mp_mainWindow->getUserConfig()->model.axisCount;
  mp_treeAll=mp_mainWindow->getRamAllAxisTree();
  mp_userConfig=mp_mainWindow->getUserConfig();
  m_usrCurveTreeManager->setNewSrcAllTree(mp_treeAll);//更新多轴曲线模板
  onBtnClearClicked();
  //更新滚轮控件
  int dcount=axisCount-m_rollBoxUi->rollCount();
  if(dcount>0)
  {
    for(int i=0;i<dcount;i++)
      m_rollBoxUi->appendBox();
  }
  else if(dcount<0)
  {
    for(int i=0;i<qAbs(dcount);i++)
      m_rollBoxUi->removeLastBox();
  }

  setListViewAxisNum(axisCount);
  qDebug()<<"step 3";
  initialServoStatus(axisCount);
  qDebug()<<"step 4";
  clearTableMenuAction();
  qDebug()<<"step 5";
  addTableMenuAction(axisCount);
  qDebug()<<"step 6";

  //更新通用指令对象
  m_generalCmd->setCmdTree(mp_mainWindow->getFunctionCmdTree());
  m_generalCmd->setComType(mp_userConfig->com.id);
  m_generalCmd->setComRnStation(mp_userConfig->com.rnStation);

  //更新运动控制状态类
  MotionStatus *status;
  QProgressBar *bar;
  for(int i=0;i<m_motionStatusPtrList.count();i++)
  {
    bar=m_progressBarList[i];
    status=m_motionStatusPtrList[i];
    disconnect(status,SIGNAL(updateProgressValue(int)),bar,SLOT(setValue(int)));
    disconnect(status,SIGNAL(planVeltimerStop(int)),this,SLOT(onMotionStatusPlanVelTimeOut(int)));
    delete status;
    delete bar;
  }
  qDebug()<<"step 7";
  m_motionStatusPtrList.clear();
  m_progressBarList.clear();
  QString strBarStyle="QProgressBar::chunk {background-color: #CD96CD;height: 5px;margin: 0.5px;}QProgressBar {border: 2px solid grey;border-radius: 5px;text-align: center;}";
  for(int i=0;i<axisCount;i++)
  {
    status=new MotionStatus(m_generalCmd,i);
    m_motionStatusPtrList.append(status);

    bar=new QProgressBar(ui->widget_progressGo);
    bar->setStyleSheet(strBarStyle);
    m_progressBarList.append(bar);
    bar->setValue(0);
    bar->setOrientation(Qt::Vertical);
    QSizePolicy sizePolicy;
    sizePolicy.setHorizontalPolicy(QSizePolicy::Expanding);
    sizePolicy.setVerticalPolicy(QSizePolicy::Expanding);
    bar->setSizePolicy(sizePolicy);
    ui->widget_progressGo->layout()->addWidget(bar);
    connect(status,SIGNAL(updateProgressValue(int)),bar,SLOT(setValue(int)));
    connect(status,SIGNAL(planVeltimerStop(int)),this,SLOT(onMotionStatusPlanVelTimeOut(int)));
  }


}

void PlotWave::onStopThreadSampling()
{
  if(ui->btn_start->isChecked())//已经开始采样,则要关闭
  {
    qDebug()<<"stop sampling....";
    disconnect(m_threadSample,SIGNAL(sendPlotData(int,int,long,CurveData)),this,SLOT(onSendPlotData(int,int,long,CurveData)));

    m_threadSample->stop();
    m_threadSample->wait();
    m_threadSample->deleteLater();
    m_threadSample=NULL;

    enableUi(true);
    ui->btn_start->setChecked(false);
  }

  ui->widget_progressGo->setHidden(true);
  for(int i=0;i<m_motionStatusPtrList.count();i++)
  {
    if(ui->listWidget_axis_tab2->item(i)->isSelected())
    {
      m_motionStatusPtrList.at(i)->stopPlanVelTimer();
    }
  }
  ui->btn_motionRun->setChecked(false);
}

//----------------slots function--------------------
void PlotWave::onCellEnteredMoreDetail(int row, int col)
{
  if (col == COL_PRM_TABLE_NAME)
  {
    ui->label_fullName->setText(tr("%1").arg(m_tablePlotPrmList.at(row).fullName));
//    qDebug()<<m_tablePlotPrmList.at(row).fullName;
  }
}

void PlotWave::onTableItemClicked(QTableWidgetItem *item)
{
  int row=item->row();
//  qDebug()<<tr("row:%1 item text:%2").arg(row).arg(item->text());
  if(item->column()==COL_PRM_TABLE_DRAW)//改变其显示与否
  {
    QString str;
    QColor color;
    if(m_tablePlotPrmList.at(row).isDraw)
    {
      str=tr("off");
      color=QColor(Qt::gray);
      m_tablePlotPrmList[row].isDraw=false;
    }
    else
    {
      str=tr("on");
//      color=QColor(97,141,211);
      color=QColor(Qt::green).darker();
      m_tablePlotPrmList[row].isDraw=true;
    }
    item->setText(str);
    item->setBackgroundColor(color);
    ui->plotCurve->graph(row)->setVisible(m_tablePlotPrmList[row].isDraw);
    ui->plotCurve->replot();
  }
//  else if(item->column()==COL_PRM_TABLE_AXISNUM)
//  {
//    if(ui->btn_start->isChecked()==false)
//      ui->tableWidget->editItem(item);
//    //还要增加轴号对应的offsetaddr的更改
//  }
}
void PlotWave::onTableItemChanged(QTableWidgetItem *item)
{
  int row=item->row();
  if(ui->btn_start->isChecked()==false)
  {
    if((item->column()==COL_PRM_TABLE_AXISNUM)&&(row!=-1))
    {
//      //校验输入合法性
//      QIntValidator intValidator(0,mp_userConfig->model.axisCount-1,this);
//      int pos=0;
//      QString text=item->text();
//      qDebug()<<"text"<<text;
//      if(intValidator.validate(text,pos)!=QValidator::Acceptable)
//      {
//        QMessageBox::information(0,tr("Range Exception"),tr("input value out of range 0~%1").arg(mp_userConfig->model.axisCount-1));
//        return;
//      }
//      //更新addrOffset
//      QTreeWidget *treeWidget;
//      QTreeWidgetItem *itemtree;
//      int axisnum=text.toInt();
//      QString name=m_tablePlotPrmList.at(row).fullName;
//      QString offset;
////      itemtree=new QTreeWidgetItem();
//      mp_treeAll=mp_mainWindow->getRamAllAxisTree();
//      itemtree=mp_treeAll->topLevelItem(axisnum)->clone();
//      treeWidget=new QTreeWidget();
//      treeWidget->addTopLevelItem(itemtree);

//      {
//        QTreeWidgetItem *item;
//        QTreeWidgetItemIterator it_src(treeWidget);
//        while((*it_src))
//        {
//          item=(*it_src);
//          if(name==item->text(COL_NAME))
//          {
//            offset=item->text(COL_ADDRESS);
//            break;
//          }
//          it_src++;
//        }
//      }
//      treeWidget->clear();
//      delete treeWidget;
//      m_tablePlotPrmList[row].axisNum=axisnum;
//      m_tablePlotPrmList[row].offsetAddr=offset.toShort();
//      ui->tableWidget->item(row,COL_PRM_TABLE_OFFSET)->setText(offset);
//      qDebug()<<"offset:"<<offset;
    }
    else if((item->column()==COL_PRM_TABLE_XOFFSET)&&(row!=-1))
    {
      QRegExp rx("^(-?\\d+)(\\.\\d+)?$");
      QRegExpValidator validator(rx,0);
      int pos;
      if(validator.validate(item->text(),pos)!=QValidator::Acceptable)
      {
         QMessageBox::information(0,tr("Range Exception"),tr("input value is invalid "));
         return;
      }
      double key=item->text().toDouble();
      QMutableVectorIterator<double> it(m_tablePlotPrmList[row].data.curveKey);
      while(it.hasNext())
      {
        double oKey=it.next();
        it.setValue(oKey+key);
      }
      ui->plotCurve->graph(row)->clearData();
      ui->plotCurve->graph(row)->addData(m_tablePlotPrmList[row].data.curveKey,m_tablePlotPrmList[row].data.curveValue);
      ui->plotCurve->replot();
    }
  }
  else//正在画图的时候也起作用
  {

  }
  //正在画图的时候也起作用
  if((item->column()==COL_PRM_TABLE_ADD)&&(row!=-1))
  {
    QRegExp rx("^(-?\\d+)(\\.\\d+)?$");
    QRegExpValidator validator(rx,0);
    int pos;
    if(validator.validate(item->text(),pos)!=QValidator::Acceptable)
    {
       QMessageBox::information(0,tr("Range Exception"),tr("input value is invalid "));
       return;
    }
    m_tablePlotPrmList[row].addValue=item->text().toDouble();
  }
  else if((item->column()==COL_PRM_TABLE_MULTI)&&(row!=-1))
  {
    QRegExp rx("^(-?\\d+)(\\.\\d+)?$");
    QRegExpValidator validator(rx,0);
    int pos;
    if(validator.validate(item->text(),pos)!=QValidator::Acceptable)
    {
       QMessageBox::information(0,tr("Range Exception"),tr("input value is invalid "));
       return;
    }
    m_tablePlotPrmList[row].multiValue=item->text().toDouble();
  }

}
void PlotWave::onTableHorizontalHeaderDoubleClicked(int index)
{
  qDebug()<<tr("index:%1").arg(index);
  if(ui->btn_start->isChecked()==false)
  {
    if(index==COL_PRM_TABLE_AXISNUM)
    {
      int value=0;
      bool ok=false;
      value=QInputDialog::getInt(this, tr("Input Axis Number"),
                                 tr("Axis Number:"), 0, 0, mp_userConfig->model.axisCount-1, 1, &ok);
      if (ok)
      {
        qDebug()<<tr("inputvalue:%1").arg(value);
        for(int i=0;i<ui->tableWidget->rowCount();i++)
        {
          updateOffsetAddrWhenAxisNumberChanged(value,i);
        }
      }
    }
  }
}

void PlotWave::onTablePopMenu(QPoint pos)
{
  QTableWidgetItem *item=ui->tableWidget->itemAt(pos);

  if(item!=NULL)
  {
    int column=item->column();
//    qDebug()<<"row="<<item->row();
//    qDebug()<<"column="<<item->column();
//    qDebug()<<"text="<<item->text();
    switch (column)
    {
    case COL_PRM_TABLE_NAME:
//      m_popuMenuTable->exec(QCursor::pos());
      break;
    case COL_PRM_TABLE_AXISNUM:
      m_popuMenuTable->exec(QCursor::pos());
      break;
    }
  }
}

void PlotWave::onTableItemDoubleClicked(QTableWidgetItem *item)
{
  if(item->column()==COL_PRM_TABLE_NAME)
  {
    int row=item->row();
    QColor clr=m_tablePlotPrmList.at(row).color;
    clr=QColorDialog::getColor(clr);
    qDebug()<<"double clicked ";
    if(clr.isValid())
    {
      m_tablePlotPrmList[row].color=clr;
      item->setTextColor(clr);
      ui->plotCurve->graph(row)->setPen(QPen(clr));
      ui->plotCurve->replot();
      qDebug()<<"double clicked -------------select color";
    }
  }
//  else if(item->column()==COL_PRM_TABLE_AXISNUM)
//  {
//    if(ui->btn_start->isChecked()==false)
//      ui->tableWidget->editItem(item);
//    //还要增加轴号对应的offsetaddr的更改
//  }
  else if(item->column()==COL_PRM_TABLE_XOFFSET)
  {
    if(ui->btn_start->isChecked()==false)
      ui->tableWidget->editItem(item);
  }
  else if(item->column()==COL_PRM_TABLE_ADD)
  {
    ui->tableWidget->editItem(item);
  }
  else if(item->column()==COL_PRM_TABLE_MULTI)
  {
    ui->tableWidget->editItem(item);
  }
}

void PlotWave::onBtnSelectAllClicked()
{
  bool static flag=true;
  if(flag)
  {
    PlotTablePrm *prm;
    int row=0;
    QMutableListIterator<PlotTablePrm> it(m_tablePlotPrmList);
    while(it.hasNext())
    {
      prm=&it.next();
      if(!prm->isDraw)
      {
        prm->isDraw=true;
        setCurveTableWidgetOnOffUi(row,true);
      }
      row++;
    }
  }
  else
  {
    PlotTablePrm *prm;
    int row=0;
    QMutableListIterator<PlotTablePrm> it(m_tablePlotPrmList);
    while(it.hasNext())
    {
      prm=&it.next();
      if(prm->isDraw)
      {
        prm->isDraw=false;
        setCurveTableWidgetOnOffUi(row,false);
      }
      row++;
    }
  }
  flag=!flag;
  PlotTablePrm prm;
  int row=0;
  foreach (prm, m_tablePlotPrmList)
  {
    if(prm.isDraw)
      ui->plotCurve->graph(row)->setVisible(true);
    else
      ui->plotCurve->graph(row)->setVisible(false);
    row++;
  }
  ui->plotCurve->replot();
}
void PlotWave::onBtnAutoScaleClicked(bool checked)
{
  Q_UNUSED(checked);
}

void PlotWave::onBtnResetClicked()
{
  ui->plotCurve->rescaleAxes(true);
  if(m_cursorX!=NULL)
    m_cursorX->fillAllMeasureCursor();
  if(m_cursorY!=NULL)
    m_cursorY->fillAllMeasureCursor();
  ui->plotCurve->replot();
}

//!
//! \brief PlotWave::onBtnHorizontalMeasureClicked
//! \param checked
//!测量模式下不能画图
void PlotWave::onBtnHorizontalMeasureClicked(bool checked)
{
  if(checked)
  {
    if(m_cursorY==NULL)
    {
      m_cursorY=new MeaCursorY(ui->plotCurve,0);
//      m_cursorY->disablePlotViewMove();
      ui->btn_y_down->setVisible(true);
      ui->btn_y_up->setVisible(true);
      ui->btn_tag->setEnabled(false);
      ui->plotCurve->setInteraction(QCP::iSelectPlottables,false);

      m_cursorY->setCurve1Checked(true);
    }
  }
  else
  {
//    if(!ui->btn_ver_measure->isChecked())
//      m_cursorY->enablePlotViewMove();
    if(!ui->btn_ver_measure->isChecked())
      ui->btn_tag->setEnabled(true);
    delete m_cursorY;
    m_cursorY=NULL;
    ui->btn_y_down->setVisible(false);
    ui->btn_y_up->setVisible(false);
    ui->btn_y_down->setChecked(false);
    ui->btn_y_up->setChecked(false);
    ui->plotCurve->setInteraction(QCP::iSelectPlottables,true);
  }
  ui->btn_start->setEnabled(!checked);
  if(ui->btn_open->isChecked())//如果是打开曲线模式，一直不让其生效
    ui->btn_start->setEnabled(false);
  ui->plotCurve->replot();
  if(false==ui->label_measure_xy->isVisible())
    ui->label_measure_xy->setVisible(true);
}
void PlotWave::onBtnVerticalMeasureClicked(bool checked)
{
  if(checked)
  {
    if(m_cursorX==NULL)
    {
      ui->btn_x_left->setVisible(true);
      ui->btn_x_right->setVisible(true);
      m_cursorX=new MeaCursorX(ui->plotCurve,0);
      connect(m_cursorX,SIGNAL(measureLeftValueChanged(double)),this,SLOT(onTest1(double)));
      connect(m_cursorX,SIGNAL(measureRightValueChanged(double)),this,SLOT(onTest2(double)));
      connect(m_cursorX,SIGNAL(measureLeftValueChanged(double)),this,SIGNAL(measureXLeftChanged(double)));
      connect(m_cursorX,SIGNAL(measureRightValueChanged(double)),this,SIGNAL(measureXRightChanged(double)));
//      m_cursorX->disablePlotViewMove();
      ui->btn_tag->setEnabled(false);
      ui->plotCurve->setInteraction(QCP::iSelectPlottables,false);

      m_cursorX->setCurve1Checked(true);
    }
  }
  else
  {
//    if(!ui->btn_hori_measure->isChecked())
//      m_cursorX->enablePlotViewMove();
    if(!ui->btn_hori_measure->isChecked())
      ui->btn_tag->setEnabled(true);
    ui->btn_x_left->setVisible(false);
    ui->btn_x_right->setVisible(false);
    ui->btn_x_left->setChecked(false);
    ui->btn_x_right->setChecked(false);
    disconnect(m_cursorX,SIGNAL(measureLeftValueChanged(double)),this,SLOT(onTest1(double)));
    disconnect(m_cursorX,SIGNAL(measureRightValueChanged(double)),this,SLOT(onTest2(double)));
    disconnect(m_cursorX,SIGNAL(measureLeftValueChanged(double)),this,SIGNAL(measureXLeftChanged(double)));
    disconnect(m_cursorX,SIGNAL(measureRightValueChanged(double)),this,SIGNAL(measureXRightChanged(double)));
    delete m_cursorX;
    m_cursorX=NULL;
    ui->plotCurve->setInteraction(QCP::iSelectPlottables,true);

  }
  ui->btn_start->setEnabled(!checked);
  if(ui->btn_open->isChecked())//如果是打开曲线模式，一直不让其生效
    ui->btn_start->setEnabled(false);
  ui->plotCurve->replot();
  if(false==ui->label_measure_xy->isVisible())
    ui->label_measure_xy->setVisible(true);
}
void PlotWave::onBtnViewMinClicked()
{
  qDebug()<<"min ";
  emit showMax(false);
}
void PlotWave::onBtnViewMaxClicked()
{
  qDebug()<<"max max";
  emit showMax(true);
}
void PlotWave::onBtnViewSettingClicked()
{
  if(m_dlgSetting==NULL)
  {
    m_dlgSetting=new DlgSetXYRange(this);
    connect(m_dlgSetting,SIGNAL(viewSetting(ViewSetting)),this,SLOT(onViewSettingUpdate(ViewSetting)));
  }
  m_dlgSetting->show();
}
void PlotWave::onBtnRemoveClicked()
{
  //清graph
  //清m_tableList
  //清table 这三个量是连的，原数据在m_tableList里
  int row=0;
  row=ui->tableWidget->currentRow();//if select none return -1
  if(ui->btn_tag->isChecked())
  {
    onBtnTagClicked(false);
    ui->btn_tag->setChecked(false);
  }
  if(row>=0)
  {
    ui->plotCurve->removeGraph(row);
    m_tablePlotPrmList.removeAt(row);
    ui->tableWidget->removeRow(row);
    ui->plotCurve->replot();
  }
  if(m_dftWidget!=NULL)
  {
    m_dftWidget->setComboxContexts(tableCurveNameList());
  }

  if(m_dftWidget!=NULL)
    m_dftWidget->setComboxContexts(QStringList(tableCurveNameList()));

}

void PlotWave::onBtnClearClicked()
{
  if(ui->tableWidget->rowCount()>0)
  {
    if(ui->btn_tag->isChecked())
    {
      onBtnTagClicked(false);
      ui->btn_tag->setChecked(false);
    }
    ui->plotCurve->clearGraphs();
    m_tablePlotPrmList.clear();
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->clearContents();
//    int len=ui->tableWidget->rowCount();
//    for(int i=0;i<len;i++)
//      ui->tableWidget->removeRow(0);
    ui->plotCurve->replot();
  }
  if(m_dftWidget!=NULL)
    m_dftWidget->setComboxContexts(QStringList(tableCurveNameList()));
}
void PlotWave::onBtnPrevViewClicked(void)
{
  if(m_boxZoom!=NULL)
    m_boxZoom->previousPlotView();
}

void PlotWave::onBtnNextViewClicked(void)
{
  if(m_boxZoom!=NULL)
    m_boxZoom->nextPlotView();
}

void PlotWave::onBtnTagClicked(bool checked)
{
  if(checked)
  {
    if(m_tag==NULL)
      m_tag=new MeasureTag(ui->plotCurve,ui->plotCurve);
//    ui->plotCurve->setInteraction(QCP::iMultiSelect,false);
//    ui->plotCurve->setInteraction(QCP::iSelectPlottables,true);
    for(int i=0;i<ui->plotCurve->graphCount();i++)
    {
      ui->plotCurve->graph(i)->setSelectable(true);
    }
    ui->btn_hori_measure->setEnabled(false);
    ui->btn_ver_measure->setEnabled(false);
  }
  else
  {
//    ui->plotCurve->setInteraction(QCP::iMultiSelect,true);
//    ui->plotCurve->setInteraction(QCP::iSelectPlottables,false);
    ui->btn_hori_measure->setEnabled(true);
    ui->btn_ver_measure->setEnabled(true);
    for(int i=0;i<ui->plotCurve->graphCount();i++)
    {
      ui->plotCurve->graph(i)->setSelected(false);
      ui->plotCurve->graph(i)->setSelectable(false);
    }
    m_tag->clearMeasureTag();
    delete m_tag;
    m_tag=NULL;
  }
  ui->btn_start->setEnabled(!checked);
}

//void PlotWave::updataUserCurveTreeList()
//{
//  mp_treeAll=mp_mainWindow->getRamAllAxisTree();

//  //生成多个轴的用户曲线数据原始模板
//  clearTreeWidgetList(m_usrCurveTreeList);

//  QTreeWidgetItem *treeItem;
//  int axisCount=mp_userConfig->model.axisCount;
//  for(int i=0;i<axisCount;i++)
//  {
//    QTreeWidget *treeWidget=new QTreeWidget;
//    for(int j=0;j<m_usrCurveTreeTemplate->topLevelItemCount();j++)
//    {
//      treeItem=m_usrCurveTreeTemplate->topLevelItem(j)->clone();
//      treeWidget->addTopLevelItem(treeItem);
//    }
//    m_usrCurveTreeList.append(treeWidget);
//    qDebug()<<"add treewidget axis="<<i;
//  }
//  //根据实际的RAM树，修改多轴模板（1地址 2轴号）
//  changeUsrTreeBySrcTree(m_usrCurveTreeList,mp_treeAll);
//}

void PlotWave::onBtnAddClicked()
{
  DialogPickCurve *dlgPickCurve=new DialogPickCurve(this,0);//传plotwave对象进去，获取其相关信息,父类为0，方便删除以释放内存
  connect(dlgPickCurve,SIGNAL(expertTreeItemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(onTreeWidgetItemDoubleClicked(QTreeWidgetItem*,int)));
  connect(dlgPickCurve,SIGNAL(usrTableCellDoubleClicked(int,QTableWidget*,QComboBox*)),this,SLOT(onUsrTableWidgetCellDoubleClicked(int,QTableWidget*,QComboBox*)));
  connect(dlgPickCurve,SIGNAL(deleteScriptCurveAtId(int)),this,SLOT(onDeleteScriptCurveIdAt(int)));

  dlgPickCurve->exec();
  disconnect(dlgPickCurve,SIGNAL(expertTreeItemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(onTreeWidgetItemDoubleClicked(QTreeWidgetItem*,int)));
  disconnect(dlgPickCurve,SIGNAL(usrTableCellDoubleClicked(int,QTableWidget*,QComboBox*)),this,SLOT(onUsrTableWidgetCellDoubleClicked(int,QTableWidget*,QComboBox*)));
  disconnect(dlgPickCurve,SIGNAL(deleteScriptCurveAtId(int)),this,SLOT(onDeleteScriptCurveIdAt(int)));
  delete dlgPickCurve;

//  m_dlgSelectCurve=new DlgSelectCurve(mp_treeAll,0);
//  connect(m_dlgSelectCurve,SIGNAL(treeItemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(onTreeWidgetItemDoubleClicked(QTreeWidgetItem*,int)));
//  m_dlgSelectCurve->exec();
//  disconnect(m_dlgSelectCurve,SIGNAL(treeItemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(onTreeWidgetItemDoubleClicked(QTreeWidgetItem*,int)));
//  delete m_dlgSelectCurve;

  //更新 新的曲线到dft窗口
  if(m_dftWidget!=NULL)
    m_dftWidget->setComboxContexts(QStringList(tableCurveNameList()));
}
void PlotWave::onBtnStartClicked()
{
  if(mp_mainWindow->getComOpenState())
  {
    ui->plotCurve->setInteraction(QCP::iSelectPlottables,false);//不允许选择
    if(ui->btn_start->isChecked())
    {
      qDebug()<<"checked";
      clearGraphData();
      m_threadSample=new ThreadSample(this);
      connect(m_threadSample,SIGNAL(sendPlotData(int,int,long,CurveData)),this,SLOT(onSendPlotData(int,int,long,CurveData)));

      m_threadSample->start();
      m_threadSample->setPriority(QThread::TimeCriticalPriority);
      m_isZoomFirst=true;

      enableUi(false);
      resetTableWidget();
      ui->plotCurve->rescaleAxes();
    }
    else
    {
      qDebug()<<"unchecked";
      disconnect(m_threadSample,SIGNAL(sendPlotData(int,int,long,CurveData)),this,SLOT(onSendPlotData(int,int,long,CurveData)));

      m_threadSample->stop();
      m_threadSample->wait();
      m_threadSample->deleteLater();
      m_threadSample=NULL;
      enableUi(true);
      QTimer::singleShot(200, this, SLOT(onSamplingStopEvaluateScript()));//解析还原数据
    }
  }
  else
  {
    QMessageBox::information(0,tr("Warring"),tr("please open the com first !"));
    ui->btn_start->setChecked(false);
  }
}
void PlotWave::onBtnServoOnClicked(bool checked)
{
  if(mp_mainWindow->getComOpenState())//是否连接到驱动器
  {
//    if(checked)
//    {
//      qDebug()<<"servo on checked";
//      COM_ERROR error;
//      for(int i=0;i<ui->listWidget_axis->count();i++)
//      {
//        if(ui->listWidget_axis->item(i)->isSelected())
//        {
//          //打开伺服
//          error=(COM_ERROR)GTSD_CMD_SetServoOn(i, mp_mainWindow->getUserConfig()->com.id,mp_mainWindow->getUserConfig()->com.rnStation);
//          if(!ServoControl::checkedCmdReturnValue(error))
//            return ;
//          m_servoStatusList[i].isOn=true;
//          qDebug()<<"servo on :"<<i;
//        }
//      }
//      ui->label_servoon->setText(tr("servo-ON"));
//    }
//    else
//    {
//      qDebug()<<"servo on unchecked";
//      COM_ERROR error;
//      for(int i=0;i<ui->listWidget_axis->count();i++)
//      {
//        if(ui->listWidget_axis->item(i)->isSelected())
//        {
//          //关伺服
//          error=(COM_ERROR)GTSD_CMD_SetServoOff(i, mp_mainWindow->getUserConfig()->com.id,mp_mainWindow->getUserConfig()->com.rnStation);
//          if(!ServoControl::checkedCmdReturnValue(error))
//            return ;
//          m_servoStatusList[i].isOn=false;
//          qDebug()<<"servo off :"<<i;
//        }
//      }
//      ui->label_servoon->setText(tr("servo-OFF"));
//    }
    //增加滚轮操作
    qint16 comid=mp_mainWindow->getUserConfig()->com.id;
    qint16 rnStation=mp_mainWindow->getUserConfig()->com.rnStation;
    if(checked)
    {
      COM_ERROR error;
      for(int i=0;i<m_rollBoxUi->rollCount();i++)
      {
        if(m_rollBoxUi->buttonIsChecked(i))
        {
          //打开伺服
          bool on=false;
          error=(COM_ERROR)GTSD_CMD_SetServoOn(i, comid,rnStation);
          if(!ServoControl::checkedCmdReturnValue(error))
            return ;
          m_servoStatusList[i].isOn=on;
          qDebug()<<"servo :axis="<<i<<" on="<<on;
//          setServoOnUi(on);
        }
      }
      GlobalFunction::delayms(200);
      for(int i=0;i<m_rollBoxUi->rollCount();i++)
      {
        if(m_rollBoxUi->buttonIsChecked(i))
        {
          //打开伺服
          bool on=false;
          on=ServoControl::checkServoIsReady(i,(quint16)comid,rnStation);//读是否上伺服
          m_servoStatusList[i].isOn=on;
          qDebug()<<"servo :axis="<<i<<" on="<<on;
          setServoOnUi(on);
        }
      }
    }
    else
    {
      COM_ERROR error;
      for(int i=0;i<m_rollBoxUi->rollCount();i++)
      {
        if(m_rollBoxUi->buttonIsChecked(i))
        {
          //关伺服
          bool on=true;
          error=(COM_ERROR)GTSD_CMD_SetServoOff(i, comid,rnStation);
          if(!ServoControl::checkedCmdReturnValue(error))
            return ;
          m_servoStatusList[i].isOn=on;
          qDebug()<<"servo :axis="<<i<<" on="<<on;
//          setServoOnUi(on);
        }
      }
      GlobalFunction::delayms(200);
      for(int i=0;i<m_rollBoxUi->rollCount();i++)
      {
        if(m_rollBoxUi->buttonIsChecked(i))
        {
          //打开伺服
          bool on=false;
          on=ServoControl::checkServoIsReady(i,(quint16)comid,rnStation);//读是否上伺服
          m_servoStatusList[i].isOn=on;
          qDebug()<<"servo :axis="<<i<<" on="<<on;
          setServoOnUi(on);
        }
      }
    }
  }
  else
  {
    QMessageBox::information(0,tr("Warring"),tr("please open the com first !"));
    ui->btn_servo_switch->setChecked(false);
  }
}

void PlotWave::onBtnOpenClicked(bool checked)
{
  qDebug()<<"checked="<<checked;
  static QVector<bool>isDrawVector(0);
  if(checked)
  {
    QString fileName;
    fileName = QFileDialog::getOpenFileName(this, tr("Open File"), ".", tr("curve Files(*.src)"));
    if (fileName.isNull())
    {
      ui->btn_open->setChecked(false);
      return;
    }

    QFile file;
    file.setFileName(fileName);
    if(false==file.open(QIODevice::ReadOnly))
    {
      QMessageBox::information(0,tr("file error"),tr("can not open file :\n%1").arg(fileName));
      ui->btn_open->setChecked(false);
      return;
    }
    //记录当前的曲线状态,以便关闭时恢复
    isDrawVector.clear();
    foreach (PlotTablePrm prm, m_tablePlotPrmList)
    {
      bool isDraw=prm.isDraw;
      isDrawVector.append(isDraw);
    }
    //隐藏原始所有
    for(int i=0;i<ui->tableWidget->rowCount();i++)
    {
      m_tablePlotPrmList[i].isDraw=false;
      ui->plotCurve->graph(i)->setVisible(false);
      setCurveTableWidgetOnOffUi(i,false);
      ui->tableWidget->hideRow(i);
    }

    QDataStream in(&file);
    quint16 streamVersion;
    quint16 curveCount=0;
    in>>streamVersion>>curveCount;
    qDebug()<<"\nversion:"<<streamVersion<<"count:"<<curveCount;
    in.setVersion(streamVersion);
    for(int i=0;i<curveCount;i++)
    {
      TableParameters pars;
      in>>pars;
      addCurve(pars);
    }
    ui->plotCurve->replot();

    file.close();
  }
  else
  {
    int totalRowCount=ui->tableWidget->rowCount();
    int prevRowCount=isDrawVector.count();
    int addRowCount=totalRowCount-prevRowCount;
    bool isDraw=false;
    for(int i=0;i<addRowCount;i++)
    {
      m_tablePlotPrmList.removeAt(prevRowCount);
      ui->plotCurve->removeGraph(prevRowCount);
      ui->tableWidget->removeRow(prevRowCount);
    }

    //移除之前增加的曲线
    //恢复以前的显示状态

    for(int i=0;i<prevRowCount;i++)
    {
      isDraw=isDrawVector.at(i);
      m_tablePlotPrmList[i].isDraw=isDraw;
      setCurveTableWidgetOnOffUi(i,isDraw);
      ui->plotCurve->graph(i)->setVisible(isDraw);
      ui->tableWidget->showRow(i);
    }
    ui->plotCurve->replot();
  }
  //改变UI显示
  changeUiWhenOpenClicked(!checked);
}

void PlotWave::onBtnSaveClicked()
{
  QString fileName;
  QVector<quint16>curveIndexs;
  QString fileNameDefaultQString =tr("sampling_curve_")+ QDateTime::currentDateTime().toString("yyyyMMdd");//默认文件名
  fileName=QFileDialog::getSaveFileName(this, tr("Save curve"), fileNameDefaultQString, tr("curve file (*.src)"));
  if(fileName.isEmpty())
    return;

  for(int i=0;i<ui->tableWidget->rowCount();i++)
  {
    //    if(ui->tableWidget->item(i,COL_PRM_TABLE_NAME)->isSelected())
    curveIndexs.append(i);//修改为全部保存
  }
  saveCurve(fileName,curveIndexs);
}

void PlotWave::onBtnFFTClicked()
{
  if(m_dftWidget==NULL)
  {
    m_dftWidget=new DftWidget(this);
    m_dftWidget->setComboxContexts(QStringList(tableCurveNameList()));
    connect(this,SIGNAL(measureXLeftChanged(double)),m_dftWidget,SLOT(onFFTStartTimeChanged(double)));
    connect(this,SIGNAL(measureXRightChanged(double)),m_dftWidget,SLOT(onFFTEndTimeChanged(double)));
    connect(m_dftWidget,SIGNAL(dftWidgetClose()),this,SLOT(onDftWidgetClose()));
    connect(m_dftWidget,SIGNAL(saveSourceCurve(QString&,QVector<quint16>&)),this,SLOT(onDftSaveCurve(QString&,QVector<quint16>&)));
  }
  if(m_dftWidget->isHidden())
    m_dftWidget->show();

  //设置范围
  if(m_tablePlotPrmList.count()!=0)
    m_dftWidget->setTimeRange(m_tablePlotPrmList.at(0).data.curveKey.first(),m_tablePlotPrmList.at(0).data.curveKey.last());

  //显示游标
  ui->btn_ver_measure->setChecked(true);
  onBtnVerticalMeasureClicked(true);
  //根据游标位置设置lineEdit数值
  m_dftWidget->onFFTStartTimeChanged(m_cursorX->getMeasureXLeftPosition());
  m_dftWidget->onFFTEndTimeChanged(m_cursorX->getMeasureXRightPosition());
}

void PlotWave::onBtnMeasureXLeftClicked(bool checked)
{
  Q_UNUSED(checked);

  if(m_cursorX->curve1Selected()){
    m_cursorX->setCurve1Checked(false);
    m_cursorX->setCurve2Checked(true);
  }
  else{
    m_cursorX->setCurve1Checked(true);
    m_cursorX->setCurve2Checked(false);
  }
}

void PlotWave::onBtnMeasureXRightClicked(bool checked)
{
  QString inf;
  if(checked)
  {
    inf="checked";
  }
  else
  {
    inf="unchecked";
  }
  qDebug()<<inf;
  m_cursorX->setCurve1Checked(checked);
  m_cursorX->setCurve2Checked(checked);
}

void PlotWave::onBtnMeasureYDownClicked(bool checked)
{
  Q_UNUSED(checked);
  if(m_cursorY->curve1Selected()){
    m_cursorY->setCurve1Checked(false);
    m_cursorY->setCurve2Checked(true);
  }
  else{
    m_cursorY->setCurve1Checked(true);
    m_cursorY->setCurve2Checked(false);
  }
}

void PlotWave::onBtnMeasureYUpClicked(bool checked)
{
  QString inf;
  if(checked)
  {
    inf="checked";
  }
  else
  {
    inf="unchecked";
  }
  qDebug()<<inf;
  m_cursorY->setCurve2Checked(checked);
  m_cursorY->setCurve1Checked(checked);
}

void PlotWave::onViewSettingUpdate(ViewSetting setting)
{
  m_viewSetting.pointNum=setting.pointNum;
  m_viewSetting.storeTime=setting.storeTime;
  m_viewSetting.xtime=setting.xtime;
  m_viewSetting.ymax=setting.ymax;
  m_viewSetting.ymin=setting.ymin;
  updatePlotView(&m_viewSetting);
}

/**
 * @brief 双击增加树来添加曲线
 * @param item
 * @param column
 */
void PlotWave::onTreeWidgetItemDoubleClicked(QTreeWidgetItem *item, int column)
{
  Q_UNUSED(column)
  if(item->childCount()>0) return;//不是叶子则返回
  int isBit;
  QString name;
  QColor color;
  int bytes;
  int axisNum;
  quint16 offsetAddr;
  name=item->text(COL_NAME);

  isBit=item->text(COL_ISBIT).toInt();
  if(isBit==1)//判断是不是位，如果是位就不增加
  {
    QMessageBox::information(0,tr("information"),tr("%1 is BIT").arg(name));
    return;
  }

  bytes=GlobalFunction::getBytesNumber(item->text(COL_TYPE));
  offsetAddr=item->text(COL_ADDRESS).toShort();

  QTreeWidgetItem *itemTop;
  itemTop=GlobalFunction::findTopLevelItem(item);
  qDebug()<<"toplevel:"<<itemTop->text(0);
  axisNum=item->treeWidget()->indexOfTopLevelItem(itemTop);
  qDebug()<<name<<"bytes:"<<bytes<<"axisnum:"<<axisNum<<"offset:"<<offsetAddr;
  int rowCount=ui->tableWidget->rowCount();

  //限制输入>16
//  if(rowCount>=CURVE_NUM_MAX){
//    QMessageBox::information(0,tr("information"),tr("The selected curves is more than 16"));
//    return;
//  }
  int curveNumber=0;//实际下发曲线条数
  if(drawCurveNumberIsBigger(CURVE_NUM_MAX-1,curveNumber))//-1是因为加上它后面增加的曲线去判断
  {
    QMessageBox::information(0,tr("information"),tr("The numbers of selected curves is too much "));
    return;
  }

  quint16 baseAddr=getCurveBaseAddress(name);

#if TEST_DEBUG
  foreach (QString  str, nameList) {
    qDebug()<<str;
  }
  qDebug()<<"baseAddr:"<<baseAddr;
#endif

  //检查是否已经存在
  QString existName;
  int existAxisNum;
  for(int i=0;i<rowCount;i++)
  {
    existName=m_tablePlotPrmList.at(i).fullName;
    existAxisNum=m_tablePlotPrmList.at(i).axisNum;
    if((existAxisNum==axisNum)&&(existName==name))
    {
      QMessageBox::information(0,tr("Parameter"),tr("the curve :%1 has been existed").arg(name));
      return;
    }
  }

  //增加数据 1.list 2.table 3.graph
  color=m_colorDefault.at(rowCount%m_colorDefault.size());
  PlotTablePrm prm;
  prm.curveType=CURVE_TYPE_RAW;
  prm.id=0;
  prm.isDraw=true;
  prm.fullName=name;
  prm.color=color;
  prm.bytes=bytes;
  prm.axisNum=axisNum;
  prm.offsetAddr=offsetAddr;
  prm.baseAddr=baseAddr;
  prm.addValue=0.0;
  prm.multiValue=1.0;
  prm.xoffset=0.0;

  m_tablePlotPrmList.append(prm);//更新数据list
  ui->tableWidget->insertRow(rowCount);//更新到界面的table
  //先断开连接，以免发生不是真正的信号响应
//  disconnect(ui->tableWidget,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(onTableItemChanged(QTableWidgetItem*)));
  setTableRowPrm(rowCount,prm);
//  connect(ui->tableWidget,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(onTableItemChanged(QTableWidgetItem*)));

  ui->plotCurve->addGraph();
  ui->plotCurve->graph(rowCount)->setPen(QPen(color));
  ui->plotCurve->graph(rowCount)->setSelectable(false);
}

void PlotWave::onUsrTableWidgetCellDoubleClicked(int row, QTableWidget *axisTable, QComboBox* unitComboBox)
{
  qDebug()<<"row="<<row<<" table address="<<(quint32)(&axisTable);
  int curveId;
  int axisNum;
  CurveType curveType;
  QTreeWidget *axisCurvePrm;
  QTreeWidgetItem *itemCurve;//指向第row行曲线
  qDebug()<<"axisTable->columnCount() "<<axisTable->columnCount();
  for(int i=0;i<axisTable->columnCount();i++)
  {
    if(axisTable->item(0,i)->isSelected())
    {
      qDebug()<<"select axis: "<<i;
      axisNum=i;
      axisCurvePrm=m_usrCurveTreeManager->usrTreeList().at(i);//第i个轴的树
      itemCurve=axisCurvePrm->topLevelItem(row);//第row行的曲线
      curveId=itemCurve->child(ROW_PRM_INDEX_CURVEID)->text(COL_CURVESETTING_INDEX_VALUE).toInt();
      curveType=(CurveType)(itemCurve->text(COL_CURVESETTING_INDEX_VALUE).toInt());
      //先查m_tablePlotPrmList中有没有这个曲线
      bool hasCurve=false;
//      foreach (PlotTablePrm prm, m_tablePlotPrmList)
//      {
//        if((prm.axisNum==axisNum)&&(prm.id==curveId))
//        {
//          hasCurve=true;
//          qDebug()<<"has curve";
//          break;
//        }
//      }

      //查控制变量是不是每个轴都有的，如果没有就不增加
      bool isTotal=true;
      QTreeWidgetItem *prmFactorItem;
      QTreeWidgetItem *factorItem;
      QTreeWidgetItem *item;
      QString controlName;
      QTreeWidget *axisTree=new QTreeWidget;
      axisTree->addTopLevelItem(mp_treeAll->topLevelItem(axisNum)->clone());

      prmFactorItem=itemCurve->child(ROW_PRM_INDEX_PRMFACTOR);
      int childSize=prmFactorItem->childCount();
      if(curveType==CURVE_TYPE_SCRIPT_CONST)
        childSize=1;
      QVector<bool>hasNameVector(childSize,false);
      for(int i=0;i<childSize;i++)
      {
        factorItem=prmFactorItem->child(i);
        controlName=factorItem->child(ROW_FACTOR_INDEX_FULLNAME)->text(COL_CURVESETTING_INDEX_VALUE);
        qDebug()<<"controlName="<<controlName;
        //遍历该轴的树，看看有没有这个controlName
        QTreeWidgetItemIterator treeItor(axisTree);
        while(*treeItor)
        {
          item=(*treeItor);
          if(item->text(COL_NAME)==controlName)
          {
            hasNameVector[i]=true;
            qDebug()<<"find :"<<controlName;
            break;
          }
          ++treeItor;
        }
      }
      foreach (bool hasName, hasNameVector)
      {
        if(hasName==false)
        {
          isTotal=false;
          break;
        }
      }
      axisTree->clear();
      delete axisTree;

      //条件满足：还没增加，而且是都有的参数 。进行增加操作
      if((hasCurve==false)&&(isTotal==true))
      {
       //是否>16条曲线,>16条就不增加
        int totalNum=0;
        int  rowCount=ui->tableWidget->rowCount();
        if(drawCurveNumberIsBigger(CURVE_NUM_MAX,totalNum)==false)
        {
          //获得填的信息树节点
          int varCount=itemCurve->child(ROW_PRM_INDEX_PRMFACTOR)->text(COL_CURVESETTING_INDEX_VALUE).toInt();
          if((totalNum+varCount)<=CURVE_NUM_MAX)
          {
            //将树的信息填到m_tablePlotPrmList
            QString unitStr;
            double unitFactor;
            unitStr=unitComboBox->currentText();
            unitFactor=unitComboBox->currentData().toDouble();
            PlotTablePrm prm;
            QColor color=m_colorDefault.at(totalNum%m_colorDefault.size());
            prm.id=curveId;
            prm.curveType=(CurveType)itemCurve->text(COL_CURVESETTING_INDEX_VALUE).toInt();
            prm.isDraw=itemCurve->child(ROW_PRM_INDEX_ISDRAW)->text(COL_CURVESETTING_INDEX_VALUE).toInt();
            prm.fullName=itemCurve->child(ROW_PRM_INDEX_FULLNAME)->text(COL_CURVESETTING_INDEX_VALUE)+tr("(%1)").arg(unitStr);
            prm.color=color;
            prm.axisNum=axisNum;
            prm.bytes=itemCurve->child(ROW_PRM_INDEX_BYTESNUM)->text(COL_CURVESETTING_INDEX_VALUE).toInt();
            prm.offsetAddr=itemCurve->child(ROW_PRM_INDEX_OFFSETADDR)->text(COL_CURVESETTING_INDEX_VALUE).toUShort();
            prm.baseAddr=itemCurve->child(ROW_PRM_INDEX_BASEADDR)->text(COL_CURVESETTING_INDEX_VALUE).toUShort();
            prm.addValue=itemCurve->child(ROW_PRM_INDEX_ADDVALUE)->text(COL_CURVESETTING_INDEX_VALUE).toDouble();
//            prm.multiValue=itemCurve->child(ROW_PRM_INDEX_MULTIVALUE)->text(COL_CURVESETTING_INDEX_VALUE).toDouble();
            prm.multiValue=unitFactor;
            prm.xoffset=itemCurve->child(ROW_PRM_INDEX_XOFFSET)->text(COL_CURVESETTING_INDEX_VALUE).toDouble();
            prm.equationDescription=itemCurve->child(ROW_PRM_INDEX_EQUATIONDESCRIPTION)->text(COL_CURVESETTING_INDEX_VALUE);
            prm.varCount=varCount;
            QTreeWidgetItem *prmFactor;
            QTreeWidgetItem *factor;
            prmFactor=itemCurve->child(ROW_PRM_INDEX_PRMFACTOR);
            PlotTablePrmFactor vprmFactor;
            PlotTablePrmFactor cprmFactor;
            vprmFactor.factorTypeId=PLOT_PRM_FACTOR_TYPE_VARIABLE;
            cprmFactor.factorTypeId=PLOT_PRM_FACTOR_TYPE_CONST;
            int factorCount=prmFactor->childCount();
            //varFactor
            for(int j=0;j<varCount;j++)
            {
              factor=prmFactor->child(j);
              vprmFactor.controlPrm.fullName=factor->child(ROW_FACTOR_INDEX_FULLNAME)->text(COL_CURVESETTING_INDEX_VALUE);
              vprmFactor.controlPrm.bytes=factor->child(ROW_FACTOR_INDEX_BYTESNUM)->text(COL_CURVESETTING_INDEX_VALUE).toInt();
              vprmFactor.controlPrm.offsetAddr=factor->child(ROW_FACTOR_INDEX_OFFSETADDR)->text(COL_CURVESETTING_INDEX_VALUE).toUShort();
              vprmFactor.controlPrm.baseAddr=factor->child(ROW_FACTOR_INDEX_BASEADDR)->text(COL_CURVESETTING_INDEX_VALUE).toUShort();
              prm.prmFactorList.append(vprmFactor);
            }
            //constFactor
            for(int j=varCount;j<factorCount;j++)
            {
              factor=prmFactor->child(j);
              cprmFactor.controlPrm.fullName=factor->child(ROW_FACTOR_INDEX_FULLNAME)->text(COL_CURVESETTING_INDEX_VALUE);
              cprmFactor.controlPrm.bytes=factor->child(ROW_FACTOR_INDEX_BYTESNUM)->text(COL_CURVESETTING_INDEX_VALUE).toInt();
              cprmFactor.controlPrm.offsetAddr=factor->child(ROW_FACTOR_INDEX_OFFSETADDR)->text(COL_CURVESETTING_INDEX_VALUE).toUShort();
              cprmFactor.controlPrm.baseAddr=factor->child(ROW_FACTOR_INDEX_BASEADDR)->text(COL_CURVESETTING_INDEX_VALUE).toUShort();
              prm.prmFactorList.append(cprmFactor);
            }
            m_tablePlotPrmList.append(prm);

            ui->tableWidget->insertRow(rowCount);//更新到界面的table
            //先断开连接，以免发生不是真正的信号响应
//            disconnect(ui->tableWidget,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(onTableItemChanged(QTableWidgetItem*)));
            setTableRowPrm(rowCount,prm);
//            connect(ui->tableWidget,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(onTableItemChanged(QTableWidgetItem*)));

            ui->plotCurve->addGraph();
            ui->plotCurve->graph(rowCount)->setPen(QPen(color));
            ui->plotCurve->graph(rowCount)->setSelectable(false);

          }
          else//加上新增的超16条
          {
            QString curName=axisCurvePrm->topLevelItem(row)->child(ROW_PRM_INDEX_FULLNAME)->text(COL_CURVESETTING_INDEX_DESCRIPTION).split(".").last();
            QMessageBox::information(0,tr("information"),tr("When add axis=%1 curve:%2\n The numbers of selected curve is too much").arg(axisNum).arg(curName));
            break;
          }
        }
        else
        {
          QString curName=axisCurvePrm->topLevelItem(row)->child(ROW_PRM_INDEX_FULLNAME)->text(COL_CURVESETTING_INDEX_DESCRIPTION).split(".").last();
          QMessageBox::information(0,tr("information"),tr("When add axis=%1 curve:%2\n The numbers of selected curve is too much").arg(axisNum).arg(curName));
          break;
        }
      }
    }
  }


}

void PlotWave::onDeleteScriptCurveIdAt(int id)
{
  qDebug()<<"row id="<<id;
  qDebug()<<"delete id="<<id+1;

  //1清除数据列表 id:m_tablePlotPrmList
  //2清除曲线
  //3清除表格显示
  //4数据列表中m_tablePlotPrmList id>id的要减1
  QMutableListIterator<PlotTablePrm> prmItor(m_tablePlotPrmList);
  QVector<int>removeRecord;
  int i=0;
  bool hasTheId=false;
  int realId=id+1;
  while(prmItor.hasNext())
  {
    if(prmItor.next().id==realId)
    {
      qDebug()<<"--remove axisNum="<<prmItor.value().axisNum<<" id="<<prmItor.value().id<<" fullName="<<prmItor.value().fullName;
      prmItor.remove();
      removeRecord.append(i);//记录第几条曲线被移除
      hasTheId=true;
      qDebug()<<"table row="<<i;
    }
    i++;
  }
  i=0;
  foreach (int index, removeRecord) {
    qDebug()<<"remove plot index="<<index;
    ui->plotCurve->removeGraph(index-i);
    ui->tableWidget->removeRow(index-i);
    i++;
  }
  if(hasTheId)
  {
    QMutableListIterator<PlotTablePrm> prmItor(m_tablePlotPrmList);
    while(prmItor.hasNext())
    {
      int curId=prmItor.next().id;
      if(curId>realId)
      {
        qDebug()<<"--reset Id axisNum="<<prmItor.value().axisNum<<" id="<<prmItor.value().id<<" fullName="<<prmItor.value().fullName;
        prmItor.value().id=curId-1;
      }
    }
    ui->tableWidget->repaint();
    ui->plotCurve->replot();
    qDebug()<<"after remove current table rowcount="<<ui->tableWidget->rowCount();
    qDebug()<<"after remove current plot count="<<ui->plotCurve->graphCount();
  }
}

void PlotWave::onPlotMouseMoveEvent(QMouseEvent *event)
{
  //-----------------设置鼠标坐标----------
  //当前鼠标位置坐标
  double dx = ui->plotCurve->xAxis->pixelToCoord(event->x());
  double dy = ui->plotCurve->yAxis->pixelToCoord(event->y());
  //当前坐标轴范围
  QCPRange cpX = ui->plotCurve->xAxis->range();		//X坐标轴范围
  QCPRange cpY = ui->plotCurve->yAxis->range();		//Y坐标轴范围
  //不在当前坐标轴范围内
  if(cpX.contains(dx)&&cpY.contains(dy))
  {
    ui->label_mousePosition->setText(tr("XY Coord:  X-> %1 ms, Y-> %2  ")\
                                     .arg(QString::number(dx*1000, 'f', 3))\
                                     .arg(QString::number(dy, 'f', 3)));
  }

  //设置是否框选放大
  if((event->buttons()&Qt::LeftButton)&&(QApplication::keyboardModifiers()==Qt::ControlModifier))
  {
    if(m_boxZoom!=NULL)
    {
      m_boxZoom->setEndPointF(event->x(),event->y());
      m_boxZoom->updateBoxZoomShape();
      m_boxZoom->setMouseIsMoved();
    }
  }

  //测量游标操作
  static QString mx,my;
  bool isMeasure=false;
  if((m_cursorX!=NULL)&&(event->buttons()&Qt::LeftButton))
  {
    if(m_cursorX->updateMeasureCursorPosition(event->x(),event->y()))
    {
      m_cursorX->disablePlotViewMove();
      m_cursorX->setMouseDragFlag();
//      ui->label_measure_xy->setText(tr("Measure Info: %1").arg(m_cursorX->measureInformation()));
      mx=m_cursorX->measureInformation();
      isMeasure=true;
    }
  }
  if((m_cursorY!=NULL)&&(event->buttons()&Qt::LeftButton))
  {
    if(m_cursorY->updateMeasureCursorPosition(event->x(),event->y()))
    {
      m_cursorY->disablePlotViewMove();
      m_cursorY->setMouseDragFlag();
//      ui->label_measure_xy->setText(tr("Measure Info: %1").arg(m_cursorX->measureInformation()));
      my=m_cursorY->measureInformation();
      isMeasure=true;
    }
  }
  if(isMeasure)
  {
    ui->label_measure_xy->setText(tr("Measure Info: %1 %2").arg(mx).arg(my));
  }

}

void PlotWave::onPlotMousePressEvent(QMouseEvent *event)
{
  if((event->button()==Qt::LeftButton)&&(QApplication::keyboardModifiers()==Qt::ControlModifier))
  {

    if(m_boxZoom==NULL)
    {
      qDebug()<<"boxZoom initial ";
      m_boxZoom=new BoxZoom(ui->plotCurve);
      ui->plotCurve->addPlottable(m_boxZoom);
    }
    m_boxZoom->disablePlotViewMove();
    m_boxZoom->setVisible(true);
    m_boxZoom->setStartPointF(event->x(),event->y());
  }
  qDebug()<<"tick step :"<<ui->plotCurve->xAxis->tickStep();

  if((m_tag!=NULL)&&(event->button()==Qt::RightButton))
  {
    if(!ui->plotCurve->selectedPlottables().isEmpty())
    {
      QCPGraph *graph=static_cast<QCPGraph *>(ui->plotCurve->selectedPlottables().at(0));
      m_tag->addMeasureTag(event->x(),event->y(),graph);
    }
  }
  //游标操作
  if(ui->btn_hori_measure->isChecked()||ui->btn_ver_measure->isChecked())
  {
    if(m_cursorX!=NULL)
      m_cursorX->setActivedStateColor(true);
    if(m_cursorY!=NULL)
      m_cursorY->setActivedStateColor(true);
  }
}

void PlotWave::onPlotMouseReleaseEvent(QMouseEvent *event)//注意鼠标释放之后不能再去检测event->buttons()
{
  if((m_boxZoom!=NULL)&&m_boxZoom->isMouseMoved())
  {
    m_boxZoom->enablePlotViewMove();
    m_boxZoom->updatePlotViewSize();
//    ui->plotCurve->removePlottable(m_boxZoom);
//    m_boxZoom=NULL;
    m_boxZoom->setVisible(false);
    m_boxZoom->clearMouseIsMoved();
    qDebug()<<"mouse boxzoom release ";
    if(m_isZoomFirst)
    {
      setCursor(QCursor(Qt::WaitCursor));
      m_isZoomFirst=false;
      drawMorePoints();
      setCursor(QCursor(Qt::ArrowCursor));
    }
    ui->plotCurve->replot();
  }

  //测量游标释放设置
  if(m_cursorX!=NULL&&m_cursorX->isMouseDrag())
  {
    m_cursorX->clearMouseDrag();
    m_cursorX->enablePlotViewMove();
  }
  if(m_cursorY!=NULL&&m_cursorY->isMouseDrag())
  {
    m_cursorY->clearMouseDrag();
    m_cursorY->enablePlotViewMove();
  }
  //游标操作
  if(ui->btn_hori_measure->isChecked()||ui->btn_ver_measure->isChecked())
  {
    if(m_cursorX!=NULL)
      m_cursorX->setActivedStateColor(false);
    if(m_cursorY!=NULL)
      m_cursorY->setActivedStateColor(false);
  }
  if(event->button()==Qt::LeftButton)
  {
    qDebug()<<"leftButton......";
  }
}
void PlotWave::onPlotMouseWheelEvent(QWheelEvent *event)
{
  Q_UNUSED(event);
  if(m_cursorX!=NULL)
    QTimer::singleShot(100,m_cursorX,SLOT(fillAllMeasureCursor()));
  if(m_cursorY!=NULL)
    QTimer::singleShot(100,m_cursorY,SLOT(fillAllMeasureCursor()));
}

void PlotWave::onPlotStyleChanged(int index)
{
  QPen pen;
  QColor labelcolor;
  switch(index)
  {
  case 0:
    pen.setColor(QColor(Qt::black));
    labelcolor=QColor(Qt::black);
    ui->plotCurve->setBackground(QBrush(QColor(211,211,211)));
    break;
  case 1:
    pen.setColor(QColor(Qt::black));
    labelcolor=QColor(Qt::black);
    ui->plotCurve->setBackground(QBrush(QColor(Qt::white)));
    break;
  case 2:
    pen.setColor(QColor(Qt::white));
    labelcolor=QColor(Qt::white);
    ui->plotCurve->setBackground(QBrush(QColor(Qt::black)));
    break;
  case 3:
    pen.setColor(QColor(Qt::black));
    labelcolor=QColor(Qt::black);
    ui->plotCurve->setBackground(QBrush(QColor(Qt::gray)));
    break;
  case 4:
    pen.setColor(QColor(Qt::white));
    labelcolor=QColor(Qt::white);
    ui->plotCurve->setBackground(QBrush(QColor(6,61,29)));
    break;
  case 5:
    pen.setColor(QColor(Qt::black));
    labelcolor=QColor(Qt::black);
    ui->plotCurve->setBackground(QBrush(QColor(240, 240, 240)));
    break;
  case 6:
    pen.setColor(QColor(Qt::white));
    labelcolor=QColor(Qt::white);
    ui->plotCurve->setBackground(QBrush(QColor(101, 101, 101)));
    break;
  }
  m_viewSetting.curViewIndex=index;
  ui->plotCurve->xAxis->setBasePen(pen);
  ui->plotCurve->yAxis->setBasePen(pen);
  ui->plotCurve->xAxis->setTickPen(pen);
  ui->plotCurve->yAxis->setTickPen(pen);
  ui->plotCurve->xAxis->setSubTickPen(pen);
  ui->plotCurve->yAxis->setSubTickPen(pen);
  ui->plotCurve->xAxis->setLabelColor(labelcolor);
  ui->plotCurve->yAxis->setLabelColor(labelcolor);
  ui->plotCurve->xAxis->setTickLabelColor(labelcolor);
  ui->plotCurve->yAxis->setTickLabelColor(labelcolor);
  ui->plotCurve->replot();
  qDebug()<<"plot style";
}
void PlotWave::onSendPlotData(int dspNumber, int axisFirstCount, long getDataLength, CurveData data)
{
//  qDebug()<<"get data";
  double sampletime = ui->lineEdit_sampling->text().toDouble()*0.001;
  quint8 storeTimeS;
  quint16 disInterval;
  double disLength;
  quint16 displayPointNum;
  quint32 storePointCount;
  quint32 realPointCount;

  double jsValueTemp;

  storeTimeS = m_viewSetting.storeTime;//存储的时间长度
  disLength = (double)m_viewSetting.xtime;//显示长度
  displayPointNum = m_viewSetting.pointNum;//显示的点数
  storePointCount = storeTimeS / sampletime;//存储的点数
  disInterval = (disLength / sampletime) / displayPointNum;//取数间隔

  if (disInterval==0)
    disInterval = 1;

  int vectorRow_first=0;
  int vectorRow_second=axisFirstCount;//second：从数组的哪一个开始，也就是从第一条曲线的总数之后
  for(int row=0;row<m_tablePlotPrmList.count();row++)//增加dsp(a,b)的第一条曲线
  {
    //0，2，4轴
    if(dspNumber==m_tablePlotPrmList.at(row).axisNum)
    {
      CurveType curveType=m_tablePlotPrmList.at(row).curveType;
      if(curveType==CURVE_TYPE_RAW)
      {
        for(int i=0;i<getDataLength;i++)
        {
          m_tablePlotPrmList[row].data.curveKey.append(data.curveKey.at(getDataLength*vectorRow_first + i));
          m_tablePlotPrmList[row].data.curveValue.append(data.curveValue.at(getDataLength*vectorRow_first + i));
          if (0 == i%disInterval)
          ui->plotCurve->graph(row)->addData(data.curveKey.at(getDataLength*vectorRow_first + i), data.curveValue.at(getDataLength*vectorRow_first + i));
        }
        vectorRow_first++;
      }
      else if(curveType==CURVE_TYPE_SCRIPT_CONST)
      {
        for(int i=0;i<getDataLength;i++)
        {
//          jsValueTemp=data.curveValue.at(getDataLength*vectorRow_first + i)*m_tablePlotPrmList[row].multiValue+m_tablePlotPrmList[row].addValue;//暂不加偏移
          jsValueTemp=data.curveValue.at(getDataLength*vectorRow_first + i)*m_tablePlotPrmList[row].multiValue;
          m_tablePlotPrmList[row].data.curveKey.append(data.curveKey.at(getDataLength*vectorRow_first + i));
          m_tablePlotPrmList[row].data.curveValue.append(jsValueTemp);
          if (0 == i%disInterval)
            ui->plotCurve->graph(row)->addData(data.curveKey.at(getDataLength*vectorRow_first + i), jsValueTemp);
        }
        vectorRow_first++;
      }
      else if(curveType==CURVE_TYPE_SCRIPT)
      {
        //构建数学解析器
        int varCount=m_tablePlotPrmList.at(row).varCount;
        int totalCount=m_tablePlotPrmList.at(row).prmFactorList.count();
        double varArray[CURVE_NUM_MAX];
        double *varPtr;
        varPtr=varArray;
        mu::Parser mParser;
        for(int var=0;var<varCount;var++)
        {
          mParser.DefineVar(tr("V%1").arg(var).toStdWString(),varPtr++);
        }
        for(int c=0;c<totalCount-varCount;c++)
        {
          mParser.DefineConst(tr("C%1").arg(c).toStdWString(),m_tablePlotPrmList.at(row).prmFactorList.at(c+varCount).constValue);
        }
        mParser.SetExpr(m_tablePlotPrmList.at(row).equationDescription.toStdWString());

        //开始读点，保留数据，解析，增加画图
        for(int i=0;i<getDataLength;i++)
        {
          //保存原始数据到动态变量中
          for(int var=0;var<varCount;var++)
          {
            m_tablePlotPrmList[row].prmFactorList[var].varDatas.curveValue.append(data.curveValue.at(getDataLength*(vectorRow_first+var) + i));
          }

          m_tablePlotPrmList[row].data.curveKey.append(data.curveKey.at(getDataLength*vectorRow_first + i));
          m_tablePlotPrmList[row].data.curveValue.append(0);
          if (0 == i%disInterval)
          {
            //改变解析变量的值
            for(int var=0;var<varCount;var++)
            {
              varArray[var]=data.curveValue.at(getDataLength*(vectorRow_first+var) + i);
            }
            //开始解析
            try{
              jsValueTemp=mParser.Eval();
              jsValueTemp*=m_tablePlotPrmList[row].multiValue;
            }
            catch (mu::Parser::exception_type &e)
            {
              qDebug()<<"ParserError: "<<QString::fromStdWString(e.GetMsg());
            }
            ui->plotCurve->graph(row)->addData(data.curveKey.at(getDataLength*vectorRow_first + i), jsValueTemp);
          }
        }

        vectorRow_first+=varCount;
      }

      ui->plotCurve->graph(row)->removeDataBefore(ui->plotCurve->graph(row)->data()->last().key - storeTimeS);
      realPointCount=m_tablePlotPrmList[row].data.curveKey.size();
      int a=realPointCount-storePointCount;
      //移除多余的点
      if(a>0)
      {
        m_tablePlotPrmList[row].data.curveKey.remove(0,a);
        m_tablePlotPrmList[row].data.curveValue.remove(0,a);
        if(curveType==CURVE_TYPE_SCRIPT)
        {
          int varCount=m_tablePlotPrmList[row].varCount;
          for(int var=0;var<varCount;var++)
          {
            m_tablePlotPrmList[row].prmFactorList[var].varDatas.curveValue.remove(0,a);
          }
        }
      }
    }
    //1,3,5轴
    else if((dspNumber+1)==m_tablePlotPrmList.at(row).axisNum)
    {
      CurveType curveType=m_tablePlotPrmList.at(row).curveType;
      if(curveType==CURVE_TYPE_RAW)
      {
        for(int i=0;i<getDataLength;i++)
        {
          m_tablePlotPrmList[row].data.curveKey.append(data.curveKey.at(getDataLength*vectorRow_second + i));
          m_tablePlotPrmList[row].data.curveValue.append(data.curveValue.at(getDataLength*vectorRow_second + i));
          if (0 == i%disInterval)
          ui->plotCurve->graph(row)->addData(data.curveKey.at(getDataLength*vectorRow_second + i), data.curveValue.at(getDataLength*vectorRow_second + i));
        }
        vectorRow_second++;
      }
      else if(curveType==CURVE_TYPE_SCRIPT_CONST)
      {
        for(int i=0;i<getDataLength;i++)
        {
//          jsValueTemp=data.curveValue.at(getDataLength*vectorRow_second + i)*m_tablePlotPrmList[row].multiValue+m_tablePlotPrmList[row].addValue;//暂不加偏移
          jsValueTemp=data.curveValue.at(getDataLength*vectorRow_second + i)*m_tablePlotPrmList[row].multiValue;
          m_tablePlotPrmList[row].data.curveKey.append(data.curveKey.at(getDataLength*vectorRow_second + i));
          m_tablePlotPrmList[row].data.curveValue.append(jsValueTemp);
          if (0 == i%disInterval)
            ui->plotCurve->graph(row)->addData(data.curveKey.at(getDataLength*vectorRow_second + i), jsValueTemp);
        }
        vectorRow_second++;
      }
      else if(curveType==CURVE_TYPE_SCRIPT)
      {
//        qDebug()<<"CURVE_TYPE_SCRIPT";
        //构建数学解析器
        int varCount=m_tablePlotPrmList.at(row).varCount;
        int totalCount=m_tablePlotPrmList.at(row).prmFactorList.count();
        double varArray[CURVE_NUM_MAX];
        double *varPtr;
        varPtr=varArray;
        mu::Parser mParser;
        for(int var=0;var<varCount;var++)
        {
          mParser.DefineVar(tr("V%1").arg(var).toStdWString(),varPtr++);
        }
        for(int c=0;c<totalCount-varCount;c++)
        {
          mParser.DefineConst(tr("C%1").arg(c).toStdWString(),m_tablePlotPrmList.at(row).prmFactorList.at(c+varCount).constValue);
        }
        mParser.SetExpr(m_tablePlotPrmList.at(row).equationDescription.toStdWString());

        for(int i=0;i<getDataLength;i++)
        {
          for(int var=0;var<varCount;var++)
          {
            m_tablePlotPrmList[row].prmFactorList[var].varDatas.curveValue.append(data.curveValue.at(getDataLength*(vectorRow_second+var) + i));
          }

          m_tablePlotPrmList[row].data.curveKey.append(data.curveKey.at(getDataLength*vectorRow_second + i));
          m_tablePlotPrmList[row].data.curveValue.append(0);
          if (0 == i%disInterval)
          {
            for(int var=0;var<varCount;var++)
            {
              varArray[var]=data.curveValue.at(getDataLength*(vectorRow_second+var) + i);
            }
            try
            {
  //            jsValueTemp=m_tablePlotPrmList[row].mathParser.Eval();
              jsValueTemp=mParser.Eval();
              jsValueTemp*=m_tablePlotPrmList[row].multiValue;
            }
            catch (mu::Parser::exception_type &e)
            {
              qDebug()<<"ParserError: "<<QString::fromStdWString(e.GetMsg());
            }
            ui->plotCurve->graph(row)->addData(data.curveKey.at(getDataLength*vectorRow_second + i), jsValueTemp);
          }
        }

        vectorRow_second+=varCount;
      }

      ui->plotCurve->graph(row)->removeDataBefore(ui->plotCurve->graph(row)->data()->last().key - storeTimeS);
      realPointCount=m_tablePlotPrmList[row].data.curveKey.size();
      int a=realPointCount-storePointCount;
      if(a>0)
      {
        if(curveType==CURVE_TYPE_SCRIPT)
        {
          int varCount=m_tablePlotPrmList[row].varCount;
          for(int var=0;var<varCount;var++)
          {
            m_tablePlotPrmList[row].prmFactorList[var].varDatas.curveValue.remove(0,a);
          }
        }
        m_tablePlotPrmList[row].data.curveKey.remove(0,a);
        m_tablePlotPrmList[row].data.curveValue.remove(0,a);
      }
    }
  }

  if (ui->btn_auto->isChecked())
    ui->plotCurve->rescaleAxes(true);
  ui->plotCurve->xAxis->setRange(data.curveKey.last() + sampletime, disLength, Qt::AlignRight);
  ui->plotCurve->replot();
}

void PlotWave::onSamplingStopEvaluateScript()
{
  evaluateCurveSamplingScript();
}

void PlotWave::recoverStackWidgetUiData(int index, int axis)
{
  switch (index) {
  case 0:

    break;
  case 1:

    break;
  case 2://电机初始相位校正
    ui->dSpinBox_posadjust->setValue(m_servoStatusList.at(axis).m_posAdj);
    ui->dSpinBox_posadjust->setStyleSheet("color:black");
    break;
  case 3:

    break;
  case 4:
    ui->dSpinBox_ua->setValue(m_servoStatusList.at(axis).m_ua);
    ui->dSpinBox_ub->setValue(m_servoStatusList.at(axis).m_ub);
    ui->dSpinBox_uc->setValue(m_servoStatusList.at(axis).m_uc);
    ui->dSpinBox_ud->setValue(m_servoStatusList.at(axis).m_ud);
    ui->dSpinBox_uq->setValue(m_servoStatusList.at(axis).m_uq);
    ui->dSpinBox_ua->setStyleSheet("color:black");
    ui->dSpinBox_ub->setStyleSheet("color:black");
    ui->dSpinBox_uc->setStyleSheet("color:black");
    ui->dSpinBox_ud->setStyleSheet("color:black");
    ui->dSpinBox_uq->setStyleSheet("color:black");
    break;
  case 5:
    ui->dSpinBox_id->setValue(m_servoStatusList.at(axis).m_id);
    ui->dSpinBox_iq->setValue(m_servoStatusList.at(axis).m_iq);
    ui->dSpinBox_id->setStyleSheet("color:black");
    ui->dSpinBox_iq->setStyleSheet("color:black");
    break;
  case 6:
    ui->dSpinBox_closeVel->setValue(m_servoStatusList.at(axis).m_closeVel);
    ui->dSpinBox_closeVel->setStyleSheet("color:black");
    break;
  case 7:
    ui->dSpinBox_profileVel->setValue(m_servoStatusList.at(axis).m_profileVel);
    ui->dSpinBox_profileVel->setStyleSheet("color:black");
    break;
  case 8:
    ui->dSpinBox_syncVel->setValue(m_servoStatusList.at(axis).m_syncVel);
    ui->dSpinBox_syncVel->setStyleSheet("color:black");
    break;
  case 9:

    break;
  case 10:
    ui->dSpinBox_posfollow->setValue(m_servoStatusList.at(axis).m_posFollow);
    ui->dSpinBox_posfollow->setStyleSheet("color:black");
    break;
  case 11:

    break;

  default:
    break;
  }
}

void PlotWave::onListWidgetModeIndexChanged(int index)
{
  if(mp_mainWindow->getComOpenState()==false)
  {
//    QMessageBox::information(0,tr("Warring"),tr("please open the com first !"));
    return;
  }
  COM_ERROR error;
  int lastAxis=0;
  for(int i=0;i<ui->listWidget_axis->count();i++)
  {
    if(ui->listWidget_axis->item(i)->isSelected())//选择了哪个轴
    {
      qDebug()<<"select axis:"<<i;
      //记录当前轴的选择的index
      m_servoStatusList[i].currentModeIndex=index;
      //向伺服发送命令
      error=(COM_ERROR)GTSD_CMD_SetServoTaskMode(i,index,mp_mainWindow->getUserConfig()->com.id,mp_mainWindow->getUserConfig()->com.rnStation);
      if(!ServoControl::checkedCmdReturnValue(error))
        break;
      qDebug()<<"set mode:"<<index<<" ret error: "<<error;
      lastAxis=i;
    }
  }
  ui->stackedWidget->setCurrentIndex(index);
  //恢复这个界面上的数据
  recoverStackWidgetUiData(index, lastAxis);
}
/**
 * @brief 当输入值改变时，使其字体变红
 * @param value
 */
void PlotWave::onDoubleSpinBoxValueChanged(double value)
{
  Q_UNUSED(value);
  QDoubleSpinBox *box=static_cast<QDoubleSpinBox *>(sender());
  box->setStyleSheet("color:red");
  qDebug()<<"value change......";
}

void PlotWave::onListWidgetAxisClicked(int index)
{
  //  Q_UNUSED(index);
  //根据轴号index,还原当前的ui状态:伺服模式 伺服开关及显示
  //先关掉连接，以免改变下触发SLOT函数

  //listView
  disconnect(ui->listWidget_mode,SIGNAL(currentRowChanged(int)),this,SLOT(onListWidgetModeIndexChanged(int)));
  ui->listWidget_mode->setCurrentRow(m_servoStatusList[index].currentModeIndex);
  ui->stackedWidget->setCurrentIndex(m_servoStatusList[index].currentModeIndex);
  ui->btn_servo_switch->setChecked(m_servoStatusList[index].isOn);
  if(m_servoStatusList[index].isOn)
    ui->label_servoon->setText(tr("servo_ON"));
  else
    ui->label_servoon->setText(tr("servo_OFF"));
  connect(ui->listWidget_mode,SIGNAL(currentRowChanged(int)),this,SLOT(onListWidgetModeIndexChanged(int)));
}

void PlotWave::onTest1(double value)
{
  qDebug()<<"left measure value:"<<value;
}

void PlotWave::onTest2(double value)
{
  qDebug()<<"right measure value:"<<value;
}
void PlotWave::onDftWidgetClose()
{
  ui->btn_ver_measure->setChecked(false);
  onBtnVerticalMeasureClicked(false);
}

void PlotWave::onDftSaveCurve(QString &fileName, QVector<quint16> &curveIndexVector)
{
  saveCurve(fileName,curveIndexVector);
}

void PlotWave::onShowCurrentModeInfo(int axis, int index)
{
  Q_UNUSED(axis);
//  qDebug()<<"index="<<index;
  QString str=m_servoModeNameList.at(index);
  ui->label_showMode->setText(str);
}
void PlotWave::setServoOnUi(bool on)
{
  ui->btn_servo_switch->setChecked(on);
  if(on)
    ui->label_servoon->setText(tr("servo_ON"));
  else
    ui->label_servoon->setText(tr("servo_OFF"));
}

void PlotWave::onRollBoxBtnClicked(int axis)
{
  qDebug()<<"clicked axis="<<axis;
  int index=m_servoStatusList.at(axis).currentModeIndex;
  m_rollBoxUi->setCurrentIndexAt(index,axis);
  ui->stackedWidget->setCurrentIndex(index);
  recoverStackWidgetUiData(index,axis);
  ui->label_showMode->setText(m_servoModeNameList.at(index));
  setServoOnUi(m_servoStatusList.at(axis).isOn);
}
void PlotWave::onRollBoxStopedAt(int axis, int index)
{
  //下放模式指令
  if(mp_mainWindow->getComOpenState()==false)
  {
    return;
  }
  COM_ERROR error;
  error=(COM_ERROR)GTSD_CMD_SetServoTaskMode(axis,index,mp_mainWindow->getUserConfig()->com.id,mp_mainWindow->getUserConfig()->com.rnStation);
  if(!ServoControl::checkedCmdReturnValue(error))
    error=(COM_ERROR)GTSD_CMD_SetServoTaskMode(axis,index,mp_mainWindow->getUserConfig()->com.id,mp_mainWindow->getUserConfig()->com.rnStation);

  //存储当前index
  m_servoStatusList[axis].currentModeIndex=index;
  //恢复UI
  recoverStackWidgetUiData(index, axis);
  ui->stackedWidget->setCurrentIndex(index);
  ui->label_showMode->setText(m_servoModeNameList.at(index));
  setServoOnUi(m_servoStatusList.at(axis).isOn);
}

void PlotWave::onTabWidgetCurrentChanged(int index)
{
  if(mp_mainWindow->getComOpenState()==false)
    return;
  qDebug()<<"index="<<index;
  TabWidgetIndex tabIndex=(TabWidgetIndex)(index);
  quint16 comid=mp_userConfig->com.id;
  qint16 rnStation=mp_userConfig->com.rnStation;
  MotionStatus::ControlSource ctlSrcIndex;

  switch (tabIndex)
  {
  case TABWIDGET_INDEX_MODE:
    for(int i=0;i<m_servoStatusList.count();i++)
    {
      m_servoStatusList[i].currentModeIndex=ServoControl::servoCurrentUserTaskMode(i,(quint16)comid,rnStation);
      m_servoStatusList[i].isOn=ServoControl::checkServoIsReady(i,(quint16)comid,rnStation);//读是否上伺服
      m_rollBoxUi->setCurrentIndexAt(m_servoStatusList.at(i).currentModeIndex,i);
      if(m_rollBoxUi->buttonIsChecked(i))
      {
        ui->stackedWidget->setCurrentIndex(m_servoStatusList.at(i).currentModeIndex);
        ui->btn_servo_switch->setEnabled(true);
        setServoOnUi(m_servoStatusList.at(i).isOn);
      }
    }
    break;
  case TABWIDGET_INDEX_MOTION:
    //------------------读当前的运动控制源-------------------------------
    for(int i=0;i<m_motionStatusPtrList.count();i++)
    {
      m_motionStatusPtrList[i]->setMotionMode(MotionStatus::MOTION_MODE_NONE);
    }
    ui->listWidget_axis_tab2->setCurrentRow(0);
    ui->listWidget_motionSrc_tab2->setCurrentRow(0);
    ui->btn_motionRun->setEnabled(false);

    ctlSrcIndex=m_motionStatusPtrList[0]->controlSrc();
    setControlSourceIndexUi(ctlSrcIndex);
    break;
  default:
    break;
  }
}

void PlotWave::onTablePopuMenuClicked()
{
  QAction *act=static_cast<QAction *>(sender());
  QString actStr=act->data().toString();
  qDebug()<<"act string="<<actStr;
  if(actStr=="copy")
  {
    qDebug()<<"copy";
    int curveNumber;
    QList<PlotTablePrm>prmList;
    for(int row=0;row<ui->tableWidget->rowCount();row++)
    {
      if(ui->tableWidget->item(row,COL_PRM_TABLE_AXISNUM)->isSelected())
      {
        //copy
        PlotTablePrm prm;
        PlotTablePrmFactor prmFactor;
        prm.curveType=m_tablePlotPrmList.at(row).curveType;
        prm.varCount=m_tablePlotPrmList.at(row).varCount;
        if(drawCurveNumberIsBigger(CURVE_NUM_MAX,curveNumber))
        {
          qDebug()<<"too much curve";
          continue;
        }
        if(prm.curveType==CURVE_TYPE_SCRIPT)
          curveNumber+=prm.varCount;
        else
          curveNumber+=1;
        if(curveNumber>CURVE_NUM_MAX)
        {
          qDebug()<<"after add  too much curve";
          continue;
        }
        qDebug()<<"add curve: "<<m_tablePlotPrmList.at(row).fullName;
        prm.id=m_tablePlotPrmList.at(row).id;
        prm.isDraw=m_tablePlotPrmList.at(row).isDraw;
        prm.fullName=m_tablePlotPrmList.at(row).fullName;
        prm.color=m_colorDefault.at((ui->tableWidget->rowCount()+row)%m_colorDefault.size());
        prm.axisNum=m_tablePlotPrmList.at(row).axisNum;
        prm.bytes=m_tablePlotPrmList.at(row).bytes;
        prm.offsetAddr=m_tablePlotPrmList.at(row).offsetAddr;
        prm.baseAddr=m_tablePlotPrmList.at(row).baseAddr;
        prm.addValue=m_tablePlotPrmList.at(row).addValue;
        prm.multiValue=m_tablePlotPrmList.at(row).multiValue;
        prm.xoffset=m_tablePlotPrmList.at(row).xoffset;
        prm.equationDescription=m_tablePlotPrmList.at(row).equationDescription;

        for(int i=0;i<m_tablePlotPrmList.at(row).prmFactorList.count();i++)
        {
          prmFactor.factorTypeId=m_tablePlotPrmList.at(row).prmFactorList.at(i).factorTypeId;
          prmFactor.constValue=m_tablePlotPrmList.at(row).prmFactorList.at(i).constValue;
          prmFactor.controlPrm.baseAddr=m_tablePlotPrmList.at(row).prmFactorList.at(i).controlPrm.baseAddr;
          prmFactor.controlPrm.bytes=m_tablePlotPrmList.at(row).prmFactorList.at(i).controlPrm.bytes;
          prmFactor.controlPrm.fullName=m_tablePlotPrmList.at(row).prmFactorList.at(i).controlPrm.fullName;
          prmFactor.controlPrm.offsetAddr=m_tablePlotPrmList.at(row).prmFactorList.at(i).controlPrm.offsetAddr;
          prm.prmFactorList.append(prmFactor);
        }
        m_tablePlotPrmList.append(prm);
        prmList.append(prm);
      }
    }

    for(int i=0;i<prmList.count();i++)
    {
      int rowCount=ui->tableWidget->rowCount();
      QColor color;
      color=prmList.at(i).color;
      ui->tableWidget->insertRow(rowCount);//更新到界面的table
//      disconnect(ui->tableWidget,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(onTableItemChanged(QTableWidgetItem*)));
      setTableRowPrm(rowCount,prmList.at(i));
//      connect(ui->tableWidget,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(onTableItemChanged(QTableWidgetItem*)));

      ui->plotCurve->addGraph();
      ui->plotCurve->graph(rowCount)->setPen(QPen(color));
      ui->plotCurve->graph(rowCount)->setSelectable(false);
      ui->plotCurve->graph(rowCount)->setVisible(prmList.at(i).isDraw);
    }

  }
  else
  {
    quint16 axis=act->data().toUInt();
    qDebug()<<"change to axis:"<<axis;
    for(int i=0;i<ui->tableWidget->rowCount();i++)
    {
      if(ui->tableWidget->item(i,COL_PRM_TABLE_AXISNUM)->isSelected())
        updateOffsetAddrWhenAxisNumberChanged(axis,i);
    }
  }

}
//----------------protected function---------------
bool PlotWave::eventFilter(QObject *obj, QEvent *event)
{
  if(mp_mainWindow->getComOpenState()==false)
  {
    return QWidget::eventFilter(obj,event);
  }

  if (event->type()==QEvent::KeyPress)
  {
    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
    if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)
    {
      //--------tab1-----------------------------------------------
      if (obj==ui->dSpinBox_posadjust)
      {
        double value;
        value = ui->dSpinBox_posadjust->value();
        COM_ERROR error;
        for(int i=0;i<m_rollBoxUi->rollCount();i++)
        {
//          if(ui->listWidget_axis->item(i)->isSelected())
          if(m_rollBoxUi->buttonIsChecked(i))
          {
            error=(COM_ERROR)GTSD_CMD_SetPosAdjRef(i, value, mp_mainWindow->getUserConfig()->com.id,mp_mainWindow->getUserConfig()->com.rnStation);
            m_servoStatusList[i].m_posAdj=value;
            qDebug() << "--------------- posADJ ------------"<<i;
          }
        }
        ui->dSpinBox_posadjust->setStyleSheet("color:black");

      }
      else if (obj == ui->dSpinBox_ua || obj == ui->dSpinBox_ub\
          || obj == ui->dSpinBox_uc || obj == ui->dSpinBox_ud\
          ||obj == ui->dSpinBox_uq)
      {
        double ua, ub, uc, ud, uq;
        ua = ui->dSpinBox_ua->value();
        ub = ui->dSpinBox_ub->value();
        uc = ui->dSpinBox_uc->value();
        ud = ui->dSpinBox_ud->value();
        uq = ui->dSpinBox_uq->value();

        COM_ERROR error;
        for(int i=0;i<m_rollBoxUi->rollCount();i++)
        {
          if(m_rollBoxUi->buttonIsChecked(i))
          {
            error=(COM_ERROR)GTSD_CMD_SetUaRef(i, ua, mp_mainWindow->getUserConfig()->com.id,mp_mainWindow->getUserConfig()->com.rnStation);
            error=(COM_ERROR)GTSD_CMD_SetUbRef(i, ub, mp_mainWindow->getUserConfig()->com.id,mp_mainWindow->getUserConfig()->com.rnStation);
            error=(COM_ERROR)GTSD_CMD_SetUcRef(i, uc, mp_mainWindow->getUserConfig()->com.id,mp_mainWindow->getUserConfig()->com.rnStation);
            error=(COM_ERROR)GTSD_CMD_SetUdRef(i, ud, mp_mainWindow->getUserConfig()->com.id,mp_mainWindow->getUserConfig()->com.rnStation);
            error=(COM_ERROR)GTSD_CMD_SetUqRef(i, uq, mp_mainWindow->getUserConfig()->com.id,mp_mainWindow->getUserConfig()->com.rnStation);
            qDebug() << "--------------- ua ub uc ud uq ------------"<<i<<ui->dSpinBox_ud->objectName();
            m_servoStatusList[i].m_ua=ua;
            m_servoStatusList[i].m_ub=ub;
            m_servoStatusList[i].m_uc=uc;
            m_servoStatusList[i].m_ud=ud;
            m_servoStatusList[i].m_uq=uq;
          }
        }
        ui->dSpinBox_ua->setStyleSheet("color:black");
        ui->dSpinBox_ub->setStyleSheet("color:black");
        ui->dSpinBox_uc->setStyleSheet("color:black");
        ui->dSpinBox_ud->setStyleSheet("color:black");
        ui->dSpinBox_uq->setStyleSheet("color:black");
      }
      else if (obj == ui->dSpinBox_id || obj == ui->dSpinBox_iq)
      {
        double id,iq;
        id = ui->dSpinBox_id->value();
        iq = ui->dSpinBox_iq->value();
        COM_ERROR error;
        for(int i=0;i<m_rollBoxUi->rollCount();i++)
        {
          if(m_rollBoxUi->buttonIsChecked(i))
          {
            error=(COM_ERROR)GTSD_CMD_SetIdRef(i, id, mp_mainWindow->getUserConfig()->com.id,mp_mainWindow->getUserConfig()->com.rnStation);
            error=(COM_ERROR)GTSD_CMD_SetIqRef(i, iq, mp_mainWindow->getUserConfig()->com.id,mp_mainWindow->getUserConfig()->com.rnStation);

            qDebug() << "--------------- id iq------------"<<i;
            m_servoStatusList[i].m_id=id;
            m_servoStatusList[i].m_iq=iq;
          }
        }
        ui->dSpinBox_id->setStyleSheet("color:black");
        ui->dSpinBox_iq->setStyleSheet("color:black");
      }
      else if (obj == ui->dSpinBox_closeVel )
      {
        double velocity;
        velocity = ui->dSpinBox_closeVel->value();

        COM_ERROR error;
        for(int i=0;i<m_rollBoxUi->rollCount();i++)
        {
          if(m_rollBoxUi->buttonIsChecked(i))
          {
            error=(COM_ERROR)GTSD_CMD_SetSpdRef(i, velocity, mp_mainWindow->getUserConfig()->com.id,mp_mainWindow->getUserConfig()->com.rnStation);
            m_servoStatusList[i].m_closeVel=velocity;
            qDebug() << "--------------- velocity close loop------------"<<i;
          }
        }
        ui->dSpinBox_closeVel->setStyleSheet("color:black");
      }
      else if (obj == ui->dSpinBox_profileVel)
      {
        double velocity;
        velocity = ui->dSpinBox_profileVel->value();

        COM_ERROR error;
        for(int i=0;i<m_rollBoxUi->rollCount();i++)
        {
          if(m_rollBoxUi->buttonIsChecked(i))
          {
            error=(COM_ERROR)GTSD_CMD_SetSpdRef(i, velocity, mp_mainWindow->getUserConfig()->com.id,mp_mainWindow->getUserConfig()->com.rnStation);
            m_servoStatusList[i].m_profileVel=velocity;
            qDebug() << "---------------open velocity  loop2------------"<<i;
          }
        }
        ui->dSpinBox_profileVel->setStyleSheet("color:black");
      }
      else if (obj == ui->dSpinBox_syncVel)
      {
        double velocity;
        velocity = ui->dSpinBox_syncVel->value();

        COM_ERROR error;
        for(int i=0;i<m_rollBoxUi->rollCount();i++)
        {
          if(m_rollBoxUi->buttonIsChecked(i))
          {
            error=(COM_ERROR)GTSD_CMD_SetSpdRef(i, velocity, mp_mainWindow->getUserConfig()->com.id,mp_mainWindow->getUserConfig()->com.rnStation);
            m_servoStatusList[i].m_syncVel=velocity;
            qDebug() << "---------------close velocity  loop2------------"<<i;
          }
        }
        ui->dSpinBox_syncVel->setStyleSheet("color:black");
      }
      else if (obj == ui->dSpinBox_posfollow)
      {
        double pos;
        pos = ui->dSpinBox_posfollow->value();

        COM_ERROR error;
        for(int i=0;i<m_rollBoxUi->rollCount();i++)
        {
          if(m_rollBoxUi->buttonIsChecked(i))
          {
            error=(COM_ERROR)GTSD_CMD_SetPosRef(i, pos, mp_mainWindow->getUserConfig()->com.id,mp_mainWindow->getUserConfig()->com.rnStation);
            m_servoStatusList[i].m_posFollow=pos;
            qDebug() << "---------------position follow------------"<<i;
          }
        }
        ui->dSpinBox_posfollow->setStyleSheet("color:black");
      }

      //--------------------tab2-----------------
      else if(obj==ui->dSBox_ampCircle)
      {
        double value;
        value = ui->dSBox_ampCircle->value();

        for(int i=0;i<ui->listWidget_axis_tab2->count();i++)
        {
          if(ui->listWidget_axis_tab2->item(i)->isSelected())
          {
            m_motionStatusPtrList[i]->setAMPCircleValue(value);
            qDebug() << "---------------axis: "<<i<<" setAMPCircleValue:"<<value;
            qDebug()<<"----------------velMode:"<<m_motionStatusPtrList[i]->currentPlanVelMode();
            qDebug()<<"----------------motionMode:"<<m_motionStatusPtrList[i]->motionMode();
          }
        }

        ui->dSBox_ampCircle->setStyleSheet("color:black");
      }

      else if(obj==ui->dSBox_ampNoCircle)
      {
        double value;
        value = ui->dSBox_ampNoCircle->value();

        for(int i=0;i<ui->listWidget_axis_tab2->count();i++)
        {
          if(ui->listWidget_axis_tab2->item(i)->isSelected())
          {
            m_motionStatusPtrList[i]->setAMPNoCircleValue(value);
            qDebug() << "---------------axis: "<<i<<" setAMPNoCircleValue:"<<value;
            qDebug()<<"----------------velMode:"<<m_motionStatusPtrList[i]->currentPlanVelMode();
            qDebug()<<"----------------motionMode:"<<m_motionStatusPtrList[i]->motionMode();
          }
        }

        ui->dSBox_ampNoCircle->setStyleSheet("color:black");
      }

      else if(obj==ui->dSBox_circleCount)
      {
        double value;
        value = ui->dSBox_circleCount->value();

        for(int i=0;i<ui->listWidget_axis_tab2->count();i++)
        {
          if(ui->listWidget_axis_tab2->item(i)->isSelected())
          {
            m_motionStatusPtrList[i]->setCountCircleValue(value);
            qDebug() << "---------------axis: "<<i<<" setCountCircleValue:"<<value;
          }
        }

        ui->dSBox_circleCount->setStyleSheet("color:black");
      }

      else if(obj==ui->dSBox_T)
      {
        double value;
        value = ui->dSBox_T->value();

        for(int i=0;i<ui->listWidget_axis_tab2->count();i++)
        {
          if(ui->listWidget_axis_tab2->item(i)->isSelected())
          {
            m_motionStatusPtrList[i]->setTcircleValue(value);
            qDebug() << "---------------axis: "<<i<<" setTcircleValue:"<<value;
          }
        }

        ui->dSBox_T->setStyleSheet("color:black");
      }

      else if(obj==ui->dSBox_time)
      {
        double value;
        value = ui->dSBox_time->value();

        for(int i=0;i<ui->listWidget_axis_tab2->count();i++)
        {
          if(ui->listWidget_axis_tab2->item(i)->isSelected())
          {
            m_motionStatusPtrList[i]->setTimesNoCircleValue(value);
            qDebug() << "---------------axis: "<<i<<" setTimesNoCircleValue:"<<value;
          }
        }

        ui->dSBox_time->setStyleSheet("color:black");
      }


      return true;
    }
  }

  return QWidget::eventFilter(obj,event);
}

//----------------private function------------------
void PlotWave::setUiStyleSheet()
{
  //-------设置按扭图标-----------------------
  QString str="  QCheckBox::indicator:unchecked {\
                      border-image: url(:/icon/start.png);width:32px;height:32px;}\
                      QCheckBox::indicator:checked {\
                      border-image: url(:/icon/stop.png);width:32px;height:32px;}\
                      QCheckBox::indicator:unchecked:hover {\
                      border-image: url(:/icon/start.png);width:40px;height:40px;}\
                      QCheckBox::indicator:checked:hover {\
                      border-image: url(:/icon/stop.png);width:40px;height:40px;\
                       } ";
  ui->btn_start->setStyleSheet(str);

  str="QTableView::item:selected {background-color: rgb(97,141,211); } \
      QTableView{background-color: lightgray}\
      QTableView::item:hover {\
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #e7effd, stop: 1 #cbdaf1);\
            border: 1px solid #bfcde4;\
        }";
  ui->tableWidget->setStyleSheet(str);

  str="QSplitter::handle {background-color:lightgray}";//为分割器增加分割线
  this->setStyleSheet(str);

  str="  QComboBox {\
      border: 0px solid gray;}";
  ui->comboBox_plotStyle->setStyleSheet(str);

  /*str="QListView {\
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
                                  stop: 0 #FAFBFE, stop: 1 #888dd9);\
  }";//#DEDEF1*/
  str="QListView {\
      show-decoration-selected: 1;\
      border:none; \
      }\
      QListView::item {\
            border: 1px solid #d9d9d9;\
            border-left-color: transparent;\
            border-top-color: transparent;\
        }\
      QListView::item:hover {\
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #e7effd, stop: 1 #cbdaf1);\
            border: 1px solid #bfcde4;\
        }\
      QListView::item:selected {\
            border: 1px solid #567dbc;\
        }\
      QListView::item:selected:active{\
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6ea1f1, stop: 1 #567dbc);\
        }\
      QListView::item:selected:!active {\
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6b9be8, stop: 1 #577fbf);\
        }";

  ui->listWidget_axis->setStyleSheet(str);
  ui->listWidget_mode->setStyleSheet(str);
  ui->listWidget_axis_tab2->setStyleSheet(str);
  ui->listWidget_motionSrc_tab2->setStyleSheet(str);
  ui->tab_advMode->setStyleSheet("QWidget#tab_advMode{background-color:rgb(240,240,240)}");
  ui->tab_motion->setStyleSheet("QWidget#tab_motion{background-color:rgb(240,240,240)}");

  ui->frame_2->setStyleSheet("QFrame#frame_2{border:none;}");
  ui->frame->setStyleSheet("QFrame#frame{border:none;}");
}

void PlotWave::initialUi()
{
  setUiStyleSheet();

  mp_userConfig=mp_mainWindow->getUserConfig();//获得主窗口的用户信息
  mp_treeAll=mp_mainWindow->getRamAllAxisTree();

  ui->splitter->setStretchFactor(0,5);
  ui->splitter->setStretchFactor(1,1);

  //--------------设置按键上的字体-------------------
  ui->btn_add->setText(tr("add"));
  ui->btn_auto->setText(tr("auto"));
  ui->btn_auto->setCheckable(true);
  ui->btn_auto->setChecked(false);
  ui->btn_clear->setText(tr("clear"));
  ui->btn_hori_measure->setText(tr("H"));
  ui->btn_hori_measure->setCheckable(true);
  ui->btn_hori_measure->setChecked(false);
  ui->btn_next->setText(tr("Next"));
  ui->btn_prev->setText(tr("Prev"));
  ui->btn_remove->setText(tr("remove"));
  ui->btn_reset->setText(tr("reset"));
  ui->btn_selectAll->setText(tr("All"));
  ui->btn_tag->setText(tr("Tag"));
  ui->btn_tag->setCheckable(true);
  ui->btn_tag->setChecked(false);
  ui->btn_ver_measure->setText(tr("V"));
  ui->btn_ver_measure->setCheckable(true);
  ui->btn_ver_measure->setChecked(false);
  ui->btn_view->setText(tr("View"));
  ui->label_sampling->setText(tr("Period ms"));
  ui->label_servoon->setText(tr("servo_OFF"));
  ui->label_measure_xy->setVisible(false);
  QFont fontlabel;
  fontlabel.setPixelSize(16);
  fontlabel.setBold(true);
  ui->label_servoon->setFont(fontlabel);
  ui->btn_x_left->setVisible(false);
  ui->btn_x_left->setCheckable(true);
  ui->btn_x_right->setVisible(false);
  ui->btn_y_down->setVisible(false);
  ui->btn_y_down->setCheckable(true);
  ui->btn_y_up->setVisible(false);

  //----设置好默认颜色------------
  m_colorDefault.append(QColor(Qt::red));
  m_colorDefault.append(QColor(Qt::green));
  m_colorDefault.append(QColor(Qt::blue));
  m_colorDefault.append(QColor(Qt::black));

  m_colorDefault.append(QColor(255,0,255));
  m_colorDefault.append(QColor(0,170,0));
  m_colorDefault.append(QColor(0,170,255));
  m_colorDefault.append(QColor(170,0,255));
  m_colorDefault.append(QColor(170,0,0));
  m_colorDefault.append(QColor(255,170,127));


  //设置tableWidget 曲线选择参数表数据
  ui->tableWidget->setMinimumWidth(250);
  ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//编辑触发模式
  ui->tableWidget->setAlternatingRowColors(true);
  ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);//设置其右键弹出菜单方法
  connect(ui->tableWidget,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(onTablePopMenu(QPoint)));
  m_popuMenuTable=new QMenu(this);
  addTableMenuAction(mp_userConfig->model.axisCount);//初始化m_popuMenuTable 右键弹出QAction
  ui->tableWidget->hideColumn(COL_PRM_TABLE_BYTESNUM);
  ui->tableWidget->hideColumn(COL_PRM_TABLE_OFFSET);
  ui->tableWidget->hideColumn(COL_PRM_TABLE_BASE);
  ui->tableWidget->hideColumn(COL_PRM_TABLE_ADD);
  ui->tableWidget->hideColumn(COL_PRM_TABLE_MULTI);
  ui->tableWidget->hideColumn(COL_PRM_TABLE_XOFFSET);
//  ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

  ui->tableWidget->horizontalHeader()->resizeSection(COL_PRM_TABLE_DRAW,75);
  ui->tableWidget->horizontalHeader()->resizeSection(COL_PRM_TABLE_NAME,75);
  ui->tableWidget->horizontalHeader()->resizeSection(COL_PRM_TABLE_AXISNUM,75);
  QStringList headerList;
  headerList<<tr("checked")\
            <<tr("name")\
            <<tr("bytes")\
            <<tr("axis")\
            <<tr("offset")\
            <<tr("base")\
            <<tr("addfactor")\
            <<tr("multifactor")\
            <<tr("xoffset");
  ui->tableWidget->setHorizontalHeaderLabels(headerList);
//  ui->tableWidget->setRowCount(headerList.count());

  //读入上次记录的曲线，并更新表格及m_tablePlotPrmList
  updateTableWidgetFromFile(CUSTOM_DEFAULT_WATCHCURVE_PATH);
//  ui->tableWidget->resizeColumnsToContents();
  //只读入user曲线模板，在Add单击时再根据所选择的版本更新对应的地址
  m_usrCurveTreeManager=new UsrCurveTreeManager(mp_treeAll,CUSTOM_DEFAULT_WATCHCURVE_USER_PATH);

  //采样格式化输入
  QRegExp ex("^[0-9]\\d*(\\.\\d+)?");//非负数
  QValidator *validator=new QRegExpValidator(ex,ui->lineEdit_sampling);
  ui->lineEdit_sampling->setValidator(validator);
  ui->lineEdit_sampling->setText(QString::number(m_viewSetting.samplingTime));


  QStringList list;
  list<<tr("velocity")<<tr("current")<<tr("position")<<tr("user");
  updateUserStyleComboBox(list);
  updatePlotView(&m_viewSetting);
  initialPlotCurve(m_tablePlotPrmList);

  //设置选择背景
  ui->comboBox_plotStyle->setItemData(0,QColor(211,211,211),Qt::BackgroundRole);
  ui->comboBox_plotStyle->setItemData(1,QColor(Qt::white),Qt::BackgroundRole);
  ui->comboBox_plotStyle->setItemData(2,QColor(Qt::black),Qt::BackgroundRole);
  ui->comboBox_plotStyle->setItemData(2,QColor(Qt::white),Qt::TextColorRole);
  ui->comboBox_plotStyle->setItemData(3,QColor(Qt::gray),Qt::BackgroundRole);
  ui->comboBox_plotStyle->setItemData(4,QColor(Qt::blue),Qt::BackgroundRole);
  ui->comboBox_plotStyle->setItemData(4,QColor(Qt::white),Qt::TextColorRole);
  ui->comboBox_plotStyle->setItemData(5,QColor(240, 240, 240),Qt::BackgroundRole);
  ui->comboBox_plotStyle->setItemData(6,QColor(101, 101, 101),Qt::BackgroundRole);

#if TEST_DEBUG
  //产生数测试
  for(int i=0;i<ui->plotCurve->graphCount();i++)
  {
    QVector<double> keys;
    QVector<double> values;
    double amp=10*(i+1);
//    double dx=(1.0/8.0)*M_PI;
    double x;
    double y;
    for(int j=0;j<1000;j++)
    {
//      x=j*dx;
      x=2*M_PI*200*0.0000625*j;
      y=amp*qSin(x);
      keys.append(x);
      values.append(y);
      m_tablePlotPrmList[i].data.curveKey.append(x);
      m_tablePlotPrmList[i].data.curveValue.append(y);
//      qDebug()<<x<<y;
    }
    ui->plotCurve->graph(i)->addData(keys,values);

    if(!m_tablePlotPrmList.at(i).isDraw)
      ui->plotCurve->graph(i)->setVisible(false);
  }
  ui->plotCurve->replot();
#endif
  ui->plotCurve->rescaleAxes(true);
  //初始化背景
  ui->comboBox_plotStyle->setCurrentIndex(m_viewSetting.curViewIndex);
  onPlotStyleChanged(m_viewSetting.curViewIndex);

  //--------tab 1 界面-----------------------------
  m_servoModeNameList<<tr("0 IDLE")\
           <<tr("1 ADC")\
           <<tr("2 Initial position Adjust")\
           <<tr("3 Motor Parameters Identification")\
           <<tr("4 Current open-loop")\
           <<tr("5 Current close-loop")\
           <<tr("6 Velocity close-loop")\
           <<tr("7 Open Velocity-loop2")\
           <<tr("8 Close Velocity-loop2")\
           <<tr("9 Position Fixed")\
           <<tr("10 Position Follow")\
           <<tr("11 STOP_VCTL")\
           <<tr("12 Circle current Follow");
  QStringList modeList;
  modeList<<tr("0IDLE")
          <<tr("1ADC")
          <<tr("2IPA")
          <<tr("3MPI")
          <<tr("4COL")
          <<tr("5CCL")
          <<tr("6VCL")
          <<tr("7VPL")
          <<tr("8VSL")
          <<tr("9FIX")
          <<tr("10PT")
          <<tr("11DB")
          <<tr("12CSC");

  ui->listWidget_mode->addItems(m_servoModeNameList);
  ui->listWidget_mode->setResizeMode(QListView::Adjust);
  ui->listWidget_mode->setCurrentRow(0);

  ui->listWidget_mode->hide();
  ui->listWidget_axis->hide();
  ui->label_showMode->setAlignment(Qt::AlignCenter);
  m_rollBoxUi=new RollBoxWidget(mp_userConfig->model.axisCount,modeList);
//  m_rollBoxUi->show();
  ui->hLayout_modeTab->insertWidget(0,m_rollBoxUi,1);
  connect(m_rollBoxUi,SIGNAL(hoverAt(int,int)),this,SLOT(onShowCurrentModeInfo(int,int)));
  connect(m_rollBoxUi,SIGNAL(dragIndexAt(int,int)),this,SLOT(onShowCurrentModeInfo(int,int)));
  connect(m_rollBoxUi,SIGNAL(clickedAt(int)),this,SLOT(onRollBoxBtnClicked(int)));
  connect(m_rollBoxUi,SIGNAL(stopAt(int,int)),this,SLOT(onRollBoxStopedAt(int,int)));

//  ui->stackedWidget->setCurrentIndex(0);
  setListViewAxisNum(mp_userConfig->model.axisCount);//初始化轴号选择列表tab1 tab2
  initialServoStatus(mp_userConfig->model.axisCount);//初始化伺服状态
  connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(onTabWidgetCurrentChanged(int)));

  //--------tab 1 界面-----------------------------end

  //--------tab 2 界面-----------------------------
  QStringList controlSrcList;
  controlSrcList<<tr("0 None")\
                <<tr("1 VelocityMode");
//                <<tr("2 PositionMode");
  ui->listWidget_motionSrc_tab2->addItems(controlSrcList);
  ui->listWidget_motionSrc_tab2->setResizeMode(QListView::Adjust);
  QButtonGroup *btnControlGroup=new QButtonGroup(this);
  ui->btn_srcPC->setCheckable(true);
  ui->btn_srcGLink2->setCheckable(true);
  btnControlGroup->addButton(ui->btn_srcPC);
  btnControlGroup->addButton(ui->btn_srcGLink2);

//  m_motionStatusPtrList
  QProgressBar *bar;
  QHBoxLayout *layout=new QHBoxLayout(ui->widget_progressGo);
  ui->widget_progressGo->setHidden(true);
  ui->widget_progressGo->setLayout(layout);
  MotionStatus *motionStatus;
  m_generalCmd=new ServoGeneralCmd(mp_mainWindow->getFunctionCmdTree(),mp_userConfig->com.id,mp_userConfig->com.rnStation,this);
  QString strBarStyle="QProgressBar::chunk {background-color: #CD96CD;height: 5px;margin: 0.5px;}QProgressBar {border: 2px solid grey;border-radius: 5px;text-align: center;}";
  for(int i=0;i<mp_userConfig->model.axisCount;i++)
  {
    motionStatus=new MotionStatus(m_generalCmd,i);
    m_motionStatusPtrList.append(motionStatus);
    bar=new QProgressBar(ui->widget_progressGo);
    bar->setStyleSheet(strBarStyle);
    m_progressBarList.append(bar);
    bar->setObjectName(tr("bar%1").arg(i));
    bar->setValue(0);
    bar->setOrientation(Qt::Vertical);
    QSizePolicy sizePolicy;
    sizePolicy.setHorizontalPolicy(QSizePolicy::Expanding);
    sizePolicy.setVerticalPolicy(QSizePolicy::Expanding);
    bar->setSizePolicy(sizePolicy);
    layout->addWidget(bar);
    connect(motionStatus,SIGNAL(updateProgressValue(int)),bar,SLOT(setValue(int)));
    connect(motionStatus,SIGNAL(planVeltimerStop(int)),this,SLOT(onMotionStatusPlanVelTimeOut(int)));
  }
  ui->stackedWidget_velocity->setCurrentIndex(0);

#if TEST_DEBUG
  QList<QProgressBar *> barList;
  barList=ui->widget_progressGo->findChildren<QProgressBar *>();
  qDebug()<<"barListCount:"<<barList.count();
  foreach (bar, barList) {
    qDebug()<<"bar name "<<bar->objectName();
  }
#endif
  //--------tab 2 界面-----------------------------end
}

QTreeWidget* PlotWave::readTreeWidgetFromFile(QString &filename)
{
  QFormBuilder builder;
  QFile file(filename);

  if (!file.open(QFile::ReadOnly))
  {
    QMessageBox::information(0, tr("open error"), tr("open %1 file error ").arg(filename));
    return NULL;
  }
  QWidget *widget=NULL;
  QTreeWidget *treeWidget=NULL;

  widget = builder.load(&file, 0);
  if (widget==NULL)
  {
    QMessageBox::information(0, tr("load error"), tr("load .xml file error "));
    file.close();
    return NULL;
  }
  file.close();

  QTreeWidget *tree=widget->findChild<QTreeWidget*>("treeWidget");//可以这样子查找
  if(tree!=NULL)
  {
    treeWidget=new QTreeWidget();
    QTreeWidgetItem *item;
    for(int i=0;i<tree->topLevelItemCount();i++)
    {
      item=tree->topLevelItem(i)->clone();
      treeWidget->addTopLevelItem(item);
    }
    QTreeWidgetItem *head;
    head=tree->headerItem()->clone();
    treeWidget->setHeaderItem(head);
    return treeWidget;
  }
  else
  {
    delete widget;
    QMessageBox::information(0, tr("load error"), tr("findChild treeWidget error! "));
    return NULL;
  }
}

bool PlotWave::readInitialFile(QString &fileName)
{
  PlotTablePrmFactor cPrmFactor;
  PlotTablePrmFactor vPrmFactor;

  cPrmFactor.factorTypeId=PLOT_PRM_FACTOR_TYPE_CONST;
  cPrmFactor.controlPrm.fullName="NULL";
  cPrmFactor.controlPrm.bytes=-1;
  cPrmFactor.controlPrm.offsetAddr=-1;
  cPrmFactor.controlPrm.baseAddr=-1;
  cPrmFactor.constValue=0;

  vPrmFactor.factorTypeId=PLOT_PRM_FACTOR_TYPE_VARIABLE;
  vPrmFactor.controlPrm.fullName="NULL";
  vPrmFactor.controlPrm.bytes=-1;
  vPrmFactor.controlPrm.offsetAddr=-1;
  vPrmFactor.controlPrm.baseAddr=-1;

  m_curveSettingTree=QtTreeManager::readTreeWidgetFromXmlFile(fileName);
//  m_curveSettingTree->show();
  if(m_curveSettingTree!=NULL)
  {
    m_viewSetting.xtime=m_curveSettingTree->topLevelItem(ROW_CURVESETTING_INDEX_VIEWSETTING)\
                        ->child(ROW_VIEWSETTING_INDEX_XYRANGE)\
                        ->child(ROW_XYRANGE_INDEX_XTIME)\
                        ->text(COL_CURVESETTING_INDEX_VALUE).toInt();
    m_viewSetting.ymax=m_curveSettingTree->topLevelItem(ROW_CURVESETTING_INDEX_VIEWSETTING)\
                        ->child(ROW_VIEWSETTING_INDEX_XYRANGE)\
                        ->child(ROW_XYRANGE_INDEX_YMAX)\
                        ->text(COL_CURVESETTING_INDEX_VALUE).toInt();
    m_viewSetting.ymin=m_curveSettingTree->topLevelItem(ROW_CURVESETTING_INDEX_VIEWSETTING)\
                        ->child(ROW_VIEWSETTING_INDEX_XYRANGE)\
                        ->child(ROW_XYRANGE_INDEX_YMIN)\
                        ->text(COL_CURVESETTING_INDEX_VALUE).toInt();
    m_viewSetting.storeTime=m_curveSettingTree->topLevelItem(ROW_CURVESETTING_INDEX_VIEWSETTING)\
                        ->child(ROW_VIEWSETTING_INDEX_XYRANGE)\
                        ->child(ROW_XYRANGE_INDEX_STORETIME)\
                        ->text(COL_CURVESETTING_INDEX_VALUE).toInt();
    m_viewSetting.pointNum=m_curveSettingTree->topLevelItem(ROW_CURVESETTING_INDEX_VIEWSETTING)\
                        ->child(ROW_VIEWSETTING_INDEX_XYRANGE)\
                        ->child(ROW_XYRANGE_INDEX_POINTNUM)\
                        ->text(COL_CURVESETTING_INDEX_VALUE).toInt();

    m_viewSetting.samplingTime=m_curveSettingTree->topLevelItem(ROW_CURVESETTING_INDEX_VIEWSETTING)\
                        ->child(ROW_VIEWSETTING_INDEX_SAMPLE_TIME)\
                        ->text(COL_CURVESETTING_INDEX_VALUE).toDouble();
    m_viewSetting.curViewIndex=m_curveSettingTree->topLevelItem(ROW_CURVESETTING_INDEX_VIEWSETTING)\
                        ->child(ROW_VIEWSETTING_INDEX_CURRENT_VIEW)\
                        ->text(COL_CURVESETTING_INDEX_VALUE).toInt();
    for(int i=0;i<m_curveSettingTree->topLevelItemCount()-1;i++)//因为第一个是视图设置的信息，所以-1
    {
      PlotTablePrm rowPrm;
      int nR = 0, nG = 0, nB = 0;
      int index=i+ROW_CURVESETTING_INDEX_PRM_ROW;
      rowPrm.curveType=(CurveType)m_curveSettingTree->topLevelItem(index)->text(COL_CURVESETTING_INDEX_VALUE).toInt();
      rowPrm.id=m_curveSettingTree->topLevelItem(index)->child(ROW_PRM_INDEX_CURVEID)->text(COL_CURVESETTING_INDEX_VALUE).toInt();
      rowPrm.isDraw=m_curveSettingTree->topLevelItem(index)->child(ROW_PRM_INDEX_ISDRAW)->text(COL_CURVESETTING_INDEX_VALUE).toInt();
      rowPrm.fullName=m_curveSettingTree->topLevelItem(index)->child(ROW_PRM_INDEX_FULLNAME)->text(COL_CURVESETTING_INDEX_VALUE);
      nR=m_curveSettingTree->topLevelItem(index)->child(ROW_PRM_INDEX_COLOR)->child(ROW_COLOR_INDEX_R)->text(COL_CURVESETTING_INDEX_VALUE).toInt();
      nG=m_curveSettingTree->topLevelItem(index)->child(ROW_PRM_INDEX_COLOR)->child(ROW_COLOR_INDEX_G)->text(COL_CURVESETTING_INDEX_VALUE).toInt();
      nB=m_curveSettingTree->topLevelItem(index)->child(ROW_PRM_INDEX_COLOR)->child(ROW_COLOR_INDEX_B)->text(COL_CURVESETTING_INDEX_VALUE).toInt();
      rowPrm.color.setRgb(nR,nG,nB);
      rowPrm.axisNum=m_curveSettingTree->topLevelItem(index)->child(ROW_PRM_INDEX_AXISNUM)->text(COL_CURVESETTING_INDEX_VALUE).toInt();
      rowPrm.bytes=m_curveSettingTree->topLevelItem(index)->child(ROW_PRM_INDEX_BYTESNUM)->text(COL_CURVESETTING_INDEX_VALUE).toInt();
      rowPrm.offsetAddr=m_curveSettingTree->topLevelItem(index)->child(ROW_PRM_INDEX_OFFSETADDR)->text(COL_CURVESETTING_INDEX_VALUE).toUInt();
      rowPrm.baseAddr=m_curveSettingTree->topLevelItem(index)->child(ROW_PRM_INDEX_BASEADDR)->text(COL_CURVESETTING_INDEX_VALUE).toUInt();
      rowPrm.addValue=m_curveSettingTree->topLevelItem(index)->child(ROW_PRM_INDEX_ADDVALUE)->text(COL_CURVESETTING_INDEX_VALUE).toDouble();
      rowPrm.multiValue=m_curveSettingTree->topLevelItem(index)->child(ROW_PRM_INDEX_MULTIVALUE)->text(COL_CURVESETTING_INDEX_VALUE).toDouble();
      rowPrm.xoffset=m_curveSettingTree->topLevelItem(index)->child(ROW_PRM_INDEX_XOFFSET)->text(COL_CURVESETTING_INDEX_VALUE).toDouble();
      rowPrm.equationDescription=m_curveSettingTree->topLevelItem(index)->child(ROW_PRM_INDEX_EQUATIONDESCRIPTION)->text(COL_CURVESETTING_INDEX_VALUE);

      QTreeWidgetItem *prmFactor;
      QTreeWidgetItem *factor;
      prmFactor=m_curveSettingTree->topLevelItem(index)->child(ROW_PRM_INDEX_PRMFACTOR);
      if((rowPrm.curveType==CURVE_TYPE_SCRIPT)||(rowPrm.curveType==CURVE_TYPE_SCRIPT_CONST))
      {
        //varFactor
        int varCount=prmFactor->text(COL_CURVESETTING_INDEX_VALUE).toInt();
        int totalCount=prmFactor->childCount();
        rowPrm.varCount=varCount;
        for(int i=0;i<varCount;i++)
        {
          factor=prmFactor->child(i);
          vPrmFactor.controlPrm.fullName=factor->child(ROW_FACTOR_INDEX_FULLNAME)->text(COL_CURVESETTING_INDEX_VALUE);
          vPrmFactor.controlPrm.bytes=factor->child(ROW_FACTOR_INDEX_BYTESNUM)->text(COL_CURVESETTING_INDEX_VALUE).toInt();
          vPrmFactor.controlPrm.offsetAddr=factor->child(ROW_FACTOR_INDEX_OFFSETADDR)->text(COL_CURVESETTING_INDEX_VALUE).toUInt();
          vPrmFactor.controlPrm.baseAddr=factor->child(ROW_FACTOR_INDEX_BASEADDR)->text(COL_CURVESETTING_INDEX_VALUE).toUInt();
          rowPrm.prmFactorList.append(vPrmFactor);
        }

        //constFactor
        for(int i=varCount;i<totalCount;i++)
        {
          factor=prmFactor->child(i);
          cPrmFactor.controlPrm.fullName=factor->child(ROW_FACTOR_INDEX_FULLNAME)->text(COL_CURVESETTING_INDEX_VALUE);
          cPrmFactor.controlPrm.bytes=factor->child(ROW_FACTOR_INDEX_BYTESNUM)->text(COL_CURVESETTING_INDEX_VALUE).toInt();
          cPrmFactor.controlPrm.offsetAddr=factor->child(ROW_FACTOR_INDEX_OFFSETADDR)->text(COL_CURVESETTING_INDEX_VALUE).toUInt();
          cPrmFactor.controlPrm.baseAddr=factor->child(ROW_FACTOR_INDEX_BASEADDR)->text(COL_CURVESETTING_INDEX_VALUE).toUInt();
          rowPrm.prmFactorList.append(cPrmFactor);
        }

      }
//      else if(rowPrm.curveType==CURVE_TYPE_SCRIPT_CONST)
//      {
//        //varFactor
//        int varCount=prmFactor->text(COL_CURVESETTING_INDEX_VALUE).toInt();
//        int totalCount=prmFactor->childCount();
//        rowPrm.varCount=varCount;
//        for(int i=0;i<varCount;i++)
//        {
//          factor=prmFactor->child(i);
//          vPrmFactor.controlPrm.fullName=factor->child(ROW_FACTOR_INDEX_FULLNAME)->text(COL_CURVESETTING_INDEX_VALUE);
//          vPrmFactor.controlPrm.bytes=factor->child(ROW_FACTOR_INDEX_BYTESNUM)->text(COL_CURVESETTING_INDEX_VALUE).toInt();
//          vPrmFactor.controlPrm.offsetAddr=factor->child(ROW_FACTOR_INDEX_OFFSETADDR)->text(COL_CURVESETTING_INDEX_VALUE).toUInt();
//          vPrmFactor.controlPrm.baseAddr=factor->child(ROW_FACTOR_INDEX_BASEADDR)->text(COL_CURVESETTING_INDEX_VALUE).toUInt();
//          rowPrm.prmFactorList.append(vPrmFactor);
//        }
//        //constFactor
//        for(int i=varCount;i<totalCount;i++)
//        {
//          factor=prmFactor->child(i);
//          cPrmFactor.controlPrm.fullName=factor->child(ROW_FACTOR_INDEX_FULLNAME)->text(COL_CURVESETTING_INDEX_VALUE);
//          cPrmFactor.controlPrm.bytes=factor->child(ROW_FACTOR_INDEX_BYTESNUM)->text(COL_CURVESETTING_INDEX_VALUE).toInt();
//          cPrmFactor.controlPrm.offsetAddr=factor->child(ROW_FACTOR_INDEX_OFFSETADDR)->text(COL_CURVESETTING_INDEX_VALUE).toUInt();
//          cPrmFactor.controlPrm.baseAddr=factor->child(ROW_FACTOR_INDEX_BASEADDR)->text(COL_CURVESETTING_INDEX_VALUE).toUInt();
//          rowPrm.prmFactorList.append(cPrmFactor);
//        }
//      }
      else
      {
        vPrmFactor.controlPrm.fullName="NULL";
        vPrmFactor.controlPrm.bytes=-1;
        vPrmFactor.controlPrm.offsetAddr=-1;
        vPrmFactor.controlPrm.baseAddr=-1;

        cPrmFactor.controlPrm.fullName="NULL";
        cPrmFactor.controlPrm.bytes=-1;
        cPrmFactor.controlPrm.offsetAddr=-1;
        cPrmFactor.controlPrm.baseAddr=-1;
        cPrmFactor.constValue=0;
        rowPrm.prmFactorList.append(vPrmFactor);
        rowPrm.prmFactorList.append(cPrmFactor);
      }
      m_tablePlotPrmList.append(rowPrm);
    }
  }
  return true;
}

//bool PlotWave::readInitialFile(QString &fileName)
//{
//  QFile file(fileName);
//  if(!file.open(QIODevice::ReadOnly))
//  {
//    QMessageBox::information(0,tr("File Open"),tr("File:%1 \n open error!").arg(fileName));
//    return false;
//  }
//  QXmlStreamReader reader;
//  QString elementName;
//  QXmlStreamAttributes xmlAttributes;
//  PlotTablePrm rowPrm;
//  reader.setDevice(&file);
//  reader.readNext();
//  while (!reader.atEnd())
//  {
//    if(reader.isStartElement())
//    {
//      elementName=reader.name().toString();
//      if(elementName=="XYRange")
//      {
//        xmlAttributes=reader.attributes();
//        if(xmlAttributes.hasAttribute("xtime")) m_viewSetting.xtime=xmlAttributes.value("xtime").toInt();
//        if(xmlAttributes.hasAttribute("ymax")) m_viewSetting.ymax=xmlAttributes.value("ymax").toDouble();
//        if(xmlAttributes.hasAttribute("ymin")) m_viewSetting.ymin=xmlAttributes.value("ymin").toDouble();
//        if(xmlAttributes.hasAttribute("storeTime")) m_viewSetting.storeTime=xmlAttributes.value("storeTime").toInt();
//        if(xmlAttributes.hasAttribute("pointNum")) m_viewSetting.pointNum=xmlAttributes.value("pointNum").toInt();
//      }
//      else if(elementName=="Sample")
//      {
//        xmlAttributes=reader.attributes();
//        if(xmlAttributes.hasAttribute("TimeMs")) m_viewSetting.samplingTime=xmlAttributes.value("TimeMs").toDouble();
//      }
//      else if(elementName=="CurrentViewIndex")
//      {
//        QString text;
//        text=reader.readElementText();
//        m_currentViewIndex=text.toUInt();
//      }
//      else if (elementName=="PrmTable")//读取表格设置内容
//      {
//        reader.readNext();
//        while(!reader.atEnd())
//        {
//          elementName=reader.name().toString();
//          if(reader.isEndElement()&&(elementName=="PrmTable")) break;
//          if(reader.isStartElement()&&elementName=="Row")
//          {
//            xmlAttributes=reader.attributes();
//            if(xmlAttributes.hasAttribute("isDraw")) rowPrm.isDraw=xmlAttributes.value("isDraw").toInt();
//            if(xmlAttributes.hasAttribute("axisNum")) rowPrm.axisNum=xmlAttributes.value("axisNum").toInt();

//            int nR = 0, nG = 0, nB = 0;
//            if (xmlAttributes.hasAttribute("ColorR")) nR = xmlAttributes.value("ColorR").toInt();
//            if (xmlAttributes.hasAttribute("ColorG")) nG = xmlAttributes.value("ColorG").toInt();
//            if (xmlAttributes.hasAttribute("ColorB")) nB = xmlAttributes.value("ColorB").toInt();
//            rowPrm.color.setRgb(nR,nG,nB);

//            if(xmlAttributes.hasAttribute("Offset")) rowPrm.offsetAddr=xmlAttributes.value("Offset").toInt();
//            if(xmlAttributes.hasAttribute("BytesNum")) rowPrm.bytes=xmlAttributes.value("BytesNum").toInt();
//            if(xmlAttributes.hasAttribute("BaseAddr")) rowPrm.baseAddr=(quint16)(xmlAttributes.value("BaseAddr").toUInt());
//            if(xmlAttributes.hasAttribute("AddValue")) rowPrm.addValue=xmlAttributes.value("AddValue").toDouble();
//            if(xmlAttributes.hasAttribute("MultiValue")) rowPrm.multiValue=xmlAttributes.value("MultiValue").toDouble();
//            if(xmlAttributes.hasAttribute("FullName")) rowPrm.fullName=xmlAttributes.value("FullName").toString();
//            m_tablePlotPrmList.append(rowPrm);
//          }
//          reader.readNext();
//        }
//      }

//    }
//    reader.readNext();
//  }
//  if(reader.hasError())
//  {
//    QMessageBox::information(0,tr("XML File"),tr("XML file \n%1\nread error!").arg(fileName));
//    return false;
//  }
//  return true;
//}
//!设置一行参数
void PlotWave::setTableRowPrm(int row, PlotTablePrm tablePrm)
{
  QString str;
  QColor color;
  QStringList listStr;
  QFont font;
  font.setBold(true);
  for (int nCol = 0; nCol < COL_PRM_TABLE_COUNT; nCol++)
  {
    QTableWidgetItem *item = new QTableWidgetItem();
    item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    switch (nCol)
    {
    case COL_PRM_TABLE_DRAW:
      if(tablePrm.isDraw)
      {
        str=tr("on");
//        color=QColor(97,141,211);
        color=QColor(Qt::green).darker();
      }
      else
      {
        str=tr("off");
        color=Qt::gray;
      }
      item->setText(str);
      item->setTextColor(QColor(Qt::white));
      item->setFont(font);
      item->setBackgroundColor(color);
      break;
    case COL_PRM_TABLE_NAME:
      listStr = tablePrm.fullName.split(".");
      str=listStr.last();
      color=tablePrm.color;
      item->setTextColor(color);
      break;
    case COL_PRM_TABLE_BYTESNUM:
      str=QString::number(tablePrm.bytes);
      break;
    case COL_PRM_TABLE_AXISNUM:
      str=QString::number(tablePrm.axisNum);
      break;
    case COL_PRM_TABLE_OFFSET:
      str=QString::number(tablePrm.offsetAddr);
      break;
    case COL_PRM_TABLE_BASE:
      str=QString::number(tablePrm.baseAddr);
      break;
    case COL_PRM_TABLE_ADD:
      str=QString::number(tablePrm.addValue,'f',2);
      break;
    case COL_PRM_TABLE_MULTI:
      str=QString::number(tablePrm.multiValue,'f',2);
      break;
    case COL_PRM_TABLE_XOFFSET:
      str=QString::number(tablePrm.xoffset,'f',2);
      break;
    default:
      break;
    }
    item->setText(str);
    ui->tableWidget->setItem(row, nCol, item);
  }
//  ui->tableWidget->resizeColumnsToContents();//2017-3-10先去掉，后面加了其它的还要
}

/**
 * @brief PlotWave::resetTableWidget
 */
void PlotWave::resetTableWidget()
{
  //clear xoffset data
//  disconnect(ui->tableWidget,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(onTableItemChanged(QTableWidgetItem*)));//避免修改时产生
  for(int i=0;i<m_tablePlotPrmList.count();i++)
  {
    m_tablePlotPrmList[i].xoffset=0;
    ui->tableWidget->item(i,COL_PRM_TABLE_XOFFSET)->setText("0");
  }

//  connect(ui->tableWidget,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(onTableItemChanged(QTableWidgetItem*)));

}
//!从配置文件中读取更新画图中的表格参数
void PlotWave::updateTableWidgetFromFile(QString &fileName)
{
  if(readInitialFile(fileName))
  {
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->clearContents();
    PlotTablePrm tablePrm;
    int row=0;
    foreach (tablePrm, m_tablePlotPrmList)
    {
      ui->tableWidget->insertRow(row);
      setTableRowPrm(row,tablePrm);
      row++;
    }
  }
  else
  {
    QMessageBox::information(0,tr("Curve Setting"),tr("default curve setting error!"));
  }
}
//!更新combobox内容
void PlotWave::updateUserStyleComboBox(QStringList &userList)
{
  for(int i=0;i<ui->comboBox_user->count();i++)
    ui->comboBox_user->removeItem(i);
  ui->comboBox_user->addItems(userList);
  ui->comboBox_user->hide();
}
void PlotWave::updatePlotView(ViewSetting *view)
{
  ui->plotCurve->xAxis->setRange(0.0f, view->xtime);			//设置X坐标轴范围
  ui->plotCurve->yAxis->setRange(view->ymin, view->ymax);	//设置Y坐标轴范围
  ui->plotCurve->replot();
}
void PlotWave::initialPlotCurve(QList<PlotTablePrm> & tablePrmList)
{
  // 允许鼠标拖动坐标轴、滚轮缩放、选取曲线，更多功能参加QCP
  ui->plotCurve->setInteraction(QCP::iRangeZoom,true);
  ui->plotCurve->setInteraction(QCP::iRangeDrag,true);
  ui->plotCurve->setInteraction(QCP::iMultiSelect,true);
  ui->plotCurve->setInteraction(QCP::iSelectPlottables,false);
  ui->plotCurve->setMultiSelectModifier(Qt::ShiftModifier);

  //设置坐标轴
  ui->plotCurve->xAxis->setLabel(tr("s"));	//设置X坐标轴标签
//  ui->plotCurve->yAxis->setLabel(tr("value"));		//设置Y坐标轴标签
  //显示上X/右Y轴坐标线、不显示刻度值
  ui->plotCurve->xAxis2->setVisible(false);
  ui->plotCurve->xAxis2->setTickLabels(false);
  ui->plotCurve->yAxis2->setVisible(false);
  ui->plotCurve->yAxis2->setTickLabels(false);
  ui->plotCurve->setBackground(QBrush(QColor(211,211,211)));
  //添加曲线
  PlotTablePrm prm;
  int i=0;
  foreach (prm, tablePrmList)
  {
    ui->plotCurve->addGraph();
    ui->plotCurve->graph(i)->setName(prm.fullName);
    ui->plotCurve->graph(i)->setPen(QPen(prm.color));
    ui->plotCurve->graph(i)->setSelectable(false);
    ui->plotCurve->graph(i)->setVisible(prm.isDraw);
    i++;
  }
}

void PlotWave::writePlotWaveFile()
{
  //---------write watchcurvesetting.xml---------------------------
  //如果现在正处于打开旧曲线状态，则不保存，因为旧曲线状态参数不全
  if(ui->btn_open->isChecked()==false)
  {
    QString filePath=CUSTOM_DEFAULT_WATCHCURVE_PATH;
    m_curveSettingTree->topLevelItem(ROW_CURVESETTING_INDEX_VIEWSETTING)\
        ->child(ROW_VIEWSETTING_INDEX_XYRANGE)\
        ->child(ROW_XYRANGE_INDEX_XTIME)\
        ->setText(COL_CURVESETTING_INDEX_VALUE,QString::number(m_viewSetting.xtime));

    m_curveSettingTree->topLevelItem(ROW_CURVESETTING_INDEX_VIEWSETTING)\
        ->child(ROW_VIEWSETTING_INDEX_XYRANGE)\
        ->child(ROW_XYRANGE_INDEX_YMAX)\
        ->setText(COL_CURVESETTING_INDEX_VALUE,QString::number(m_viewSetting.ymax,'f',3));

    m_curveSettingTree->topLevelItem(ROW_CURVESETTING_INDEX_VIEWSETTING)\
        ->child(ROW_VIEWSETTING_INDEX_XYRANGE)\
        ->child(ROW_XYRANGE_INDEX_YMIN)\
        ->setText(COL_CURVESETTING_INDEX_VALUE,QString::number(m_viewSetting.ymin,'f',3));

    m_curveSettingTree->topLevelItem(ROW_CURVESETTING_INDEX_VIEWSETTING)\
        ->child(ROW_VIEWSETTING_INDEX_XYRANGE)\
        ->child(ROW_XYRANGE_INDEX_STORETIME)\
        ->setText(COL_CURVESETTING_INDEX_VALUE,QString::number(m_viewSetting.storeTime));

    m_curveSettingTree->topLevelItem(ROW_CURVESETTING_INDEX_VIEWSETTING)\
        ->child(ROW_VIEWSETTING_INDEX_XYRANGE)\
        ->child(ROW_XYRANGE_INDEX_POINTNUM)\
        ->setText(COL_CURVESETTING_INDEX_VALUE,QString::number(m_viewSetting.pointNum));

    m_curveSettingTree->topLevelItem(ROW_CURVESETTING_INDEX_VIEWSETTING)\
        ->child(ROW_VIEWSETTING_INDEX_SAMPLE_TIME)\
        ->setText(COL_CURVESETTING_INDEX_VALUE,QString::number(m_viewSetting.samplingTime,'f',5));

    m_curveSettingTree->topLevelItem(ROW_CURVESETTING_INDEX_VIEWSETTING)\
        ->child(ROW_VIEWSETTING_INDEX_CURRENT_VIEW)\
        ->setText(COL_CURVESETTING_INDEX_VALUE,QString::number(m_viewSetting.curViewIndex));

    //remove old row first
    QTreeWidgetItem *tItem;
    qDebug()<<"count:"<<m_curveSettingTree->topLevelItemCount();
    int topCount=m_curveSettingTree->topLevelItemCount();
    for(int i=0;i<topCount-1;i++)
    {
      tItem=m_curveSettingTree->takeTopLevelItem(0+ROW_CURVESETTING_INDEX_PRM_ROW);
      qDebug()<<tItem->text(0)<<" : "<<tItem->text(1);
    }

    //copy prmdata to tree
    PlotTablePrm prm;
    QTreeWidgetItem *itemChild1;
    QTreeWidgetItem *itemChild2;
    QTreeWidgetItem *topItem;
    QTreeWidgetItem *itemCurveId;
    QTreeWidgetItem *itemIsDraw;
    QTreeWidgetItem *itemFullName;
    QTreeWidgetItem *itemColor;
    QTreeWidgetItem *itemColorR;
    QTreeWidgetItem *itemColorG;
    QTreeWidgetItem *itemColorB;
    QTreeWidgetItem *itemAxisNum;
    QTreeWidgetItem *itemBytesNum;
    QTreeWidgetItem *itemOffsetAddr;
    QTreeWidgetItem *itemBaseAddr;
    QTreeWidgetItem *itemAddValue;
    QTreeWidgetItem *itemMultiValue;
    QTreeWidgetItem *itemXoffset;
    QTreeWidgetItem *itemEquationDes;
    QTreeWidgetItem *itemPrmFactor;
    foreach (prm, m_tablePlotPrmList)
    {
      qDebug()<<"prm: "<<prm.fullName;
      topItem=new QTreeWidgetItem(m_curveSettingTree);
      topItem->setText(COL_CURVESETTING_INDEX_KEY,"Row");
      topItem->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"CurveType");

      //CurveId
      itemCurveId=new QTreeWidgetItem;
      itemCurveId->setText(COL_CURVESETTING_INDEX_KEY,"CurveId");
      itemCurveId->setText(COL_CURVESETTING_INDEX_VALUE,QString::number(prm.id));
      itemCurveId->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"NULL");

      //IsDraw
      itemIsDraw=new QTreeWidgetItem;
      itemIsDraw->setText(COL_CURVESETTING_INDEX_KEY,"IsDraw");
      if(prm.isDraw)
        itemIsDraw->setText(COL_CURVESETTING_INDEX_VALUE,"1");
      else
        itemIsDraw->setText(COL_CURVESETTING_INDEX_VALUE,"0");
      itemIsDraw->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"NULL");
      //FullName
      itemFullName=new QTreeWidgetItem;
      itemFullName->setText(COL_CURVESETTING_INDEX_KEY,"FullName");
      itemFullName->setText(COL_CURVESETTING_INDEX_VALUE,prm.fullName);
      itemFullName->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"NULL");
      //Color
      itemColor=new QTreeWidgetItem;
      itemColor->setText(COL_CURVESETTING_INDEX_KEY,"Color");
      itemColor->setText(COL_CURVESETTING_INDEX_VALUE,"NULL");
      itemColor->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"NULL");
      itemColorR=new QTreeWidgetItem(itemColor);//R
      itemColorR->setText(COL_CURVESETTING_INDEX_KEY,"ColorR");
      itemColorR->setText(COL_CURVESETTING_INDEX_VALUE,QString::number(prm.color.red()));
      itemColorR->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"NULL");
      itemColorG=new QTreeWidgetItem(itemColor);//G
      itemColorG->setText(COL_CURVESETTING_INDEX_KEY,"ColorG");
      itemColorG->setText(COL_CURVESETTING_INDEX_VALUE,QString::number(prm.color.green()));
      itemColorG->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"NULL");
      itemColorB=new QTreeWidgetItem(itemColor);//B
      itemColorB->setText(COL_CURVESETTING_INDEX_KEY,"ColorB");
      itemColorB->setText(COL_CURVESETTING_INDEX_VALUE,QString::number(prm.color.blue()));
      itemColorB->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"NULL");
      //AxisNum
      itemAxisNum=new QTreeWidgetItem;
      itemAxisNum->setText(COL_CURVESETTING_INDEX_KEY,"AxisNum");
      itemAxisNum->setText(COL_CURVESETTING_INDEX_VALUE,QString::number(prm.axisNum));
      itemAxisNum->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"NULL");
      //BytesNum
      itemBytesNum=new QTreeWidgetItem;
      itemBytesNum->setText(COL_CURVESETTING_INDEX_KEY,"BytesNum");
      itemBytesNum->setText(COL_CURVESETTING_INDEX_VALUE,QString::number(prm.bytes));
      itemBytesNum->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"NULL");
      //OffsetAddr
      itemOffsetAddr=new QTreeWidgetItem;
      itemOffsetAddr->setText(COL_CURVESETTING_INDEX_KEY,"OffsetAddr");
      itemOffsetAddr->setText(COL_CURVESETTING_INDEX_VALUE,QString::number(prm.offsetAddr));
      itemOffsetAddr->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"NULL");
      //BaseAddr
      itemBaseAddr=new QTreeWidgetItem;
      itemBaseAddr->setText(COL_CURVESETTING_INDEX_KEY,"BaseAddr");
      itemBaseAddr->setText(COL_CURVESETTING_INDEX_VALUE,QString::number(prm.baseAddr));
      itemBaseAddr->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"NULL");
      //AddValue
      itemAddValue=new QTreeWidgetItem;
      itemAddValue->setText(COL_CURVESETTING_INDEX_KEY,"AddValue");
      itemAddValue->setText(COL_CURVESETTING_INDEX_VALUE,QString::number(prm.addValue,'f',2));
      itemAddValue->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"NULL");
      //MultiValue
      itemMultiValue=new QTreeWidgetItem;
      itemMultiValue->setText(COL_CURVESETTING_INDEX_KEY,"MultiValue");
      itemMultiValue->setText(COL_CURVESETTING_INDEX_VALUE,QString::number(prm.multiValue,'f',6));
      itemMultiValue->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"NULL");
      //Xoffset
      itemXoffset=new QTreeWidgetItem;
      itemXoffset->setText(COL_CURVESETTING_INDEX_KEY,"Xoffset");
      itemXoffset->setText(COL_CURVESETTING_INDEX_VALUE,QString::number(prm.xoffset,'f',2));
      itemXoffset->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"NULL");

      if(prm.curveType==CURVE_TYPE_RAW)
      {
        topItem->setText(COL_CURVESETTING_INDEX_VALUE,"0");

        //EquationDescription
        itemEquationDes=new QTreeWidgetItem;
        itemEquationDes->setText(COL_CURVESETTING_INDEX_KEY,"EquationDescription");
        itemEquationDes->setText(COL_CURVESETTING_INDEX_VALUE,"NULL");
        itemEquationDes->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"NULL");

        //PrmFactor
        itemPrmFactor=new QTreeWidgetItem;
        itemPrmFactor->setText(COL_CURVESETTING_INDEX_KEY,"PrmFactor");
        itemPrmFactor->setText(COL_CURVESETTING_INDEX_VALUE,"-1");
        itemPrmFactor->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"VarCount");
        //Factor var
        itemChild1=new QTreeWidgetItem(itemPrmFactor);
        itemChild1->setText(COL_CURVESETTING_INDEX_KEY,"Factor");
        itemChild1->setText(COL_CURVESETTING_INDEX_VALUE,"-1");
        itemChild1->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"PlotPrmFatorTypeID");
        itemChild2=new QTreeWidgetItem(itemChild1);
        itemChild2->setText(COL_CURVESETTING_INDEX_KEY,"FullName");
        itemChild2->setText(COL_CURVESETTING_INDEX_VALUE,"NULL");
        itemChild2->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"NULL");
        itemChild2=new QTreeWidgetItem(itemChild1);
        itemChild2->setText(COL_CURVESETTING_INDEX_KEY,"BytesNum");
        itemChild2->setText(COL_CURVESETTING_INDEX_VALUE,"-1");
        itemChild2->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"NULL");
        itemChild2=new QTreeWidgetItem(itemChild1);
        itemChild2->setText(COL_CURVESETTING_INDEX_KEY,"OffsetAddr");
        itemChild2->setText(COL_CURVESETTING_INDEX_VALUE,"-1");
        itemChild2->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"NULL");
        itemChild2=new QTreeWidgetItem(itemChild1);
        itemChild2->setText(COL_CURVESETTING_INDEX_KEY,"BaseAddr");
        itemChild2->setText(COL_CURVESETTING_INDEX_VALUE,"-1");
        itemChild2->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"NULL");
        //Factor const
        itemChild1=new QTreeWidgetItem(itemPrmFactor);
        itemChild1->setText(COL_CURVESETTING_INDEX_KEY,"Factor");
        itemChild1->setText(COL_CURVESETTING_INDEX_VALUE,"-1");
        itemChild1->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"PlotPrmFatorTypeID");
        itemChild2=new QTreeWidgetItem(itemChild1);
        itemChild2->setText(COL_CURVESETTING_INDEX_KEY,"FullName");
        itemChild2->setText(COL_CURVESETTING_INDEX_VALUE,"NULL");
        itemChild2->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"NULL");
        itemChild2=new QTreeWidgetItem(itemChild1);
        itemChild2->setText(COL_CURVESETTING_INDEX_KEY,"BytesNum");
        itemChild2->setText(COL_CURVESETTING_INDEX_VALUE,"-1");
        itemChild2->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"NULL");
        itemChild2=new QTreeWidgetItem(itemChild1);
        itemChild2->setText(COL_CURVESETTING_INDEX_KEY,"OffsetAddr");
        itemChild2->setText(COL_CURVESETTING_INDEX_VALUE,"-1");
        itemChild2->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"NULL");
        itemChild2=new QTreeWidgetItem(itemChild1);
        itemChild2->setText(COL_CURVESETTING_INDEX_KEY,"BaseAddr");
        itemChild2->setText(COL_CURVESETTING_INDEX_VALUE,"-1");
        itemChild2->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"NULL");

      }
      else if(prm.curveType==CURVE_TYPE_SCRIPT)
      {
        topItem->setText(COL_CURVESETTING_INDEX_VALUE,"1");

        //EquationDescription
        itemEquationDes=new QTreeWidgetItem;
        itemEquationDes->setText(COL_CURVESETTING_INDEX_KEY,"EquationDescription");
        itemEquationDes->setText(COL_CURVESETTING_INDEX_VALUE,prm.equationDescription);
        itemEquationDes->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"NULL");

        //PrmFactor
        itemPrmFactor=new QTreeWidgetItem;
        itemPrmFactor->setText(COL_CURVESETTING_INDEX_KEY,"PrmFactor");
        itemPrmFactor->setText(COL_CURVESETTING_INDEX_VALUE,QString::number(prm.varCount));
        itemPrmFactor->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"VarCount");

        for(int i=0;i<prm.prmFactorList.count();i++)
        {
          itemChild1=new QTreeWidgetItem(itemPrmFactor);
          itemChild1->setText(COL_CURVESETTING_INDEX_KEY,"Factor");
          itemChild1->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"PlotPrmFatorTypeID");
          if(prm.prmFactorList.at(i).factorTypeId==PLOT_PRM_FACTOR_TYPE_VARIABLE)
          {
            //Factor var
            itemChild1->setText(COL_CURVESETTING_INDEX_VALUE,"1");
          }
          else
          {
            //Factor const
            itemChild1->setText(COL_CURVESETTING_INDEX_VALUE,"0");
          }
          itemChild2=new QTreeWidgetItem(itemChild1);
          itemChild2->setText(COL_CURVESETTING_INDEX_KEY,"FullName");
          itemChild2->setText(COL_CURVESETTING_INDEX_VALUE,prm.prmFactorList.at(i).controlPrm.fullName);
          itemChild2->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"NULL");
          itemChild2=new QTreeWidgetItem(itemChild1);
          itemChild2->setText(COL_CURVESETTING_INDEX_KEY,"BytesNum");
          itemChild2->setText(COL_CURVESETTING_INDEX_VALUE,QString::number(prm.prmFactorList.at(i).controlPrm.bytes));
          itemChild2->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"NULL");
          itemChild2=new QTreeWidgetItem(itemChild1);
          itemChild2->setText(COL_CURVESETTING_INDEX_KEY,"OffsetAddr");
          itemChild2->setText(COL_CURVESETTING_INDEX_VALUE,QString::number(prm.prmFactorList.at(i).controlPrm.offsetAddr));
          itemChild2->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"NULL");
          itemChild2=new QTreeWidgetItem(itemChild1);
          itemChild2->setText(COL_CURVESETTING_INDEX_KEY,"BaseAddr");
          itemChild2->setText(COL_CURVESETTING_INDEX_VALUE,QString::number(prm.prmFactorList.at(i).controlPrm.baseAddr));
          itemChild2->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"NULL");
        }
      }
      else if(prm.curveType==CURVE_TYPE_SCRIPT_CONST)
      {
        topItem->setText(COL_CURVESETTING_INDEX_VALUE,"2");

        //EquationDescription
        itemEquationDes=new QTreeWidgetItem;
        itemEquationDes->setText(COL_CURVESETTING_INDEX_KEY,"EquationDescription");
        itemEquationDes->setText(COL_CURVESETTING_INDEX_VALUE,prm.equationDescription);
        itemEquationDes->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"NULL");

        //PrmFactor
        itemPrmFactor=new QTreeWidgetItem;
        itemPrmFactor->setText(COL_CURVESETTING_INDEX_KEY,"PrmFactor");
        itemPrmFactor->setText(COL_CURVESETTING_INDEX_VALUE,"1");
        itemPrmFactor->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"VarCount");
        //Factor var
        itemChild1=new QTreeWidgetItem(itemPrmFactor);
        itemChild1->setText(COL_CURVESETTING_INDEX_KEY,"Factor");
        itemChild1->setText(COL_CURVESETTING_INDEX_VALUE,"1");
        itemChild1->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"PlotPrmFatorTypeID");
        itemChild2=new QTreeWidgetItem(itemChild1);
        itemChild2->setText(COL_CURVESETTING_INDEX_KEY,"FullName");
        itemChild2->setText(COL_CURVESETTING_INDEX_VALUE,prm.prmFactorList.at(0).controlPrm.fullName);
        itemChild2->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"NULL");
        itemChild2=new QTreeWidgetItem(itemChild1);
        itemChild2->setText(COL_CURVESETTING_INDEX_KEY,"BytesNum");
        itemChild2->setText(COL_CURVESETTING_INDEX_VALUE,QString::number(prm.prmFactorList.at(0).controlPrm.bytes));
        itemChild2->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"NULL");
        itemChild2=new QTreeWidgetItem(itemChild1);
        itemChild2->setText(COL_CURVESETTING_INDEX_KEY,"OffsetAddr");
        itemChild2->setText(COL_CURVESETTING_INDEX_VALUE,QString::number(prm.prmFactorList.at(0).controlPrm.offsetAddr));
        itemChild2->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"NULL");
        itemChild2=new QTreeWidgetItem(itemChild1);
        itemChild2->setText(COL_CURVESETTING_INDEX_KEY,"BaseAddr");
        itemChild2->setText(COL_CURVESETTING_INDEX_VALUE,QString::number(prm.prmFactorList.at(0).controlPrm.baseAddr));
        itemChild2->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"NULL");
        //Factor const
        itemChild1=new QTreeWidgetItem(itemPrmFactor);
        itemChild1->setText(COL_CURVESETTING_INDEX_KEY,"Factor");
        itemChild1->setText(COL_CURVESETTING_INDEX_VALUE,"-1");
        itemChild1->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"PlotPrmFatorTypeID");
        itemChild2=new QTreeWidgetItem(itemChild1);
        itemChild2->setText(COL_CURVESETTING_INDEX_KEY,"FullName");
        itemChild2->setText(COL_CURVESETTING_INDEX_VALUE,"NULL");
        itemChild2->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"NULL");
        itemChild2=new QTreeWidgetItem(itemChild1);
        itemChild2->setText(COL_CURVESETTING_INDEX_KEY,"BytesNum");
        itemChild2->setText(COL_CURVESETTING_INDEX_VALUE,"-1");
        itemChild2->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"NULL");
        itemChild2=new QTreeWidgetItem(itemChild1);
        itemChild2->setText(COL_CURVESETTING_INDEX_KEY,"OffsetAddr");
        itemChild2->setText(COL_CURVESETTING_INDEX_VALUE,"-1");
        itemChild2->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"NULL");
        itemChild2=new QTreeWidgetItem(itemChild1);
        itemChild2->setText(COL_CURVESETTING_INDEX_KEY,"BaseAddr");
        itemChild2->setText(COL_CURVESETTING_INDEX_VALUE,"-1");
        itemChild2->setText(COL_CURVESETTING_INDEX_DESCRIPTION,"NULL");
      }
      topItem->addChild(itemCurveId);
      topItem->addChild(itemIsDraw);
      topItem->addChild(itemFullName);
      topItem->addChild(itemColor);
      topItem->addChild(itemAxisNum);
      topItem->addChild(itemBytesNum);
      topItem->addChild(itemOffsetAddr);
      topItem->addChild(itemBaseAddr);
      topItem->addChild(itemAddValue);
      topItem->addChild(itemMultiValue);
      topItem->addChild(itemXoffset);
      topItem->addChild(itemEquationDes);
      topItem->addChild(itemPrmFactor);

    }

    QtTreeManager::writeTreeWidgetToXmlFile(filePath,m_curveSettingTree);//保存曲线界面设置文件
  }
  m_usrCurveTreeManager->saveTreeCurveTemplate(CUSTOM_DEFAULT_WATCHCURVE_USER_PATH);//保存模板文件
}
void PlotWave::clearGraphData()
{
  CurveType curType;
  for(int i=0;i<m_tablePlotPrmList.count();i++)
  {
    curType=m_tablePlotPrmList.at(i).curveType;
    ui->plotCurve->graph(i)->clearData();
    m_tablePlotPrmList[i].data.curveKey.clear();
    m_tablePlotPrmList[i].data.curveValue.clear();
    if(curType==CURVE_TYPE_SCRIPT)
    {
      int varCount=m_tablePlotPrmList[i].varCount;
      for(int j=0;j<varCount;j++)
        m_tablePlotPrmList[i].prmFactorList[j].varDatas.curveValue.clear();
    }
  }
  ui->plotCurve->replot();
}
void PlotWave::drawMorePoints()
{
  evaluateCurveSamplingScript();
  for (int i = 0; i < m_tablePlotPrmList.count(); i++)
  {
    ui->plotCurve->graph(i)->clearData();
    ui->plotCurve->graph(i)->addData(m_tablePlotPrmList[i].data.curveKey, m_tablePlotPrmList[i].data.curveValue);
  }
  ui->plotCurve->replot();
}
void PlotWave::setListViewAxisNum(int axisCount)
{
  disconnect(ui->listWidget_axis,SIGNAL(currentRowChanged(int)),this,SLOT(onListWidgetAxisClicked(int)));
  disconnect(ui->listWidget_axis_tab2,SIGNAL(currentRowChanged(int)),this,SLOT(onUpdateMotionTabUi(int)));

  if(ui->listWidget_axis->count()>0)
  {
    ui->listWidget_axis->clear();
    ui->listWidget_axis_tab2->clear();
  }
  QListWidgetItem *listItem;
  QListWidgetItem *listItem2;
  for(int i=0;i<axisCount;i++)
  {
    listItem=new QListWidgetItem(QIcon(ICON_FILE_PATH+ICON_MOTOR),tr("Axis_%1").arg(i));
    ui->listWidget_axis->addItem(listItem);
    listItem2=listItem->clone();
    ui->listWidget_axis_tab2->addItem(listItem2);
  }
  ui->listWidget_axis->setSelectionMode(QAbstractItemView::ExtendedSelection);
  ui->listWidget_axis_tab2->setSelectionMode(QAbstractItemView::ExtendedSelection);
  ui->listWidget_axis->item(0)->setSelected(true);
  ui->listWidget_axis->setResizeMode(QListView::Adjust);
  ui->listWidget_axis_tab2->setResizeMode(QListView::Adjust);
  ui->listWidget_axis_tab2->item(0)->setSelected(true);

  connect(ui->listWidget_axis,SIGNAL(currentRowChanged(int)),this,SLOT(onListWidgetAxisClicked(int)));
  connect(ui->listWidget_axis_tab2,SIGNAL(currentRowChanged(int)),this,SLOT(onUpdateMotionTabUi(int)));
}
QStringList PlotWave::tableCurveNameList()
{
  QStringList list;
  for(int i=0;i<ui->tableWidget->rowCount();i++)
  {
    list<<tr("(axis_%1)%2").arg(ui->tableWidget->item(i,COL_PRM_TABLE_AXISNUM)->text()).arg(ui->tableWidget->item(i,COL_PRM_TABLE_NAME)->text());
  }
  return list;
}
void PlotWave::saveCurve(QString &fileName, QVector<quint16> &curveIndexVector)
{
  QFile file;
  file.setFileName(fileName);
  if(!file.open(QIODevice::WriteOnly))
  {
    QMessageBox::information(0,tr("file error"),tr("can not open file :\n%1").arg(fileName));
    return;
  }

  quint16 idCount=curveIndexVector.count();
  QDataStream out;
  out.setDevice(&file);
  out.setVersion(QDataStream::Qt_5_5);
  out<<quint16(out.version())<<idCount;

  TableParameters pars;
  foreach (double id, curveIndexVector)
  {
    pars.axisNum=m_tablePlotPrmList.at(id).axisNum;
    pars.bytes=m_tablePlotPrmList.at(id).bytes;
    pars.color=m_tablePlotPrmList.at(id).color;
    pars.fullName=m_tablePlotPrmList.at(id).fullName;
    pars.isDraw=m_tablePlotPrmList.at(id).isDraw;
    pars.offsetAddr=m_tablePlotPrmList.at(id).offsetAddr;
    pars.curveKey=m_tablePlotPrmList.at(id).data.curveKey;
    pars.curveValue=m_tablePlotPrmList.at(id).data.curveValue;
    out<<pars;
  }
  file.close();
}
void PlotWave::addCurve(TableParameters &pars)
{
  //增加数据 1.list 2.table 3.graph
  int rowCount=ui->tableWidget->rowCount();
  QColor color=pars.color;
  PlotTablePrm prm;
  prm.isDraw=pars.isDraw;
  prm.fullName=pars.fullName;
  prm.color=pars.color;
  prm.bytes=pars.bytes;
  prm.axisNum=pars.axisNum;
  prm.offsetAddr=pars.offsetAddr;
  m_tablePlotPrmList.append(prm);//更新数据list
  ui->tableWidget->insertRow(rowCount);//更新到界面的table
  //先断开连接，以免发生不是真正的信号响应
//  disconnect(ui->tableWidget,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(onTableItemChanged(QTableWidgetItem*)));
  setTableRowPrm(rowCount,prm);
//  connect(ui->tableWidget,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(onTableItemChanged(QTableWidgetItem*)));

  ui->plotCurve->addGraph();
  ui->plotCurve->graph(rowCount)->setPen(QPen(color));
  ui->plotCurve->graph(rowCount)->setVisible(pars.isDraw);
  ui->plotCurve->graph(rowCount)->addData(pars.curveKey,pars.curveValue);

  m_tablePlotPrmList[rowCount].data.curveKey=pars.curveKey;
  m_tablePlotPrmList[rowCount].data.curveValue=pars.curveValue;
}
void PlotWave::initialServoStatus(quint16 axixCount)
{
  m_servoStatusList.clear();
  ServoStatus status;
  status.currentModeIndex=0;
  status.isOn=false;
  status.m_closeVel=0;
  status.m_id=0;
  status.m_iq=0;
  status.m_posAdj=0;
  status.m_posFollow=0;
  status.m_profileVel=0;
  status.m_syncVel=0;
  status.m_ua=0;
  status.m_ub=0;
  status.m_uc=0;
  status.m_ud=0;
  status.m_uq=0;
  //初始化伺服状态
  for(int i=0;i<axixCount;i++)
  {
    m_servoStatusList.append(status);
  }
}
void PlotWave::updateOffsetAddrWhenAxisNumberChanged(int axisIndex, int row)
{
  QString name=m_tablePlotPrmList.at(row).fullName;
  QString offset;
  QTreeWidgetItem *itemtree;
  QTreeWidgetItem *itemPrmFactor;
  QTreeWidgetItem *itemFactor;
  QTreeWidget *treeWidget;
  CurveType curveType=m_tablePlotPrmList.at(row).curveType;
  int curveId;
  int totalCount;
  switch (curveType)
  {
  case CURVE_TYPE_RAW://在整个表里面找索引
    mp_treeAll=mp_mainWindow->getRamAllAxisTree();
    itemtree=mp_treeAll->topLevelItem(axisIndex)->clone();
    treeWidget=new QTreeWidget();
    treeWidget->addTopLevelItem(itemtree);

    {
      QTreeWidgetItem *item;
      QTreeWidgetItemIterator it_src(treeWidget);
      while((*it_src))
      {
        item=(*it_src);
        if(name==item->text(COL_NAME))
        {
          offset=item->text(COL_ADDRESS);
          break;
        }
        it_src++;
      }
    }
    treeWidget->clear();
    delete treeWidget;
    m_tablePlotPrmList[row].axisNum=axisIndex;
    m_tablePlotPrmList[row].offsetAddr=offset.toShort();
    ui->tableWidget->item(row,COL_PRM_TABLE_OFFSET)->setText(offset);

    qDebug()<<"set my input offset:"<<offset;
    break;

  case CURVE_TYPE_SCRIPT_CONST://在多轴模板树里面找
    treeWidget=m_usrCurveTreeManager->usrTreeList().at(axisIndex);
    m_tablePlotPrmList[row].axisNum=axisIndex;
    curveId=m_tablePlotPrmList.at(row).id;
    itemtree=treeWidget->topLevelItem(curveId-1);//指向对应曲线的那个树节点信息
    offset=itemtree->child(ROW_PRM_INDEX_PRMFACTOR)->child(0)->child(ROW_FACTOR_INDEX_OFFSETADDR)->text(COL_CURVESETTING_INDEX_VALUE);
    m_tablePlotPrmList[row].prmFactorList[0].controlPrm.offsetAddr=offset.toUShort();
    ui->tableWidget->item(row,COL_PRM_TABLE_OFFSET)->setText(offset);
    qDebug()<<"script const:"<<offset.toUShort();
    break;
  case CURVE_TYPE_SCRIPT://在多轴模板树里面找
    treeWidget=m_usrCurveTreeManager->usrTreeList().at(axisIndex);
    m_tablePlotPrmList[row].axisNum=axisIndex;
    curveId=m_tablePlotPrmList.at(row).id;
    itemtree=treeWidget->topLevelItem(curveId-1);//指向对应曲线的那个树节点信息
    //改变V变量地址
    //改变C变量地址
    itemPrmFactor=itemtree->child(ROW_PRM_INDEX_PRMFACTOR);

    totalCount=m_tablePlotPrmList[row].prmFactorList.count();
    if(totalCount!=itemPrmFactor->childCount())
    {
      qDebug()<<"error in list count";
      return;
    }

    for(int i=0;i<itemPrmFactor->childCount();i++)
    {
      itemFactor=itemPrmFactor->child(i);
      offset=itemFactor->child(ROW_FACTOR_INDEX_OFFSETADDR)->text(COL_CURVESETTING_INDEX_VALUE);
      m_tablePlotPrmList[row].prmFactorList[i].controlPrm.offsetAddr=offset.toUShort();
      qDebug()<<itemFactor->child(ROW_FACTOR_INDEX_FULLNAME)->text(COL_CURVESETTING_INDEX_VALUE)<<"="<<offset.toUShort();
    }
    ui->tableWidget->item(row,COL_PRM_TABLE_OFFSET)->setText(offset);
    break;
  }
  ui->tableWidget->item(row,COL_PRM_TABLE_AXISNUM)->setText(QString::number(axisIndex));
}
void PlotWave::addTableMenuAction(quint16 axisCount)
{
  QAction *act;
  for(int i=0;i<axisCount;i++)
  {
    act=new QAction(tr("Axis_%1").arg(i),m_popuMenuTable);
    act->setData(i);
    connect(act,SIGNAL(triggered(bool)),this,SLOT(onTablePopuMenuClicked()));
    m_popuMenuTable->addAction(act);
  }
  m_popuMenuTable->addSeparator();
  act=new QAction(tr("Copy"),m_popuMenuTable);
  act->setData("copy");
  connect(act,SIGNAL(triggered(bool)),this,SLOT(onTablePopuMenuClicked()));
  m_popuMenuTable->addAction(act);
}
void PlotWave::clearTableMenuAction()
{
//  int count=m_actAxisNumberList.count();
//  for(int i=0;i<count;i++)
//  {
//    delete m_actAxisNumberList[i];
//  }
//  m_actAxisNumberList.clear();
  m_popuMenuTable->clear();
}
void PlotWave::enableUi(bool value)
{
  ui->btn_next->setEnabled(value);
  ui->btn_prev->setEnabled(value);
  ui->btn_tag->setEnabled(value);
  ui->btn_ver_measure->setEnabled(value);
  ui->btn_hori_measure->setEnabled(value);
  ui->btn_open->setEnabled(value);
  ui->btn_save->setEnabled(value);
  ui->btn_fft->setEnabled(value);
  ui->btn_add->setEnabled(value);
  ui->btn_remove->setEnabled(value);
  ui->btn_clear->setEnabled(value);
  ui->lineEdit_sampling->setEnabled(value);
  m_popuMenuTable->setEnabled(value);
}

void PlotWave::changeUiWhenOpenClicked(bool checked)
{
  ui->btn_save->setEnabled(checked);
  ui->btn_fft->setEnabled(checked);
  ui->btn_add->setEnabled(checked);
  ui->btn_remove->setEnabled(checked);
  ui->btn_clear->setEnabled(checked);
  ui->lineEdit_sampling->setEnabled(checked);
  m_popuMenuTable->setEnabled(checked);
  ui->btn_start->setEnabled(checked);
  ui->btn_motionRun->setEnabled(checked);
  ui->btn_servo_switch->setEnabled(checked);
  ui->tabWidget->setEnabled(checked);
}
void PlotWave::setCurveTableWidgetOnOffUi(int row,bool isON)
{
  if(isON)
  {
    ui->tableWidget->item(row,COL_PRM_TABLE_DRAW)->setText(tr("on"));
    ui->tableWidget->item(row,COL_PRM_TABLE_DRAW)->setBackgroundColor(QColor(Qt::green).darker());
  }
  else
  {
    ui->tableWidget->item(row,COL_PRM_TABLE_DRAW)->setText(tr("off"));
    ui->tableWidget->item(row,COL_PRM_TABLE_DRAW)->setBackgroundColor(QColor(Qt::gray));
  }
}

void PlotWave::clearTreeWidgetList(QList<QTreeWidget *> &treeList)
{
  QTreeWidget *treeWidget;
  for(int i=0;i<treeList.count();i++)
  {
    treeWidget=treeList[i];
    treeWidget->clear();
    delete treeWidget;
  }
  treeList.clear();
}
void PlotWave::changeUsrTreeBySrcTree(QList<QTreeWidget *>&usrCurveTreeList,const QTreeWidget *srcAllTree)
{
  if(srcAllTree->topLevelItemCount()!=usrCurveTreeList.count())
    return;

  for(int i=0;i<srcAllTree->topLevelItemCount();i++)
  {
    QTreeWidget *treeAxis=new QTreeWidget;
    QTreeWidget *treeUsr=usrCurveTreeList[i];//第i轴曲线模板
    QTreeWidgetItem *item;
    QTreeWidgetItem *itemPrmFactor;
    QTreeWidgetItem *itemFactor;
    item=srcAllTree->topLevelItem(i)->clone();
    treeAxis->addTopLevelItem(item);//第i个轴的ram树
    for(int j=0;j<treeUsr->topLevelItemCount();j++)
    {
      item=treeUsr->topLevelItem(j);//第i轴第j条曲线
      item->child(ROW_PRM_INDEX_AXISNUM)->setText(COL_CURVESETTING_INDEX_VALUE,QString::number(i));//更新成对应的轴号AxisNum

      CurveType curveType;
      curveType=(CurveType)item->text(COL_CURVESETTING_INDEX_VALUE).toUInt();

      QTreeWidgetItem *itemSrc;
      QTreeWidgetItemIterator itSrc(treeAxis);
      QString offset;
      QString name;

      if(curveType==CURVE_TYPE_SCRIPT_CONST)
      {
        itemPrmFactor=item->child(ROW_PRM_INDEX_PRMFACTOR);
        itemFactor=itemPrmFactor->child(0);//只有一个变量
        name=itemFactor->child(ROW_FACTOR_INDEX_FULLNAME)->text(COL_CURVESETTING_INDEX_VALUE);//FullName
        while((*itSrc))
        {
          itemSrc=(*itSrc);
          if(name==itemSrc->text(COL_NAME))
          {
            offset=itemSrc->text(COL_ADDRESS);
            qDebug()<<"--find-- "<<itemSrc->text(COL_NAME)<<"offset "<<offset;
            break;
          }
          itSrc++;
        }
        itemFactor->child(ROW_FACTOR_INDEX_OFFSETADDR)->setText(COL_CURVESETTING_INDEX_VALUE,offset);
      }
      else//脚本类型
      {
        itemPrmFactor=item->child(ROW_PRM_INDEX_PRMFACTOR);
        for(int k=0;k<itemPrmFactor->childCount();k++)
        {
          itemFactor=itemPrmFactor->child(k);
          name=itemFactor->child(ROW_FACTOR_INDEX_FULLNAME)->text(COL_CURVESETTING_INDEX_VALUE);
          QTreeWidgetItemIterator itSrc(treeAxis);//重新定位到树开始
          while((*itSrc))
          {
            itemSrc=(*itSrc);
            if(name==itemSrc->text(COL_NAME))
            {
              offset=itemSrc->text(COL_ADDRESS);
              qDebug()<<"--find-- "<<itemSrc->text(COL_NAME)<<"offset "<<offset;
              break;
            }
            itSrc++;
          }
          itemFactor->child(ROW_FACTOR_INDEX_OFFSETADDR)->setText(COL_CURVESETTING_INDEX_VALUE,offset);
          //目前这几种中都是已知的，暂时不对BaseAddr处理，在里面写0-gServo 而 1-gAux后面遇到再考虑
        }
      }
    }
    treeAxis->clear();
    delete treeAxis;
  }
}

/**
 * @brief PlotWave::drawCurveNumberIsBigger
 * @param compareNum
 * @param totalCurveNum 返回实际的曲线数量
 * @return
 */
bool PlotWave::drawCurveNumberIsBigger(int compareNum,int &totalCurveNum)
{
  int number=0;
  bool ret=false;
  PlotTablePrm prm;
  foreach (prm, m_tablePlotPrmList)
  {
    if(prm.curveType==CURVE_TYPE_SCRIPT)
    {
      number+=prm.varCount;
    }
    else
    {
      number++;
    }
  }
  qDebug()<<"curve number count:"<<number;
  ret=(number>compareNum)?true:false;
  totalCurveNum=number;

  return ret;
}

double PlotWave::readRAMValue(int axisIndex, quint16 offsetAddr,quint16 baseAddr,int bytsNum)
{
  double value=0;
  int rev;
  int comtype=mp_userConfig->com.id;
  int station=mp_userConfig->com.rnStation;
  int16 value16 = 0;
  int32 value32 = 0;
  int64 value64 = 0;

  switch (bytsNum)
  {
  case 2:
    rev=GTSD_CMD_Read16BitByAdr(axisIndex, (int16)offsetAddr, &value16,(void*)(&baseAddr),(int16) comtype,station);
    if(rev!=0)
      GTSD_CMD_Read16BitByAdr(axisIndex, (int16)offsetAddr, &value16,(void*)(&baseAddr),(int16) comtype,station);
    value=value16;
    break;
  case 4:
    rev=GTSD_CMD_Read32BitByAdr(axisIndex, (int16)offsetAddr, &value32, (void*)(&baseAddr),(int16)comtype,station);
    if(rev!=0)
      GTSD_CMD_Read32BitByAdr(axisIndex, (int16)offsetAddr, &value32, (void*)(&baseAddr),(int16)comtype,station);
    value=value32;
    break;
  case 8:
    rev=GTSD_CMD_Read64BitByAdr(axisIndex, (int16)offsetAddr, &value64,(void*)(&baseAddr), (int16)comtype,station);
    if(rev!=0)
      GTSD_CMD_Read64BitByAdr(axisIndex, (int16)offsetAddr, &value64,(void*)(&baseAddr), (int16)comtype,station);
    value=value64;
    break;
  }

  return value;
}

void PlotWave::setBtnMotionRunEnable(MotionStatus::MotionMode motionMode, MotionStatus::ControlSource ctlSrcIndex)
{
  if((ctlSrcIndex==MotionStatus::SOURCE_GLINK2)||(motionMode==MotionStatus::MOTION_MODE_NONE))
    ui->btn_motionRun->setEnabled(false);
  else
    ui->btn_motionRun->setEnabled(true);
}

UsrCurveTreeManager *PlotWave::getUsrCurveTreeManager() const
{
  return m_usrCurveTreeManager;
}

MainWindow *PlotWave::getMainWindow() const
{
  return mp_mainWindow;
}
/**
 * @brief PlotWave::resetPlotWarve
 * 在主界面关闭连接时调用：1关运动 2关画图
 */
void PlotWave::resetPlotWarve()
{
  qDebug()<<"reset plotwarve";
  if(ui->btn_motionRun->isChecked())
  {
    onBtnMotionRunClicked(false);
    ui->btn_motionRun->setChecked(false);
  }
  if(m_threadSample!=NULL)
  {
    disconnect(m_threadSample,SIGNAL(sendPlotData(int,int,long,CurveData)),this,SLOT(onSendPlotData(int,int,long,CurveData)));

    m_threadSample->stop();
    m_threadSample->wait();
    m_threadSample->deleteLater();
    m_threadSample=NULL;
    enableUi(true);
    qDebug()<<"close sampling curve";
    ui->btn_start->setChecked(false);
  }
}

quint16 PlotWave::getCurveBaseAddress(const QString&name)
{
  quint16 baseAddr=0;
  QMap<QString,QVariant>*dataMapPtr;
  dataMapPtr=mp_mainWindow->getModuleShareMapData();//获得共享数据
  QStringList nameList=name.split(".");

  if(dataMapPtr->contains(nameList.at(0)))
  {
    baseAddr=(quint16)dataMapPtr->value(nameList.at(0)).toUInt();
  }
  else
  {
    QMessageBox::information(0,tr("information"),tr("PrmExtendtion file load error !"));
  }
  return baseAddr;
}

QTreeWidget *PlotWave::getUserCurveTree() const
{
  return m_usrCurveTreeTemplate;
}

void PlotWave::onUpdateMotionTabUi(int axisIndex)
{
  if(mp_mainWindow->getComOpenState())
  {
    MotionStatus::ControlSource ctlSrcIndex;
    MotionStatus::MotionMode motionMode;
    ctlSrcIndex=m_motionStatusPtrList[axisIndex]->controlSrc();
    setControlSourceIndexUi(ctlSrcIndex);
    motionMode=m_motionStatusPtrList[axisIndex]->motionMode();
//    if(motionMode!=MotionStatus::MOTION_MODE_NONE)
    setMotionModeIndexUi(axisIndex,motionMode);

    //设置go开关的使能状态
    setBtnMotionRunEnable(motionMode, ctlSrcIndex);
  }
}
void PlotWave::onListWidgetMotionSrcTab2ItemClicked(QListWidgetItem *item)
{
  if(mp_mainWindow->getComOpenState())
  {
    int index=ui->listWidget_motionSrc_tab2->row(item);//点击了哪一个运动源
    int axisIndex=ui->listWidget_axis_tab2->currentRow();//当前的轴号
    QString str="color:black";
    MotionStatus::MotionMode mode=(MotionStatus::MotionMode)index;
    qDebug()<<index;
    for(int i=0;i<ui->listWidget_axis_tab2->count();i++)
    {
      if(ui->listWidget_axis_tab2->item(i)->isSelected())
      {
        m_motionStatusPtrList.at(i)->setMotionMode(mode);
      }
    }
    switch(mode)
    {
    case MotionStatus::MOTION_MODE_VEL_CLOSE:
//      for(int i=0;i<ui->listWidget_axis_tab2->count();i++)
//      {
//        if(ui->listWidget_axis_tab2->item(i)->isSelected())
//        {
//          m_motionStatusPtrList.at(i)->setMotionMode(mode);
//        }
//      }
      ui->stackedWidget_velocity->setCurrentIndex( m_motionStatusPtrList[axisIndex]->currentPlanVelMode());
      ui->checkBox_vclCircleSwitch->setChecked(m_motionStatusPtrList[axisIndex]->currentPlanVelMode());
      ui->dSBox_ampCircle->setValue(m_motionStatusPtrList[axisIndex]->AMPCircleValue());
      ui->dSBox_ampNoCircle->setValue(m_motionStatusPtrList[axisIndex]->AMPNoCircleValue());
      ui->dSBox_circleCount->setValue(m_motionStatusPtrList[axisIndex]->countCircleValue());
      ui->dSBox_T->setValue(m_motionStatusPtrList[axisIndex]->TcircleValue());
      ui->dSBox_time->setValue(m_motionStatusPtrList[axisIndex]->timesNoCircleValue());

      ui->dSBox_ampCircle->setStyleSheet(str);
      ui->dSBox_ampNoCircle->setStyleSheet(str);
      ui->dSBox_circleCount->setStyleSheet(str);
      ui->dSBox_T->setStyleSheet(str);
      ui->dSBox_time->setStyleSheet(str);
      break;

    case MotionStatus::MOTION_MODE_POS:
      break;
    case MotionStatus::MOTION_MODE_NONE:
      break;

    }
    setBtnMotionRunEnable(mode, MotionStatus::SOURCE_PC);
  }
}

void PlotWave::onBtnConrtolSourceGLink2Clicked()
{
  if(mp_mainWindow->getComOpenState())
  {
    for(int i=0;i<ui->listWidget_axis_tab2->count();i++)
    {
      if(ui->listWidget_axis_tab2->item(i)->isSelected())
      {
        m_motionStatusPtrList.at(i)->setControlSrc(MotionStatus::SOURCE_GLINK2);
        if(m_motionStatusPtrList.at(i)->controlSrc()!=MotionStatus::SOURCE_GLINK2)
        {
          ui->btn_srcPC->setChecked(false);
        }
      }
    }
  }
  else
    ui->btn_srcPC->setChecked(false);
  ui->stackedWidget_sourceCmd->setCurrentIndex(1);
  ui->btn_motionRun->setChecked(false);
  ui->btn_motionRun->setEnabled(false);
}
void PlotWave::onCheckBoxVelocityClicked(bool checked)
{
  MotionStatus::PlanVelMode velMode;
  if(checked)
  {
    ui->stackedWidget_velocity->setCurrentIndex(1);
    velMode=MotionStatus::VELOCITY_CIRCLE;
  }
  else
  {
    ui->stackedWidget_velocity->setCurrentIndex(0);
    velMode=MotionStatus::VELOCITY_NOCIRCLE;
  }
  for(int i=0;i<ui->listWidget_axis_tab2->count();i++)
  {
    if(ui->listWidget_axis_tab2->item(i)->isSelected())
    {
      m_motionStatusPtrList[i]->setPlanVelMode(velMode);
    }
  }
}

void PlotWave::onBtnMotionRunClicked(bool run)
{
//  qDebug()<<"onBtnMotionRunClicked";
//  qDebug()<<"run:"<<run;
  if(mp_mainWindow->getComOpenState()==false)
  {
    ui->btn_motionRun->setChecked(false);
    return;
  }
  ui->widget_progressGo->setHidden(false);
  //如果处于测量模式，则退出测量模式
  if(ui->btn_hori_measure->isChecked())
  {
    ui->btn_hori_measure->setChecked(false);
    onBtnHorizontalMeasureClicked(false);
  }
  if(ui->btn_ver_measure->isChecked())
  {
    ui->btn_ver_measure->setChecked(false);
    onBtnVerticalMeasureClicked(false);
  }
  if(run)
  {
    //开画图
    if(((ui->btn_start->isChecked())==false)&&(ui->btn_open->isChecked()==false))//当前状态处于关闭时和不处于查看旧曲线时，打开画图
    {
      ui->btn_start->setChecked(true);
      onBtnStartClicked();
    }
    m_motionStatusPtrList.at(0)->delayms(500);
    //1准备 2开伺服
    for(int i=0;i<m_motionStatusPtrList.count();i++)
    {
      m_progressBarList.at(i)->setHidden(true);
      if(ui->listWidget_axis_tab2->item(i)->isSelected())
      {
        m_progressBarList.at(i)->setHidden(false);
        m_progressBarList.at(i)->setValue(0);
        m_motionStatusPtrList.at(i)->setRunningPrepare();
        m_motionStatusPtrList.at(i)->setServoOn(true);
      }
    }
    m_motionStatusPtrList.at(0)->delayms(100);

    //开始跑：设置伺服速度，开定时器
    bool servoAllSelectOn=true;
    for(int i=0;i<m_motionStatusPtrList.count();i++)
    {
      if(ui->listWidget_axis_tab2->item(i)->isSelected())
      {
        m_motionStatusPtrList.at(i)->setRunning(true);
        ui->btn_motionRun->setChecked(m_motionStatusPtrList.at(i)->running());
        if(false==m_motionStatusPtrList.at(i)->running())
          servoAllSelectOn=false;
      }
    }
    //检查伺服是否全开，只要有一个没有开，则不画图，关完伺服。
    if(!servoAllSelectOn)
    {
      //关伺服
      ui->widget_progressGo->setHidden(true);
      for(int i=0;i<m_motionStatusPtrList.count();i++)
      {
        if(ui->listWidget_axis_tab2->item(i)->isSelected())
        {
          m_motionStatusPtrList.at(i)->setRunning(false);
          m_motionStatusPtrList.at(0)->delayms(50);
          m_motionStatusPtrList.at(i)->setServoOn(false);
        }
      }
      if((ui->btn_start->isChecked())==true)
      {
        ui->btn_start->setChecked(false);
        onBtnStartClicked();
      }
    }

  }

  else//中间关伺服操作
  {
    ui->widget_progressGo->setHidden(true);
    for(int i=0;i<m_motionStatusPtrList.count();i++)
    {
      if(ui->listWidget_axis_tab2->item(i)->isSelected())
      {
        m_motionStatusPtrList.at(i)->setRunning(false);
        m_motionStatusPtrList.at(0)->delayms(50);
        m_motionStatusPtrList.at(i)->setServoOn(false);
      }
    }
  }
}
void PlotWave::onMotionStatusPlanVelTimeOut(int axisIndex)
{
  static quint8 axisInSum=0;
  axisInSum++;
  qDebug()<<"axis sum:"<<axisInSum;
  qDebug()<<"input axis_"<<axisIndex<<" set run spd=0";
  if(axisInSum>=ui->listWidget_axis_tab2->selectedItems().count())
  {
    m_motionStatusPtrList.at(0)->delayms(50);
    qDebug()<<"set servo off";
    for(int i=0;i<m_motionStatusPtrList.count();i++)
    {
      if(ui->listWidget_axis_tab2->item(i)->isSelected())
        m_motionStatusPtrList.at(i)->setServoOn(false);
    }
    ui->btn_motionRun->setChecked(false);
    ui->widget_progressGo->setHidden(true);
    axisInSum=0;

    //关闭采样
    if(ui->btn_start->isChecked())
    {
      m_motionStatusPtrList.at(0)->delayms(100);
      ui->btn_start->setChecked(false);
      onBtnStartClicked();
    }
    qDebug()<<"set sampling curve off";
  }
}

void PlotWave::onBtnControlSourcePCClicked()
{
  if(mp_mainWindow->getComOpenState())
  {
    for(int i=0;i<ui->listWidget_axis_tab2->count();i++)
    {
      if(ui->listWidget_axis_tab2->item(i)->isSelected())
      {
        m_motionStatusPtrList.at(i)->setControlSrc(MotionStatus::SOURCE_PC);
        if(m_motionStatusPtrList.at(i)->controlSrc()!=MotionStatus::SOURCE_PC)
        {
          ui->btn_srcGLink2->setChecked(false);
        }
      }
    }
  }
  else
    ui->btn_srcGLink2->setChecked(false);
  ui->listWidget_motionSrc_tab2->setCurrentRow(0);
  ui->stackedWidget_sourceCmd->setCurrentIndex(0);
}

void PlotWave::setControlSourceIndexUi(int index)
{
  switch (index) {
  case MotionStatus::SOURCE_PC:
    ui->btn_srcPC->setChecked(true);
    break;
  case MotionStatus::SOURCE_GLINK2:
    ui->btn_srcGLink2->setChecked(true);
    break;
  default:
    break;
  }
  ui->stackedWidget_sourceCmd->setCurrentIndex(index);
}
void PlotWave::setMotionModeIndexUi(int axisIndex,MotionStatus::MotionMode motionMode)
{
  ui->listWidget_motionSrc_tab2->setCurrentRow(motionMode);
//  MotionStatus::MotionMode motionMode=(MotionStatus::MotionMode)(index);
  //还原相应界面的数据
  QString str="color:black";
  switch(motionMode)
  {
  case MotionStatus::MOTION_MODE_VEL_CLOSE:
    ui->dSBox_ampCircle->setValue(m_motionStatusPtrList[axisIndex]->AMPCircleValue());
    ui->dSBox_ampNoCircle->setValue(m_motionStatusPtrList[axisIndex]->AMPNoCircleValue());
    ui->dSBox_circleCount->setValue(m_motionStatusPtrList[axisIndex]->countCircleValue());
    ui->dSBox_T->setValue(m_motionStatusPtrList[axisIndex]->TcircleValue());
    ui->dSBox_time->setValue(m_motionStatusPtrList[axisIndex]->timesNoCircleValue());

    ui->dSBox_ampCircle->setStyleSheet(str);
    ui->dSBox_ampNoCircle->setStyleSheet(str);
    ui->dSBox_circleCount->setStyleSheet(str);
    ui->dSBox_T->setStyleSheet(str);
    ui->dSBox_time->setStyleSheet(str);
    if(m_motionStatusPtrList[axisIndex]->currentPlanVelMode()==MotionStatus::VELOCITY_NOCIRCLE)
    {
      ui->checkBox_vclCircleSwitch->setChecked(false);
      ui->stackedWidget_velocity->setCurrentIndex(0);
    }
    else
    {
      ui->checkBox_vclCircleSwitch->setChecked(true);
      ui->stackedWidget_velocity->setCurrentIndex(1);
    }
    break;
  case MotionStatus::MOTION_MODE_POS:
    break;

  }
}
