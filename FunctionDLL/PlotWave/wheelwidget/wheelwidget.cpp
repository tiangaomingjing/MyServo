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

#include <QtWidgets>

#include "wheelwidget.h"
#include <QDebug>

#define WHEEL_SCROLL_OFFSET 50000.0

AbstractWheelWidget::AbstractWheelWidget(bool touch, QWidget *parent)
    : QWidget(parent)
    , m_currentItem(0)
    , m_itemOffset(0)
    , m_isDragging(false)
{
// ![0]
    QScroller::grabGesture(this, touch ? QScroller::TouchGesture : QScroller::LeftMouseButtonGesture);
// ![0]
}

AbstractWheelWidget::~AbstractWheelWidget()
{ }

int AbstractWheelWidget::currentIndex() const
{
    return m_currentItem;
}

void AbstractWheelWidget::setCurrentIndex(int index)
{
    if (index >= 0 && index < itemCount()) {
        m_currentItem = index;
        m_itemOffset = 0;
        update();
    }
}

bool AbstractWheelWidget::event(QEvent *e)
{
  static quint32 num=0;
    switch (e->type()) {
// ![1]
    case QEvent::ScrollPrepare:
    {
        // We set the snap positions as late as possible so that we are sure
        // we get the correct itemHeight
        QScroller *scroller = QScroller::scroller(this);
        scroller->setSnapPositionsY( WHEEL_SCROLL_OFFSET-itemHeight()*1000, itemHeight() );

        QScrollPrepareEvent *se = static_cast<QScrollPrepareEvent *>(e);
        se->setViewportSize(QSizeF(size()));
        // we claim a huge scrolling area and a huge content position and
        // hope that the user doesn't notice that the scroll area is restricted
        se->setContentPosRange(QRectF(0.0, 0.0, 0.0, qAbs(WHEEL_SCROLL_OFFSET*2)));
        se->setContentPos(QPointF(0.0, WHEEL_SCROLL_OFFSET + m_currentItem * itemHeight() + m_itemOffset));
        se->accept();
        num=0;
//        qDebug()<<"content pos"<<se->contentPos();

//        qDebug()<<"m_currentItem="<<m_currentItem<<" m_itemOffset="<<m_itemOffset;
        m_isDragging=false;

        return true;
    }
// ![1]
// ![2]
    case QEvent::Scroll:
    {
        QScrollEvent *se = static_cast<QScrollEvent *>(e);

        qreal y = se->contentPos().y();

        int iy = y - WHEEL_SCROLL_OFFSET;
        int ih = itemHeight();


// ![2]
// ![3]
        // -- calculate the current item position and offset and redraw the widget
        int ic = itemCount();
        if (ic>0) {
            m_currentItem = iy / ih % ic;
            m_itemOffset = iy % ih;
//            qDebug()<<"se->contentPos().y()="<<y<<"iy="<<iy<<" currentItem="<<m_currentItem<<" m_itemOffset="<<m_itemOffset;

            // take care when scrolling backwards. Modulo returns negative numbers
            if (m_itemOffset < 0) {
                m_itemOffset += ih;
                m_currentItem--;
            }

            if (m_currentItem < 0)
                m_currentItem += ic;
        }
        m_isDragging=true;

        // -- repaint
        update();

        se->accept();
        num++;
//        qDebug()<<"QEvent::Scroll -num="<<num;
        if(se->scrollState()==QScrollEvent::ScrollFinished)//滚动停止
        {
          qDebug()<<"-------------------stop at currentItem is "<<m_currentItem;
          m_isDragging=false;
          emit stopped(m_currentItem);
        }

        return true;
// ![3]
    }
    default:
        return QWidget::event(e);
    }
    return true;
}

