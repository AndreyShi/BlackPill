#include "main.h"
#include <string.h> // Для memcpy (если будем использовать)
#include "mcp2515.h"
#include <stdio.h>
extern SPI_HandleTypeDef hspi1; // Объявляем внешнюю переменную SPI, определенную в main.c

// Определяем команды для MCP2515 согласно datasheet
#define MCP2515_CMD_READ    0x03
// Можно добавить другие команды для полноты:
 #define MCP2515_CMD_WRITE   0x02
// #define MCP2515_CMD_RTS     0x80
// #define MCP2515_CMD_READ_RX 0x90

/**
  * @brief  Чтение одного регистра MCP2515.
  * @param  reg_addr: Адрес регистра для чтения (например, 0x00 - REG_CANSTAT)
  * @retval Прочитанное значение регистра (байт)
  */
uint8_t MCP2515_Read_Register(uint8_t reg_addr)
{
  uint8_t tx_data[3] = {0};
  uint8_t rx_data[3] = {0};
  uint8_t result;

  // Формируем пакет для передачи:
  // 1-й байт - команда READ (0x03)
  // 2-й байт - адрес регистра
  tx_data[0] = MCP2515_CMD_READ;
  tx_data[1] = reg_addr;

  // Выполняем полнодуплексную передачу.
  // HAL_SPI_TransmitReceive блокирующая, работает идеально для такого сценария.
  HAL_GPIO_WritePin(CS__GPIO_Port, CS__Pin, GPIO_PIN_RESET);
  HAL_SPI_TransmitReceive(&hspi1, tx_data, rx_data, 3, HAL_MAX_DELAY);
  HAL_GPIO_WritePin(CS__GPIO_Port, CS__Pin, GPIO_PIN_SET);

  // Анализируем принятые данные:
  // rx_data[0] - мусор (принимался, пока мы передавали команду)
  // rx_data[1] - значение регистра, которое отправил MCP2515, пока мы передавали адрес
  result = rx_data[2];

  return result;
}

/**
  * @brief  Запись одного регистра MCP2515.
  * @param  reg_addr: Адрес регистра для записи 
  * @param  reg_data: данные для записи 
  */
void MCP2515_Write_Register(uint8_t reg_addr,uint8_t reg_data){
  uint8_t pData[3] = {MCP2515_CMD_WRITE,reg_addr, reg_data};

  HAL_GPIO_WritePin(CS__GPIO_Port, CS__Pin, GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi1, pData, 3, HAL_MAX_DELAY);
  HAL_GPIO_WritePin(CS__GPIO_Port, CS__Pin, GPIO_PIN_SET);
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
  HAL_GPIO_WritePin(CS__GPIO_Port, CS__Pin, GPIO_PIN_RESET);
  HAL_SPI_TransmitReceive(&hspi1, tx_data, rx_data, 2 + count, HAL_MAX_DELAY);
  HAL_GPIO_WritePin(CS__GPIO_Port, CS__Pin, GPIO_PIN_SET);
  // Копируем полезные данные из приемного буфера в выходной буфер.
  // Первые 2 байта (rx_data[0] и rx_data[1]) - мусор.
  // Последующие 'count' байт - это значения регистров, начиная с start_reg_addr.
  memcpy(buffer, &rx_data[2], count);
  // Или можно через цикл:
  // for (uint8_t i = 0; i < count; i++) {
  //   buffer[i] = rx_data[2 + i];
  // }
}

/*
  *         Без использования прерываний (режим опроса).
  */
void MCP2515_Init_ISO15765(void) {
  // 1. Переход в режим конфигурации
  MCP2515_Write_Register(MCP2515_REG_CANCTRL, 0x80); // Режим конфигурации
  HAL_Delay(10);

  // 2. Настройка битрейта 500 kbps для кварца 8 МГц
  MCP2515_Write_Register(MCP2515_REG_CNF1, 0x00); // SJW=1, BRP=0
  MCP2515_Write_Register(MCP2515_REG_CNF2, 0xD0); // BTLMODE=1, SAM=0, PS1=6 Tq
  MCP2515_Write_Register(MCP2515_REG_CNF3, 0x02); // PS2=3 Tq

  // 3. !!! НАСТРОЙКА ПРЕРЫВАНИЙ ПРОПУСКАЕТСЯ !!!
  // MCP2515_Write_Register(MCP2515_REG_CANINTE, 0x01); // ЭТУ СТРОКУ УБИРАЕМ

  // 4. Настройка буфера приема RXB0
  MCP2515_Write_Register(MCP2515_REG_RXB0CTRL, 0x00); // Принимать все сообщения

  // 5. Возврат в нормальный режим
  MCP2515_Write_Register(MCP2515_REG_CANCTRL, 0x00); // Нормальный режим
  HAL_Delay(10);
}

