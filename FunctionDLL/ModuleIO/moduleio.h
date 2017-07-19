#ifndef MODULEIO_H
#define MODULEIO_H

#include "moduleio_golbal.h"
#include "abstractfuncwidget.h"
#include "iowidget.h"
#include <QXmlStreamWriter>

namespace Ui {
class ModuleIO;
}
class QTreeWidget ;
class QStackedWidget;
class QTreeWidgetItem;
class IOWidget;

class MODULEIOSHARED_EXPORT ModuleIO : public AbstractFuncWidget
{
  Q_OBJECT

public:
  explicit ModuleIO(QWidget *parent = 0);
  ~ModuleIO();

public slots:
  void onWriteFuncTreetoServoRam(void)Q_DECL_OVERRIDE;//主程序中的 config 按钮请求(FLASH RAM无关)
  void onWriteFuncTreetoServoFlash(void)Q_DECL_OVERRIDE;//主程序中的 save 按钮请求(FLASH RAM无关)
  void onActionReadFuncValueFromRam(void)Q_DECL_OVERRIDE;//从RAM中读取数据到功能模块树（还可以右键弹出请求）
  void updateUiWhenNavigationTreeClicked(void)Q_DECL_OVERRIDE;
private slots:
  void onItemExpanded(QTreeWidgetItem*item)Q_DECL_OVERRIDE;
  void onTreeItemClickedEdit(QTreeWidgetItem *item,int column)Q_DECL_OVERRIDE;
  void onItemSelecttionChangedHighlight(void)Q_DECL_OVERRIDE;
  void onEnterKeyPress(void)Q_DECL_OVERRIDE;
  void onActionReadFuncValueFromFlash(){}//从Flas中读取数据到功能模块树（还可以右键弹出请求）

  //响应io界面的信息
  void onIOEnableClicked(bool checked);
  void onIOComboBoxActived(int index);
  void onIOPolarityChanged(bool checked);
  void onIONoteSelectionChanged();
  void onIONoteEnterPressed();

  void onMainWindowTimeOut(int time);

public:
  void setTreeWidgetHeaderLables()Q_DECL_OVERRIDE{}
  void setActiveNow(bool actived)Q_DECL_OVERRIDE;//当前的窗口激活显示时，可以增加自己的功能(在速度环，位置环时就需要重绘QML UI)
  bool isWrite2Flash()Q_DECL_OVERRIDE {return true;}
  bool isWrite2Ram()Q_DECL_OVERRIDE{return false;}

private:
  QTreeWidget* getUiTree()Q_DECL_OVERRIDE;
  QStackedWidget* getUiStackedWidget()Q_DECL_OVERRIDE;
  void setActionReadFuncValueFromFlashEnable()Q_DECL_OVERRIDE;

  void setUiWidgetStyle(void);

//  void buildTreeWidget(QString &fileName_noxml)Q_DECL_OVERRIDE{}
//  void saveTreeXMLFile(QString &fileName)Q_DECL_OVERRIDE{}
//  void setTreeStyle(QTreeWidget *tree)Q_DECL_OVERRIDE{}
  void connectionSignalSlotHandler()Q_DECL_OVERRIDE;//为了几个不同的功能模块有关不同的信号与槽连接关系
  void createUiByQml()Q_DECL_OVERRIDE;

  bool writeIODescription(const QString &fileName);
  void writeStructXmlFile(QTreeWidgetItem *item, QXmlStreamWriter &writer);

  quint32 operationValue(quint32 bitstart, quint32 bitwidth);
  bool getBit(QTreeWidgetItem *item,quint32 &value);
  bool setBit(QTreeWidgetItem *item, bool bit);

private:
  Ui::ModuleIO *ui;
  QList<IOWidget *>m_ioWidgetList;
};

#endif // MODULEIO_H
