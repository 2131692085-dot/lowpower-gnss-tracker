#ifndef __COM_DELAY_H__
#define __COM_DELAY_H__

#include "stm32f1xx_hal.h"

/**
 * 延迟函数 - 微秒
 */
void Com_Delay_us(uint16_t us);

/**
 * 延迟函数 - 毫秒
 */
void Com_Delay_ms(uint16_t ms);

/**
 * 延迟函数 - 秒
 */
void Com_Delay_s(uint16_t s);

#endif /* __COM_DELAY_H__ */
