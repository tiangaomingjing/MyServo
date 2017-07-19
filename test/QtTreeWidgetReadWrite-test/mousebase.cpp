#include "mousebase.h"
#include <QMouseEvent>

MouseBase::MouseBase(QWidget *parent):
  QFrame(parent)
{
  this->setMouseTracking(true);
}

void MouseBase::mouseMoveEvent(QMouseEvent *e)
{
  emit currentMousePosition(e->x(),e->y());
}
