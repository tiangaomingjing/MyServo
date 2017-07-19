#include "threadsample.h"
#include "plotwave.h"
#define SAMPLING_PRECISE 0.0625
#define ARRAY_MAXSIZE 10000

ThreadSample::ThreadSample(PlotWave *plotwave):
  mp_plotWave(plotwave),
  m_bRun(true)
{

}
ThreadSample::~ThreadSample()
{
  qDebug()<<"threadSample release ->";
}

//-----------------public function-----------------------
//停止线程
void ThreadSample::stop()
{
  m_bRun = false;
}

//-----------------private function-----------------------
void ThreadSample::startSampling( int16 axisNum,  WAVE_BUF_PRM &wave)
{
  GTSD_CMD_StartPlot(axisNum,wave,mp_plotWave->getUserConfigFromMainWidow()->com.id,mp_plotWave->getUserConfigFromMainWidow()->com.rnStation);
}
void ThreadSample::stopSampling( int16 axisNum, WAVE_BUF_PRM &wave)
{
  GTSD_CMD_StopPlot(axisNum,wave,mp_plotWave->getUserConfigFromMainWidow()->com.id,mp_plotWave->getUserConfigFromMainWidow()->com.rnStation);
}
short ThreadSample::getDspData( int16 axisNum, double** data, int32& number)
{
  return GTSD_CMD_PcGetWaveData(axisNum,data,number,mp_plotWave->getUserConfigFromMainWidow()->com.id,mp_plotWave->getUserConfigFromMainWidow()->com.rnStation);
}

