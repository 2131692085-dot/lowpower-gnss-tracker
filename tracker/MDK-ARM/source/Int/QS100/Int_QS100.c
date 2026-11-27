#include "Int_QS100.h"
#include "Link_Config.h"
#include "usart.h"
#include "gpio.h"
#include <string.h>

static char reply[384];
static char command[576];
static const uint8_t server[4] = {CLOUD_IPV4};

static int issue(const char *cmd, uint32_t timeout)
{
    size_t used = 0;
    uint32_t start = HAL_GetTick();
    reply[0] = 0;
    if (HAL_UART_Transmit(&huart3, (uint8_t *)cmd, strlen(cmd), 1000) != HAL_OK) return 0;
    while (HAL_GetTick() - start < timeout && used < sizeof(reply)-1) {
        uint8_t ch;
        if (HAL_UART_Receive(&huart3, &ch, 1, 50) != HAL_OK) continue;
        reply[used++] = (char)ch;
        reply[used] = 0;
        if (ch != '\n') continue;
        if (strstr(reply, "\nOK\r\n")) return 1;
        if (strstr(reply, "ERROR")) return 0;
    }
    return 0;
}

static int attached(void)
{
    for (unsigned i = 0; i < 15; ++i) {
        if (issue("AT+CGATT?\r\n", 1500)) {
            char *p = strstr(reply, "+CGATT:");
            unsigned state;
            if (p && sscanf(p, "+CGATT: %u", &state) == 1 && state == 1) return 1;
        }
        HAL_Delay(1000);
    }
    return 0;
}

uint8_t Int_QS100_SendData(uint8_t *data)
{
    size_t length = strlen((char *)data);
    unsigned socket_id;
    int sent = 0;
    if (!CLOUD_PORT || !length || length > 255) return 0;
    if (!issue("ATE0\r\n", 2000) || !attached()) return 0;
    if (!issue("AT+NSOCR=STREAM,6,0,0\r\n", 3000)) return 0;
    char *id = strstr(reply, "+NSOCR:");
    if (!id || sscanf(id, "+NSOCR: %u", &socket_id) != 1 || socket_id > 7) return 0;
    snprintf(command, sizeof(command), "AT+NSOCO=%u,%u.%u.%u.%u,%u\r\n",
             socket_id, server[0], server[1], server[2], server[3], CLOUD_PORT);
    if (issue(command, 20000)) {
        static const char hex[] = "0123456789ABCDEF";
        int n = snprintf(command, sizeof(command), "AT+NSOSD=%u,%u,", socket_id, (unsigned)length);
        for (size_t i = 0; i < length; ++i) {
            command[n++] = hex[data[i] >> 4];
            command[n++] = hex[data[i] & 15];
        }
        strcpy(command+n, ",0x200,1\r\n");
        if (issue(command, 4000)) {
            snprintf(command, sizeof(command), "AT+SEQUENCE=%u,1\r\n", socket_id);
            for (unsigned i = 0; i < 10; ++i) {
                HAL_Delay(300);
                if (!issue(command, 1500)) break;
                if (strstr(reply, "\n1\r\n")) { sent = 1; break; }
                if (strstr(reply, "\n0\r\n")) break;
            }
        }
    }
    snprintf(command, sizeof(command), "AT+NSOCL=%u\r\n", socket_id);
    issue(command, 2000);
    return sent;
}

void Int_QS100_Enter_LowPower(void)
{
    if (CLOUD_PORT) issue("AT+FASTOFF=0\r\n", 1500);
}

void Int_QS100_Exit_LowPower(void)
{
    #if CLOUD_PORT
    HAL_GPIO_WritePin(QS100_WU_GPIO_Port, QS100_WU_Pin, GPIO_PIN_SET);
    HAL_Delay(5);
    HAL_GPIO_WritePin(QS100_WU_GPIO_Port, QS100_WU_Pin, GPIO_PIN_RESET);
    HAL_Delay(5);
#endif
}
