#ifndef XMLBUILDER_H
#define XMLBUILDER_H

#include <QObject>
#include <globaldef.h>
#include <QXmlStreamReader>
#include <QTreeWidget>
#include <QTreeWidgetItemIterator>
#include <QXmlStreamWriter>

#define NORMAL_NAME "memberName"
#define NORMAL_VALUE "value"
#define NORMAL_TYPE "memberType"
#define NORMAL_OFFSET "addressOffset"
#define NORMAL_ISBITS "isBits"
#define NORMAL_BITSTART "bitStart"
#define NORMAL_BITWIDTH "bitWidth"
#define NORMAL_AXISNUM "axisNum"
#define NORMAL_HASCHILD "hasChild"
#define NORMAL_CHILDNUM "childNum"
#define NORMAL_PARENT "parent"

#define NORMAL_UINT "unit"
#define NORMAL_INTRODUCTION "introduction"
#define NORMAL_UPLIMIT "uplimit"
#define NORMAL_DOWNLIMIT "downlimit"
#define NORMAL_FRAMOFFSET "framAddr"

#define EXTENSION_OFFSETADDRESS "offsetAddress"
#define EXTENSION_DEVICE "device"
#define EXTENSION_ADVCONTROLPRM "advControlPrm"
#define EXTENSION_ADVCONTROLPRM_GSERVDRV "gSevDrv"
#define EXTENSION_ADVCONTROLPRM_GAUXFUNC "gAuxFunc"

enum COL_FLASH_RAM_INDEX
{
  COL_NAME,
  COL_VALUE,
  COL_TYPE,
  COL_ADDRESS,
  COL_ISBIT,
  COL_BITSTART,
  COL_BITWIDTH,
  COL_AXISNUM,
  COL_HASCHILD,
  COL_CHILDNUM,
  COL_PARENT
};
enum COL_FUNC_INDEX
{
  COL_FUNC_NAME,
  COL_FUNC_VALUE,
  COL_FUNC_TYPE,
  COL_FUNC_UINT,
  COL_FUNC_DOWNLIMIT,
  COL_FUNC_UPLIMIT,
  COL_FUNC_INTRODUCTION,
  COL_FUNC_OFFSET
};
enum COL_CMD_FUNC_INDEX
{
  COL_CMD_FUNC_NAME,
  COL_CMD_FUNC_TYPE,
  COL_CMD_FUNC_UNIT,
  COL_CMD_FUNC_CMD,
  COL_CMD_FUNC_LENGTH,
  COL_CMD_FUNC_SETINDEX,
  COL_CMD_FUNC_GETINDEX,
  COL_CMD_FUNC_KGAIN,
  COL_CMD_FUNC_ID
};
enum COL_FUNC_EXTENSION_INDEX
{
  COL_FUNC_EXTENSION_NAME=0,
  COL_FUNC_EXTENSION_PARAMETER,
  COL_FUNC_EXTENSION_INTRODUCTION
};
//row index
typedef enum{
  ROW_FUNC_EXT_INDEX_OFFSETADD,
  ROW_FUNC_EXT_INDEX_DEVICE,
  ROW_FUNC_EXT_INDEX_ADVCONTROLPRM,
  ROW_FUNC_EXT_INDEX_MODULEPRM
}RowFuncExtionIndex;

typedef enum{
  ROW_ADVCTLPRM_INDEX_SERDRV,
  ROW_ADVCTLPRM_INDEX_AUX
}RowAdvCtlPrmIndex;

typedef enum{
  ROW_MODULEPRM_INDEX_MOTOR
}RowModulePrmIndex;

typedef enum{
  ROW_MOTOR_AXIS_INDEX_IMAXINFO,
  ROW_MOTOR_AXIS_INDEX_GAININFO,
}RowMotorAxisIndex;

typedef enum{
  ROW_AXIS_GAININFO_INDEX_GA,
  ROW_AXIS_GAININFO_INDEX_GB,
  ROW_AXIS_GAININFO_INDEX_GC
}RowAxisGainInfoIndex;

typedef enum{
  ROW_GAINWRFLASH_INDEX_NAME,
  ROW_GAINWRFLASH_INDEX_TYPE,
  ROW_GAINWRFLASH_INDEX_OFFSETADDR
}RowGainWRFlashInfoIndex;
typedef enum{
  ROW_IMAXINFO_INDEX_NAME,
  ROW_IMAXINFO_INDEX_TYPE,
  ROW_IMAXINFO_INDEX_OFFSETADDR
}RowImaxInfoIndex;

class XmlBuilder : public QObject
{
  Q_OBJECT
public:
  explicit XmlBuilder(QObject *parent = 0);
  ~XmlBuilder();

public slots:

public:
  bool parseSysConfig(QString &filename);
  bool parseUserConfig(QString &filename);
  bool parseFileRecent(QString &filename);
  void saveUserConfigXml(QString &fileName ,FileRecent*recFile,UserConfig *userCfg);
  SysConfig *getSysConfig(void){return &m_sysConfig;}
  FileRecent *getFileRecentInfo(void){return &m_fileRecent;}
  UserConfig *getUserConfig(void){return &m_userConfig;}
  static QTreeWidget *readTreeWidgetFromFile(QString &filename);//读取树的数据
  static void saveFlashAllAxisTreeFile(QString &fileName,QTreeWidget *treeWidget);//保存输入树为Qt格式的xml文件
  static void saveFunctionTreeFile(QString &fileName,QTreeWidget *treeWidget);//保存输入树为Qt格式的xml文件
  static void setTreeValueFromTree(QTreeWidget *dstTree,QTreeWidget *srcTree);//应用于轴与轴之间的复制

//  static void fullAxisTreeWidgetNormalization(QTreeWidget *srcTreeWidget);
  static void ramFlashTreeWidgetNormalization(QTreeWidget *srcTreeWidget);//RAM FLASH树正确位置显示
  static void functionTreeWidgetNormalization(QTreeWidget *srcTreeWidget);//功能块树正确位置显示

private:
  void readType(QXmlStreamReader &reader);
  void readMode(SysType &sysType,QXmlStreamReader &reader);
  void readTree(SysType &sysType,QXmlStreamReader &reader);
  static void writeStructXmlFile(QTreeWidgetItem *item, QXmlStreamWriter &writer);
private:
  SysConfig m_sysConfig;
  FileRecent m_fileRecent;
  UserConfig m_userConfig;
};

#endif // XMLBUILDER_H