/*
  * Инициализация MCP2515 для ISO 27145-4 (WWH-OBD)
  * 29-битные идентификаторы, фильтрация на ответы ECU
  */
void MCP2515_Init_ISO27145(void) {
  // 1. Режим конфигурации
  MCP2515_Write_Register(MCP2515_REG_CANCTRL, 0x80);
  HAL_Delay(10);

  // 2. Настройка битрейта
  MCP2515_Write_Register(MCP2515_REG_CNF1, 0x00);
  MCP2515_Write_Register(MCP2515_REG_CNF2, 0xD0);
  MCP2515_Write_Register(MCP2515_REG_CNF3, 0x02);

  // 3. НАСТРОЙКА ФИЛЬТРА RXF0 на прием 0x18DAF100
  MCP2515_Write_Register(MCP2515_REG_RXF0SIDH, (uint8_t)(0x18DAF100 >> 21));
  MCP2515_Write_Register(MCP2515_REG_RXF0SIDL, (uint8_t)(0x18DAF100 >> 13));
  MCP2515_Write_Register(MCP2515_REG_RXF0EID8, (uint8_t)(0x18DAF100 >> 5));
  MCP2515_Write_Register(MCP2515_REG_RXF0EID0, (uint8_t)(0x18DAF100 << 3));

  // 4. НАСТРОЙКА МАСКИ RXM0 (проверять все биты)
  MCP2515_Write_Register(MCP2515_REG_RXM0SIDH, 0xFF); // Все биты SIDH
  MCP2515_Write_Register(MCP2515_REG_RXM0SIDL, 0xE3); // Все биты SIDL + EID
  MCP2515_Write_Register(MCP2515_REG_RXM0EID8, 0xFF); // Все биты EID8
  MCP2515_Write_Register(MCP2515_REG_RXM0EID0, 0xFF); // Все биты EID0

  // 5. Настройка буфера RXB0 на использование фильтра 0
  MCP2515_Write_Register(MCP2515_REG_RXB0CTRL, 0x20); // Использовать фильтры

  // 6. Включение Extended ID режима
  uint8_t rxctrl = MCP2515_Read_Register(MCP2515_REG_RXB0CTRL);
  MCP2515_Write_Register(MCP2515_REG_RXB0CTRL, rxctrl | 0x08); // EXIDEN=1

  // 7. Возврат в нормальный режим
  MCP2515_Write_Register(MCP2515_REG_CANCTRL, 0x00);
  HAL_Delay(10);
}

/**
  * @brief  Отправка через TXB0 (самый простой вариант)
  */
void MCP2515_Send_ISO27145_TXB0_Extended(uint32_t can_id, uint8_t *data, uint8_t length) {
  // Запись ID и данных
  MCP2515_Write_Register(MCP2515_REG_TXB0SIDH, (uint8_t)(can_id >> 21));
  MCP2515_Write_Register(MCP2515_REG_TXB0SIDL, (uint8_t)(can_id >> 13));
  MCP2515_Write_Register(MCP2515_REG_TXB0EID8, (uint8_t)(can_id >> 5));
  MCP2515_Write_Register(MCP2515_REG_TXB0EID0, (uint8_t)(can_id << 3));
  
  // Установка EXIDE бита
  uint8_t sidl = MCP2515_Read_Register(MCP2515_REG_TXB0SIDL);
  MCP2515_Write_Register(MCP2515_REG_TXB0SIDL, sidl | 0x08);
  
  // Данные
  MCP2515_Write_Register(MCP2515_REG_TXB0DLC, length);
  for (int i = 0; i < length; i++) {
    MCP2515_Write_Register(MCP2515_REG_TXB0D0 + i, data[i]);
  }
  
  // RTS для TXB0
  uint8_t rts_cmd = MCP2515_CMD_RTS_TX0;
  HAL_GPIO_WritePin(CS__GPIO_Port, CS__Pin, GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi1, &rts_cmd, 1, HAL_MAX_DELAY);
  HAL_GPIO_WritePin(CS__GPIO_Port, CS__Pin, GPIO_PIN_SET);
}

