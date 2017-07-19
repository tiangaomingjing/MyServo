#ifndef SERVOMODESTATUS_H
#define SERVOMODESTATUS_H

#include <QtCore>

class ServoModeStatus
{
public:
  explicit ServoModeStatus();
private:
  double m_posAdj;
  double m_ua;
  double m_ub;
  double m_uc;
  double m_ud;
  double m_uq;
  double m_id;
  double m_iq;
  double m_vcl;
  double m_vol;
  double m_vcl2;
  double m_posFollow;
  int m_currentMode;
  bool m_servoOn;
};

#endif // SERVOMODESTATUS_H
