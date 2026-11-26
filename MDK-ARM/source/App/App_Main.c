#include "App_Main.h"
#include <stdio.h>
#include "Int_LLCC68.h"
#include "Int_W5500.h"
/**
 * 主函数入口
 */
void App_main(void) {
    printf("App Main Start !!! \n");

    // Int_LLCC68_Init();

    // while (1) {
    //     uint8_t buffer[10] = {0};
    //     uint16_t buffer_length = 0;
    //     Int_LLCC68_Recevie(buffer, &buffer_length);
    //     if ( buffer_length > 0 ) {
    //         printf("buffer = %s \n", buffer);
    //     }
    //     HAL_Delay(3);
    // }
    Int_W5500_Init();
    while ( 1 ) {
        Int_W5500_SendData("sgg", 3);
        HAL_Delay(3000);
    }
}
