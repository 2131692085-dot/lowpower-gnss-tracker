#include "Int_W5500.h"
#include "spi.h"
#include "gpio.h"
#include "wizchip_conf.h"
#include <stdio.h>
#include "socket.h"

#include "Link_Config.h"

#define SOCKET_NUM 0
#if GATEWAY_USE_CLOUD
#define SERVER_PORT CLOUD_PORT
static uint8_t SERVER_IP[4] = {CLOUD_IPV4};
#else
#define SERVER_PORT LAN_SERVER_PORT
static uint8_t SERVER_IP[4] = {LAN_SERVER_IPV4};
#endif
static uint8_t GATEWAY[4] = {ROUTER_IPV4};
static uint8_t IP[4] = {GATEWAY_IPV4};
static uint8_t SUB[4] = {SUBNET_MASK};
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
uint8_t Int_W5500_SendData(uint8_t *data, uint16_t length)
{
    int delivered = 0;
    if (socket(SOCKET_NUM, Sn_MR_TCP, 0, SF_IO_NONBLOCK) != SOCKET_NUM) return 0;
    int8_t result = connect(SOCKET_NUM, SERVER_IP, SERVER_PORT);
    uint32_t start = HAL_GetTick();
    if (result == SOCK_BUSY || result == SOCK_OK) {
        while (getSn_SR(SOCKET_NUM) != SOCK_ESTABLISHED) {
            if (getSn_SR(SOCKET_NUM) == SOCK_CLOSED || HAL_GetTick() - start >= 5000) goto done;
            HAL_Delay(2);
        }
        if (send(SOCKET_NUM, data, length) == length) {
            start = HAL_GetTick();
            while (HAL_GetTick() - start < 5000) {
                uint8_t flags = getSn_IR(SOCKET_NUM);
                if (flags & Sn_IR_SENDOK) { delivered = 1; break; }
                if ((flags & Sn_IR_TIMEOUT) || getSn_SR(SOCKET_NUM) == SOCK_CLOSED) break;
                HAL_Delay(2);
            }
        }
    }
done:
    if (delivered) disconnect(SOCKET_NUM);
    close(SOCKET_NUM);
    return delivered;
}