void AbstractWheelWidget::paintEvent(QPaintEvent* event)
{
  Q_UNUSED( event );

  // -- first calculate size and position.
  int w = width();
  int h = height();

  QPainter painter(this);
  QPalette palette = QApplication::palette();
  QPalette::ColorGroup colorGroup = isEnabled() ? QPalette::Active : QPalette::Disabled;

  // linear gradient brush
  QLinearGradient grad(0.5, 0, 0.5, 1.0);
//    QColor clr=palette.color(colorGroup, QPalette::ButtonText).lighter(200);
  QColor clr=QColor(Qt::gray).darker();
  grad.setColorAt(0,clr);
  grad.setColorAt(0.1, QColor(Qt::gray));
  grad.setColorAt(0.4, palette.color(colorGroup, QPalette::Button));
  grad.setColorAt(0.5, palette.color(colorGroup, QPalette::Button));
  grad.setColorAt(0.9, QColor(Qt::gray));
  grad.setColorAt(1.0, clr);
  grad.setCoordinateMode( QGradient::ObjectBoundingMode );
  QBrush gBrush( grad );

  //paint endge
  qreal rate=0.03;
  qreal h1=rate*h;
  qreal h2=(1-rate)*h;
  qreal w1=rate*w;
  qreal w2=(1-rate)*w;
  QVector<QPointF>leftPointFs;
  QVector<QPointF>rightPointFs;
  leftPointFs<<QPointF(0,h1)<<QPointF(w1,0)<<QPointF(w1,h)<<QPointF(0,h2);
  rightPointFs<<QPointF(w2,0)<<QPointF(w,h1)<<QPointF(w,h2)<<QPointF(w2,h);
  painter.setBrush(gBrush);
  QPainterPath lPath;
  QPainterPath rPath;
  lPath.addPolygon(QPolygonF(leftPointFs));
  rPath.addPolygon(QPolygonF(rightPointFs));
  painter.fillPath(lPath,gBrush);
  painter.fillPath(rPath,gBrush);

  //draw middle
  QLinearGradient gradMid(0.5, 0, 0.5, 1.0);
//    QColor clr=palette.color(colorGroup, QPalette::ButtonText).lighter(200);
  QColor clrMid=QColor(Qt::gray);
  gradMid.setColorAt(0,clrMid);
  gradMid.setColorAt(0.3, palette.color(colorGroup, QPalette::Button));
  gradMid.setColorAt(0.7, palette.color(colorGroup, QPalette::Button));
  gradMid.setColorAt(1.0, clrMid);
  gradMid.setCoordinateMode( QGradient::ObjectBoundingMode );
  QBrush gBrushMid( gradMid );

  painter.setBrush(gBrushMid);
  QRectF midRectF(w1,0,w-2*w1,h);
  painter.fillRect(midRectF,gBrushMid);


  // paint a border and background
//    painter.setPen(palette.color(colorGroup, QPalette::ButtonText));
//    painter.setBrushOrigin( QPointF( 0.0, 0.0 ) );
//    painter.drawRect( 0, 0, w-1, h-1 );
//    painter.fillRect(QRectF(0,0, w-1, h-1),gBrush);


  //draw endge line
//    painter.setPen(clrMid);
//    painter.drawLine(leftPointFs.at(0),leftPointFs.at(3));
//    painter.drawLine(leftPointFs.at(1),leftPointFs.at(2));
//    painter.drawLine(rightPointFs.at(0),rightPointFs.at(3));
//    painter.drawLine(rightPointFs.at(1),rightPointFs.at(2));

  // paint inner border
//    painter.setPen(palette.color(colorGroup, QPalette::Button));
//    painter.setBrush(Qt::NoBrush);
//    painter.drawRoundRect( 1, 1, w-3, h-3,3,3 );

  // paint the items
  painter.setClipRect( QRect( 3, 3, w-6, h-6 ) );
  painter.setPen(palette.color(colorGroup, QPalette::ButtonText));

  int iH = itemHeight();
  int iC = itemCount();
  if (iC > 0) {

      m_itemOffset = m_itemOffset % iH;
      int drawNum=h/iH;
//        double dPi=M_PI/drawNum;
      int index=0;

      QFont font;
      QFontMetrics fm(this->font());
      int fontSize=fm.height()*1;
      QColor numberColor;
      int k=0;

      for (int i=-h/2/iH; i<=h/2/iH+1; i++) {
          numberColor=QColor(Qt::gray).darker(180);
          int itemNum = m_currentItem + i;
          while (itemNum < 0)
              itemNum += iC;
          while (itemNum >= iC)
              itemNum -= iC;

          if(index>drawNum/2)
            k--;
          else
            k++;

          font.setBold(false);
          double psize=fontSize*(0.8+0.2*k/(drawNum/2))+0.5;
          font.setPixelSize(psize);

          if(index==drawNum/2)
          {
            font.setBold(true);
            numberColor=QColor(Qt::black);
            font.setPixelSize(fontSize*1.1);
            if(m_isDragging)
              emit dragIndex(itemNum);
          }
          painter.setFont(font);
//          painter.setPen(numberColor);
          paintItem(&painter, itemNum, QRect(w1+6, h/2 +i*iH - m_itemOffset - iH/2, w-2*w1-6, iH ));
          index++;

      }
  }

  //draw center
  painter.setClipRect( QRect( 0, 0, w-0, h-0 ) );
//    QLinearGradient gradShow(0.5, 0, 0.5, 1.0);
  QColor clr2,clr3;
  clr2.setRgb(164,172,199,200);
  clr3.setRgb(164,172,199,60);
//    gradShow.setColorAt(0,clr2);
//    gradShow.setColorAt(0.2, clr3);
//    gradShow.setColorAt(0.8, clr3);
//    gradShow.setColorAt(1.0, clr2);
//    gradShow.setCoordinateMode( QGradient::ObjectBoundingMode );
//    QBrush gBrush2( gradShow );
//    painter.fillRect( QRectF( 0, h/2 - iH/2-0.1*iH, w, iH*1.2 ), gBrush2 );
  painter.setPen(clr2);
  painter.setBrush(Qt::NoBrush);
//    painter.drawRect( QRectF( 0, h/2 - iH/2-0.1*iH, w, iH*1.2 ));
  painter.drawLine(QPointF(w1, h/2 - iH/2-0.1*iH),QPointF(w-w1, h/2 - iH/2-0.1*iH));
  painter.drawLine(QPointF(w1, h/2 - iH/2-0.1*iH+1.2*iH),QPointF(w-w1, h/2 - iH/2-0.1*iH+1.2*iH));

}

