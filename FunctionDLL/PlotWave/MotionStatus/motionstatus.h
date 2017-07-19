#ifndef MOTIONSTATUS_H
#define MOTIONSTATUS_H

#include <QObject>
class QTreeWidget;
class ServoGeneralCmd;
class QTreeWidgetItem;
class QTimer;

class MotionStatus : public QObject
{
  Q_OBJECT
public:
  explicit MotionStatus(ServoGeneralCmd *cmd,quint16 axis,QObject *parent = 0);
  ~MotionStatus();
  enum ControlSource{SOURCE_PC=0,SOURCE_GLINK2,SOURCE_COUNT};
  enum MotionMode{MOTION_MODE_NONE=0,MOTION_MODE_VEL_CLOSE,MOTION_MODE_POS,MOTION_COUNT};
  enum PlanVelMode{VELOCITY_NOCIRCLE=0,VELOCITY_CIRCLE,VELOCITY_COUNT};

  QTimer *planeVelTimer(void){return m_planVelTimer;}

  MotionStatus::ControlSource controlSrc();
  void setControlSrc(MotionStatus::ControlSource controlSrc);

  MotionStatus::MotionMode motionMode() const;
  bool setMotionMode(MotionStatus::MotionMode motionMode);

  void setPlanVelMode(PlanVelMode velMode);
  PlanVelMode currentPlanVelMode()const;

  bool setServoSpeed(double value);

  double AMPNoCircleValue() const;
  bool setAMPNoCircleValue(double value);

  double timesNoCircleValue() const;
  bool setTimesNoCircleValue(double value);

  double AMPCircleValue() const;
  bool setAMPCircleValue(double value);

  double TcircleValue() const;
  bool setTcircleValue(double value);

  double countCircleValue()const;
  bool setCountCircleValue(double value);

  void startPlanVelTimer(void);
  void stopPlanVelTimer(void);

  void setRunningPrepare();
  void setServoOn(bool on);
  bool checkServoIsOn(void);

  void delayms(quint16 ms);

  bool running() const;
  void setRunning(bool running);

signals:
  void planVeltimerStop(int axisIndex);
  void updateProgressValue(int value);
  void servoIsRunning(bool on);

private slots:
  void onPlanVelTimerOut(void);
private:
  void casePlanVelocityMotionPrepar();
  void casePlanVelocityMotionWhenSetRunning(bool run);

private:
  typedef struct _plan_vel_noCircle
  {
    double AMP;
    double times;
    quint32 totalCount;
    quint32 currentCount;
  }PlanVelNoCircle;

  typedef struct _plan_vel_circle
  {
    double AMP;
    double T;
    quint32 count;
    quint32 currentCount;
    bool change;
  }PlanVelCircle;

  typedef struct _plan_vel
  {
    PlanVelMode currentVelMode;
    PlanVelNoCircle noCircle;
    PlanVelCircle circle;
  }PlanVel;

  typedef struct _plan_Motion
  {
    MotionMode currentMode;
    PlanVel velPlan;
  }PlanMotion;

  ControlSource m_controlSrc;
  quint16 m_axisIndex;
  ServoGeneralCmd *m_cmd;
  bool m_running;//上伺服
  PlanMotion m_planMotion;
  QTimer *m_planVelTimer;
};

#endif // MOTIONSTATUS_H
