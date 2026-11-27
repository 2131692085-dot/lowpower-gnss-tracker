#include "Com_Delay.h"

/**
 * 延迟函数 - 微秒
 */
void Com_Delay_us(uint16_t us) {
    uint32_t delay = HAL_RCC_GetSysClockFreq() / 7200000 * us;
    while ( delay > 0 ) {
        __NOP(); // 1us
        delay--;
    }
}

/**
 * 延迟函数 - 毫秒
 */
void Com_Delay_ms(uint16_t ms) {
    HAL_Delay(ms);
}

/**
 * 延迟函数 - 秒
 */
void Com_Delay_s(uint16_t s) {
    HAL_Delay(s * 1000);
}
