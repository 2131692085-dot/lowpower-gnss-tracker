#include "App_Main.h"
#include "Int_LLCC68.h"
#include "Int_W5500.h"
#include <stdio.h>

void App_main(void)
{
    static uint8_t packet[255];
    Int_W5500_Init();
    while (Int_LLCC68_Init() != 0) {
        puts("LoRa: init failed, retrying");
        HAL_Delay(1000);
    }
    for (;;) {
        uint16_t length = 0;
        if (!Int_LLCC68_Recevie(packet, &length) && length) {
            printf("LoRa RX: %u bytes\n", length);
            puts(Int_W5500_SendData(packet, length) ? "TCP: forwarded" : "TCP: failed");
        }
        HAL_Delay(3);
    }
}
