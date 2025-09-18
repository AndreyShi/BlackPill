#include <stdint.h>

/* Defines ------------------------------------------------------------------*/
// Команды MCP2515
#define MCP2515_CMD_READ          0x03
#define MCP2515_CMD_WRITE         0x02
#define MCP2515_CMD_BIT_MODIFY    0x05
#define MCP2515_CMD_RTS_TX0       0x81

// Важные регистры MCP2515
#define MCP2515_REG_CANCTRL       0x0F
#define MCP2515_REG_CANSTAT       0x0E
#define MCP2515_REG_CNF1          0x2A
#define MCP2515_REG_CNF2          0x29
#define MCP2515_REG_CNF3          0x28
#define MCP2515_REG_CANINTE       0x2B
#define MCP2515_REG_CANINTF       0x2C
#define MCP2515_REG_TXB0CTRL      0x30
#define MCP2515_REG_TXB0SIDH      0x31
#define MCP2515_REG_TXB0SIDL      0x32
#define MCP2515_REG_TXB0DLC       0x35
#define MCP2515_REG_TXB0D0        0x36
#define MCP2515_REG_RXB0CTRL      0x60
#define MCP2515_REG_RXB0SIDH      0x61
#define MCP2515_REG_RXB0DLC       0x65
#define MCP2515_REG_RXB0D0        0x66

// CAN ID для OBD (ISO 15765-4)
#define CAN_OBD_REQUEST_ID        0x7DF   // Широковещательный запрос
#define CAN_OBD_RESPONSE_ID       0x7E8   // Ответ от двигателя

// PID коды
#define PID_ENGINE_RPM            0x0C
#define PID_COOLANT_TEMP          0x05    // Температура охлаждающей жидкости
#define PID_DTC_STATUS            0x01    // Статус DTC (Diagnostic Trouble Codes)
#define PID_FREEZE_DTC            0x02    // Замороженные кадры DTC

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

/*
  *         Без использования прерываний (режим опроса).
  */
void MCP2515_Init_ISO15765(void);


/**
  * @brief  Инициализация MCP2515 с фильтром на OBD ответы
  */
void MCP2515_Init_With_Filter(void);


/**
  * @brief  Проверка и чтение принятого сообщения (режим опроса)
  * @param  data: указатель на буфер для данных (минимум 8 байт)
  * @retval 0 - сообщения нет, >0 - количество принятых байт
  */
uint8_t MCP2515_Read_Message_Polling(uint8_t *data);

/**
  * @brief  Отправка OBD2 запроса на получение RPM
  */
void MCP2515_Send_OBD_Request(uint16_t can_id, uint8_t pid);

/**
  * @brief  Парсинг RPM из данных OBD2 ответа
  */
float Parse_Engine_RPM(uint8_t *data, uint8_t length);

/**
  * @brief  Парсинг температуры охлаждающей жидкости
  */
float Parse_Coolant_Temperature(uint8_t *data, uint8_t length);

/**
  * @brief  Парсинг статуса DTC (Check Engine)
  */
typedef struct {
  uint8_t mil_status;        // 0 - MIL выключен, 1 - MIL включен (Check Engine)
  uint8_t dtc_count;         // Количество активных DTC
  uint8_t supported_tests;   // Поддерживаемые тесты
  uint8_t test_completion;   // Завершение тестов
} DTC_Status;

DTC_Status Parse_DTC_Status(uint8_t *data, uint8_t length);

// Пример использования в main.c или в другом месте
void example_usage(void);