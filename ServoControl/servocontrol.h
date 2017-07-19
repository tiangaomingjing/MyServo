#ifndef SERVOCONTROL_H
#define SERVOCONTROL_H

#include <QObject>
#include "globaldef.h"
#include "Basetype_def.h"
#include "ServoDriverComDll.h"

class QTreeWidget;

class ServoControl : public QObject
{
  Q_OBJECT
public:
  explicit ServoControl(QObject *parent = 0);
  ~ServoControl();
  static bool checkedCmdReturnValue(COM_ERROR retvalue); 
  static void updateFlashTreeWidget(QTreeWidget *tree, int axisIndex, COM_TYPE comtype, short comStation);
  static void updateAllFlashTreeWidget(QTreeWidget *treeSrc, COM_TYPE comtype, short station);
  static void updateFlashTreeWidgetByExpanded(QTreeWidgetItem *item, int axisIndex, COM_TYPE comtype, short station);
  static void updateRamTreeWidgetByExpanded(QTreeWidgetItem *item, int axisIndex, const QMap<QString,QVariant>&map,COM_TYPE comtype, short station);

  static COM_ERROR updateFunctionValueFromRam(QTreeWidgetItem *item, QTreeWidget *cmdTree, int axisIndex, COM_TYPE comtype, short comStation);
  static COM_ERROR updateFunctionValueFromRamAllTree(QTreeWidget *tree,QTreeWidget *cmdTree,int axisIndex,COM_TYPE comtype,short comStation);
  static void updateFunctionValueFromFlash(QTreeWidgetItem *item, QTreeWidget *cmdTree, quint16 offsetAddr, int axisIndex, COM_TYPE comtype, short comStation);
  static void updateFunctionValueFromFlashAllTree(QTreeWidget *tree, QTreeWidget *cmdTree, quint16 offsetAddr, int axisIndex, COM_TYPE comtype, short comStation);

  static void writeServoFlash(QTreeWidgetItem *item, int axisIndex, COM_TYPE comtype, short comStation);
  static void writeServoRam(QTreeWidgetItem *item, int axisIndex,const QMap<QString,QVariant>&map, COM_TYPE comtype, short comStation);
  void writeServoFlashByAxisTree(QTreeWidget *tree, int axisIndex, COM_TYPE comtype, short comStation);
  void writeServoFlashByAllAxisTree(QTreeWidget *alltree, COM_TYPE comtype, short comStation);

  static void writeFunctionValue2Ram(QTreeWidgetItem *item, QTreeWidget *cmdTree, int axisIndex, COM_TYPE comtype, short comStation);
  static void writeFunctionValue2RamAllTree(QTreeWidget *tree, QTreeWidget *cmdTree, int axisIndex, COM_TYPE comtype, short comStation);
  static void writeFunctionValue2Flash(QTreeWidgetItem *item, QTreeWidget *cmdTree, quint16 offsetAddr, int axisIndex, COM_TYPE comtype, short comStation);
  static void writeFunctionValue2FlashAllTree(QTreeWidget *uiTree, QTreeWidget *cmdTree, quint16 offsetAddr, int axisIndex, COM_TYPE comtype, short comStation);

  static void clearAlarm(short axisIndex, COM_TYPE comType , short comStation);
  static qint16 readDeviceVersion(int axis, quint16 &version, qint16 comType=GTSD_COM_TYPE_NET, short comStation=0xf0);
  static qint16 readDeviceFirmwareVersion(int axis,quint16 &fversion,qint16 comType=GTSD_COM_TYPE_NET,short comStation=0xf0);

  static bool checkServoIsReady(int axis, int16 com_type = GTSD_COM_TYPE_NET, short comStation = 0xf0);
  static qint16 servoCurrentUserTaskMode(int axis, int16 com_type = GTSD_COM_TYPE_NET, short comStation  = 0xf0);
signals:
  void progressValue(int axis,int value);

public slots:
private:
  quint32 totalItemCount(QTreeWidget*tree);
};

#endif // SERVOCONTROL_H
