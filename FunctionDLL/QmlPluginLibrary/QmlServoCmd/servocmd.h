#ifndef SERVOCMD_H
#define SERVOCMD_H

#include <QObject>
#include <QtCore>
class QTreeWidget;
class QTreeWidgetItem;
class TreeModel;
class TreeItem;

typedef struct{
  QString type;
  QString unit;
  int cmd;
  int length;
  int setIndex;
  int getIndex;
  double kgain;
  int id;
}CmdTreeData;

typedef struct{
  QString value;
  QString valueType;
  unsigned short address;
}AdvFastCmdData;

class ServoCmd : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(ServoCmd)
//  Q_ENUMS(CmdType)
  Q_PROPERTY(int axisIndex READ axisIndex WRITE setAxisIndex NOTIFY axisIndexChanged)
  Q_PROPERTY(int comType READ comType WRITE setComType NOTIFY comTypeChanged)
  Q_PROPERTY(unsigned short comStation READ comStation WRITE setComStation NOTIFY comStationChanged)

public:
  ServoCmd(QObject *parent = 0);
  ~ServoCmd();

  int axisIndex(){return m_axisIndex;}
  void setAxisIndex(int axis){m_axisIndex=axis;emit axisIndexChanged(m_axisIndex);}

  int comType(){return m_comType;}
  void setComType(int comtype){m_comType=comtype;emit comTypeChanged(m_comType);}

  unsigned short comStation(){return m_comRnStation;}
  void setComStation(unsigned short station){m_comRnStation=station;emit comStationChanged(station);}

//--------------几大功能块写Flash指令--------------------------------
  //!使用之前,版本有更新要先loadExtendTree,loadCmdTree
  //!当写function 功能的Flash时分几步
  //!1 从原的function tree 中找到name fram uint16?
  //!2 从extendTree 中找到轴之前的偏移地址
  //!3 从cmd 中找到增益
  //!在writeFunctionTreeModel2Flash时，程序要先调用loadExtendTree loadCmdTree
  Q_INVOKABLE void loadExtendTree(const QUrl &extendTreeFilePath);
  Q_INVOKABLE unsigned short getOffsetAddress(int axis);
  Q_INVOKABLE int getDspTotalNumber();
  Q_INVOKABLE int getDspAxis();
  Q_INVOKABLE void loadCmdTree(const QUrl & cmdTreeFilePath);
  Q_INVOKABLE void writeFunctionTreeModel2Flash(TreeModel *model);//适用于qml中的树形结构TreeModel,写到Flash
  //功能块参数写到Flash中 根据wname 在treeFilePath找到偏移地址，找到增益 写入Flash
  Q_INVOKABLE void writeFunctionValue2Flash(const QUrl & treeFilePath,const QString &wname,double value);
  Q_INVOKABLE double readFunctionValueFromFlash(const QUrl & treeFilePath,const QString &wname);
  Q_INVOKABLE void updateFunctionTreeModelFromFlash(TreeModel *srcModel);

//---------------高级功能读写指令------------------
  //!当配置文件变化时要重新加载FastAccessCommandTree ,用来读写铁电
  Q_INVOKABLE bool loadAdvanceFastAccessCommandTree(const QUrl & treePath);
  Q_INVOKABLE quint64 writeAdvanceFlash(const QString &cmdName, qint64 value);
  Q_INVOKABLE quint64 readAdvanceFlash(const QString &cmdName);

//-----------------通用指令------------------------------
  //!使用前确定cmdtree 已经加载进来（只是写到RAM中）
  Q_INVOKABLE double getKgian(const QString &cmdName);
  Q_INVOKABLE QString readCommand(const QString cmdReadName);//通用的读指令(RAM)
  Q_INVOKABLE QString writeCommand(const QString cmdWriteName, double value);//通用的写指令(RAM)
//-----------------专用指令------------------------------
  Q_INVOKABLE bool setServoTaskMode(int mode);//设置伺服模式
  Q_INVOKABLE int setPosAdjRef(double value);//设置寻相值
  Q_INVOKABLE int setSpeedRef(double value);
  Q_INVOKABLE bool checkServoIsReady();
  Q_INVOKABLE bool setServoOn(bool on);//伺服使能
  Q_INVOKABLE int connectServo(bool connected);//使用之前设置好连接类型 m_comType
  Q_INVOKABLE int resetDsp(int axis);
  Q_INVOKABLE bool checkRestDspFinish(int axis);//axis= 0 2 4 8
  Q_INVOKABLE void delayMs(int ms);
  Q_INVOKABLE quint16 readDeviceVersion();

private:
  void writeFunctionTreeItem2Flash(TreeItem*item);
  void updateFunctionTreeItemFromFlash(TreeItem*item);
  void fillCmdTreeDataMaps(QTreeWidgetItem *item);
  static void connectProgressCallBack(void *arg,qint16 *value);

signals:
  void axisIndexChanged(int axisIndex);
  void comTypeChanged(int comType);
  void comStationChanged(unsigned int comStation);
public slots:
private:
  QTreeWidget *m_cmdTree;//这个从文件中获取
  QTreeWidget *m_extendTree;//扩展树
  int m_axisIndex;//设置当前操作的轴号
  int m_comType;//设置端口Id
  unsigned short m_comRnStation;
  QMap<QString ,CmdTreeData>m_cmdTreeDataMaps;
  QVector<unsigned short>m_addressOffsetVectors;
  QMap<QString,AdvFastCmdData>m_advFastCmdDataMaps;
};

#endif // SERVOCMD_H

