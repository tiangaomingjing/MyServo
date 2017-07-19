/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/


#include "arrow.h"

#include <math.h>

#include <QPen>
#include <QPainter>
#include <QDebug>

const qreal Pi = 3.14;

//! [0]
Arrow::Arrow(AxisItem *startItem, AxisItem *endItem, QGraphicsItem *parent)
    : QGraphicsLineItem(parent)
{
    myStartItem = startItem;
    myEndItem = endItem;
    myStartItem->addArrow(this);
    myEndItem->addArrow(this);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    myColor = Qt::black;
    setPen(QPen(myColor, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    adjust();
}
//! [0]

//! [1]
QRectF Arrow::boundingRect() const
{
    qreal extra = (pen().width() + 20) / 2.0;

    return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(),
                                      line().p2().y() - line().p1().y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}
//! [1]

//! [2]
QPainterPath Arrow::shape() const
{
    QPainterPath path = QGraphicsLineItem::shape();
    path.addPolygon(arrowHead);
    return path;
}
//! [2]

//! [3]
void Arrow::updatePosition()
{
    QLineF line(mapFromItem(myStartItem, 0, 0), mapFromItem(myEndItem, 0, 0));
    setLine(line);
}
//! [3]

//! [4]
void Arrow::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
          QWidget *)
{
    if (myStartItem->collidesWithItem(myEndItem))
        return;

    QPen myPen = pen();
    myPen.setColor(myColor);
    qreal arrowSize = 10;
    painter->setPen(myPen);
    painter->setBrush(myColor);
//! [4] //! [5]

    QLineF centerLine(sourcePoint, destPoint);

    setLine(centerLine);
//! [5] //! [6]

    double angle = ::acos(line().dx() / line().length());
    if (line().dy() >= 0)
        angle = (Pi * 2) - angle;

        QPointF arrowP1 = destPoint + QPointF(sin(angle - Pi / 3) * arrowSize,
                                        cos(angle - Pi / 3) * arrowSize);
        QPointF arrowP2 = destPoint + QPointF(sin(angle - Pi + Pi / 3) * arrowSize,
                                        cos(angle - Pi + Pi / 3) * arrowSize);

        arrowHead.clear();
        arrowHead << destPoint << arrowP1 << arrowP2;
//! [6] //! [7]
        painter->drawLine(line());
        painter->drawPolygon(arrowHead);
        if (isSelected())
        {
          painter->setPen(QPen(QColor(Qt::red), 1, Qt::DashLine));
          QLineF myLine = line();
          myLine.translate(0, 6.0);
          painter->drawLine(myLine);
          myLine.translate(0,-12.0);
          painter->drawLine(myLine);
        }
}
void Arrow::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
  Q_UNUSED(event);
  emit mouseDoubleClicked();
}

void Arrow::adjust()
{
  if (!myStartItem || !myEndItem)
      return;

  QSizeF src,dst;
  src=myStartItem->size();
  dst=myEndItem->size();

  qDebug()<<QObject::tr("src size:%1,%2").arg(src.width()).arg(src.height());
  qDebug()<<QObject::tr("button size:%1,%2").arg(myStartItem->button()->width()).arg(myStartItem->button()->height());

  QLineF line(mapFromItem(myStartItem, src.width(), src.height()/2), mapFromItem(myEndItem, 0, dst.height()/2));
  qreal length = line.length();

  prepareGeometryChange();

  if (length > qreal(20.))
  {
      sourcePoint = line.p1();
      destPoint = line.p2();
  }
  else
  {
      sourcePoint = destPoint = line.p1();
  }
}
