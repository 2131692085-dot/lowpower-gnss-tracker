#include "App_LowPower.h"
#include "Link_Config.h"
#include "rtc.h"

void App_LowPower_Enter(void)
{
    RTC_TimeTypeDef now;
    RTC_AlarmTypeDef alarm = {0};
    uint32_t seconds;
    Int_AT6558R_Enter_LowPower();
    Int_QS100_Enter_LowPower();
    if (HAL_RTC_GetTime(&hrtc, &now, RTC_FORMAT_BIN) == HAL_OK) {
        seconds = (now.Hours * 3600U + now.Minutes * 60U + now.Seconds +
                   REPORT_INTERVAL_SECONDS) % 86400U;
        alarm.Alarm = RTC_ALARM_A;
        alarm.AlarmTime.Hours = seconds / 3600U;
        alarm.AlarmTime.Minutes = seconds / 60U % 60U;
        alarm.AlarmTime.Seconds = seconds % 60U;
        __HAL_RTC_ALARM_CLEAR_FLAG(&hrtc, RTC_FLAG_ALRAF);
        __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
        if (HAL_RTC_SetAlarm_IT(&hrtc, &alarm, RTC_FORMAT_BIN) == HAL_OK) {
            HAL_PWR_EnterSTANDBYMode();
        }
    }
    HAL_Delay(REPORT_INTERVAL_SECONDS * 1000U);
}

void App_LowPower_Exit(void)
{
    Int_AT6558R_Exit_LowPower();
    Int_QS100_Exit_LowPower();
}
