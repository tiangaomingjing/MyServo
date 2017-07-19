#include "qvboxlayoutproxy.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLayout>
#include <QFormLayout>
#include "qspaceritemproxy.h"

QVBoxLayoutProxy::QVBoxLayoutProxy(QWidget *layoutParent, QObject *parent) : QObject(parent),
  m_vlayout(new QVBoxLayout(layoutParent))
{

}

void QVBoxLayoutProxy::addWidget(QWidget *widget)
{
  m_vlayout->addWidget(widget);
}
void QVBoxLayoutProxy::addLayout(QHBoxLayout *layout, int stretch)
{
  m_vlayout->addLayout(static_cast<QLayout*>(layout),stretch);
}
void QVBoxLayoutProxy::addLayout(QVBoxLayout *layout, int stretch)
{
  m_vlayout->addLayout(static_cast<QLayout*>(layout),stretch);
}
void QVBoxLayoutProxy::addLayout(QFormLayout *layout, int stretch)
{
  m_vlayout->addLayout(static_cast<QLayout*>(layout),stretch);
}
void QVBoxLayoutProxy::addSpacerItem(QSpacerItemProxy *spacerItem)
{
  m_vlayout->addSpacerItem(spacerItem->spaceItem());
}
