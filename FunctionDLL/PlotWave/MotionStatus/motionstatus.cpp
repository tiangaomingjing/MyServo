#include "motionstatus.h"
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QDebug>
#include <QTimer>
#include <QtMath>
#include <QTime>
#include <QCoreApplication>

#include "ServoDriverComDll.h"
#include "servogeneralcmd.h"

#define CMD_SRC_SEL_NAME "gSevDrv.sev_obj.pos.seq.prm.cmd_src_sel"
#define PTR_VALUE 0
#define MIN_INTERVAL 100

MotionStatus::MotionStatus(ServoGeneralCmd *cmd, quint16 axis, QObject *parent) :
  QObject(parent),
  m_axisIndex(axis),
  m_cmd(cmd),
  m_controlSrc(SOURCE_PC),
  m_running(false)
{
  m_planMotion.currentMode=MOTION_MODE_NONE;
  m_planMotion.velPlan.currentVelMode=VELOCITY_NOCIRCLE;
  m_planMotion.velPlan.noCircle.AMP=5;
  m_planMotion.velPlan.noCircle.times=5;
  m_planMotion.velPlan.circle.AMP=5;
  m_planMotion.velPlan.circle.T=1000;
  m_planMotion.velPlan.circle.count=10;
  m_planMotion.velPlan.circle.change=false;

  m_planVelTimer=new QTimer();
  m_planVelTimer->setInterval(MIN_INTERVAL);
  m_planVelTimer->setSingleShot(false);
  connect(m_planVelTimer,SIGNAL(timeout()),this,SLOT(onPlanVelTimerOut()));
}
MotionStatus::~MotionStatus()
{
  qDebug()<<"MotionStatus release -> "<<m_axisIndex;
  delete m_planVelTimer;
}

MotionStatus::ControlSource MotionStatus::controlSrc()
{
  int value=(int)m_cmd->read(CMD_SRC_SEL_NAME,m_axisIndex);
  if(value!=-1)
    m_controlSrc=(ControlSource)value;
  return m_controlSrc;
}
/**
 * @brief MotionStatus::setControlSrc 写控制源，再读一次回来判断
 * @param controlSrc
 */
void MotionStatus::setControlSrc(MotionStatus::ControlSource controlSrc)
{
  int controlId=controlSrc;
  qDebug()<<" ControlSource "<<controlSrc;
  controlId=(int)m_cmd->write(CMD_SRC_SEL_NAME,controlId,m_axisIndex);
  if(controlId!=-1)
    m_controlSrc = (ControlSource)controlSrc;
  qDebug()<<"write "<<CMD_SRC_SEL_NAME<<" axis "<<m_axisIndex;
  qDebug()<<"write retvalue:"<<controlId;
}

MotionStatus::MotionMode MotionStatus::motionMode() const
{
  return m_planMotion.currentMode;
}

/**
 * @brief MotionStatus::setMotionMode
 * @param motionMode  映射关系
 * -----MOTION_MODE_VEL_CLOSE<-->SERTASKMODE_VEL_CLOSE(1<-->6)---
 * -----MOTION_MODE_POS <-->SERTASKMODE_IDLE  (2<-->10)----目前这么处理
 * @return
 */
bool MotionStatus::setMotionMode(MotionStatus::MotionMode motionMode)
{
  qint16 mode;
  if(motionMode!=MotionStatus::MOTION_MODE_NONE)
  {
    switch (motionMode)
    {
    case MOTION_MODE_VEL_CLOSE:
      mode=SERTASKMODE_VEL_CLOSE;
      break;
    case MOTION_MODE_POS:
      mode=SERTASKMODE_POS_SYNC;//目前先进入IDLE,还没有实现功能
      break;
    default:
      mode=SERTASKMODE_IDLE;
      break;
    }
    if(0!=GTSD_CMD_SetServoTaskMode(m_axisIndex,mode,m_cmd->comType(),m_cmd->comRnStation()))
      GTSD_CMD_SetServoTaskMode(m_axisIndex,mode,m_cmd->comType(),m_cmd->comRnStation());
    SERVO_MODE retSerMode;
    GTSD_CMD_GetServoTaskMode(m_axisIndex,&retSerMode,m_cmd->comType(),m_cmd->comRnStation());

    ServoTaskMode ret=(ServoTaskMode)retSerMode.usr_mode;
    qDebug()<<"ret servo-task-mode:"<<ret;
    switch (ret)
    {
    case SERTASKMODE_VEL_CLOSE:
      m_planMotion.currentMode=MOTION_MODE_VEL_CLOSE;
      break;
    case SERTASKMODE_POS_SYNC:
      m_planMotion.currentMode=MOTION_MODE_POS;
      break;
    }
  }
  else
    m_planMotion.currentMode=MOTION_MODE_NONE;
  return true;
}

