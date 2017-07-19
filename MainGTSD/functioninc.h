#ifndef FUNCTIONINC
#define FUNCTIONINC
#include "Factory/factory.h"

#include "advflash.h"
#include "advram.h"
#include "cfgcurrentloop.h"
#include "cfgdriverpower.h"
#include "cfgencoder.h"
#include "cfgio.h"
#include "cfglimit.h"
#include "cfgmotor.h"
#include "cfgmoveprofile.h"
#include "cfgspeedloop.h"
#include "devicestatus.h"
#include "cfgpositionloop.h"

namespace RegisterFunction
{
  void registerAll(void)
  {
    Factory::registerClass<AdvFLASH>();
    Factory::registerClass<AdvRAM>();
    Factory::registerClass<CfgCurrentLoop>();
    Factory::registerClass<CfgDriverPower>();
    Factory::registerClass<CfgEncoder>();
    Factory::registerClass<CfgIO>();
    Factory::registerClass<CfgLimit>();
    Factory::registerClass<CfgMotor>();
    Factory::registerClass<CfgMoveProfile>();
    Factory::registerClass<CfgPositionLoop>();
    Factory::registerClass<CfgSpeedLoop>();
    Factory::registerClass<DeviceStatus>();
  }

}

#endif // FUNCTIONINC

