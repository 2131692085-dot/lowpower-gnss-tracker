#ifndef __APP_LOWPOWER_H__
#define __APP_LOWPOWER_H__

#include "stm32f1xx_hal.h"
#include "Int_AT6558R.h"
#include "Int_QS100.h"

/**
 * 进入低功耗
 */
void App_LowPower_Enter(void);

/**
 * 退出低功耗
 */
void App_LowPower_Exit(void);

#endif /* __APP_LOWPOWER_H__ */
