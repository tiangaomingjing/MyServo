#include "measuretag.h"

MeasureTag::MeasureTag(QCustomPlot *plot, QObject *parent) : QObject(parent),
  mp_plot(plot)
{

}

MeasureTag::~MeasureTag()
{
//  clearMeasureTag();//由于加入父类,在父类析构时删除
}
void MeasureTag::addMeasureTag(float x,float y, QCPGraph *graph)
{
  QPen pen;
  pen.setColor(graph->pen().color());
  QPointF point=checkRange(x,y);
  ItemTracer *tracer=new ItemTracer();
  tracer->tacer=new QCPItemTracer(mp_plot);
  mp_plot->addItem(tracer->tacer);
  tracer->tacer->setGraph(graph);
  tracer->tacer->setGraphKey(point.x());
  tracer->tacer->setInterpolating(false);
  tracer->tacer->setStyle(QCPItemTracer::tsCircle);
  tracer->tacer->setPen(QPen(Qt::green));
  tracer->tacer->setBrush(Qt::green);
  tracer->tacer->setSize(7);
  tracer->tacer->updatePosition();
  QString str;
  point.setX(tracer->tacer->position->coords().x());
  point.setY(tracer->tacer->position->coords().y());
  str=QString::number(tracer->tacer->position->coords().x(),'f',3)+","+\
      QString::number(tracer->tacer->position->coords().y(),'f',3);

  //add text
  tracer->text = new QCPItemText(mp_plot);
  mp_plot->addItem(tracer->text);
  tracer->text->position->setType(QCPItemPosition::ptPlotCoords);
  tracer->text->setPositionAlignment(Qt::AlignHCenter|Qt::AlignTop);
  tracer->text->position->setCoords(point.x(),point.y()+mp_plot->yAxis->tickStep()); // lower right corner of axis rect
  tracer->text->setText(str);
  tracer->text->setTextAlignment(Qt::AlignHCenter);
//  tracer->text->setFont(QFont(font().family(), 9));
  tracer->text->setPadding(QMargins(1, 1, 1, 1));
  tracer->text->setPen(QPen(pen));
  tracer->text->setColor(QColor(graph->pen().color()));

  // add arrow pointing at group tracer, coming from label:
  tracer->arrow = new QCPItemCurve(mp_plot);
  mp_plot->addItem(tracer->arrow);
  tracer->arrow->start->setParentAnchor(tracer->text->bottom);
  tracer->arrow->startDir->setParentAnchor(tracer->arrow->start);
  tracer->arrow->startDir->setCoords(0, -1); // direction 30 pixels to the left of parent anchor (tracerArrow->start)
  tracer->arrow->end->setCoords(point.x(), point.y());
  tracer->arrow->endDir->setParentAnchor(tracer->arrow->end);
  tracer->arrow->endDir->setCoords(0, -2);
  tracer->arrow->setHead(QCPLineEnding::esSpikeArrow);
  tracer->arrow->setPen(QPen(pen));

  m_tracerList.append(tracer);
  mp_plot->replot();
}

void MeasureTag::clearMeasureTag()
{
  if(!m_tracerList.isEmpty())
  {
    ItemTracer *tracer;
    QMutableListIterator<ItemTracer*> it(m_tracerList);
    while(it.hasNext())
    {
      tracer=it.next();
      mp_plot->removeItem(tracer->tacer);
      mp_plot->removeItem(tracer->arrow);
      mp_plot->removeItem(tracer->text);
    }
    m_tracerList.clear();
    mp_plot->replot();
  }
}

QPointF MeasureTag::checkRange(float x,float y)
{
  QCPRange cpx,cpy;
  float coordx,coordy;
  coordx=mp_plot->xAxis->pixelToCoord(x);
  coordy=mp_plot->yAxis->pixelToCoord(y);
  cpx = mp_plot->xAxis->range();
  cpy = mp_plot->yAxis->range();

  if(!cpx.contains(coordx))
  {
    if(coordx<cpx.lower)
      coordx=cpx.lower;
    else coordx=cpx.upper;
  }
  if(!cpy.contains(coordy))
  {
    if(coordy<cpy.lower)
      coordy=cpy.lower;
    else coordy=cpy.upper;
  }
  QPointF p(coordx,coordy);
  return p;
}
