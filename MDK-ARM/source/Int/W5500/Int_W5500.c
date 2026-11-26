#include "Int_W5500.h"
#include "spi.h"
#include "gpio.h"
#include "wizchip_conf.h"
#include <stdio.h>
#include "socket.h"

#define SOCKET_NUM 0
#define LOCAL_PORT 8888
#define SERVER_PORT 777

uint8_t GATEWAY[4] = {192, 168, 45, 1};
uint8_t IP[4] = {192, 168, 45, 120};
uint8_t SERVER_IP[4] = {192, 168, 45, 20};
uint8_t SUB[4] = {255, 255, 255, 0}; // 1111 1111 1111 0000
uint8_t MAC[6] = {110, 120, 130, 140, 150, 160}; 

/**
 * W5500联网芯片 - 复位
 */
void __Int_W5500_Reset(void) {
    HAL_GPIO_WritePin(W5500_RST_GPIO_Port, W5500_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(3);
    HAL_GPIO_WritePin(W5500_RST_GPIO_Port, W5500_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(3);
}

/**
 * W5500联网芯片 - 初始化
 */
void Int_W5500_Init(void) {
    
    // 1. 开启时钟

    // 2. 配置引脚

    // 3. 配置芯片
    // 3.1 复位
    __Int_W5500_Reset();
    // 3.2 修改默认的回调
    wizchip_registe_callback();
    // 3.3 配置寄存器
    // IP, MAC, SUB, GATEWAY
    setGAR(GATEWAY);
    setSUBR(SUB);
    setSHAR(MAC);
    setSIPR(IP);

    HAL_Delay(3000);

    printf("W5500 Init Success!!! \n");
}

/**
 * W5500联网芯片 - 发送数据
 */
void Int_W5500_SendData(uint8_t *data, uint16_t length) {

    while (1) {
        uint8_t status = getSn_SR(SOCKET_NUM);

        if ( status == SOCK_CLOSED ) {
            // 2. 创建网络客户端
            int8_t state = socket(SOCKET_NUM, Sn_MR_TCP, LOCAL_PORT, 0);
            if ( state == SOCKET_NUM ) {
                printf("Create Socket Success !!! \n");
            } else {
                printf("Create Socket Fail !!!\n");
            }
        }
        if (status == SOCK_INIT) {
            // 3. 连接网络服务器
            int8_t state = connect(SOCKET_NUM, SERVER_IP, SERVER_PORT);
            if ( state == SOCK_OK ) {
                printf("Connect Server Success !!! \n");
            } else {
                printf("Connect Server Fail !!! \n");
            }
        }
        if ( status == SOCK_ESTABLISHED ) {
            break;
        }
        HAL_Delay(1000);
    }

    // 4. 发送数据
    send(SOCKET_NUM, data, length);
    printf("Send Data Success !!! \n");
    // 5. 关闭客户端
    close(SOCKET_NUM);
    printf("Close Socket Success !!! \n");
}
