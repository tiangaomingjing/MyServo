#include "axisitem.h"
#include "arrow.h"
#include <QGraphicsSceneResizeEvent>

AxisItem::AxisItem(QGraphicsItem *parent, Qt::WindowFlags wFlags):
  QGraphicsProxyWidget(parent,wFlags)
{
  m_button=new QPushButton;
  this->setWidget(m_button);
  m_button->setCheckable(true);
  m_button->setText("axis");
  setObjectName(m_button->text());
}
AxisItem::~AxisItem()
{

}
void AxisItem::addArrow(Arrow *arrow)
{
  arrowList<<arrow;
  arrow->adjust();
}

void AxisItem::removeArrow(Arrow *arrow)
{
    int index = arrowList.indexOf(arrow);

    if (index != -1)
        arrowList.removeAt(index);
}

QList<Arrow*> AxisItem::arrows()
{
  return arrowList;
}

void AxisItem::resizeEvent(QGraphicsSceneResizeEvent *event)
{
  Arrow *arrow;
  foreach (arrow, arrowList)
  {
    arrow->adjust();
  }
  QGraphicsProxyWidget::resizeEvent(event);
}
