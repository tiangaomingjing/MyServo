#ifndef CFGCURRENTLOOP_H
#define CFGCURRENTLOOP_H

#include "cfgcurrentloop_global.h"
#include "abstractfuncwidget.h"

namespace Ui {
class CfgCurrentLoop;
}
class QTreeWidget;
class QWidget;

class CFGCURRENTLOOPSHARED_EXPORT CfgCurrentLoop:public AbstractFuncWidget
{
Q_OBJECT
public:
  CfgCurrentLoop(QWidget *parent=0);
  ~CfgCurrentLoop();
  bool isWrite2Ram()Q_DECL_OVERRIDE {return true;}
  bool isWrite2Flash()Q_DECL_OVERRIDE {return true;}
  void setActiveNow(bool actived) Q_DECL_OVERRIDE;
signals:
  void  activeNow(bool actived);//通知qml更新replot();

private:
  QTreeWidget* getUiTree()Q_DECL_OVERRIDE;
  QStackedWidget *getUiStackedWidget()Q_DECL_OVERRIDE;
  void createUiByQml()Q_DECL_OVERRIDE;

private:
  Ui::CfgCurrentLoop *ui;
};

#endif // CFGCURRENTLOOP_H
