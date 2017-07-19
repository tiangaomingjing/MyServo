#include "dftwidget.h"
#include "ui_dftwidget.h"
#include "dftaddform.h"
#include "plotwave.h"
#include "ServoDriverAlgorithmDll/ServoDriverAlgorithmDll.h"
#include "boxzoom.h"
#include "viewform.h"

#include <QTableWidgetItem>

DftWidget::DftWidget(PlotWave *plot, QWidget *parent) :
  QWidget(parent),
  mp_plotWave(plot),
  m_addForm(NULL),
  m_piRatio(57.3),
  m_boxZoom1(NULL),
  m_boxZoom2(NULL),
  m_viewForm(NULL),
  ui(new Ui::DftWidget)
{
  ui->setupUi(this);

  ui->btn_add->setText(tr("add"));
  ui->btn_amp->setText(tr("AMP"));
  ui->btn_clear->setText(tr("clear"));
  ui->btn_fitall->setText(tr("FIT"));
  ui->btn_open->setText(tr("open"));
  ui->btn_pha->setText(tr("PHA"));
  ui->btn_remove->setText(tr("remove"));
  ui->btn_save->setText(tr("save"));
  ui->btn_all->setText(tr("all"));

  m_fftInformation.fftType=FFT_SIGNAL;
  m_fftInformation.curveSelectIndex=0;

  QPen pen;
  pen.setColor(QColor(Qt::gray));
  pen.setStyle(Qt::DotLine);
  //plot ui
  ui->dftPlot->xAxis->setRange(1e-5,1e10);
  ui->dftPlot->xAxis->setScaleType(QCPAxis::stLogarithmic);
  ui->dftPlot->xAxis->setScaleLogBase(10);
  ui->dftPlot->xAxis->setNumberFormat("eb");
  ui->dftPlot->xAxis->setNumberPrecision(0);
  ui->dftPlot->xAxis->setSubTickCount(8);
  ui->dftPlot->xAxis->grid()->setSubGridVisible(true);
  ui->dftPlot->xAxis->grid()->setSubGridPen(pen);
  ui->dftPlot->yAxis->grid()->setSubGridPen(pen);
  ui->dftPlot->setBackground(QBrush(QColor(240, 240, 240)));
  ui->dftPlot->setInteraction(QCP::iRangeDrag,true);
  ui->dftPlot->setInteraction(QCP::iRangeZoom,true);
  ui->dftPlot->setInteraction(QCP::iSelectPlottables,false);

  ui->dftPlot_2->xAxis->setRange(1e-5,1e10);
  ui->dftPlot_2->xAxis->setScaleType(QCPAxis::stLogarithmic);
  ui->dftPlot_2->xAxis->setScaleLogBase(10);
  ui->dftPlot_2->xAxis->setNumberFormat("eb");
  ui->dftPlot_2->xAxis->setNumberPrecision(0);
  ui->dftPlot_2->xAxis->setSubTickCount(8);
  ui->dftPlot_2->xAxis->grid()->setSubGridVisible(true);
  ui->dftPlot_2->xAxis->grid()->setSubGridPen(pen);
  ui->dftPlot_2->yAxis->grid()->setSubGridPen(pen);
  ui->dftPlot_2->setBackground(QBrush(QColor(240, 240, 240)));
  ui->dftPlot_2->setInteraction(QCP::iRangeDrag,true);
  ui->dftPlot_2->setInteraction(QCP::iRangeZoom,true);
  ui->dftPlot_2->setInteraction(QCP::iSelectPlottables,true);
  ui->dftPlot_2->hide();
  ui->label_dftph->hide();

  //table ui
  ui->tableWidget->insertColumn(0);
  ui->tableWidget->insertColumn(1);
  ui->tableWidget->insertColumn(2);
  ui->tableWidget->insertColumn(3);
  ui->tableWidget->hideColumn(3);
  ui->tableWidget->setMouseTracking(true);
  QStringList headerList;
  headerList<<tr("Visible")<<tr("CurveName")<<tr("UserNote")<<tr("CurveID");
  ui->tableWidget->setHorizontalHeaderLabels(headerList);
  QString str="QTableWidget::item:selected { background-color: skyblue } \
      QTableView{background-color: lightgray}";
//  ui->tableWidget->setStyleSheet("QTableWidget::item:selected { background-color: lightblue }");//设置选中颜色
  ui->tableWidget->setStyleSheet(str);

  //control ui
  ui->comboBox_output->hide();
  ui->label_output->hide();

  connect(ui->btn_all,SIGNAL(clicked(bool)),this,SLOT(onBtnAllClicked()));
  connect(ui->btn_add,SIGNAL(clicked(bool)),this,SLOT(onBtnAddClicked()));
  connect(ui->btn_remove,SIGNAL(clicked(bool)),this,SLOT(onBtnRemoveClicked()));
  connect(ui->btn_clear,SIGNAL(clicked(bool)),this,SLOT(onBtnClearClicked()));
  connect(ui->btn_amp,SIGNAL(toggled(bool)),this,SLOT(onBtnAmpClicked(bool)));
  connect(ui->btn_pha,SIGNAL(toggled(bool)),this,SLOT(onBtnPhaClicked(bool)));
  connect(ui->btn_fitall,SIGNAL(clicked(bool)),this,SLOT(onBtnFitAllClicked()));
  connect(ui->btn_open,SIGNAL(clicked(bool)),this,SLOT(onBtnOpenClicked()));
  connect(ui->btn_save,SIGNAL(clicked(bool)),this,SLOT(onBtnSaveClicked()));
  connect(ui->btn_view,SIGNAL(clicked(bool)),this,SLOT(onBtnViewClicked()));
  connect(ui->radioButton_signal,SIGNAL(clicked(bool)),this,SLOT(onRadioSignalClicked(bool)));
  connect(ui->radioButton_system,SIGNAL(clicked(bool)),this,SLOT(onRadioSystemClicked(bool)));
  connect(ui->comboBox_input,SIGNAL(activated(int)),this,SLOT(onComboxInputClicked(int)));

  connect(ui->dftPlot,SIGNAL(mouseMove(QMouseEvent*)),this,SLOT(onPlotAmplitudeMouseMoved(QMouseEvent*)));
  connect(ui->dftPlot,SIGNAL(mousePress(QMouseEvent*)),this,SLOT(onPlotAmplitudeMousePressed(QMouseEvent*)));
  connect(ui->dftPlot,SIGNAL(mouseRelease(QMouseEvent*)),this,SLOT(onPlotAmplitudeMouseReleased(QMouseEvent*)));
  connect(ui->dftPlot_2,SIGNAL(mouseMove(QMouseEvent*)),this,SLOT(onPlotPhaseMouseMoved(QMouseEvent*)));
  connect(ui->dftPlot_2,SIGNAL(mousePress(QMouseEvent*)),this,SLOT(onPlotPhaseMousePressed(QMouseEvent*)));
  connect(ui->dftPlot_2,SIGNAL(mouseRelease(QMouseEvent*)),this,SLOT(onPlotPhaseMouseReleased(QMouseEvent*)));

  connect(ui->tableWidget,SIGNAL(itemClicked(QTableWidgetItem*)),this,SLOT(onTableItemClicked(QTableWidgetItem*)));
  connect(ui->tableWidget,SIGNAL(cellEntered(int,int)),this,SLOT(onTableWidgetCellEnteredMoreDetail(int,int)));
}

