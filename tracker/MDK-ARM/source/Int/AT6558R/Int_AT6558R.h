#ifndef __INT_AT6558R_H__
#define __INT_AT6558R_H__

#include "stm32f1xx_hal.h"

/**
 * GPS芯片 - 初始化
 */
void Int_AT6558R_Init(void);

/**
 * GPS芯片 - 获取定位信息
 */
void Int_AT6558R_GetGPS(uint8_t *gps);
/**
 * GPS芯片 - 进入低功耗
 */
void Int_AT6558R_Enter_LowPower(void);
/**
 * GPS芯片 - 进入低功耗
 */
void Int_AT6558R_Exit_LowPower(void);

#endif /* __INT_AT6558R_H__ */
