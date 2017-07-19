#ifndef MEASURETAG_H
#define MEASURETAG_H

#include <QObject>
#include <qcustomplot.h>
#include <QList>

class MeasureTag : public QObject
{
  Q_OBJECT
public:
  explicit MeasureTag(QCustomPlot *plot, QObject *parent = 0);
   ~MeasureTag(void);

  void addMeasureTag(float x, float y, QCPGraph *graph);
  void clearMeasureTag(void);

signals:

public slots:
private:
  QPointF checkRange(float x,float y);
private:
  typedef struct _itemTracer
  {
    QCPItemTracer * tacer;
    QCPItemText *text;
    QCPItemCurve *arrow;
  }ItemTracer;
  QCustomPlot *mp_plot;
  QList<ItemTracer *> m_tracerList;
};

#endif // MEASURETAG_H