DftWidget::~DftWidget()
{
  delete ui;
  qDebug()<<"dftwidget release ->";
}

void DftWidget::addCurve(quint16 id,QString &name,QString &fullName,QString &note, QColor &color, bool isShow,FFTResult &fftData)
{
  int rowCount=ui->tableWidget->rowCount();
  ui->tableWidget->insertRow(rowCount);//更新到界面的table
  QTableWidgetItem *item = new QTableWidgetItem();
  item->setText(name);
  item->setTextColor(color);
  ui->tableWidget->setItem(rowCount, 1, item);

  QString showText;
  if(isShow)
    showText=tr("on");
  else
    showText=tr("off");
  item=new QTableWidgetItem();
  item->setText(showText);
  item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
  ui->tableWidget->setItem(rowCount, 0, item);

  item=new QTableWidgetItem();
  item->setText(note);
  ui->tableWidget->setItem(rowCount, 2, item);

  item=new QTableWidgetItem();
  item->setText(QString::number(id));
  ui->tableWidget->setItem(rowCount, 3, item);

  m_curveShowVector.append(isShow);
  m_curveFullName.append(fullName);

  ui->dftPlot->addGraph();
  ui->dftPlot->graph(rowCount)->setPen(QPen(color));
  ui->dftPlot->graph(rowCount)->addData(fftData.freq,fftData.am);
  ui->dftPlot->rescaleAxes(true);

  ui->dftPlot_2->addGraph();
  ui->dftPlot_2->graph(rowCount)->setPen(QPen(color));
  ui->dftPlot_2->graph(rowCount)->addData(fftData.freq,fftData.ph);
  ui->dftPlot_2->rescaleAxes(true);

  ui->tableWidget->resizeColumnsToContents();
  ui->tableWidget->resizeRowsToContents();

  QPen pen;
  pen.setColor(Qt::yellow);
  pen.setWidth(4);
  ui->dftPlot->graph(rowCount)->setSelectedPen(pen);
  ui->dftPlot_2->graph(rowCount)->setSelectedPen(pen);

  if(isShow==false)
  {
    ui->dftPlot->graph(rowCount)->setVisible(false);
    ui->dftPlot_2->graph(rowCount)->setVisible(false);
  }
  ui->dftPlot->replot();
  ui->dftPlot_2->replot();

}

