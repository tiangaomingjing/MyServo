#ifndef SERVOGENERALCMD_H
#define SERVOGENERALCMD_H

#include "servogeneralcmd_global.h"
#include <QObject>
#include <QMap>
class QTreeWidget;
class QTreeWidgetItem;

class SERVOGENERALCMDSHARED_EXPORT ServoGeneralCmd:public QObject
{
  Q_OBJECT
public:
  ServoGeneralCmd(QTreeWidget *cmdTree,qint16 comType,qint16 rnStation,QObject *parent=0);
  ~ServoGeneralCmd();
  //---------通用指令--------------
  double read(const QString &cmdReadName,qint16 axisIndex);
  double write(const QString &cmdWriteName,double value,qint16 axisIndex);

  void setComType(const qint16 comType);

  void setComRnStation(const qint16 comRnStation);

  void setCmdTree(QTreeWidget *cmdTree);

  qint16 comType() const;

  qint16 comRnStation() const;

private:
  void fillCmdTreeDataMaps(QTreeWidgetItem *item);

private:
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

  QMap<QString ,CmdTreeData>m_cmdTreeDataMaps;

  qint16 m_comType;
  qint16 m_comRnStation;
  QTreeWidget *m_cmdTree;
};

#endif // SERVOGENERALCMD_H