void AbstractWheelWidget::mousePressEvent(QMouseEvent *e)
{

  QPointF p(0.0, WHEEL_SCROLL_OFFSET + m_currentItem * itemHeight() + m_itemOffset);
  qDebug()<<"mouse press event p="<<p;

  QScroller *scroller = QScroller::scroller(this);

  double y=e->pos().y();

//  qDebug()<<"e.x="<<x<<"e.y="<<y;
//  qDebug()<<"global pos"<<e->globalPos();
//  qDebug()<<"pos"<<e->pos();
//  qDebug()<<"screen pos"<<e->screenPos();
//  qDebug()<<"window pos"<<e->windowPos();


//  int index=(y+0.5)/itemHeight();
//  qDebug()<<"index="<<index;
//  qDebug()<<"current Item="<<m_currentItem;
  int h = height();
  double dh=y-h/2;
//  qDebug()<<"dh="<<dh<<" h="<<h;

  scroller->scrollTo(QPointF(0, WHEEL_SCROLL_OFFSET + m_currentItem * itemHeight() + m_itemOffset+dh), 500);
}
void AbstractWheelWidget::mouseMoveEvent(QMouseEvent *e)
{
  qreal y=e->localPos().y();
  int H=height();
  qreal index=(y-0.5*H)/itemHeight()+m_currentItem;
//  qDebug()<<"index real="<<index;
  int ic=itemCount();
  if(index<0)
  {
    index+=ic;
  }
  int num=(int)(index+0.5);
  num=num%ic;
  emit hoverIndex(num);

//  qDebug()<<" local pos="<<e->localPos()<<" index="<<index<<" num="<<num;

}


/*void AbstractWheelWidget::wheelEvent(QWheelEvent *wheelEvent)
{
  qDebug()<<wheelEvent->angleDelta();
  qDebug()<<wheelEvent->delta();
  int dw=wheelEvent->delta();
  int ic=itemCount();
  if(dw<0)
  {
    m_currentItem++;
    m_currentItem=m_currentItem%ic;
  }
  else
  {
    m_currentItem--;
    if(m_currentItem<0)
      m_currentItem+=ic;
  }
  setCurrentIndex(m_currentItem);
  wheelEvent->accept();
}*/


/*!
    Rotates the wheel widget to a given index.
    You can also give an index greater than itemCount or less than zero in which
    case the wheel widget will scroll in the given direction and end up with
    (index % itemCount)
*/
void AbstractWheelWidget::scrollTo(int index)
{
    QScroller *scroller = QScroller::scroller(this);

    scroller->scrollTo(QPointF(0, WHEEL_SCROLL_OFFSET + index * itemHeight()), 5000);
}

/*!
    \class StringWheelWidget
    \brief The StringWheelWidget class is an implementation of the AbstractWheelWidget class that draws QStrings as items.
    \sa AbstractWheelWidget
*/

StringWheelWidget::StringWheelWidget(bool touch)
    : AbstractWheelWidget(touch)
{ }

QStringList StringWheelWidget::items() const
{
    return m_items;
}

void StringWheelWidget::setItems( const QStringList &items )
{
    m_items = items;
    if (m_currentItem >= items.count())
        m_currentItem = items.count()-1;
    update();
}


QSize StringWheelWidget::sizeHint() const
{
    // determine font size
    QFontMetrics fm(font());

    return QSize( fm.width("m") * 12 + 6, fm.height() * 7 + 6 );
}

QSize StringWheelWidget::minimumSizeHint() const
{
    QFontMetrics fm(font());

    return QSize( fm.width("m") * 11 + 6, fm.height() * 3 + 6 );
}

void StringWheelWidget::paintItem(QPainter* painter, int index, const QRect &rect)
{
    painter->drawText(rect, Qt::AlignHCenter|Qt::AlignVCenter, m_items.at(index));
}

int StringWheelWidget::itemHeight() const
{
//    QFont font("times",16);
    QFontMetrics fm(font());
    return fm.height()*1.5;
}

int StringWheelWidget::itemCount() const
{
    return m_items.count();
}

int StringWheelWidget::id() const
{
  return m_id;
}

void StringWheelWidget::setId(int id)
{
  m_id = id;
}


