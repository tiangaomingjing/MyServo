#ifndef GLOBALDEF
#define GLOBALDEF
#include <QtCore>
#include <QApplication>
#include <QList>
#include <QTreeWidgetItem>

#define FILELIST_MAXSIZE 10
#define ICON_MOTOR "motor.png"
#define ICON_CONFIGTREE "configtree.png"
#define ICON_CONNECTOK "connectok.png"
#define ICON_CONNECTERROR "connecterror.png"
#define ICON_CHILD "child.png"
#define ICON_MENU_CONFIG "menu_config.png"
#define ICON_MENU_NEW "menu_new.png"
#define ICON_MENU_SAVE "menu_save.png"
#define ICON_MENU_SAVEAS "menu_saveas.png"
#define ICON_MENU_OPEN "menu_open.png"
#define ICON_MENU_EXIT "menu_exit.png"
#define ICON_MENU_CONNECT "menu_connect.png"
#define ICON_MENU_DISCONNECT "menu_disconnect.png"
#define ICON_PIC_DIALOG_1 "configdialog1.png"
#define ICON_PIC_DIALOG_2 "configdialog2.png"
#define ICON_ETHERNETPORT "ethernetport.png"
#define ICON_FIRMWARE "firmware.png"
#define ICON_MENU_HISTORY_FILE "menu_recentfile.png"
#define ICON_MENU_TREEVIEW "menu_treeview.png"
#define ICON_MENU_PLOTVIEW "menu_plotview.png"
#define ICON_MENU_XMLUPDATE "menu_updatexml.png"
#define ICON_MENU_FILE2SERVO "menu_file2servo.png"
#define ICON_MENU_SERVO2FILE "menu_servo2file.png"
#define ICON_MENU_FULLSCREEN "menu_viewfull.png"
#define ICON_MENU_SMALLSCREEN "menu_viewsmall.png"
#define ICON_MENU_RESETSERVO "menu_reload.png"
#define ICON_MENU_RESTORESETTING "menu_restoresetting.png"
#define ICON_MENU_DOWNLOAD "menu_download.png"
#define ICON_TREE_GLOBAL_ANALYSIS "tree_analysis.png"
#define ICON_TREE_GLOBAL "tree_global.png"
#define ICON_TREE_GLOBAL_IO "tree_iomodule.png"

#define ICON_STATUS_CONNECT "connect.png"
#define ICON_STATUS_DISCONNECT "disconnect.png"
#define ICON_STATUS_LED_ON "ledon.png"
#define ICON_STATUS_LED_OFF "ledoff.png"
#define ICON_STATUS_SERVO_ERROR "servoerror.png"
#define ICON_STARTUP "startup.png"
#define ICON_FUNCCONFIG "func_config.png"
#define ICON_FUNCSAVE "func_save.png"
#define ICON_CLONE "clone.png"
#define ICON_FPGA_CONFIG "fpga.png"
#define ICON_FILE_CLONE "fileclone.png"
#define ICON_CONFIG_WIZARD "configwizard.png"
#define ICON_SERVO_DEVICE "servo_device.png"
#define ICON_SDT_LOGO "sdtlogo.png"

#define EXE_FILE_PATH                 QString(QApplication::applicationDirPath())
#define BIN_FILE_PATH                 GlobalFunction::binFilePath()
#define CUSTOM_START_FILE_PATH        QString(BIN_FILE_PATH+"Custom/configlist.xml")
#define CUSTOM_FILE_PATH              QString(BIN_FILE_PATH+"Custom/")
#define CUSTOM_DEFAULT_FILE_PATH      QString(CUSTOM_FILE_PATH+"configdefault/configdefault.xml")
#define CUSTOM_DEFAULT_WATCHCURVE_PATH      QString(CUSTOM_FILE_PATH+"configdefault/watchcurvesetting.xml")
#define CUSTOM_DEFAULT_WATCHCURVE_USER_PATH      QString(CUSTOM_FILE_PATH+"configdefault/watchcurvesetting_user.xml")
#define CUSTOM_USER_FILE_PATH         QString(CUSTOM_FILE_PATH+"configuser/")
#define SYSCONFIG_FILE_PATH           QString(BIN_FILE_PATH+"SystemConfiguration/")
#define SYSCONFIG_START_FILE_PATH     QString(SYSCONFIG_FILE_PATH+"SysConfig.xml")
#define ICON_FILE_PATH                QString(BIN_FILE_PATH+"Resource/Icon/")
#define RESOURCE_FILE_PATH            QString(BIN_FILE_PATH+"Resource/")
#define FPGA_SETTING_NAME_PCDEBUG             "FpgaFlashControl_PcDebug.xml"
#define FPGA_SETTING_NAME_RNNET             "FpgaFlashControl_RnNet.xml"

typedef struct _com
{
  int  id;
  QString comName;
  QString ipAddress;
  int ipPort;
  qint16 rnStation;
}Com;
typedef struct _model
{
  int  axisCount;
  QString modelName;
  QStringList version;
}Model;
typedef struct _systype
{
  int typeId;
  QString  typeName;
  QList<Model>modelList;
  QTreeWidgetItem *treeItem;
}SysType;

typedef struct _sysconfig
{
  QList<Com>comList;//端口
  QList<SysType> sysTypeList;//树
}SysConfig;

typedef struct _fileRecent
{
  bool firstBit;
  QStringList filePathList;
}FileRecent;
typedef struct _userConfig
{
  Com com;
  int typeId;
  QString typeName;
  Model model;
  QTreeWidgetItem *treeItem;
}UserConfig;

typedef struct _highLightInfo
{
  QTreeWidgetItem *editItem;
  bool editFlag;
  bool enterFlag;
  QString prevItemText;
}HighLightInfo;
namespace GlobalFunction
{
  int getBytesNumber(QString &str);
  QTreeWidgetItem* findTopLevelItem(QTreeWidgetItem *item);
  void delayms(quint16 ms);
  QString exeFilePath(void);
  QString binFilePath(void);
}

#endif // GLOBALDEF

