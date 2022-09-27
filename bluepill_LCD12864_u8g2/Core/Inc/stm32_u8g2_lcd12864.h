// to support u82g, LCD12864 ST7920 with STM32F103 (blue pill) and STM32F401 (Blackpill)

// Must define below in ioc file
// LCD_RESET  and CS  are not needed
// CS keep high.   RESET can use resistor and a cap to make it low for a while and keep high
//#define LCD_SPI hspi2
// Pin Config --> RCC --> High Speed Clock (HSE) : Crystal/Ceremic Resonator

// SPI setting for stm32 and LCD12864 - ST7920 to work
// SPI:  transmit only master, NSS disable
// SPI prescaler 64  ( when bluepill is running 72MHZ )
// 8X9
// baudrate =562KB is ok  FPS=17
// CPSA : 2 edge (important)
// Clock Polarity , CPOL = HIGH or LOW both work

// SPI prescaler 32 when bluepill is running 64MHZ
// 8X8
// baudrate=1MB, is OK   FPS=22

// SPI prescaler 32 when bluepill is running 72MHZ
// 8X9
// baudrate=1.125MB, is NOT OK


#include <U8g2lib.h>  //needed for CPP style u8g2


uint8_t u8x8_byte_3wire_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

uint8_t u8x8_stm32_gpio_and_delay_cb(U8X8_UNUSED u8x8_t *u8x8,
U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int,
U8X8_UNUSED void *arg_ptr);

class U8G2_ST7920_128X64_F_STM32_HW_SPI: public U8G2 {
public:
    U8G2_ST7920_128X64_F_STM32_HW_SPI(const u8g2_cb_t *rotation = U8G2_R0) : U8G2() {

        u8g2_Setup_st7920_s_128x64_f(&u8g2, rotation, u8x8_byte_3wire_hw_spi, u8x8_stm32_gpio_and_delay_cb);

    }
};