void MotionStatus::setPlanVelMode(PlanVelMode velMode)
{
  m_planMotion.velPlan.currentVelMode=velMode;
  switch (velMode) {
  case VELOCITY_NOCIRCLE:
    m_planVelTimer->setSingleShot(false);
    break;
  case VELOCITY_CIRCLE:
    m_planVelTimer->setSingleShot(false);
    break;
  }
}

MotionStatus::PlanVelMode MotionStatus::currentPlanVelMode()const
{
  return m_planMotion.velPlan.currentVelMode;
}

bool MotionStatus::setServoSpeed(double value)
{
  if(0!=GTSD_CMD_SetSpdRef(m_axisIndex,value,m_cmd->comType(),m_cmd->comRnStation()))
    GTSD_CMD_SetSpdRef(m_axisIndex,value,m_cmd->comType(),m_cmd->comRnStation());
  //还要读回来
  SPD_STATE spd;
  GTSD_CMD_GetSpdRef(m_axisIndex, &spd, m_cmd->comType(),m_cmd->comRnStation());
  if(qAbs(spd.chd_spd_tmp-value)>1)
    return false;
  return true;
}

double MotionStatus::AMPNoCircleValue() const
{
  return m_planMotion.velPlan.noCircle.AMP;
}
bool MotionStatus::setAMPNoCircleValue(double value)
{
  m_planMotion.velPlan.noCircle.AMP=value;
  return true;
}

double MotionStatus::timesNoCircleValue() const
{
  return m_planMotion.velPlan.noCircle.times;
}
bool MotionStatus::setTimesNoCircleValue(double value)
{
  m_planMotion.velPlan.noCircle.times=value;
  double timeMs=value*1000;
  int interval=MIN_INTERVAL;
  m_planVelTimer->setInterval(interval);
  m_planMotion.velPlan.noCircle.totalCount=timeMs/interval;
  m_planMotion.velPlan.noCircle.currentCount=0;
  qDebug()<<"value "<<value<<"timeMs "<<timeMs<<" totalCount"<<m_planMotion.velPlan.noCircle.totalCount;
  return true;
}

double MotionStatus::AMPCircleValue() const
{
  return m_planMotion.velPlan.circle.AMP;
}
bool MotionStatus::setAMPCircleValue(double value)
{
  m_planMotion.velPlan.circle.AMP=value;
  return true;
}

double MotionStatus::TcircleValue() const
{
  return m_planMotion.velPlan.circle.T;
}
bool MotionStatus::setTcircleValue(double value)
{
  m_planMotion.velPlan.circle.T=value;
  m_planVelTimer->setInterval((int)value/2);
  return true;
}

double MotionStatus::countCircleValue()const
{
  return m_planMotion.velPlan.circle.count;
}
bool MotionStatus::setCountCircleValue(double value)
{
  m_planMotion.velPlan.circle.count=value;
  m_planMotion.velPlan.circle.currentCount=0;
  return true;
}
void MotionStatus::startPlanVelTimer()
{
  m_planVelTimer->start();
  m_planMotion.velPlan.circle.currentCount=0;
}

void MotionStatus::stopPlanVelTimer()
{
  m_planVelTimer->stop();
}

//---------------------slots----------------------------
void MotionStatus::onPlanVelTimerOut()
{
//  qDebug()<<"current velmode:"<<m_planMotion.velPlan.currentVelMode;
  double prs;

  switch (m_planMotion.velPlan.currentVelMode)
  {
  case VELOCITY_NOCIRCLE:
    m_planMotion.velPlan.noCircle.currentCount++;
    if(m_planMotion.velPlan.noCircle.currentCount>m_planMotion.velPlan.noCircle.totalCount)
    {
      setRunning(false);
      emit planVeltimerStop(m_axisIndex);
      m_planMotion.velPlan.noCircle.currentCount=0;
      qDebug()<<"velocity nocircle timeout :axis_"<<m_axisIndex;
    }
    prs=100.00/m_planMotion.velPlan.noCircle.totalCount*m_planMotion.velPlan.noCircle.currentCount;
    emit updateProgressValue((int)prs);
//    qDebug()<<"velocity nocircle currentCount:"<<m_planMotion.velPlan.noCircle.currentCount;
    break;
  case VELOCITY_CIRCLE://周期循环,改变速度的正负
    m_planMotion.velPlan.circle.currentCount++;

    if(m_planMotion.velPlan.circle.currentCount>=m_planMotion.velPlan.circle.count*2)
    {
      setRunning(false);
//      setServoOn(false);
      qDebug()<<"velocity circle timeout :axis_"<<m_axisIndex<<" set running false";
      qDebug()<<"amp current count:"<<m_planMotion.velPlan.circle.currentCount;
      m_planMotion.velPlan.circle.currentCount=0;
      emit planVeltimerStop(m_axisIndex);
    }
    else
    {
      m_planMotion.velPlan.circle.change=!m_planMotion.velPlan.circle.change;
      if(m_planMotion.velPlan.circle.change)
        setServoSpeed(-m_planMotion.velPlan.circle.AMP);
      else
        setServoSpeed(m_planMotion.velPlan.circle.AMP);
      prs=100*m_planMotion.velPlan.circle.currentCount/(m_planMotion.velPlan.circle.count*2);
      emit updateProgressValue((int)prs);
    }
//    qDebug()<<"progress value:"<<prs;
//    qDebug()<<"amp circle value:"<<m_planMotion.velPlan.circle.AMP;
//    qDebug()<<"amp current count:"<<m_planMotion.velPlan.circle.currentCount;
    break;
  }
}
void MotionStatus::delayms(quint16 ms)
{
  QTime dieTime = QTime::currentTime().addMSecs(ms);
  while( QTime::currentTime() < dieTime )
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}
bool MotionStatus::checkServoIsOn()
{
  SERVO_STATE state;
  if(0!=GTSD_CMD_GetServoState(m_axisIndex,&state,m_cmd->comType(),m_cmd->comRnStation()))
    GTSD_CMD_GetServoState(m_axisIndex,&state,m_cmd->comType(),m_cmd->comRnStation());
  bool ret=(bool)(state.serv_ready);
  return ret;
}
void MotionStatus::casePlanVelocityMotionPrepar()
{
  if(setServoSpeed(0)==false)
    setServoSpeed(0);

  switch(m_planMotion.velPlan.currentVelMode)
  {
    case VELOCITY_NOCIRCLE:
      setTimesNoCircleValue(m_planMotion.velPlan.noCircle.times);
      m_planMotion.velPlan.noCircle.currentCount=0;
      break;
    case VELOCITY_CIRCLE:
      setTcircleValue(m_planMotion.velPlan.circle.T);
      m_planMotion.velPlan.circle.currentCount=0;
      m_planMotion.velPlan.circle.change=false;
      break;
  }
}

