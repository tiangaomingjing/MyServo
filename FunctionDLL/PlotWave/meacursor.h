#ifndef MEACURSOR_H
#define MEACURSOR_H

#include <QWidget>
#include <qcustomplot.h>
class QTimer;

class MeaCursor : public QWidget
{
  Q_OBJECT
public:
  explicit MeaCursor(QCustomPlot *plot, QWidget *parent = 0);
  virtual ~MeaCursor();


  virtual bool updateMeasureCursorPosition(float mouseX,float mouseY)=0;
  virtual QString measureInformation(void)=0;
  void setMouseDragFlag(void){m_isDrag=true;}
  bool isMouseDrag(void){return m_isDrag;}
  void clearMouseDrag(void){m_isDrag=false;}
  virtual void disablePlotViewMove();
  virtual void enablePlotViewMove();
  void show()
  {
    m_curveLeft->setVisible(true);
    m_curveRight->setVisible(true);
  }
  void hide()
  {
    m_curveLeft->setVisible(false);
    m_curveRight->setVisible(false);
  }

  void setCurve1Checked(bool checked);
  void setCurve2Checked(bool checked);
  void setActivedStateColor(bool active);
  bool curve1Selected(void);
  bool curve2Selected(void);

public slots:
    virtual void fillAllMeasureCursor(void)=0;
signals:
protected:
  virtual void initialMeaCursor(void)=0;
protected:
  QCPCurve *m_curveLeft;
  QCPCurve *m_curveRight;
  QCustomPlot *mp_plot;
  QPointF m_point[4];
  bool m_isDrag;
private slots:
  void onTimeOut();
private:
  QTimer *m_timer;
  int m_timerCount;
};

#endif // MEACURSOR_H
