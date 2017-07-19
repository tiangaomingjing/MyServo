#include "boxzoom.h"

BoxZoom::BoxZoom(QCustomPlot *plot):
  QCPCurve(plot->xAxis,plot->yAxis),
  mp_plot(plot),
  m_isMouseMove(false),
  m_viewIndex(0)
{
  QPen pen;
  pen.setBrush(QBrush(QColor(Qt::gray)));
  pen.setStyle(Qt::DashDotLine);
  pen.setWidthF(1);
  setPen(pen);
}

BoxZoom::~BoxZoom()
{
  qDebug()<<"boxZoom instance release -> ";
}
void BoxZoom::updateBoxZoomShape()
{
  QVector<double> pdKey;
  QVector<double> pdValue;
  pdKey << m_startPoint.x() << m_endPoint.x() << m_endPoint.x() << m_startPoint.x() << m_startPoint.x();
  pdValue << m_startPoint.y() << m_startPoint.y() << m_endPoint.y() << m_endPoint.y() << m_startPoint.y();
  this->setData(pdKey,pdValue);
  mp_plot->replot();
}

void BoxZoom::updatePlotViewSize()
{
  QCPRange rangex(m_startPoint.x(),m_endPoint.x());
  QCPRange rangey(m_startPoint.y(),m_endPoint.y());
  rangex.normalize();
  rangey.normalize();
  RangeRecord record;
  record.xrang=rangex;
  record.yrang=rangey;
  m_rangeRecord.append(record);//记录操作的视图范围
  m_viewIndex++;
  if(m_rangeRecord.size()>MAXSIZE_RECORD)
  {
    m_viewIndex=MAXSIZE_RECORD-1;
    m_rangeRecord.removeAt(0);
    qDebug()<<"record size:"<<m_rangeRecord.size();
  }
  mp_plot->xAxis->setRange(rangex);
  mp_plot->yAxis->setRange(rangey);
  mp_plot->replot();
}

void BoxZoom::setStartPointF(float x,float y)
{
  float xcoord,ycoord;
  xcoord=checkRang(x,true);
  ycoord=checkRang(y,false);
  m_startPoint.setX(xcoord);
  m_startPoint.setY(ycoord);
}

void BoxZoom::setEndPointF(float x,float y)
{
  float xcoord,ycoord;
  xcoord=checkRang(x,true);
  ycoord=checkRang(y,false);
  m_endPoint.setX(xcoord);
  m_endPoint.setY(ycoord);
}
void BoxZoom::disablePlotViewMove()
{
  mp_plot->setInteraction(QCP::iRangeDrag,false);
  mp_plot->setInteraction(QCP::iRangeZoom,false);
  mp_plot->setInteraction(QCP::iMultiSelect,false);
  mp_plot->setInteraction(QCP::iSelectPlottables,false);
}

void BoxZoom::enablePlotViewMove()
{
  mp_plot->setInteraction(QCP::iRangeDrag,true);
  mp_plot->setInteraction(QCP::iRangeZoom,true);
  mp_plot->setInteraction(QCP::iMultiSelect,true);
  mp_plot->setInteraction(QCP::iSelectPlottables,true);
}

void BoxZoom::previousPlotView()
{
  m_viewIndex--;
  if(m_viewIndex<0)
    m_viewIndex=0;

  QCPRange rangex(m_rangeRecord.at(m_viewIndex).xrang);
  QCPRange rangey(m_rangeRecord.at(m_viewIndex).yrang);
  mp_plot->xAxis->setRange(rangex);
  mp_plot->yAxis->setRange(rangey);
  mp_plot->replot();
}
void BoxZoom::nextPlotView()
{
  m_viewIndex++;
  if(m_viewIndex>m_rangeRecord.size()-1)
    m_viewIndex=m_rangeRecord.size()-1;

  QCPRange rangex(m_rangeRecord.at(m_viewIndex).xrang);
  QCPRange rangey(m_rangeRecord.at(m_viewIndex).yrang);
  mp_plot->xAxis->setRange(rangex);
  mp_plot->yAxis->setRange(rangey);
  mp_plot->replot();
}

float BoxZoom::checkRang(float value,bool isX)
{
  QCPRange cp;
  float coord;
  if(isX)
  {
    cp = mp_plot->xAxis->range();		//X坐标轴范围
    coord=mp_plot->xAxis->pixelToCoord(value);
  }
  else
  {
    cp = mp_plot->yAxis->range();		//X坐标轴范围
    coord=mp_plot->yAxis->pixelToCoord(value);
  }
  if(!cp.contains(coord))
  {
    if(coord<cp.lower)
      coord=cp.lower;
    else coord=cp.upper;
  }
  return coord;
}
