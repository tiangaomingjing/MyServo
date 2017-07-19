#ifndef _QML_REGISTER_INCLUDES_H
#define _QML_REGISTER_INCLUDES_H
#include "led.h"
#include "person.h"
#include "iconlabel.h"
#include "qhboxlayoutproxy.h"
#include "qvboxlayoutproxy.h"
#include "qmlfactory.h"
#include "qformlayoutproxy.h"
#include "qspaceritemproxy.h"
#include "DeviceStatus/treedatadevicestatus.h"
#include "qtreewidgetproxy.h"

#include <QQmlEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <QLineEdit>
#include <QLabel>
#include <QWidget>
#include <QUrl>
#include <QDebug>
#include <QPushButton>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QTreeWidget>
#include <QTreeWidgetItem>

namespace QML_Register
{
  void declareClassToQML(void);
}

#endif