//-------------------protected function------------------
void ThreadSample::run()
{
  qDebug()<<"run .....";
  //读曲线中脚本自定义的数据常量输入
  mp_plotWave->updateTablePlotPrmListConstValueWhenSampling();
//  mp_plotWave->evaluateCurveSamplingScript();

  int axisCount=  mp_plotWave->getUserConfigFromMainWidow()->model.axisCount;
  double samplingTime=mp_plotWave->getSamplingTime();
  int samplingCount=samplingTime/SAMPLING_PRECISE;
  if(samplingCount==0)
    samplingCount=1;
  QList<WAVE_BUF_PRM>wavePrmList;
  WAVE_BUF_PRM wavePrm;

  for(int i=0;i<axisCount/2;i++)
    wavePrmList.append(wavePrm);

  //先归好不同轴的表格类
  QList<QList<PlotTablePrm>>plotTablePrmDoubleList;
  QList<PlotTablePrm>plotTablePrmList;
  for(int i=0;i<axisCount;i++)//4个空的表list
    plotTablePrmDoubleList.append(plotTablePrmList);

  PlotTablePrm tablePrm;
  int axisNum=0;
  foreach (tablePrm, mp_plotWave->getTableParameterList())//分离每一个轴到每个表中
  {
    axisNum=tablePrm.axisNum;
    plotTablePrmDoubleList[axisNum].append(tablePrm);
  }

  //4合成2个表(dspa,dspb)(0,1 : 2,3)
  QList<QList<PlotTablePrm>>plotTableControlDoubleList;
  for(int i=0;i<axisCount/2;i++)//2个空的表list
    plotTableControlDoubleList.append(plotTablePrmList);
  for(int i=0;i<axisCount;i++)
  {
    int index=i/2;
    foreach (tablePrm,plotTablePrmDoubleList[i])
    {
      plotTableControlDoubleList[index].append(tablePrm);
    }
  }

  int dspIndex=0;
  foreach (plotTablePrmList,plotTableControlDoubleList)//填控制每一个dsp的wave_buf结构体
  {
//    int curveCount=plotTablePrmList.size();
    int curveCount=0;
    int index=0;
    foreach (tablePrm, plotTablePrmList) //遍历里面有多少条曲线
    {
      if(tablePrm.curveType==CURVE_TYPE_SCRIPT)
      {
        curveCount+=tablePrm.varCount;
        for(int i=0;i<tablePrm.varCount;i++)
        {
          wavePrmList[dspIndex].inf[index].bytes=tablePrm.prmFactorList.at(i).controlPrm.bytes;
          wavePrmList[dspIndex].inf[index].ofst=tablePrm.prmFactorList.at(i).controlPrm.offsetAddr;
          wavePrmList[dspIndex].inf[index].base=tablePrm.prmFactorList.at(i).controlPrm.baseAddr;
          index++;
        }
      }
      else
      {
        curveCount++;
        if(tablePrm.curveType==CURVE_TYPE_SCRIPT_CONST)//从正常曲线拖过来，只有一个动态变量
        {
          wavePrmList[dspIndex].inf[index].bytes=tablePrm.prmFactorList.at(0).controlPrm.bytes;
          wavePrmList[dspIndex].inf[index].ofst=tablePrm.prmFactorList.at(0).controlPrm.offsetAddr;
          wavePrmList[dspIndex].inf[index].base=tablePrm.prmFactorList.at(0).controlPrm.baseAddr;
        }
        else//正常曲线
        {
          wavePrmList[dspIndex].inf[index].bytes=tablePrm.bytes;
          wavePrmList[dspIndex].inf[index].ofst=tablePrm.offsetAddr;
          wavePrmList[dspIndex].inf[index].base=tablePrm.baseAddr;
        }
        index++;
      }

    }
    wavePrmList[dspIndex].cmd.bit.NUM=curveCount;
    wavePrmList[dspIndex].cmd.bit.ENP=1;
    wavePrmList[dspIndex].cmd.bit.TIM=samplingCount;

    dspIndex++;
  }

  for(int i=0;i<axisCount/2;i++)//先关闭采样连接（主要是因为非正常关闭时要点两次）
  {
    wavePrm=wavePrmList.at(i);
    if(plotTableControlDoubleList[i].size()>0)
      stopSampling(0+2*i,wavePrm);
  }
  for(int i=0;i<axisCount/2;i++)//开始采样，打开偶数的轴，同时传入wave结构体
  {
    wavePrm=wavePrmList.at(i);
    if(plotTableControlDoubleList[i].size()>0)
      startSampling(0+2*i,wavePrm);
  }

  //定义要发送的数据
  dspIndex=0;

  int firstAxisCount=0;
  CurveData data;
  int32 retPointCount=0;

  double *ppValue=NULL;
  QVector<quint64>withTimeTotalPointCount;
  withTimeTotalPointCount.resize(axisCount/2);
  qDebug()<<"start run .....";

  while(m_bRun)
  {
//    qDebug()<<"thread run.....";
    for(int i=0;i<axisCount/2;i++)
    {
      if(plotTableControlDoubleList[i].size()>0)//第i个DSP轴有曲线的选择
      {
        int result=-1;
        int curveTotalCount=0;
        int firstTemp=0;
        double key,value;
        dspIndex=i;
//        curveTotalCount=plotTableControlDoubleList[i].size();
        PlotTablePrm prm;
        foreach (prm, plotTableControlDoubleList[i])
        {
          if(prm.curveType==CURVE_TYPE_SCRIPT)
          {
            curveTotalCount+=prm.varCount;
            if(prm.axisNum==2*i)
              firstTemp+=prm.varCount;
          }
          else
          {
            curveTotalCount++;
            if(prm.axisNum==2*i)
              firstTemp++;
          }
        }

        firstAxisCount=firstTemp;
        result=getDspData(0+2*i,&ppValue,retPointCount);
//        qDebug()<<"result value:"<<result<<"return pointCount"<<retPointCount;
        if(result==0)//通讯正常
        {
          if(retPointCount>0)//有数据到来
          {
            for(int row=0;row<curveTotalCount;row++)//将采集到的所有数据放到vector中
            {
              for(int column=0;column<retPointCount;column++)
              {
                key=(withTimeTotalPointCount.at(i)+column)*samplingTime*0.001;//*0.001ms->s
                value=ppValue[row*ARRAY_MAXSIZE+column];
                data.curveKey.append(key);
                data.curveValue.append(value);
//                plotTableControlDoubleList[i][row].data.curveKey.append(key);
//                plotTableControlDoubleList[i][row].data.curveValue.append(value);
              }
            }
            withTimeTotalPointCount[i]+=retPointCount;
            emit sendPlotData(2*i,firstAxisCount,retPointCount,data);
//            qDebug()<<"emit data finish.....";
          }
        }
        else
        {
          //发送通讯错误信息(目前没有作用，因为它getDspData的返回值没有作通信的处理)
//          emit comDisconnected(result);
          qDebug()<<"************disconnect***********";
          m_bRun=false;
        }
      }
      data.curveKey.clear();
      data.curveValue.clear();
    }
    msleep(80);
  }


  for(int i=0;i<axisCount/2;i++)//关闭采样连接
  {
    wavePrm=wavePrmList.at(i);
    if(plotTableControlDoubleList[i].size()>0)
      stopSampling(0+2*i,wavePrm);
  }
  m_bRun=true;
}
