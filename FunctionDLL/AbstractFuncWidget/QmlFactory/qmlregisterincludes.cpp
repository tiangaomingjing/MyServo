#include "qmlregisterincludes.h"

namespace QML_Register
{
  void declareClassToQML(void)
  {
    qmlRegisterType<QLineEdit>("QtClass",1,0,"QLineEdit");
    qmlRegisterType<QLabel>("QtClass",1,0,"QLabel");
    qmlRegisterType<QWidget>("QtClass",1,0,"QWidget");
    qmlRegisterType<QPushButton>("QtClass",1,0,"QPushButton");
    qmlRegisterType<QHBoxLayout>("QtClass",1,0,"QHBoxLayout");
    qmlRegisterType<QVBoxLayout>("QtClass",1,0,"QVBoxLayout");
    qmlRegisterType<QFormLayout>("QtClass",1,0,"QVBoxLayout");
    qmlRegisterType<QTreeWidget>("QtClass",1,0,"QTreeWidget");
//    qmlRegisterType<QTreeWidgetItem>("QtClass",1,0,"QTreeWidgetItem");

    qmlRegisterType<QmlFactory>("QtClass",1,0,"QmlFactory");
    qmlRegisterType<Person>("QtClass",1,0,"Person");
    qmlRegisterType<Led>("QtClass",1,0,"Led");
    qmlRegisterType<QHBoxLayoutProxy>("QtClass",1,0,"QHBoxLayoutProxy");
    qmlRegisterType<QVBoxLayoutProxy>("QtClass",1,0,"QVBoxLayoutProxy");
    qmlRegisterType<QFormLayoutProxy>("QtClass",1,0,"QFormLayoutProxy");
    qmlRegisterType<QSpacerItemProxy>("QtClass",1,0,"QSpacerItemProxy");
    qmlRegisterType<IconLabel>("QtClass",1,0,"IconLabel");
    qmlRegisterType<QTreeWidgetProxy>("QtClass",1,0,"QTreeWidgetProxy");

    //func data
    qmlRegisterType<TreeDataDeviceStatus>("QtClass",1,0,"TreeDataDeviceStatus");

  }
}
