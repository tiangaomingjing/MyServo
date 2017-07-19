#ifndef MEACURSORY_H
#define MEACURSORY_H

#include <QWidget>
#include <meacursor.h>

class MeaCursorY : public MeaCursor
{
  Q_OBJECT
public:
  explicit MeaCursorY(QCustomPlot *plot, QWidget *parent = 0);
  virtual ~MeaCursorY();
  bool updateMeasureCursorPosition(float mouseX,float mouseY);
  QString measureInformation(void);
public slots:
    void fillAllMeasureCursor(void);

protected:
    void initialMeaCursor(void);

signals:

};

#endif // MEACURSORY_H
