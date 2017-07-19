#include "qformlayoutproxy.h"
#include <QFormLayout>

QFormLayoutProxy::QFormLayoutProxy(QWidget *layoutParent, QObject *parent) : QObject(parent),
  m_formLayout(new QFormLayout(layoutParent))
{

}

void	QFormLayoutProxy::addRow(QWidget * label, QWidget * field)
{
  m_formLayout->addRow(label,field);
}

void	QFormLayoutProxy::addRow(const QString  labelText, QWidget * field)
{
  m_formLayout->addRow(labelText,field);
}
