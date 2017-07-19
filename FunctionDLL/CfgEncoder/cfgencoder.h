#ifndef CFGENCODER_H
#define CFGENCODER_H

#include "cfgencoder_global.h"
#include "abstractfuncwidget.h"
namespace Ui {
class CfgEncoder;
}
class QTreeWidget;
class QWidget;

class CFGENCODERSHARED_EXPORT CfgEncoder :public AbstractFuncWidget
{
Q_OBJECT
public:
  CfgEncoder(QWidget* parent=0);
  ~CfgEncoder(void);
  bool isWrite2Ram() Q_DECL_OVERRIDE {return false;}
  bool isWrite2Flash() Q_DECL_OVERRIDE {return true;}

  //use in qml
  Q_INVOKABLE bool getComConnectSatus();

signals:
  void activeNow(bool actived);//notify to QmlUi

private:
  QTreeWidget* getUiTree()Q_DECL_OVERRIDE;
  QStackedWidget *getUiStackedWidget()Q_DECL_OVERRIDE;
  void createUiByQml()Q_DECL_OVERRIDE;
  void setActiveNow(bool actived)Q_DECL_OVERRIDE;

private:
  Ui::CfgEncoder *ui;
};

#endif // CFGENCODER_H
