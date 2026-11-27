#ifndef __INT_DS3553_H__
#define __INT_DS3553_H__

#include "stm32f1xx_hal.h"

/**
 * 计步芯片 - 初始化
 */
void Int_DS3553_Init(void);

/**
 * 计步芯片 - 获取计步值
 */
int Int_DS3553_GetStepCount(uint32_t *steps);

#endif /* __INT_DS3553_H__ */