void MotionStatus::casePlanVelocityMotionWhenSetRunning(bool run)
{
  bool isOn=true;
  if(run)//开始运动
  {
    switch(m_planMotion.velPlan.currentVelMode)
    {
      case VELOCITY_NOCIRCLE:
      {
        isOn=checkServoIsOn();
        qDebug()<<"-----------------axis:"<<m_axisIndex<<"no circle ->is on:"<<isOn;

//        isOn=true;
        if(isOn)
        {
          setServoSpeed(m_planMotion.velPlan.noCircle.AMP);
          m_planVelTimer->start();
          m_running = true;
        }
        else
        {
          if(m_planVelTimer->isActive())
            m_planVelTimer->stop();
          m_running = false;
        }
      }
        break;
      case VELOCITY_CIRCLE:
      {
        isOn=checkServoIsOn();

        qDebug()<<"-----------------axis:"<<m_axisIndex<<"circle ->is on:"<<isOn;
//        isOn=true;
        if(isOn)
        {
          setServoSpeed(m_planMotion.velPlan.circle.AMP);
          m_planVelTimer->start();
          m_running = true;
        }
        else
        {
          if(m_planVelTimer->isActive())
            m_planVelTimer->stop();
          m_running = false;
        }
      }
        break;
    }
  }
  else//停止运动
  {
    setServoSpeed(0);
    switch(m_planMotion.velPlan.currentVelMode)
    {
      case VELOCITY_NOCIRCLE:
      {
        m_planVelTimer->stop();
        isOn=checkServoIsOn();
        if(isOn)
          m_running=true;
        else
          m_running=false;
      }
      break;

      case VELOCITY_CIRCLE:
      {
        m_planVelTimer->stop();
        isOn=checkServoIsOn();
        if(isOn)
          m_running=true;
        else
          m_running=false;
      }
      break;
    }
  }

}

void MotionStatus::setRunningPrepare()
{
  //先判断当前处理什么运动模式 V ? P
  switch (m_planMotion.currentMode)
  {
  case MOTION_MODE_VEL_CLOSE:
    casePlanVelocityMotionPrepar();
    break;
  case MOTION_MODE_POS:

    break;
  }
}
void MotionStatus::setServoOn(bool on)
{
  quint16 ret;
  if(on)
  {
    ret=GTSD_CMD_SetServoOn(m_axisIndex,m_cmd->comType(),m_cmd->comRnStation());
    if(ret!=0)
      ret=GTSD_CMD_SetServoOn(m_axisIndex,m_cmd->comType(),m_cmd->comRnStation());
  }
  else {
    ret=GTSD_CMD_SetServoOff(m_axisIndex,m_cmd->comType(),m_cmd->comRnStation());
    if(ret!=0)
      ret=GTSD_CMD_SetServoOff(m_axisIndex,m_cmd->comType(),m_cmd->comRnStation());
  }
}

bool MotionStatus::running() const
{
  return m_running;
}

void MotionStatus::setRunning(bool running)
{
  //先判断当前处理什么运动模式 V ? P
  switch (m_planMotion.currentMode)
  {
  case MOTION_MODE_VEL_CLOSE:
    casePlanVelocityMotionWhenSetRunning(running);
    break;
  case MOTION_MODE_POS:

    break;
  }

}