void DftWidget::setComboxContexts(QStringList &stringList)
{
  ui->comboBox_input->clear();
  ui->comboBox_output->clear();
  ui->comboBox_input->addItems(stringList);
  ui->comboBox_output->addItems(stringList);
}
void DftWidget::setPhaseValueStyle(int style)
{
  if(style==0)
    m_piRatio=1;
  else
    m_piRatio=57.3;
}
void DftWidget::setTimeRange(double min, double max)
{
  ui->doubleSpinBox_start->setMinimum(min);
  ui->doubleSpinBox_end->setMaximum(max);
}

//-----------------public slots functions---------------
void DftWidget::onFFTStartTimeChanged(double value)
{
  ui->doubleSpinBox_start->setValue(value);
}
void DftWidget::onFFTEndTimeChanged(double value)
{
  ui->doubleSpinBox_end->setValue(value);
}

//-------------------private slots functions------------------------
void DftWidget::onBtnAllClicked()
{
  bool static flag=true;
  if(flag)
  {
    int row=0;
    QMutableVectorIterator<bool> it(m_curveShowVector);
    while(it.hasNext())
    {
      bool temp=it.next();
      if(!temp)
      {
        it.setValue(true);
        ui->tableWidget->item(row,COL_PRM_TABLE_DRAW)->setText(tr("on"));
      }
      row++;
    }
  }
  else
  {
    int row=0;
    QMutableVectorIterator<bool> it(m_curveShowVector);
    while(it.hasNext())
    {
      bool temp=it.next();
      if(temp)
      {
        it.setValue(false);
        ui->tableWidget->item(row,COL_PRM_TABLE_DRAW)->setText(tr("off"));
      }
      row++;
    }
  }
  flag=!flag;
  int row=0;
  foreach (bool checked, m_curveShowVector)
  {
    if(checked)
    {
      ui->dftPlot->graph(row)->setVisible(true);
      ui->dftPlot_2->graph(row)->setVisible(true);
    }
    else
    {
      ui->dftPlot->graph(row)->setVisible(false);
      ui->dftPlot_2->graph(row)->setVisible(false);
    }
    row++;
  }
  ui->dftPlot->replot();
  ui->dftPlot_2->replot();
}

