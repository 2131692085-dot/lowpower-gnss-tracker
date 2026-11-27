#ifndef __INT_QS100_H__
#define __INT_QS100_H__

#include "stm32f1xx_hal.h"
#include <stdio.h>
#include "Com_Debug.h"
#include "Com_Delay.h"

/**
 * QS100联网芯片 - 初始化
 */


/**
 * QS100联网芯片 - 发送数据
 */
uint8_t Int_QS100_SendData(uint8_t *data);

/**
 * QS100联网芯片 - 进入低功耗
 */
void Int_QS100_Enter_LowPower(void);

/**
 * QS100联网芯片 - 退出低功耗
 */
void Int_QS100_Exit_LowPower(void);

#endif /* __INT_QS100_H__ */
