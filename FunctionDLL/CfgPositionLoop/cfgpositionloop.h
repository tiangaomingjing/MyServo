#ifndef CFGPOSITIONLOOP_H
#define CFGPOSITIONLOOP_H

#include "cfgpositionloop_global.h"
#include "abstractfuncwidget.h"

namespace Ui {
class CfgPositionLoop;
}
class QTreeWidget;
class QWidget;

class CFGPOSITIONLOOPSHARED_EXPORT CfgPositionLoop:public AbstractFuncWidget
{
Q_OBJECT
public:
  CfgPositionLoop(QWidget *parent=0);
  ~CfgPositionLoop();
  bool isWrite2Ram()Q_DECL_OVERRIDE {return true;}
  bool isWrite2Flash()Q_DECL_OVERRIDE {return true;}
signals:
  bool activeNow(void);//向外发送信号

private:
  QTreeWidget* getUiTree();
  QStackedWidget *getUiStackedWidget();
  void setActionReadFuncValueFromFlashEnable() Q_DECL_OVERRIDE;
  void createUiByQml()Q_DECL_OVERRIDE;
  void setActiveNow(bool actived)Q_DECL_OVERRIDE;

private:
  Ui::CfgPositionLoop *ui;
};

#endif // CFGPOSITIONLOOP_H
