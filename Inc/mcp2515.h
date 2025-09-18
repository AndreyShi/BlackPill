#include <stdint.h>

/* Defines ------------------------------------------------------------------*/
// Команды MCP2515
#define MCP2515_CMD_READ          0x03
#define MCP2515_CMD_WRITE         0x02
#define MCP2515_CMD_BIT_MODIFY    0x05

// Команды SPI для MCP2515
#define MCP2515_CMD_RESET     0xC0  // Сброс контроллера
#define MCP2515_CMD_RTS       0x80  // Базовый RTS команда

// Команды RTS (Request To Send) для конкретных буферов
#define MCP2515_CMD_RTS_TX0   0x81  // Запрос отправки для TXB0
#define MCP2515_CMD_RTS_TX1   0x82  // Запрос отправки для TXB1  
#define MCP2515_CMD_RTS_TX2   0x84  // Запрос отправки для TXB2
#define MCP2515_CMD_RTS_ALL   0x87  // Запрос отправки для всех буферов

// Команды чтения с автоматическим инкрементом
#define MCP2515_CMD_READ_RX0  0x90  // Чтение RXB0 с автоинкрементом
#define MCP2515_CMD_READ_RX1  0x94  // Чтение RXB1 с автоинкрементом

// Команды изменения битов
#define MCP2515_CMD_BITMOD    0x05  // Изменение отдельных битов

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

// CAN ID для ISO 27145 (WWH-OBD)
#define CAN_ISO27145_REQUEST_ID  0x18DB33F1  // WWH-OBD (29-bit)
#define CAN_ISO27145_RESPONSE_ID 0x18DAF100  // WWH-OBD (29-bit)

// PID коды
#define PID_ENGINE_RPM            0x0C
#define PID_COOLANT_TEMP          0x05    // Температура охлаждающей жидкости
#define PID_DTC_STATUS            0x01    // Статус DTC (Diagnostic Trouble Codes)
#define PID_FREEZE_DTC            0x02    // Замороженные кадры DTC


#define NRC_SERVICE_NOT_SUPPORTED    0x11  // Сервис не поддерживается
#define NRC_SUBFUNCTION_NOT_SUPPORTED 0x12 // Подфункция не поддерживается
#define NRC_INVALID_FORMAT           0x13  // Неверный формат
#define NRC_CONDITIONS_NOT_CORRECT   0x22  // Условия не выполнены
#define NRC_REQUEST_OUT_OF_RANGE     0x31  // Запрос вне диапазона

#define MCP2515_REG_RXM0SIDH  0x20  // Маска 0 Standard ID High
#define MCP2515_REG_RXM0SIDL  0x21  // Маска 0 Standard ID Low + Extended ID
#define MCP2515_REG_RXM0EID8  0x22  // Маска 0 Extended ID High
#define MCP2515_REG_RXM0EID0  0x23  // Маска 0 Extended ID Low

#define MCP2515_REG_RXM1SIDH  0x24  // Маска 1 Standard ID High  
#define MCP2515_REG_RXM1SIDL  0x25  // Маска 1 Standard ID Low + Extended ID
#define MCP2515_REG_RXM1EID8  0x26  // Маска 1 Extended ID High
#define MCP2515_REG_RXM1EID0  0x27  // Маска 1 Extended ID Low

// Фильтры RXF0 - RXF5
#define MCP2515_REG_RXF0SIDH  0x00  // Фильтр 0 Standard ID High
#define MCP2515_REG_RXF0SIDL  0x01  // Фильтр 0 Standard ID Low + Extended ID
#define MCP2515_REG_RXF0EID8  0x02  // Фильтр 0 Extended ID High
#define MCP2515_REG_RXF0EID0  0x03  // Фильтр 0 Extended ID Low

#define MCP2515_REG_RXF1SIDH  0x04  // Фильтр 1 Standard ID High
#define MCP2515_REG_RXF1SIDL  0x05  // Фильтр 1 Standard ID Low + Extended ID
#define MCP2515_REG_RXF1EID8  0x06  // Фильтр 1 Extended ID High
#define MCP2515_REG_RXF1EID0  0x07  // Фильтр 1 Extended ID Low

#define MCP2515_REG_RXF2SIDH  0x08  // Фильтр 2 Standard ID High
#define MCP2515_REG_RXF2SIDL  0x09  // Фильтр 2 Standard ID Low + Extended ID
#define MCP2515_REG_RXF2EID8  0x0A  // Фильтр 2 Extended ID High
#define MCP2515_REG_RXF2EID0  0x0B  // Фильтр 2 Extended ID Low

#define MCP2515_REG_RXF3SIDH  0x10  // Фильтр 3 Standard ID High
#define MCP2515_REG_RXF3SIDL  0x11  // Фильтр 3 Standard ID Low + Extended ID
#define MCP2515_REG_RXF3EID8  0x12  // Фильтр 3 Extended ID High
#define MCP2515_REG_RXF3EID0  0x13  // Фильтр 3 Extended ID Low

