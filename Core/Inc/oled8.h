#include "i2c.h"

#define OLED_I2C_HANDLE hi2c1
#define OLED_I2C_ADDR 0x78
#define OLED_X_SIZE 128
#define OLED_Y_SIZE 32
#define OLED_TIMEOUT 100

typedef unsigned char uint8_t;

void oled8_update(void);
void oled8_init(void);
void oled8_putc(uint8_t x, uint8_t y, char c);
void oled8_puts(uint8_t x, uint8_t y, char* s);
void oled8_putn(uint8_t x, uint8_t y, int num, uint8_t len);
void oled8_putf(uint8_t x, uint8_t y, float num, uint8_t lenl, uint8_t lenr);
