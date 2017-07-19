#ifndef THREADSAMPLE_H
#define THREADSAMPLE_H

#include <QThread>
#include "servocontrol.h"
#include "plotwave_globaldef.h"
class PlotWave;

class ThreadSample : public QThread
{
  Q_OBJECT
public:
  ThreadSample(PlotWave *plotwave);
  ~ThreadSample();
  void stop();	//重载线程停止函数
signals:
  void sendPlotData(int dspNumber,int axisFirstCount,long getDataLength,CurveData data);

protected:
  void run();		//重载线程运行函数
private:
  void startSampling( int16 axisNum, WAVE_BUF_PRM &wave);
  void stopSampling( int16 axisNum, WAVE_BUF_PRM &wave);
  short getDspData( int16 axisNum, double** data, int32& number);
private:
  volatile bool m_bRun;	//线程是否在运行：使用了volatile关键字，这样可以使它在任何时候都保持最新的值，从而可以避免在多个线程中访问它时出错
  PlotWave *mp_plotWave;
};

#endif // THREADSAMPLE_H
