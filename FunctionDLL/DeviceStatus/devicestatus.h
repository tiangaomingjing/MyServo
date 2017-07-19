#ifndef DEVICESTATUS_H
#define DEVICESTATUS_H

#include "devicestatus_global.h"
#include "abstractfuncwidget.h"
#include "ServoDriverComDll.h"

namespace Ui {
class DeviceStatus;
}
class QTreeWidget;
class QWidget;

class DEVICESTATUSSHARED_EXPORT DeviceStatus:public AbstractFuncWidget
{
  Q_OBJECT
public:
  DeviceStatus(QWidget *parent=0);
  ~DeviceStatus();
  bool isWrite2Ram()Q_DECL_OVERRIDE {return false;}
  bool isWrite2Flash()Q_DECL_OVERRIDE {return false;}

  //only use in qml function
  Q_INVOKABLE int getAxisNum()const {return this->m_axisNumber;}
  Q_INVOKABLE void clearAlarm(void);
signals:
  void netError(COM_ERROR error);
  void almError();
  void almClearFinish();
  void timeOutToQml();//更新时间到qml界面更新

public slots:
  //因为它的树里面有子节点，与其它不一样，所以重写其方法，做差异性变化
  void onActionReadFuncValueFromRam(void) Q_DECL_OVERRIDE;//从RAM中读取数据到功能模块树，同时连接到主程序中的请求信号（还可以右键弹出请求）

private slots:
  void onMainWindowTimeOut(int axisInex);
private:
  QTreeWidget* getUiTree()Q_DECL_OVERRIDE;
  QStackedWidget *getUiStackedWidget()Q_DECL_OVERRIDE;
  void setActionReadFuncValueFromFlashEnable() Q_DECL_OVERRIDE;
  void createUiByQml()Q_DECL_OVERRIDE;
  void connectionSignalSlotHandler() Q_DECL_OVERRIDE;

  int getBitValue(quint32 value,quint32 index);

private:
  Ui::DeviceStatus *ui;
  QQmlEngine *m_qmlEngine;
};

#endif // DEVICESTATUS_H
