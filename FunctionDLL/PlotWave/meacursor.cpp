#include "meacursor.h"
#include <QTimer>
#define TIMER_INTERVAL 200
#define BLINKING_COUNT 4

MeaCursor::MeaCursor(QCustomPlot *plot,QWidget *parent) : QWidget(parent),
  mp_plot(plot),
  m_curveLeft(new QCPCurve(plot->xAxis,plot->yAxis)),
  m_curveRight(new QCPCurve(plot->xAxis,plot->yAxis)),
  m_isDrag(false),
  m_timerCount(0)
{
  m_timer=new QTimer(this);
  m_timer->setInterval(TIMER_INTERVAL);
  connect(m_timer,SIGNAL(timeout()),this,SLOT(onTimeOut()));
}
MeaCursor::~MeaCursor()
{
  qDebug()<<"abstrace meacursor release ->";
}

void MeaCursor::disablePlotViewMove()
{
  mp_plot->setInteraction(QCP::iRangeDrag,false);
  mp_plot->setInteraction(QCP::iRangeZoom,false);
  mp_plot->setInteraction(QCP::iMultiSelect,true);
  mp_plot->setInteraction(QCP::iSelectPlottables,false);
}

void MeaCursor::enablePlotViewMove()
{
  mp_plot->setInteraction(QCP::iRangeDrag,true);
  mp_plot->setInteraction(QCP::iRangeZoom,true);
  mp_plot->setInteraction(QCP::iMultiSelect,true);
  mp_plot->setInteraction(QCP::iSelectPlottables,false);
  mp_plot->setCursor(QCursor(Qt::ArrowCursor));
}

void MeaCursor::setCurve1Checked(bool checked)
{
  if(checked)
  {
    m_curveLeft->setSelected(true);
    m_timer->start();
  }
  else
  {
    m_curveLeft->setSelected(false);
  }
  mp_plot->replot();
}

void MeaCursor::setCurve2Checked(bool checked)
{
  if(checked)
  {
    m_curveRight->setSelected(true);
    m_timer->start();
  }
  else
  {
    m_curveRight->setSelected(false);
  }
  mp_plot->replot();
}
void MeaCursor::setActivedStateColor(bool active)
{
  QPen pen;
  pen.setWidthF(2);
  if(active)
  {
    pen.setBrush(QBrush(QColor(Qt::yellow)));
    pen.setStyle(Qt::SolidLine);
  }
  else
  {
    pen.setBrush(QBrush(QColor(Qt::darkGray)));
    pen.setStyle(Qt::DotLine);
  }
  if(m_curveLeft->selected())
    m_curveLeft->setSelectedPen(pen);
  if(m_curveRight->selected())
    m_curveRight->setSelectedPen(pen);
  mp_plot->replot();
}

bool MeaCursor::curve1Selected()
{
  return m_curveLeft->selected();
}

bool MeaCursor::curve2Selected()
{
  return m_curveRight->selected();
}

void MeaCursor::onTimeOut()
{
  qDebug()<<tr("timerCount %1").arg(m_timerCount);
  QPen pen;
  if(m_timerCount%2==0)
  {
    pen.setBrush(QBrush(QColor(Qt::yellow)));
    pen.setStyle(Qt::SolidLine);
    pen.setWidthF(2);
  }
  else
  {
    pen.setBrush(QBrush(QColor(Qt::darkGray)));
    pen.setStyle(Qt::DotLine);
    pen.setWidthF(2);
  }
  if(m_curveLeft->selected())
    m_curveLeft->setSelectedPen(pen);
  if(m_curveRight->selected())
    m_curveRight->setSelectedPen(pen);
  mp_plot->replot();
  m_timerCount++;
  if(m_timerCount>=BLINKING_COUNT)
  {
    m_timer->stop();
    m_timerCount=0;
  }
}
