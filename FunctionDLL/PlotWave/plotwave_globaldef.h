#ifndef PLOTWAVE_GLOBALDEF
#define PLOTWAVE_GLOBALDEF
#include <MuParser/include/muParser.h>

#include <QtCore>
#include <QJSValue>
#include <QColor>

typedef enum{
  ROW_CURVESETTING_INDEX_VIEWSETTING=0,
  ROW_CURVESETTING_INDEX_PRM_ROW
}RowCurveSettingIndex;

typedef enum{
  COL_CURVESETTING_INDEX_KEY=0,
  COL_CURVESETTING_INDEX_VALUE,
  COL_CURVESETTING_INDEX_DESCRIPTION
}ColumnCurveSettingIndex;

typedef enum{
  ROW_VIEWSETTING_INDEX_XYRANGE=0,
  ROW_VIEWSETTING_INDEX_SAMPLE_TIME,
  ROW_VIEWSETTING_INDEX_CURRENT_VIEW
}RowViewSettingIndex;

typedef enum{
  ROW_XYRANGE_INDEX_XTIME=0,
  ROW_XYRANGE_INDEX_YMAX,
  ROW_XYRANGE_INDEX_YMIN,
  ROW_XYRANGE_INDEX_STORETIME,
  ROW_XYRANGE_INDEX_POINTNUM,
}RowXYRangeIndex;

typedef enum{
  ROW_PRM_INDEX_CURVEID,
  ROW_PRM_INDEX_ISDRAW,
  ROW_PRM_INDEX_FULLNAME,
  ROW_PRM_INDEX_COLOR,
  ROW_PRM_INDEX_AXISNUM,
  ROW_PRM_INDEX_BYTESNUM,
  ROW_PRM_INDEX_OFFSETADDR,
  ROW_PRM_INDEX_BASEADDR,
  ROW_PRM_INDEX_ADDVALUE,
  ROW_PRM_INDEX_MULTIVALUE,
  ROW_PRM_INDEX_XOFFSET,
  ROW_PRM_INDEX_EQUATIONDESCRIPTION,
  ROW_PRM_INDEX_PRMFACTOR
}RowPrmIndex;

typedef enum{
  ROW_COLOR_INDEX_R=0,
  ROW_COLOR_INDEX_G,
  ROW_COLOR_INDEX_B
}RowColorIndex;

typedef enum{
  ROW_FACTOR_INDEX_FULLNAME=0,
  ROW_FACTOR_INDEX_BYTESNUM,
  ROW_FACTOR_INDEX_OFFSETADDR,
  ROW_FACTOR_INDEX_BASEADDR
}RowFactorIndex;

typedef enum{
  CURVE_TYPE_RAW=0,
  CURVE_TYPE_SCRIPT,
  CURVE_TYPE_SCRIPT_CONST//直接取过来的数，不用脚本
}CurveType;


typedef enum{
  PLOT_PRM_FACTOR_TYPE_CONST=0,
  PLOT_PRM_FACTOR_TYPE_VARIABLE
}PlotPrmFatorTypeID;

typedef struct _plotControlPrm
{
  QString fullName;
  int bytes;
  quint16 offsetAddr;
  quint16 baseAddr;
}PlotControlPrm;

typedef struct _curveData
{
  QVector<double>curveKey;
  QVector<double>curveValue;
}CurveData;

typedef struct _constPlotFator
{
  PlotControlPrm controlPrm;
  double constValue;
}ConstPlotFator;
typedef struct _varPlotFator
{
  PlotControlPrm controlPrm;
  CurveData varDatas;
}VarPlotFator;

typedef struct _viewSetting
{
  int xtime;
  double ymax;
  double ymin;
  int storeTime;
  quint16 pointNum;
  double samplingTime;//采样时间
  int curViewIndex;
}ViewSetting;

typedef struct _plotTablePrmFactor
{
  PlotPrmFatorTypeID factorTypeId;
  PlotControlPrm controlPrm;
  double constValue;
  CurveData varDatas;
}PlotTablePrmFactor;

typedef struct _plotTablePrm
{
  CurveType curveType;
  int id;//正常的曲线的id都是0，只有用户里才从1---
  bool isDraw;
  QString fullName;
  QColor color;
  int axisNum;
  int bytes;
  quint16 offsetAddr;
  quint16 baseAddr;
  double addValue;
  double multiValue;
  double xoffset;
  CurveData data;//最终结果数据
  QString equationDescription;//方程函数关系
  int varCount;
  QList<PlotTablePrmFactor> prmFactorList;
}PlotTablePrm;

typedef enum enum_prm_table_col : int
{
  COL_PRM_TABLE_DRAW,				//绘图
  COL_PRM_TABLE_NAME,				//参数名或文件名
  COL_PRM_TABLE_BYTESNUM,			//字节数
  COL_PRM_TABLE_AXISNUM,				//偏移地址
  COL_PRM_TABLE_OFFSET,				//偏移地址
  COL_PRM_TABLE_BASE,//偏移基地址
  COL_PRM_TABLE_ADD,//加因子
  COL_PRM_TABLE_MULTI,//相乘因子
  COL_PRM_TABLE_XOFFSET,

  COL_PRM_TABLE_COUNT				//列数
}ENUM_PRM_TABLE_COL;

#endif // PLOTWAVE_GLOBALDEF

