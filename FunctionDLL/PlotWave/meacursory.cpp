#include "meacursory.h"

MeaCursorY::MeaCursorY(QCustomPlot *plot,QWidget *parent) : MeaCursor(plot,parent)
{
  initialMeaCursor();
}
MeaCursorY::~MeaCursorY()
{
  mp_plot->removePlottable(m_curveLeft);
  mp_plot->removePlottable(m_curveRight);
  qDebug()<<" meacursor Y release ->";
}
void MeaCursorY::initialMeaCursor()
{
  QCPRange rangex,rangey;
  double tickstep;
  tickstep=mp_plot->yAxis->tickStep();
  rangex=mp_plot->xAxis->range();
  rangey=mp_plot->yAxis->range();
  float center;
  center=rangey.center();
  m_point[0].setX(rangex.lower);
  m_point[1].setX(rangex.upper);
  m_point[0].setY(center-tickstep);
  m_point[1].setY(center-tickstep);
  m_point[2].setX(rangex.lower);
  m_point[3].setX(rangex.upper);
  m_point[2].setY(center+tickstep);
  m_point[3].setY(center+tickstep);
  QVector<double>keys;
  QVector<double>values;
  keys<<m_point[0].x()<<m_point[1].x();
  values<<m_point[0].y()<<m_point[1].y();
  m_curveLeft->setData(keys,values);
  keys.clear();
  values.clear();
  keys<<m_point[2].x()<<m_point[3].x();
  values<<m_point[2].y()<<m_point[3].y();
  m_curveRight->setData(keys,values);
  QPen pen;
  pen.setBrush(QBrush(QColor(Qt::darkGray)));
  pen.setStyle(Qt::DotLine);
  pen.setWidthF(2);
  m_curveLeft->setPen(pen);
  m_curveRight->setPen(pen);

  pen.setBrush(QBrush(QColor(Qt::yellow)));
  pen.setWidthF(2);
  m_curveLeft->setSelectedPen(pen);
  m_curveRight->setSelectedPen(pen);

  mp_plot->addPlottable(m_curveLeft);
  mp_plot->addPlottable(m_curveRight);
  mp_plot->replot();
}

bool MeaCursorY::updateMeasureCursorPosition(float mouseX,float mouseY)
{
  Q_UNUSED(mouseX)
  bool leftSelected,rightSelected;
  float dy;
  dy=mp_plot->yAxis->pixelToCoord(mouseY);
  leftSelected=m_curveLeft->selected();
  rightSelected=m_curveRight->selected();

  if(leftSelected&&rightSelected)
  {
    if (qAbs(dy - m_point[2].y()) > qAbs(dy - m_point[0].y()))
    {
      m_point[2].setY(m_point[2].y()+ (dy -  m_point[0].y()));
      m_point[0].setY(dy);
    }
    else
    {
      m_point[0].setY(m_point[0].y()+dy -  m_point[2].y());
      m_point[2].setY(dy);
    }
  }
  else if(leftSelected)
  {
    m_point[0].setY(dy);
  }
  else if(rightSelected)
  {
    m_point[2].setY(dy);
  }
  if(leftSelected||rightSelected)
  {
    mp_plot->setCursor(QCursor(Qt::SplitVCursor));
    fillAllMeasureCursor();
    return true;
  }
  else
  {
    fillAllMeasureCursor();
    return false;
  }

}
void MeaCursorY::fillAllMeasureCursor(void)
{
  QCPRange range;
  range=mp_plot->xAxis->range();
  m_point[1].setX(range.upper);
  m_point[1].setY(m_point[0].y());
  m_point[0].setX(range.lower);

  m_point[3].setX(range.upper);
  m_point[3].setY(m_point[2].y());
  m_point[2].setX(range.lower);
  QVector<double>keys0;
  QVector<double>keys1;
  QVector<double>values0;
  QVector<double>values1;
  keys0<<m_point[0].x()<<m_point[1].x();
  values0<<m_point[0].y()<<m_point[1].y();

  keys1<<m_point[2].x()<<m_point[3].x();
  values1<<m_point[2].y()<<m_point[3].y();

  m_curveLeft->setData(keys0,values0);
  m_curveRight->setData(keys1,values1);
  mp_plot->replot();
}

QString MeaCursorY::measureInformation()
{
  QString dy=QStringLiteral("ΔY");
  return tr("%2 = %1  ").arg(QString::number((m_point[2].y()-m_point[0].y()),'f',2)).arg(dy);
}
