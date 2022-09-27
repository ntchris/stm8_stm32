// to support u82g, LCD12864 ST7920 with STM32F103 (blue pill) and STM32F401 (Blackpill)

#include <stm32_u8g2_lcd12864.h>
#include <stdint.h>
#include <u8g2.h>
#include <main.h>
#include <string>



extern SPI_HandleTypeDef hspi2;
#define LCD_SPI hspi2

U8G2_ST7920_128X64_F_STM32_HW_SPI u8g2;
/*
 * no need
void u8g_hw_port_delay_100ns(uint8_t ns) {
    // Same as in u8g_hw_port_delay_ns function.
    // 100 / 28 = 3.57;
    for (uint16_t i = 0; i < (ns * 3.57); i++) {
        __NOP();
    }
}
void u8g_hw_port_delay_10us(uint8_t us) {
    // Same as in u8g_hw_port_delay_ns function.
    // 3.57 * 100 ? 357;
    for (uint16_t i = 0; i < (us * 357); i++) {
        __NOP();
    }
}

void u8g_hw_port_delay_ns(uint8_t ns) {
    // Core @72 MHZ: 14ns per instruction.
    // __NOP(); is direct "nop;" instruction to cpu.
    // Divide ns / 28 (extra instruction for jump back to beginning of the loop) for loop cycles.
    for (uint8_t i = 0; i < (ns / 28); i++) {
        __NOP();
    }
}
*/
uint8_t u8x8_stm32_gpio_and_delay_cb(U8X8_UNUSED u8x8_t *u8x8,
U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int,
U8X8_UNUSED void *arg_ptr) {
    switch (msg) {

    case U8X8_MSG_GPIO_AND_DELAY_INIT:

        break;
    case U8X8_MSG_DELAY_NANO:
        //  u8g_hw_port_delay_ns(arg_int);

        break;
    case U8X8_MSG_DELAY_10MICRO:
        //  u8g_hw_port_delay_10us(arg_int);

        break;

    case U8X8_MSG_DELAY_100NANO:
        //   u8g_hw_port_delay_100ns(arg_int);

        break;

    case U8X8_MSG_DELAY_MILLI:
        // HAL_Delay(arg_int);
        break;

    case U8X8_MSG_GPIO_DC:
        // no need for LCD12864 ST7920
        break;

    case U8X8_MSG_GPIO_CS1: // Set the spi chip slect line level.
        break;

    case U8X8_MSG_GPIO_RESET:
        //HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, (GPIO_PinState)arg_int);
        break;
    }
    return 1;
}

uint8_t u8x8_byte_3wire_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    switch (msg) {
    case U8X8_MSG_BYTE_SEND:
        // use DMA to xfer arg_ptr
        HAL_SPI_Transmit(&LCD_SPI, (uint8_t*) arg_ptr, arg_int, 10000);
        break;
    case U8X8_MSG_BYTE_INIT:
        break;

    case U8X8_MSG_BYTE_START_TRANSFER:
        // no need, always pull high

        break;
    case U8X8_MSG_BYTE_END_TRANSFER:
        // no need, always pull high
        break;
    default:
        return 0;
    }
    return 1;
}

void setup_u8g2_stm32() {
    // c style def
    // u8g2_Setup_st7920_s_128x64_f(&u8g2, U8G2_R0, u8x8_byte_3wire_hw_spi, u8x8_stm32_gpio_and_delay_cb);

    u8g2.initDisplay();
    u8g2.setPowerSave( 0);
}

void draw_test_screen(uint16_t fps = 0) {

    /* c style draw
      u8g2_ClearBuffer (&u8g2);
     u8g2_SetFont(&u8g2, u8g2_font_ncenB08_tr);
     u8g2_DrawStr(&u8g2, 0, 12, "LCD12864 ST7920");
     u8g2_SetFont(&u8g2, u8g2_font_ncenB10_tr);
     u8g2_DrawStr(&u8g2, 1, 30, "STM32F103C8T6");
     u8g2_DrawCircle(&u8g2, 50, 35, 25, U8G2_DRAW_ALL);
     u8g2_SetFont(&u8g2, u8g2_font_ncenB08_tr);
     std::string str = std::to_string(fps);
     u8g2_DrawStr(&u8g2, 110, 60, str.c_str());

     u8g2_SendBuffer(&u8g2);
     */

    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(0, 12, "LCD12864 ST7920 u8g2");
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(1, 30, "STM32F103C8T6");
    u8g2.drawCircle(50, 35, 25, U8G2_DRAW_ALL);
    if (fps) {
        u8g2.setFont(u8g2_font_ncenB08_tr);
        std::string str = std::to_string(fps);
        u8g2.drawStr(110, 60, str.c_str());
    }
    u8g2.sendBuffer();

}