void DftWidget::onBtnAddClicked()
{
  double startT,endT,min,max;
  startT=ui->doubleSpinBox_start->value();
  endT=ui->doubleSpinBox_end->value();
  min=mp_plotWave->getTableParameterList().at(0).data.curveKey.first();
  max=mp_plotWave->getTableParameterList().at(0).data.curveKey.last();

  if(startT<min)
  {
    qDebug()<<tr("min %1,max %2").arg(min).arg(max);
    QMessageBox::information(0,tr("Range Error"),tr("startTime out range"));
    return;
  }
  if(endT>max)
  {
    qDebug()<<tr("min %1,max %2").arg(min).arg(max);
    QMessageBox::information(0,tr("Range Error"),tr("endTime out range"));
    return;
  }

  //标准化两个输入的上下限
  double start=ui->doubleSpinBox_start->value();
  double end=ui->doubleSpinBox_end->value();
  double temp=0;
  if(end<start)
  {
    temp=start;
    start=end;
    end=temp;
    ui->doubleSpinBox_start->setValue(start);
    ui->doubleSpinBox_end->setValue(end);
  }

  QString name;
  QString fullName;
  QColor color;
  int length;
  int index;
  length=ui->comboBox_input->currentText().length();
  index=ui->comboBox_input->currentText().indexOf(")");

  int length2;
  int index2;

  switch (m_fftInformation.fftType)
  {
  case FFT_SIGNAL:
    m_fftInformation.curveSelectIndex=ui->comboBox_input->currentIndex();
    fullName=tr("%1 ( %2 - %3 s)").arg(ui->comboBox_input->currentText())\
        .arg(QString::number(ui->doubleSpinBox_start->value(),'f',2))\
        .arg(QString::number(ui->doubleSpinBox_end->value(),'f',2));
    name=tr("%1").arg(ui->comboBox_input->currentText().right(length-index-1));
    break;
  case FFT_SYSTEM:
    m_fftInformation.curveSelectIndex=ui->comboBox_input->currentIndex();
    m_fftInformation.curveSelectIndexOut=ui->comboBox_output->currentIndex();
    fullName=tr(">>%1\n<<%2\n( %3 - %4 s)").arg(ui->comboBox_input->currentText())\
        .arg(ui->comboBox_output->currentText())\
        .arg(QString::number(ui->doubleSpinBox_start->value(),'f',2))\
        .arg(QString::number(ui->doubleSpinBox_end->value(),'f',2));
    length2=ui->comboBox_output->currentText().length();
    index2=ui->comboBox_output->currentText().lastIndexOf(")");
    name=tr(">>%1\n<<%2").arg(ui->comboBox_input->currentText().right(length-index-1))\
        .arg(ui->comboBox_output->currentText().right(length2-index2-1));
    break;
  }
  fftAnalyze(m_fftInformation);

  color=mp_plotWave->getTableParameterList().at(m_fftInformation.curveSelectIndex).color;
  addCurve(m_fftInformation.curveSelectIndex,name,fullName,tr(" "),color,true,m_fftResult);
}
void DftWidget::onBtnClearClicked()
{
  if(ui->tableWidget->rowCount()>=0)
  {
    ui->dftPlot->clearGraphs();
    ui->dftPlot_2->clearGraphs();
    m_curveShowVector.clear();
    m_curveFullName.clear();
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->clearContents();
    ui->dftPlot->replot();
    ui->dftPlot_2->replot();
  }
}
void DftWidget::onBtnRemoveClicked()
{
  //清graph
  //清vector
  //清table 这三个量是连的，原数据在m_tableList里
  int row=0;
  row=ui->tableWidget->currentRow();//if select none return -1
  if(row>=0)
  {
    ui->dftPlot->removeGraph(row);
    ui->dftPlot_2->removeGraph(row);
    m_curveShowVector.removeAt(row);
    m_curveFullName.removeAt(row);
    ui->tableWidget->removeRow(row);
    ui->dftPlot->replot();
    ui->dftPlot_2->replot();
  }
}

void DftWidget::onBtnAmpClicked(bool checked)
{
  if(checked)
  {
    ui->dftPlot->show();
    ui->label_dftam->show();
  }
  else
  {
    ui->dftPlot->hide();
    ui->label_dftam->hide();
  }
}

void DftWidget::onBtnPhaClicked(bool checked)
{
  if(checked)
  {
    ui->dftPlot_2->show();
    ui->label_dftph->show();
  }
  else
  {
    ui->dftPlot_2->hide();
    ui->label_dftph->hide();
  }
}

