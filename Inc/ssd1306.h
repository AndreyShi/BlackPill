#include "main.h"
#include "string.h"
#include "stdio.h"
#include <stdarg.h>

// Адрес дисплея (обычно 0x3C или 0x3D)
#define OLED_ADDRESS 0x3C

// Команды для дисплея
#define OLED_COMMAND 0x00
#define OLED_DATA 0x40

// Размеры дисплея
#define OLED_WIDTH 128
#define OLED_HEIGHT 32
#define FONT_WIDTH 5
#define FONT_HEIGHT 7

// Структура для работы с дисплеем
typedef struct {
    I2C_HandleTypeDef *hi2c;
    uint8_t buffer[OLED_WIDTH * OLED_HEIGHT / 8];
    uint8_t currentX;
    uint8_t currentY;
} OLED_HandleTypeDef;

extern OLED_HandleTypeDef oled;

// Функция отправки команды
void OLED_WriteCommand(OLED_HandleTypeDef *oled, uint8_t command);
// Функция отправки данных
void OLED_WriteData(OLED_HandleTypeDef *oled, uint8_t data);

// Инициализация дисплея
void OLED_Init(OLED_HandleTypeDef *oled, I2C_HandleTypeDef *hi2c);
// Очистка дисплея
void OLED_Clear(OLED_HandleTypeDef *oled);

// Обновление экрана
void OLED_UpdateScreen(OLED_HandleTypeDef *oled);

// Установка позиции курсора
void OLED_SetCursor(OLED_HandleTypeDef *oled, uint8_t x, uint8_t y);

void OLED_ClearCharArea(OLED_HandleTypeDef *oled, uint8_t x, uint8_t y);

// Простой вывод символа (базовый шрифт 5x7)
void OLED_WriteChar(OLED_HandleTypeDef *oled, char ch);

// Вывод строки
void OLED_WriteString(uint8_t update_src, OLED_HandleTypeDef *oled, uint8_t row, uint8_t col, char *str, ...);

// Перевернуть экран по горизонтали
void OLED_FlipHorizontal(OLED_HandleTypeDef *oled, uint8_t flip);

// Перевернуть экран по вертикали
void OLED_FlipVertical(OLED_HandleTypeDef *oled, uint8_t flip);

// Установка позиции текста (строка 0-3, столбец 0-25)
void OLED_SetTextCursor(OLED_HandleTypeDef *oled, uint8_t row, uint8_t col);

// Инвертировать цвета дисплея
void OLED_InvertColors(OLED_HandleTypeDef *oled, uint8_t invert);