#define MCP2515_REG_RXF4SIDH  0x14  // Фильтр 4 Standard ID High
#define MCP2515_REG_RXF4SIDL  0x15  // Фильтр 4 Standard ID Low + Extended ID
#define MCP2515_REG_RXF4EID8  0x16  // Фильтр 4 Extended ID High
#define MCP2515_REG_RXF4EID0  0x17  // Фильтр 4 Extended ID Low

#define MCP2515_REG_RXF5SIDH  0x18  // Фильтр 5 Standard ID High
#define MCP2515_REG_RXF5SIDL  0x19  // Фильтр 5 Standard ID Low + Extended ID
#define MCP2515_REG_RXF5EID8  0x1A  // Фильтр 5 Extended ID High
#define MCP2515_REG_RXF5EID0  0x1B  // Фильтр 5 Extended ID Low

#define MCP2515_REG_TXB0CTRL  0x30  // Control register
#define MCP2515_REG_TXB0SIDH  0x31  // Standard Identifier High
#define MCP2515_REG_TXB0SIDL  0x32  // Standard Identifier Low + Extended ID
#define MCP2515_REG_TXB0EID8  0x33  // Extended Identifier High
#define MCP2515_REG_TXB0EID0  0x34  // Extended Identifier Low
#define MCP2515_REG_TXB0DLC   0x35  // Data Length Code
#define MCP2515_REG_TXB0D0    0x36  // Data Byte 0
#define MCP2515_REG_TXB0D1    0x37  // Data Byte 1
#define MCP2515_REG_TXB0D2    0x38  // Data Byte 2
#define MCP2515_REG_TXB0D3    0x39  // Data Byte 3
#define MCP2515_REG_TXB0D4    0x3A  // Data Byte 4
#define MCP2515_REG_TXB0D5    0x3B  // Data Byte 5
#define MCP2515_REG_TXB0D6    0x3C  // Data Byte 6
#define MCP2515_REG_TXB0D7    0x3D  // Data Byte 7

#define MCP2515_REG_TXB1CTRL  0x40  // Control register
#define MCP2515_REG_TXB1SIDH  0x41  // Standard Identifier High
#define MCP2515_REG_TXB1SIDL  0x42  // Standard Identifier Low + Extended ID
#define MCP2515_REG_TXB1EID8  0x43  // Extended Identifier High
#define MCP2515_REG_TXB1EID0  0x44  // Extended Identifier Low
#define MCP2515_REG_TXB1DLC   0x45  // Data Length Code
#define MCP2515_REG_TXB1D0    0x46  // Data Byte 0
#define MCP2515_REG_TXB1D1    0x47  // Data Byte 1
#define MCP2515_REG_TXB1D2    0x48  // Data Byte 2
#define MCP2515_REG_TXB1D3    0x49  // Data Byte 3
#define MCP2515_REG_TXB1D4    0x4A  // Data Byte 4
#define MCP2515_REG_TXB1D5    0x4B  // Data Byte 5
#define MCP2515_REG_TXB1D6    0x4C  // Data Byte 6
#define MCP2515_REG_TXB1D7    0x4D  // Data Byte 7

#define MCP2515_REG_TXB2CTRL  0x50  // Control register
#define MCP2515_REG_TXB2SIDH  0x51  // Standard Identifier High
#define MCP2515_REG_TXB2SIDL  0x52  // Standard Identifier Low + Extended ID
#define MCP2515_REG_TXB2EID8  0x53  // Extended Identifier High
#define MCP2515_REG_TXB2EID0  0x54  // Extended Identifier Low
#define MCP2515_REG_TXB2DLC   0x55  // Data Length Code
#define MCP2515_REG_TXB2D0    0x56  // Data Byte 0
#define MCP2515_REG_TXB2D1    0x57  // Data Byte 1
#define MCP2515_REG_TXB2D2    0x58  // Data Byte 2
#define MCP2515_REG_TXB2D3    0x59  // Data Byte 3
#define MCP2515_REG_TXB2D4    0x5A  // Data Byte 4
#define MCP2515_REG_TXB2D5    0x5B  // Data Byte 5
#define MCP2515_REG_TXB2D6    0x5C  // Data Byte 6
#define MCP2515_REG_TXB2D7    0x5D  // Data Byte 7
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

/*
  * Инициализация MCP2515 для ISO 27145-4 (WWH-OBD)
  * 29-битные идентификаторы, фильтрация на ответы ECU
  */
void MCP2515_Init_ISO27145(void);


void MCP2515_Send_ISO27145_TXB0_Extended(uint32_t can_id, uint8_t *data, uint8_t length);


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


/**
  * @brief  Обработка отрицательного ответа
  */
int Handle_Negative_Response(uint8_t *data, uint8_t length);

// Пример использования в main.c или в другом месте
void example_usage(void);