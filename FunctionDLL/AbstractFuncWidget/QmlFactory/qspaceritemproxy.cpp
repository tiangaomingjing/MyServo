#include "qspaceritemproxy.h"
#include <QSpacerItem>

QSpacerItemProxy::QSpacerItemProxy(int type, QObject *parent) : QObject(parent)
{
  m_type=type;

  if(type!=0)
    m_spacer=new QSpacerItem(10,10,QSizePolicy::Minimum,QSizePolicy::Expanding);
  else
    m_spacer=new QSpacerItem(10,10,QSizePolicy::Expanding,QSizePolicy::Minimum);
}

void QSpacerItemProxy::setType(int type)
{
  m_type=type;
  if(type!=0)
    m_spacer->changeSize(10,10,QSizePolicy::Minimum,QSizePolicy::Expanding);
  else
    m_spacer->changeSize(10,10,QSizePolicy::Expanding,QSizePolicy::Minimum);
}
