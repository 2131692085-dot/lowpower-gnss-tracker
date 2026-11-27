#include "Int_AT6558R.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>
#include "gpio.h"
/**
 * 使用USART发送数据
 */
void __Int_AT6558R_TransmitData( uint8_t *data ) {
    uint8_t xor = data[0];
    for (uint16_t i = 1; i < strlen((char *)data); i++)
    {
        xor ^= data[i];
    }
    
    uint8_t cmd[100] = {0};
    sprintf( (char *)cmd, "$%s*%02X\r\n", data, xor );
    HAL_UART_Transmit(&huart2, cmd, strlen((char *)cmd), 1000);
}

/**
 * GPS芯片 - 初始化
 */
void Int_AT6558R_Init(void) {

    //1. 开启时钟
    //MX_USART2_UART_Init();

    //2. 配置引脚

    //3. 配置芯片(初始化)
    HAL_Delay(100); /* USART2 and module use the existing 9600 baud setting. */
    __Int_AT6558R_TransmitData("PCAS02,1000");
    __Int_AT6558R_TransmitData("PCAS04,1");
}

/**
 * GPS芯片 - 获取定位信息
 */
void Int_AT6558R_GetGPS(uint8_t *gps) {
    // 定长数据接收
    uint16_t data_length = 0;
    // HAL_UART_Receive(&huart2, gps, data_length, 1000);

    // 变长数据接收
    HAL_UARTEx_ReceiveToIdle(&huart2, gps, 512, &data_length, 1000);
}

/**
 * GPS芯片 - 进入低功耗
 */
void Int_AT6558R_Enter_LowPower(void) {
    HAL_GPIO_WritePin(GPS_EN_GPIO_Port, GPS_EN_Pin, GPIO_PIN_RESET);
}
/**
 * GPS芯片 - 退出低功耗
 */
void Int_AT6558R_Exit_LowPower(void) {
    HAL_GPIO_WritePin(GPS_EN_GPIO_Port, GPS_EN_Pin, GPIO_PIN_SET);
}
