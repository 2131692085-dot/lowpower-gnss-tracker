#include "Int_DS3553.h"
#include "i2c.h"
#include "gpio.h"
#include "Com_Debug.h"

#define DS3553_I2C_ADDRESS_R 0x4F
#define DS3553_I2C_ADDRESS_W 0x4E

#define REG_ADD_ID 0x01
#define REG_ADD_USER_SET 0xC3
#define REG_ADD_STEP_CNT_L 0xC4
#define REG_ADD_STEP_CNT_M 0xC5
#define REG_ADD_STEP_CNT_H 0xC6

/**
 * 计步芯片 - I2C - 写数据
 */
void __Int_DS3553_I2C_WriteData( uint16_t mem_address, uint8_t data ) {
    // 片选线拉低
    HAL_GPIO_WritePin(DS3553_CS_GPIO_Port, DS3553_CS_Pin, GPIO_PIN_RESET);
    // 延迟时间
    HAL_Delay(5);
    HAL_I2C_Mem_Write(&hi2c1, DS3553_I2C_ADDRESS_W, mem_address, I2C_MEMADD_SIZE_8BIT, &data, 1, 1000);
    // 片选线拉高
    HAL_GPIO_WritePin(DS3553_CS_GPIO_Port, DS3553_CS_Pin, GPIO_PIN_SET);
    // 延迟时间    
    HAL_Delay(12);
}

/**
 * 计步芯片 - I2C - 读数据
 */
static int read_register(uint16_t mem_address, uint8_t *data) {
    // 片选线拉低
    HAL_GPIO_WritePin(DS3553_CS_GPIO_Port, DS3553_CS_Pin, GPIO_PIN_RESET);
    // 延迟时间
    HAL_Delay(5);
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&hi2c1, DS3553_I2C_ADDRESS_R, mem_address, I2C_MEMADD_SIZE_8BIT, data, 1, 1000);
    // 片选线拉高
    HAL_GPIO_WritePin(DS3553_CS_GPIO_Port, DS3553_CS_Pin, GPIO_PIN_SET);
    // 延迟时间    
    HAL_Delay(12);
    return status == HAL_OK;
}

/**
 * 计步芯片 - 初始化
 */
void Int_DS3553_Init(void) {

    // 1. 开启时钟（I2C1）
    //MX_I2C1_Init();

    // 2. 配置引脚(CS)
    //MX_GPIO_Init();

    // 3. 配置芯片（初始值）
    // 修改寄存器的初始值
    // I2C => EEPRom => Address, Data
    // uint8_t id = __Int_DS3553_I2C_ReadData(REG_ADD_ID);
    // Com_Debug_println("id = %d ", id);
    uint8_t init_data = 0x18;

    init_data &= ~( 1 << 4 );
    init_data |=  ( 1 << 1 );

    __Int_DS3553_I2C_WriteData(REG_ADD_USER_SET, init_data);
}

/**
 * 计步芯片 - 获取计步值
 */
int Int_DS3553_GetStepCount(uint32_t *steps)
{
    uint8_t h, m, l, h2, m2;
    for (unsigned i = 0; i < 2; ++i) {
        if (!read_register(REG_ADD_STEP_CNT_H, &h) || !read_register(REG_ADD_STEP_CNT_M, &m) ||
            !read_register(REG_ADD_STEP_CNT_L, &l) || !read_register(REG_ADD_STEP_CNT_M, &m2) ||
            !read_register(REG_ADD_STEP_CNT_H, &h2)) return 0;
        if (h == h2 && m == m2) {
            *steps = ((uint32_t)h << 16) | ((uint32_t)m << 8) | l;
            return 1;
        }
    }
    return 0;
}