void DftWidget::onBtnFitAllClicked()
{
  ui->dftPlot->rescaleAxes(true);
  ui->dftPlot_2->rescaleAxes(true);
  ui->dftPlot->replot();
  ui->dftPlot_2->replot();
}
void DftWidget::onBtnOpenClicked()
{
  QString fileName;
  fileName = QFileDialog::getOpenFileName(this, tr("Open File"), ".", tr("curve Files(*.dat)"));
  if (fileName.isNull())
  {
    return;
  }

  QFile file;
  file.setFileName(fileName);
  if(false==file.open(QIODevice::ReadOnly))
  {
    QMessageBox::information(0,tr("file error"),tr("can not open file :\n%1").arg(fileName));
    return;
  }
  QDataStream in(&file);
  quint16 streamVersion;
  quint16 curveCount=0;
  in>>streamVersion>>curveCount;
  qDebug()<<"\nversion:"<<streamVersion<<"count:"<<curveCount;
  in.setVersion(streamVersion);

  quint16 id;
  QString curveName;
  QString fullName;
  bool isDraw;
  QColor color;
  QString note;
  QVector<double> keys;
  QVector<double> ams;
  QVector<double> phas;
  for(int i=0;i<curveCount;i++)
  {
    in>>isDraw>>curveName>>fullName>>color>>note>>id>>keys>>ams>>phas;
    FFTResult result;
    result.freq=keys;
    result.am=ams;
    result.ph=phas;
    addCurve(id,curveName,fullName,note,color,isDraw,result);
    keys.clear();
    ams.clear();
    phas.clear();
  }
  qDebug()<<keys.last();

  file.close();
}

void DftWidget::onBtnSaveClicked()
{
  quint16 id;
  QString fileName;
  QString fileNameSrc;
  QString fileNameDefaultQString =tr("curve_fft_")+ QDateTime::currentDateTime().toString("yyyyMMddhhmmss");//默认文件名
  fileName = QFileDialog::getSaveFileName(this, tr("Save curve"), fileNameDefaultQString, tr("curve file (*.dat)"));
  if (fileName.isNull()) return;
  QFileInfo fileInfo;
  fileInfo.setFile(fileName);
  fileNameSrc=fileInfo.path()+"/"+fileInfo.baseName()+".src";//原文件名称

//  qDebug()<<fileName<<"\n"<<fileNameSrc;
//  qDebug()<<"file path is "<<fileInfo.path();
//  qDebug()<<"file name is "<<fileInfo.fileName();
//  qDebug()<<"fileBaseName is "<<fileInfo.baseName();
//  qDebug()<<"fileExtension is "<<fileInfo.suffix();

  QFile file;
  file.setFileName(fileName);
  if(false==file.open(QIODevice::WriteOnly))
  {
    QMessageBox::information(0,tr("file error"),tr("can not open file :\n%1").arg(fileName));
    return;
  }

  QDataStream dataOut;
  quint16 curveCount=0;
  QString curveName;
  QString fullName;
  bool isDraw;
  QColor color;
  QString note;
  QVector<double> keys;
  QVector<double> ams;
  QVector<double> phas;
  QVector<quint16> curveIndexs;
  bool hasSelectCurveFlag=false;

  for(int i=0;i<ui->tableWidget->rowCount();i++)
  {
    if(ui->tableWidget->item(i,COL_PRM_TABLE_NAME)->isSelected())
      curveCount++;
  }

  dataOut.setDevice(&file);
  dataOut.setVersion(QDataStream::Qt_5_5);
  dataOut<<quint16(dataOut.version())<<curveCount;
  qDebug()<<"version:"<<dataOut.version()<<"count:"<<curveCount;
  for(int i=0;i<ui->tableWidget->rowCount();i++)
  {
    if(ui->tableWidget->item(i,COL_PRM_TABLE_NAME)->isSelected())
    {
      id=ui->tableWidget->item(i,3)->text().toUInt();//获得id
      isDraw=m_curveShowVector.at(i);
      curveName=ui->tableWidget->item(i,COL_PRM_TABLE_NAME)->text();
      fullName=m_curveFullName.at(i);
      color=ui->tableWidget->item(i,COL_PRM_TABLE_NAME)->textColor();
      note=ui->tableWidget->item(i,2)->text();
      QCPDataMapIterator dataMapIt(*(ui->dftPlot->graph(i)->data()));
      QCPDataMapIterator dataMapIt2(*(ui->dftPlot_2->graph(i)->data()));
      while(dataMapIt.hasNext())
      {
        QCPData cpdata,cpdata2;
        dataMapIt.next();
        dataMapIt2.next();
        cpdata=dataMapIt.value();
        cpdata2=dataMapIt2.value();
        keys.append(cpdata.key);
        ams.append(cpdata.value);
        phas.append(cpdata2.value);
      }
      dataOut<<isDraw<<curveName<<fullName<<color<<note<<id<<keys<<ams<<phas;
      curveIndexs.append(id);
      keys.clear();
      ams.clear();
      phas.clear();
      hasSelectCurveFlag=true;
    }
  }
  if(hasSelectCurveFlag)
    emit saveSourceCurve(fileNameSrc,curveIndexs);
  file.close();

}

