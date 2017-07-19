#ifndef AXISBACKGROUND_H
#define AXISBACKGROUND_H
#include <QGraphicsItem>
#include <QSizeF>

class AxisBackground : public QGraphicsItem
{
public:
  AxisBackground(QGraphicsItem *parent = 0);
  void setSize(QSizeF size);
protected:
  QRectF boundingRect() const Q_DECL_OVERRIDE;
  QPainterPath shape() const Q_DECL_OVERRIDE;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)Q_DECL_OVERRIDE;

private:
  QSizeF size;
};

#endif // AXISBACKGROUND_H
