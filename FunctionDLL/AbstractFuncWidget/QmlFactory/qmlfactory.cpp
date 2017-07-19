#include "qmlfactory.h"
#include "led.h"
#include "qhboxlayoutproxy.h"
#include "qvboxlayoutproxy.h"
#include "iconlabel.h"
#include "qformlayoutproxy.h"
#include "qspaceritemproxy.h"

#include <QTimer>
#include <QDebug>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <qtreewidgetproxy.h>

QmlFactory::QmlFactory(QObject *parent) : QObject(parent)
{
  QTimer::singleShot(100,this,SLOT(onFactoryCreated()));
}
QmlFactory::~QmlFactory()
{

}
void QmlFactory::onFactoryCreated()
{
  qDebug()<<"factory create...";
  emit initialFactory();

}

QLineEdit* QmlFactory::createQLineEdit(QWidget *parent)
{
  return new QLineEdit(parent);
}

QLabel * QmlFactory::createQLabel(QWidget *parent)
{
  return new QLabel(parent);
}
QPushButton *QmlFactory::createQPushButton(QWidget *parent)
{
  return new QPushButton(parent);
}
Led *QmlFactory::createLed(QWidget *parent)
{
  return new Led(parent);
}

QVBoxLayout *QmlFactory::createVBoxLayout(QWidget *parent)
{
  return new QVBoxLayout(parent);
}
QHBoxLayoutProxy *QmlFactory::createQHBoxLayoutProxy(QWidget *layoutParent,QObject *parent)
{
  return new QHBoxLayoutProxy(layoutParent,parent);
}

QVBoxLayoutProxy *QmlFactory::createQVBoxLayoutProxy(QWidget *layoutParent,QObject *parent)
{
  return new QVBoxLayoutProxy(layoutParent,parent);
}
QFormLayoutProxy *QmlFactory::createQFormLayoutProxy(QWidget *layoutParent, QObject *parent)
{
  return new QFormLayoutProxy(layoutParent,parent);
}

IconLabel *QmlFactory::createIconLabel(QWidget *parent)
{
  return new IconLabel(parent);
}

QSpacerItemProxy *QmlFactory::createQSpacerItemProxy(int type,QObject *parent)
{
  return new QSpacerItemProxy(type,parent);
}
QTreeWidgetProxy *QmlFactory::createQTreeWidgetProxy(QTreeWidget *treesrc,QObject *parent)
{
  return new QTreeWidgetProxy(treesrc,parent);
}
