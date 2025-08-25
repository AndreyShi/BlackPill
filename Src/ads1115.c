#include "ads1115.h"
#include "main.h"
extern I2C_HandleTypeDef hi2c1;
// Функция для записи 16-битного значения в регистр
void ADS1115_WriteReg(uint8_t reg, uint16_t value) {
    uint8_t data[3];
    data[0] = reg;
    data[1] = (value >> 8) & 0xFF;  // Старший байт
    data[2] = value & 0xFF;         // Младший байт
    HAL_I2C_Master_Transmit(&hi2c1, ADS1115_ADDR, data, 3, HAL_MAX_DELAY);
}

// Функция для чтения 16-битного значения из регистра
uint16_t ADS1115_ReadReg(uint8_t reg) {
    uint8_t data[2];
    HAL_I2C_Master_Transmit(&hi2c1, ADS1115_ADDR, &reg, 1, HAL_MAX_DELAY);
    HAL_I2C_Master_Receive(&hi2c1, ADS1115_ADDR, data, 2, HAL_MAX_DELAY);
    return (data[0] << 8) | data[1];
}

// Инициализация ADS1115
void ADS1115_Init() {
    ADS1115_WriteReg(ADS1115_REG_CONFIG, ADS1115_CONFIG_DIFF_A0_A1);
}

// Чтение дифференциального значения (A0 - A1)
int16_t ADS1115_ReadDiff_A0_A1() {
    // Запускаем преобразование (если в режиме однократного преобразования)
    ADS1115_WriteReg(ADS1115_REG_CONFIG, ADS1115_CONFIG_DIFF_A0_A1 | 0x8000);
    HAL_Delay(10);  // Ждём завершения преобразования (~8ms при 128 SPS)
    return (int16_t)ADS1115_ReadReg(ADS1115_REG_CONV);
}