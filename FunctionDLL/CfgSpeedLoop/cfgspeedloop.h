#ifndef CFGSPEEDLOOP_H
#define CFGSPEEDLOOP_H

#include "cfgspeedloop_global.h"
#include "abstractfuncwidget.h"

namespace Ui {
class CfgSpeedLoop;
}
class QTreeWidget;
class QWidget;
class CFGSPEEDLOOPSHARED_EXPORT CfgSpeedLoop:public AbstractFuncWidget
{
Q_OBJECT
public:
  CfgSpeedLoop(QWidget *parent=0);
  ~CfgSpeedLoop();
  bool isWrite2Ram()Q_DECL_OVERRIDE {return true;}
  bool isWrite2Flash()Q_DECL_OVERRIDE {return true;}
signals:
  bool activeNow(void);//向外发送信号

private:
  QTreeWidget* getUiTree()Q_DECL_OVERRIDE;
  QStackedWidget *getUiStackedWidget()Q_DECL_OVERRIDE;
  void setActionReadFuncValueFromFlashEnable() Q_DECL_OVERRIDE;
  void createUiByQml()Q_DECL_OVERRIDE;
  void setActiveNow(bool actived)Q_DECL_OVERRIDE;

private:
  Ui::CfgSpeedLoop *ui;
};

#endif // CFGSPEEDLOOP_H
