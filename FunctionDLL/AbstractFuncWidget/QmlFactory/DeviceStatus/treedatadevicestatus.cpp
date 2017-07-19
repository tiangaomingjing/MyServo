#include "treedatadevicestatus.h"
#include <QVector>

#define MAXBITSIZE 16

TreeDataDeviceStatus::TreeDataDeviceStatus(QObject *parent) : QObject(parent)
{
  for(int i=0;i<MAXBITSIZE;i++)
    m_bits.append(false);
}

void TreeDataDeviceStatus::setBits(quint16 index, bool value)
{
  m_bits[index]=value;
}