void DftWidget::onBtnViewClicked()
{
  m_viewForm=new ViewForm(0);
  connect(m_viewForm,SIGNAL(setPlotViewRange(QVector<double>&)),this,SLOT(onSetPlotViewRange(QVector<double>&)));
  m_viewForm->show();
}

void DftWidget::onRadioSignalClicked(bool checked)
{
  if(checked)
  {
    ui->comboBox_output->hide();
    ui->label_output->hide();
    m_fftInformation.fftType=FFT_SIGNAL;
  }
}
void DftWidget::onRadioSystemClicked(bool checked)
{
  if(checked)
  {
    ui->comboBox_output->show();
    ui->label_output->show();
    m_fftInformation.fftType=FFT_SYSTEM;
  }
}

void DftWidget::onPlotAmplitudeMouseMoved(QMouseEvent *event)
{
  //-----------------设置鼠标坐标----------
  //当前鼠标位置坐标
  double dx = ui->dftPlot->xAxis->pixelToCoord(event->x());
  double dy = ui->dftPlot->yAxis->pixelToCoord(event->y());
  //当前坐标轴范围
  QCPRange cpX = ui->dftPlot->xAxis->range();		//X坐标轴范围
  QCPRange cpY = ui->dftPlot->yAxis->range();		//Y坐标轴范围
  //不在当前坐标轴范围内
  if(cpX.contains(dx)&&cpY.contains(dy))
  {
    ui->label_dftam->setText(tr("Amplitude Information:  frequency: %1 HZ , AMP: %2 DB")\
                                     .arg(QString::number(dx, 'E', 4))\
                                     .arg(QString::number(dy, 'f', 2)));
  }

  //设置是否框选放大
  if((event->buttons()&Qt::LeftButton)&&(QApplication::keyboardModifiers()==Qt::ControlModifier))
  {
    if(m_boxZoom1!=NULL)
    {
      m_boxZoom1->setEndPointF(event->x(),event->y());
      m_boxZoom1->updateBoxZoomShape();
      m_boxZoom1->setMouseIsMoved();
    }
  }
}
void DftWidget::onPlotAmplitudeMousePressed(QMouseEvent *event)
{
  if((event->button()==Qt::LeftButton)&&(QApplication::keyboardModifiers()==Qt::ControlModifier))
  {
    if(m_boxZoom1==NULL)
    {
      qDebug()<<"boxZoom initial ";
      m_boxZoom1=new BoxZoom(ui->dftPlot);
      ui->dftPlot->addPlottable(m_boxZoom1);
    }
    m_boxZoom1->disablePlotViewMove();
    m_boxZoom1->setVisible(true);
    m_boxZoom1->setStartPointF(event->x(),event->y());
  }
}
void DftWidget::onPlotAmplitudeMouseReleased(QMouseEvent *event)
{
  Q_UNUSED(event);
  if((m_boxZoom1!=NULL)&&m_boxZoom1->isMouseMoved())
  {
    m_boxZoom1->enablePlotViewMove();
    m_boxZoom1->updatePlotViewSize();
    m_boxZoom1->setVisible(false);
    m_boxZoom1->clearMouseIsMoved();
    qDebug()<<"mouse boxzoom release ";
  }
}

