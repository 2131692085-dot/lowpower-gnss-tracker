#ifndef __APP_WORK_H__
#define __APP_WORK_H__

#include "stm32f1xx_hal.h"
#include "Int_DS3553.h"
#include "Int_AT6558R.h"
#include "Int_QS100.h"
#include <stdio.h>
#include "Com_Debug.h"
#include "Com_Delay.h"

typedef struct
{
    uint8_t uid[25];
    uint8_t date[20];
    double lon;
    double lat;
    uint32_t step;
} Location_Value;

/**
 * 初始化
 */
void App_Work_Init(void);
/**
 * 正常工作
 */
void App_Work_Execute(void);

#endif /* __APP_WORK_H__ */
