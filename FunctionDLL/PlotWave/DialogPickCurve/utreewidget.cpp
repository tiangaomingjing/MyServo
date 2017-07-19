#include "utreewidget.h"

#include <QMouseEvent>

UTreeWidget::UTreeWidget(QWidget *parent):
  QTreeWidget(parent)
{
  this->setMouseTracking(true);
}

void UTreeWidget::mouseMoveEvent(QMouseEvent *e)
{
  emit currentMousePosition(e->x(),e->y());
}

