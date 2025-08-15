#include <stdint.h>
// Адрес ADS1115 (ADDR -> GND = 0x48)
#define ADS1115_ADDR (0x48 << 1)  // HAL требует 7-битный адрес << 1

// Регистры ADS1115
#define ADS1115_REG_CONV 0x00  // Регистр результата
#define ADS1115_REG_CONFIG 0x01  // Регистр конфигурации

// Настройка дифференциального режима A0 - A1, усиление ±2.048V, 128 SPS
#define ADS1115_CONFIG_DIFF_A0_A1 0x8580  // Основано на datasheet

// Функция для записи 16-битного значения в регистр
void ADS1115_WriteReg(uint8_t reg, uint16_t value);

// Функция для чтения 16-битного значения из регистра
uint16_t ADS1115_ReadReg(uint8_t reg);

// Инициализация ADS1115
void ADS1115_Init();

// Чтение дифференциального значения (A0 - A1)
int16_t ADS1115_ReadDiff_A0_A1();