void DftWidget::onPlotPhaseMouseMoved(QMouseEvent *event)
{
  //-----------------设置鼠标坐标----------
  //当前鼠标位置坐标
  double dx = ui->dftPlot_2->xAxis->pixelToCoord(event->x());
  double dy = ui->dftPlot_2->yAxis->pixelToCoord(event->y());
  //当前坐标轴范围
  QCPRange cpX = ui->dftPlot_2->xAxis->range();		//X坐标轴范围
  QCPRange cpY = ui->dftPlot_2->yAxis->range();		//Y坐标轴范围
  //不在当前坐标轴范围内
  if(cpX.contains(dx)&&cpY.contains(dy))
  {
    ui->label_dftph->setText(tr("Phase Information:  frequency: %1 HZ , PHA: %2 degree")\
                                     .arg(QString::number(dx, 'E', 4))\
                                     .arg(QString::number(dy, 'f', 2)));
  }

  //设置是否框选放大
  if((event->buttons()&Qt::LeftButton)&&(QApplication::keyboardModifiers()==Qt::ControlModifier))
  {
    if(m_boxZoom2!=NULL)
    {
      m_boxZoom2->setEndPointF(event->x(),event->y());
      m_boxZoom2->updateBoxZoomShape();
      m_boxZoom2->setMouseIsMoved();
    }
  }
}

void DftWidget::onPlotPhaseMousePressed(QMouseEvent *event)
{
  if((event->button()==Qt::LeftButton)&&(QApplication::keyboardModifiers()==Qt::ControlModifier))
  {
    if(m_boxZoom2==NULL)
    {
      qDebug()<<"boxZoom initial ";
      m_boxZoom2=new BoxZoom(ui->dftPlot_2);
      ui->dftPlot_2->addPlottable(m_boxZoom2);
    }
    m_boxZoom2->disablePlotViewMove();
    m_boxZoom2->setVisible(true);
    m_boxZoom2->setStartPointF(event->x(),event->y());
  }
}

void DftWidget::onPlotPhaseMouseReleased(QMouseEvent *event)
{
  Q_UNUSED(event);
  if((m_boxZoom2!=NULL)&&m_boxZoom2->isMouseMoved())
  {
    m_boxZoom2->enablePlotViewMove();
    m_boxZoom2->updatePlotViewSize();
    m_boxZoom2->setVisible(false);
    m_boxZoom2->clearMouseIsMoved();
    qDebug()<<"mouse boxzoom release ";
  }
}

void DftWidget::onTableItemClicked(QTableWidgetItem * item)
{
  int row=item->row();
  if(item->column()==COL_PRM_TABLE_DRAW)//改变其显示与否
  {
    QString str;
//    QColor color;
    if(m_curveShowVector.at(row))
    {
      str=tr("off");
//      color=QColor(Qt::gray);
      m_curveShowVector[row]=false;
    }
    else
    {
      str=tr("on");
//      color=QColor(Qt::green);
      m_curveShowVector[row]=true;
    }
    QFont font;
    font.setBold(true);
    font.setPixelSize(12);
    item->setText(str);
    item->setFont(font);
//    item->setBackgroundColor(color);
    ui->dftPlot->graph(row)->setVisible(m_curveShowVector.at(row));
    ui->dftPlot->replot();
    ui->dftPlot_2->graph(row)->setVisible(m_curveShowVector.at(row));
    ui->dftPlot_2->replot();
  }
  if(item->column()==COL_PRM_TABLE_NAME)//改变其显示与否
  {
    for(int i=0;i<ui->dftPlot->graphCount();i++)
    {
      ui->dftPlot->graph(i)->setSelected(false);
      ui->dftPlot_2->graph(i)->setSelected(false);
    }
    ui->dftPlot->graph(row)->setSelected(true);
    ui->dftPlot_2->graph(row)->setSelected(true);
    ui->dftPlot->replot();
    ui->dftPlot_2->replot();
  }
}

void DftWidget::onTableWidgetCellEnteredMoreDetail(int row, int col)
{
  if (col == COL_PRM_TABLE_NAME)
  {
    ui->label_fullName->setText(tr("%1").arg(m_curveFullName.at(row)));
  }
}

void DftWidget::onSetPlotViewRange(QVector<double> &settings)
{
  foreach (double setValue, settings)
  {
    qDebug()<<setValue;
  }
  QCPRange rangex1(settings.at(0),settings.at(1));
  QCPRange rangey1(settings.at(2),settings.at(3));
  QCPRange rangex2(settings.at(4),settings.at(5));
  QCPRange rangey2(settings.at(6),settings.at(7));
  ui->dftPlot->xAxis->setRange(rangex1);
  ui->dftPlot_2->xAxis->setRange(rangex2);
  ui->dftPlot->yAxis->setRange(rangey1);
  ui->dftPlot_2->yAxis->setRange(rangey2);
  ui->dftPlot->replot();
  ui->dftPlot_2->replot();
}

