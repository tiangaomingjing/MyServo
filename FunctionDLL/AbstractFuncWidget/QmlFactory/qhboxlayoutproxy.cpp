#include "qhboxlayoutproxy.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLayout>
#include "qspaceritemproxy.h"

QHBoxLayoutProxy::QHBoxLayoutProxy(QWidget *layoutParent,QObject *parent) : QObject(parent),
  m_hlayout(new QHBoxLayout(layoutParent))
{

}

void QHBoxLayoutProxy::addWidget(QWidget *widget)
{
  m_hlayout->addWidget(widget);
}
void QHBoxLayoutProxy::addLayout(QHBoxLayout *layout, int stretch)
{
  m_hlayout->addLayout(static_cast<QLayout*>(layout),stretch);
}
void QHBoxLayoutProxy::addLayout(QVBoxLayout *layout, int stretch)
{
  m_hlayout->addLayout(static_cast<QLayout*>(layout),stretch);
}

void QHBoxLayoutProxy::addSpacerItem(QSpacerItemProxy *spacerItem)
{
  m_hlayout->addSpacerItem(spacerItem->spaceItem());
}
