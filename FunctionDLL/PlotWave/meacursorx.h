#ifndef MEACURSORX_H
#define MEACURSORX_H

#include <QWidget>
#include <meacursor.h>

class MeaCursorX : public MeaCursor
{
  Q_OBJECT
public:
  explicit MeaCursorX(QCustomPlot *plot, QWidget *parent = 0);
  virtual ~MeaCursorX();

  bool updateMeasureCursorPosition(float mouseX,float mouseY);
  QString measureInformation(void);
  double getMeasureXLeftPosition() {return m_point[0].x();}
  double getMeasureXRightPosition(){return m_point[2].x();}
signals:
  void measureLeftValueChanged(double value);
  void measureRightValueChanged(double value);
protected:
    void initialMeaCursor(void);
public slots:
    void fillAllMeasureCursor(void);
signals:

};

#endif // MEACURSORX_H
