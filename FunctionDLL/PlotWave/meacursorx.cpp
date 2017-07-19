#include "meacursorx.h"

MeaCursorX::MeaCursorX(QCustomPlot *plot,QWidget *parent) : MeaCursor(plot,parent)
{
  initialMeaCursor();
}
MeaCursorX::~MeaCursorX()
{
  mp_plot->removePlottable(m_curveLeft);
  mp_plot->removePlottable(m_curveRight);
  qDebug()<<" meacursor X release ->";
}
void MeaCursorX::initialMeaCursor()
{
  QCPRange rangex,rangey;
  double tickstep;
  tickstep=mp_plot->xAxis->tickStep();
  rangex=mp_plot->xAxis->range();
  rangey=mp_plot->yAxis->range();
  float center;
  center=rangex.center();
  m_point[0].setX(center-tickstep);
  m_point[1].setX(center-tickstep);
  m_point[0].setY(rangey.lower);
  m_point[1].setY(rangey.upper);
  m_point[2].setX(center+tickstep);
  m_point[3].setX(center+tickstep);
  m_point[2].setY(rangey.lower);
  m_point[3].setY(rangey.upper);
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

bool MeaCursorX::updateMeasureCursorPosition(float mouseX,float mouseY)
{
  Q_UNUSED(mouseY)
  bool leftSelected,rightSelected;
  bool ret=false;
  float dx;
  dx=mp_plot->xAxis->pixelToCoord(mouseX);
  leftSelected=m_curveLeft->selected();
  rightSelected=m_curveRight->selected();

  if(leftSelected&&rightSelected)
  {
    if (qAbs(dx - m_point[2].x()) > qAbs(dx - m_point[0].x()))
    {
      m_point[2].setX(m_point[2].x()+ (dx -  m_point[0].x()));
      m_point[0].setX(dx);
//      qDebug()<<"near left "<<"dx"<<dx<<" p0"<<m_point[0].x()<<" p2"<<m_point[2].x();
    }
    else
    {
      m_point[0].setX(m_point[0].x()+dx -  m_point[2].x());
      m_point[2].setX(dx);
//      qDebug()<<"near right "<<"dx"<<dx<<" p0"<<m_point[0].x()<<" p2"<<m_point[2].x();
    }
    emit measureLeftValueChanged(m_point[0].x());
    emit measureRightValueChanged(m_point[2].x());
  }
  else if(leftSelected)
  {
    m_point[0].setX(dx);
    emit measureLeftValueChanged(m_point[0].x());
  }
  else if(rightSelected)
  {
    m_point[2].setX(dx);
    emit measureRightValueChanged(m_point[2].x());
  }
  if(leftSelected||rightSelected)
  {
    mp_plot->setCursor(QCursor(Qt::SplitHCursor));
    ret=true;
  }
  fillAllMeasureCursor();
  return ret;

}
void MeaCursorX::fillAllMeasureCursor(void)
{
  QCPRange range;
  range=mp_plot->yAxis->range();
  m_point[1].setX(m_point[0].x());
  m_point[1].setY(range.upper);
  m_point[0].setY(range.lower);

  m_point[3].setX(m_point[2].x());
  m_point[3].setY(range.upper);
  m_point[2].setY(range.lower);
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

QString MeaCursorX::measureInformation()
{
  double dx_s=m_point[2].x()-m_point[0].x();
  double dx_f=1/dx_s;
  QString dx=QStringLiteral("ΔX");
  QString df=QStringLiteral("ΔFreq");
  return tr("%3 = %1 ms , %4 = %2 HZ ").arg(QString::number(dx_s*1000,'f',3)).arg(QString::number(dx_f))
      .arg(dx).arg(df);
}
