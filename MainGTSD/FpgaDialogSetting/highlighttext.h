#ifndef HIGHLIGHTTEXT_H
#define HIGHLIGHTTEXT_H
#include <QTreeWidgetItem>
#include <QObject>
#include <QDebug>

class HighlightText:public QObject
{
  Q_OBJECT
public:
  HighlightText(int index,QObject*parent):QObject(parent),
    m_textIndex(index),
    hasClick(false),
    hasEnter(false)
  {

  }
  ~HighlightText(){}
  void storePrevItem(QTreeWidgetItem *item)
  {
    m_prevItem=item;
    m_prevText=m_prevItem->text(m_textIndex);
    hasClick=true;
  }
  void hightlightText()
  {
    if(hasClick)
    {
      if(!hasEnter)
      {
        if(m_prevText!=m_prevItem->text(m_textIndex))
        {
          m_prevItem->setTextColor(m_textIndex,QColor(Qt::red));
          qDebug()<<"set red";
        }
      }
      hasEnter=false;
    }

  }
  void resetTextColor()
  {
    m_prevItem->setTextColor(m_textIndex,QColor(Qt::black));
    hasEnter=true;
  }
  QString &prevText() {return m_prevText;}

private:
  QString m_prevText;
  int m_textIndex;
  QTreeWidgetItem *m_prevItem;
  bool hasClick;
  bool hasEnter;
};

#endif // HIGHLIGHTTEXT_H

