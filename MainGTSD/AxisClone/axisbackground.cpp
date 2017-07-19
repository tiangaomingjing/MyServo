#include "axisbackground.h"
#include <QGradientStops>
#include <QPainter>

AxisBackground::AxisBackground(QGraphicsItem *parent):
  QGraphicsItem(parent)
{
  size.setWidth(200);
  size.setHeight(450);
  setZValue(-1000);
  setFlag(ItemIsMovable,false);
  setFlag(ItemIsSelectable,true);
}
void AxisBackground::setSize(QSizeF size)
{
  this->size=size;
}
QRectF AxisBackground::boundingRect() const
{
    return QRectF(QPointF(0,0), size);
}

QPainterPath AxisBackground::shape() const
{
  QPainterPath path;
  path.addRoundedRect(QRectF(QPointF(0,0), size),10,10);
  return path;
}

void AxisBackground::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
  Q_UNUSED(widget);
  Q_UNUSED(option);

  QRectF frame(QPointF(0,0), size);
  QGradientStops stops;
//! [1]

//! [2]
  // paint a background rect (with gradient)
  QLinearGradient gradient(frame.topLeft(), frame.topLeft() + QPointF(250,250));
//  stops << QGradientStop(0.0, QColor(60, 60,  60));
  stops << QGradientStop(1.0, QColor(34, 60,  26));
  stops << QGradientStop(frame.height() / 2 / frame.height(), QColor(102, 176, 54));

  //stops << QGradientStop(((frame.height() + h)/2 )/frame.height(), QColor(157, 195,  55));
  stops << QGradientStop(0.0, QColor(215, 215, 215));
  gradient.setStops(stops);
  painter->setBrush(QBrush(gradient));
  painter->drawRoundedRect(frame, 10.0, 10.0);

}