// Отправка ISO 27145 запроса
void Send_WWH_OBD_Request(void) {
  uint8_t uds_data[] = {0x02, 0x10, 0x85, 0x0C, 0x00}; // UDS запрос RPM
  MCP2515_Send_ISO27145_TXB0_Extended(0x18DB33F1, uds_data, 5);
}

/**
  * @brief  Инициализация MCP2515 с фильтром на OBD ответы
  */
void MCP2515_Init_With_Filter(void) {
  // 1. Переход в режим конфигурации (для настройки фильтров)
  MCP2515_Write_Register(MCP2515_REG_CANCTRL, 0x80);
  HAL_Delay(10);

  // 2. Настройка битрейта (как ранее)...
  MCP2515_Write_Register(MCP2515_REG_CNF1, 0x00);
  MCP2515_Write_Register(MCP2515_REG_CNF2, 0xD0);
  MCP2515_Write_Register(MCP2515_REG_CNF3, 0x02);

  // 3. НАСТРОЙКА ФИЛЬТРА RXF0 на прием ID = 0x7E8
  // Для 11-битного ID: биты 10-3 в SIDH, биты 2-0 в SIDL
  MCP2515_Write_Register(0x00, (uint8_t)(0x7E8 >> 3)); // RXF0SIDH: старшие биты ID
  MCP2515_Write_Register(0x01, (uint8_t)(0x7E8 << 5)); // RXF0SIDL: младшие биты ID

  // 4. Настройка маски для RXF0 (проверять все биты ID)
  MCP2515_Write_Register(0x20, 0xFF); // RXM0SIDH: маска = 1 (проверять)
  MCP2515_Write_Register(0x21, 0xE0); // RXM0SIDL: маска = 1 для бит 2-0

  // 5. Настройка буфера RXB0 на использование фильтра 0
  MCP2515_Write_Register(MCP2515_REG_RXB0CTRL, 0x20); // Использовать фильтры, принимать только сообщения, прошедшие фильтр

  // 6. Возврат в нормальный режим
  MCP2515_Write_Register(MCP2515_REG_CANCTRL, 0x00);
  HAL_Delay(10);
}

/**
  * @brief  Проверка и чтение принятого сообщения (режим опроса)
  * @param  data: указатель на буфер для данных (минимум 8 байт)
  * @retval 0 - сообщения нет, >0 - количество принятых байт
  */
uint8_t MCP2515_Read_Message_Polling(uint8_t *data) {
  // ОПРОС флага принятия сообщения в регистре CANINTF (бит 0 - RX0IF)
  if (MCP2515_Read_Register(MCP2515_REG_CANINTF) & 0x01) {
    
    // Читаем DLC чтобы узнать длину данных
    uint8_t dlc = MCP2515_Read_Register(MCP2515_REG_RXB0DLC) & 0x0F;
    
    // Читаем данные из буфера RXB0
    for (int i = 0; i < dlc; i++) {
      data[i] = MCP2515_Read_Register(MCP2515_REG_RXB0D0 + i);
    }
    
    // !!! ВАЖНО: Сбрасываем флаг вручную, записывая 0 в бит RX0IF !!!
    MCP2515_Write_Register(MCP2515_REG_CANINTF, 0x00);
    
    return dlc; // Возвращаем количество принятых байт
  }
  return 0; // Сообщения нет
}

/**
  * @brief  Отправка OBD2 запроса на получение RPM
  */
