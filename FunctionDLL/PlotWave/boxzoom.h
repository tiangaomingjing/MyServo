#ifndef BOXZOOM_H
#define BOXZOOM_H

#include <QObject>
#include <QPointF>
#include "qcustomplot.h"
#define MAXSIZE_RECORD 5

class BoxZoom :public QCPCurve
{
public:
  explicit BoxZoom(QCustomPlot *plot);
  virtual ~BoxZoom();
  void updateBoxZoomShape();
  void updatePlotViewSize();

  void setStartPointF(float x,float y);
  void setEndPointF(float x,float y);
  void setMouseIsMoved(void){m_isMouseMove=true;}
  bool isMouseMoved(void) {return m_isMouseMove;}
  void clearMouseIsMoved(void){m_isMouseMove=false;}
  void disablePlotViewMove();
  void enablePlotViewMove();
public slots:
  void previousPlotView(void);
  void nextPlotView(void);
private:
  float checkRang(float value, bool isX=true);
private:
  typedef struct _rangeRecord
  {
    QCPRange xrang;
    QCPRange yrang;
  }RangeRecord;
  QList<RangeRecord> m_rangeRecord;
  QPointF m_startPoint;
  QPointF m_endPoint;
  QCustomPlot *mp_plot;
  bool m_isMouseMove;
  int m_viewIndex;
};

#endif // BOXZOOM_H
