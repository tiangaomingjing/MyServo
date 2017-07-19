#ifndef ADVRAM_H
#define ADVRAM_H

#include "advram_global.h"
#include "abstractfuncwidget.h"
#include "globaldef.h"
namespace Ui {
class AdvRAM;
}
class QTreeWidget;
class QWidget;

class ADVRAMSHARED_EXPORT AdvRAM:public AbstractFuncWidget
{
  Q_OBJECT
public:
  AdvRAM(QWidget *parent=0);
  ~AdvRAM(void);
  void setTreeWidgetHeaderLables();
  bool isWrite2Ram()Q_DECL_OVERRIDE {return false;}
  bool isWrite2Flash()Q_DECL_OVERRIDE {return false;}

public slots:
//  void updateTreeWidgetValuefromServo()Q_DECL_OVERRIDE;
  void onWriteFuncTreetoServoRam(void)Q_DECL_OVERRIDE{ }
  void onWriteFuncTreetoServoFlash()Q_DECL_OVERRIDE{}
  void onActionReadFuncValueFromRam(void)Q_DECL_OVERRIDE{ }
  void updateUiWhenNavigationTreeClicked()Q_DECL_OVERRIDE{}
private slots:
  void onItemExpanded(QTreeWidgetItem*item)Q_DECL_OVERRIDE;
  void onTreeItemClickedEdit(QTreeWidgetItem *item,int column)Q_DECL_OVERRIDE;
  void onItemSelecttionChangedHighlight(void)Q_DECL_OVERRIDE;
  void onEnterKeyPress(void)Q_DECL_OVERRIDE;

private:
  QTreeWidget* getUiTree()Q_DECL_OVERRIDE;
  QStackedWidget *getUiStackedWidget()Q_DECL_OVERRIDE;
  void buildTreeWidget(QString &fileName_noxml)Q_DECL_OVERRIDE;
  void setActionReadFuncValueFromFlashEnable() Q_DECL_OVERRIDE { }//空因为没有相关的右键action

private:
  Ui::AdvRAM *ui;
//  HighLightInfo m_highLightInfo;
};

#endif // ADVRAM_H
