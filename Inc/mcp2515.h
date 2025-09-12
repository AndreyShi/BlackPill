#include <stdint.h>

// Определяем команды для MCP2515 согласно datasheet
#define MCP2515_CMD_READ    0x03
// Можно добавить другие команды для полноты:
// #define MCP2515_CMD_WRITE   0x02
// #define MCP2515_CMD_RTS     0x80
// #define MCP2515_CMD_READ_RX 0x90

/**
  * @brief  Чтение одного регистра MCP2515.
  * @param  reg_addr: Адрес регистра для чтения (например, 0x00 - REG_CANSTAT)
  * @retval Прочитанное значение регистра (байт)
  */
uint8_t MCP2515_Read_Register(uint8_t reg_addr);

/**
  * @brief  Запись одного регистра MCP2515.
  * @param  reg_addr: Адрес регистра для записи 
  * @param  reg_data: данные для записи 
  */
void MCP2515_Write_Register(uint8_t reg_addr,uint8_t reg_data);

/**
  * @brief  Последовательное чтение нескольких регистров MCP2515.
  * @param  start_reg_addr: Адрес первого регистра для чтения
  * @param  buffer: Указатель на буфер, куда будут записаны данные
  * @param  count: Количество байт для чтения
  * @retval None
  */
void MCP2515_Read_Registers(uint8_t start_reg_addr, uint8_t *buffer, uint8_t count);

// Пример использования в main.c или в другом месте
void example_usage(void);