void DftWidget::onComboxInputClicked(int index)
{
  qDebug()<<"index"<<index<<"curve size:"<<mp_plotWave->getTableParameterList().count();
  double min ,max;
  double tempMin,tempMax;
  min=mp_plotWave->getTableParameterList().at(index).data.curveKey.first();
  tempMin=min;
  do{
  tempMin+=mp_plotWave->getSamplingTime()/1000;
  ui->doubleSpinBox_start->setValue(tempMin);
  }while(ui->doubleSpinBox_start->value()<min);

  max=mp_plotWave->getTableParameterList().at(index).data.curveKey.last();
  tempMax=max;
  do{
    tempMax-=mp_plotWave->getSamplingTime()/1000;
    ui->doubleSpinBox_end->setValue(tempMax);
  }while(ui->doubleSpinBox_end->value()>max);
  setTimeRange(min,max);
  qDebug()<<tr("lineEdit start %1,end %2,min %3,max %4").arg(ui->doubleSpinBox_start->text())\
            .arg(ui->doubleSpinBox_end->text()).arg(min).arg(max);
}
void DftWidget::closeEvent(QCloseEvent *event)
{
  Q_UNUSED(event);
  emit dftWidgetClose();
}

void DftWidget::fftAnalyze(FFTAnalyzeInfo &fftInfo)
{
  m_fftResult.am.clear();
  m_fftResult.freq.clear();
  m_fftResult.ph.clear();

  double start=ui->doubleSpinBox_start->value();
  double end=ui->doubleSpinBox_end->value();

  double t1=mp_plotWave->getTableParameterList().at(fftInfo.curveSelectIndex).data.curveKey.first();
  double t2=mp_plotWave->getTableParameterList().at(fftInfo.curveSelectIndex).data.curveKey.last();
  double errorT=t2-t1;
  int startIndex=0;
  int endIndex=0;
  startIndex=(start/errorT)*mp_plotWave->getTableParameterList().at(fftInfo.curveSelectIndex).data.curveKey.count();
  endIndex=(end/errorT)*mp_plotWave->getTableParameterList().at(fftInfo.curveSelectIndex).data.curveKey.count();
  if((endIndex-startIndex+1)%2!=0)
    endIndex=endIndex-1;
  qDebug()<<"start "<<start<<" end"<<end;
  qDebug()<<"t1 "<<t1<<"t2 "<<t2;
  qDebug()<<"1-> "<<start/errorT<<"2-> "<<end/errorT;
  qDebug()<<"startindex "<<startIndex<<" endindex"<<endIndex;
  qDebug()<<"length "<<endIndex-startIndex+1;
  double samplingFreq;
  int length=endIndex-startIndex+1;
  double *in=new double[length];
  double *out=new double[length];
  double *outp=new double[length];
  samplingFreq=(1.0/mp_plotWave->getSamplingTime())*1000;
  for(int i=0;i<length/2;i++)
  {
    m_fftResult.freq.append(samplingFreq*i/length);
  }

  QVector<double>::const_iterator valueIt=mp_plotWave->getTableParameterList().at(fftInfo.curveSelectIndex).data.curveValue.cbegin();

  for (int i = 0; i < length; i++)
  {
    in[i]=(*valueIt);
    valueIt++;
  }

  if(fftInfo.fftType==FFT_SIGNAL)
  {
    SignalAnalysisFFT(in, out, outp, length);
  }
  else
  {
    double *rout=new double[length];
    QVector<double>::const_iterator outIt=mp_plotWave->getTableParameterList().at(fftInfo.curveSelectIndexOut).data.curveValue.cbegin();

    for (int i = 0; i < length; i++)
    {
      rout[i]=(*outIt);
      outIt++;
    }

    RespAnalysisInFreDomain(in, rout, out, outp, length);
    delete []rout;
  }
  for(int i=0;i<length/2;i++)
  {
    m_fftResult.am.append(out[i]);
    m_fftResult.ph.append(outp[i]*m_piRatio);
  }
  delete []in;//释放数组
  delete []out;
  delete []outp;
}