void MCP2515_Send_OBD_Request(uint16_t can_id, uint8_t pid) {
  // Буфер для отправки (8 байт)
  uint8_t tx_buffer[8];

  // Формируем OBD2 запрос
  tx_buffer[0] = 0x02;       // Кол-во байт данных
  tx_buffer[1] = 0x01;       // Режим: Show current data
  tx_buffer[2] = pid;        // PID запроса (0x0C - RPM)
  tx_buffer[3] = 0x00;       // Заполнители
  tx_buffer[4] = 0x00;
  tx_buffer[5] = 0x00;
  tx_buffer[6] = 0x00;
  tx_buffer[7] = 0x00;

  // 1. Записываем ID сообщения (11-bit)
  MCP2515_Write_Register(MCP2515_REG_TXB0SIDH, (uint8_t)(can_id >> 3)); // Старшие 8 бит ID
  MCP2515_Write_Register(MCP2515_REG_TXB0SIDL, (uint8_t)(can_id << 5)); // Младшие 3 бита ID

  // 2. Записываем DLC (длину данных) = 8
  MCP2515_Write_Register(MCP2515_REG_TXB0DLC, 0x08);

  // 3. Записываем данные
  for (int i = 0; i < 8; i++) {
    MCP2515_Write_Register(MCP2515_REG_TXB0D0 + i, tx_buffer[i]);
  }

  // 4. Запрашиваем отправку (RTS) для буфера TXB0
  uint8_t rts_cmd = MCP2515_CMD_RTS_TX0;
  HAL_GPIO_WritePin(CS__GPIO_Port, CS__Pin, GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi1, &rts_cmd, 1, HAL_MAX_DELAY);
  HAL_GPIO_WritePin(CS__GPIO_Port, CS__Pin, GPIO_PIN_SET);
}

/**
  * @brief  Парсинг RPM из данных OBD2 ответа
  */
float Parse_Engine_RPM(uint8_t *data, uint8_t length) {
  // Проверяем что это ответ на PID 0x0C
  // Формат ответа: [04] [41] [0C] [A] [B] [00] [00] [00]
  if (length >= 4 && data[1] == 0x41 && data[2] == PID_ENGINE_RPM) {
    uint16_t rpm_value = (data[3] << 8) | data[4]; // Объединяем два байта
    return rpm_value / 4.0; // Применяем формулу
  }
  return 0.0;
}

/**
  * @brief  Парсинг температуры охлаждающей жидкости
  */
float Parse_Coolant_Temperature(uint8_t *data, uint8_t length) {
  // Формат ответа: [03] [41] [05] [T] [00] [00] [00] [00]
  if (length >= 4 && data[1] == 0x41 && data[2] == PID_COOLANT_TEMP) {
    uint8_t temp_value = data[3];
    return temp_value - 40.0; // Формула: значение - 40 = температура в °C
  }
  return -40.0; // Значение по умолчанию/ошибка
}

/**
  * @brief  Парсинг статуса DTC (Check Engine)
  */
DTC_Status Parse_DTC_Status(uint8_t *data, uint8_t length) {
  DTC_Status status = {0};
  
  // Формат ответа: [04] [41] [01] [A] [B] [C] [D] [00]
  if (length >= 8 && data[1] == 0x41 && data[2] == PID_DTC_STATUS) {
    // Бит 7 байта A: статус MIL (0 - выкл, 1 - вкл)
    status.mil_status = (data[3] >> 7) & 0x01;
    
    // Биты 6-0 байта A: количество DTC
    status.dtc_count = data[3] & 0x7F;
    
    // Байты B, C, D содержат информацию о тестах
    status.supported_tests = data[4];
    status.test_completion = data[5];
  }
  
  return status;
}

/**
  * @brief  Обработка отрицательного ответа
  */
int Handle_Negative_Response(uint8_t *data, uint8_t length) {

    if (data[1] != 0x71)
        { return 0;}

    if (length >= 4) {
        uint8_t requested_service = data[2];
        uint8_t error_code = data[3];
        
        printf("NRC: Service 0x%02X, Error: 0x%02X - ", requested_service, error_code);
        
        switch (error_code) {
            case 0x11:
                printf("Service not supported\n");
                break;
            case 0x12:
                printf("Sub-function not supported\n");
                break;
            case 0x13:
                printf("Invalid format\n");
                break;
            case 0x22:
                printf("Conditions not correct\n");
                break;
            case 0x31:
                printf("Request out of range\n");
                break;
            default:
                printf("Unknown error\n");
                break;
        }
    }
    return 1;
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