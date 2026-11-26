#ifndef __INT_W5500_H__
#define __INT_W5500_H__

#include "stm32f1xx_hal.h"

/**
 * W5500联网芯片 - 初始化
 */
void Int_W5500_Init(void);

/**
 * W5500联网芯片 - 发送数据
 */
void Int_W5500_SendData(uint8_t *data, uint16_t length);

#endif /* __INT_W5500_H__ */
