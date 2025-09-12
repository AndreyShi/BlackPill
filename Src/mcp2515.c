#include "main.h"
#include <string.h> // Для memcpy (если будем использовать)
#include "mcp2515.h"
#include <stdio.h>
extern SPI_HandleTypeDef hspi1; // Объявляем внешнюю переменную SPI, определенную в main.c

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
uint8_t MCP2515_Read_Register(uint8_t reg_addr)
{
  uint8_t tx_data[2];
  uint8_t rx_data[2];
  uint8_t result;

  // Формируем пакет для передачи:
  // 1-й байт - команда READ (0x03)
  // 2-й байт - адрес регистра
  tx_data[0] = MCP2515_CMD_READ;
  tx_data[1] = reg_addr;

  // Выполняем полнодуплексную передачу.
  // HAL_SPI_TransmitReceive блокирующая, работает идеально для такого сценария.
  // Автоматический NSS (PA4) опустится перед началом передачи и поднимется после.
  HAL_SPI_TransmitReceive(&hspi1, tx_data, rx_data, 2, HAL_MAX_DELAY);

  // Анализируем принятые данные:
  // rx_data[0] - мусор (принимался, пока мы передавали команду)
  // rx_data[1] - значение регистра, которое отправил MCP2515, пока мы передавали адрес
  result = rx_data[1];

  return result;
}

/**
  * @brief  Последовательное чтение нескольких регистров MCP2515.
  * @param  start_reg_addr: Адрес первого регистра для чтения
  * @param  buffer: Указатель на буфер, куда будут записаны данные
  * @param  count: Количество байт для чтения
  * @retval None
  */
void MCP2515_Read_Registers(uint8_t start_reg_addr, uint8_t *buffer, uint8_t count)
{
  uint8_t tx_data[3 + count]; // Буфер на передачу: Команда + Адрес + count байт "заглушки"
  uint8_t rx_data[3 + count]; // Буфер на прием

  // Подготавливаем команду и адрес
  tx_data[0] = MCP2515_CMD_READ;
  tx_data[1] = start_reg_addr;

  // Остальные байты в передающем буфере могут быть любыми (0x00 или 0xFF)
  for (uint8_t i = 2; i < (2 + count); i++) {
    tx_data[i] = 0x00; // Заполняем заглушкой
  }

  // Выполняем передачу. Длина = 2 байта (команда+адрес) + количество запрашиваемых байт.
  HAL_SPI_TransmitReceive(&hspi1, tx_data, rx_data, 2 + count, HAL_MAX_DELAY);

  // Копируем полезные данные из приемного буфера в выходной буфер.
  // Первые 2 байта (rx_data[0] и rx_data[1]) - мусор.
  // Последующие 'count' байт - это значения регистров, начиная с start_reg_addr.
  memcpy(buffer, &rx_data[2], count);
  // Или можно через цикл:
  // for (uint8_t i = 0; i < count; i++) {
  //   buffer[i] = rx_data[2 + i];
  // }
}

// Пример использования в main.c или в другом месте
void example_usage(void)
{
  uint8_t canstat_value;
  uint8_t rx_buf[4];

  // Прочитать один регистр CANSTAT (адрес 0x0E)
  canstat_value = MCP2515_Read_Register(0x0E);

  // Теперь в canstat_value лежит состояние контроллера

  // Прочитать последовательно 4 регистра, начиная с 0x60 (например, буфер RX)
  MCP2515_Read_Registers(0x60, rx_buf, 4);
  // Теперь в массиве rx_buf[0]...rx_buf[3] лежат 4 байта данных из регистров MCP2515
  printf("canstat_value %d, %d %d %d %d\n",canstat_value,rx_buf[0],rx_buf[1],rx_buf[2],rx_buf[3]);
}