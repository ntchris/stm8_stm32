// to support u82g, LCD12864 ST7920 with STM32F103 (blue pill) and STM32F401 (Blackpill)


#include <stm32_u8g2_lcd12864.h>
#include <stdint.h>
#include <u8g2.h>
#include <main.h>
#include <string>
#include <string.h>

#define LCD_SPI hspi2

extern SPI_HandleTypeDef hspi2;

// define u8g2 class/instance
U8G2_ST7920_128X64_F_STM32_HW_SPI u8g2;



void u8g_hw_port_delay_ns(uint8_t ns) {
    // Core @72 MHZ: 14ns per instruction.
    // __NOP(); is direct "nop;" instruction to cpu.
    // Divide ns / 28 (extra instruction for jump back to beginning of the loop) for loop cycles.
    for (uint8_t i = 0; i < (ns / 28); i++) {
        __NOP();
    }
}

uint8_t u8x8_stm32_gpio_and_delay_cb(U8X8_UNUSED u8x8_t *u8x8,
U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int,
U8X8_UNUSED void *arg_ptr) {
    switch (msg) {

    case U8X8_MSG_GPIO_AND_DELAY_INIT:

        break;
    case U8X8_MSG_DELAY_NANO:
        u8g_hw_port_delay_ns(arg_int);

        break;
    case U8X8_MSG_DELAY_10MICRO:

        break;

    case U8X8_MSG_DELAY_100NANO:

        break;

    case U8X8_MSG_DELAY_MILLI:
        // HAL_Delay(arg_int);
        break;

    case U8X8_MSG_GPIO_DC:
         break;

    case U8X8_MSG_GPIO_CS1: // Set the spi chip slect line level.
        break;

    case U8X8_MSG_GPIO_RESET:
        //HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, (GPIO_PinState)arg_int);
        break;
    }
    return 1;
}

inline uint8_t u8x8_byte_3wire_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    switch (msg) {
    case U8X8_MSG_BYTE_SEND:

         HAL_SPI_Transmit(&LCD_SPI, (uint8_t*) arg_ptr, arg_int, 10000);

        break;
    case U8X8_MSG_BYTE_INIT:
        break;

    case U8X8_MSG_BYTE_START_TRANSFER:

        break;
    case U8X8_MSG_BYTE_END_TRANSFER:

         break;
    default:
        return 0;
    }
    return 1;
}

void reset_lcd() {
    HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_RESET);
    HAL_Delay(30);
    HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_SET);
    HAL_Delay(100);

}
void setup_u8g2_stm32() {
// c style def
// u8g2_Setup_st7920_s_128x64_f(&u8g2, U8G2_R0, u8x8_byte_3wire_hw_spi, u8x8_stm32_gpio_and_delay_cb);
    reset_lcd();
    u8g2.initDisplay();
    u8g2.setPowerSave(0);
    u8g2.clearBuffer();
//u8g2.sendBuffer();
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
//u8g2.clearDisplay();
    u8g2.clearBuffer();
    //u8g2.drawBox(5, 5, 118, 56);

    /*
    uint32_t start_ts = HAL_GetTick();
    u8g2.sendBuffer();
    uint32_t end_ts = HAL_GetTick();
    uint delta_msec = end_ts - start_ts;
    printf("%u\n\r", delta_msec);
*/

     //u8g2.setDrawColor(1);
     u8g2.setFont(u8g2_font_ncenB08_tr);
     u8g2.drawStr(0, 12, "LCD12864 ST7920 u8g2");
     u8g2.setFont(u8g2_font_ncenB08_tr);
     u8g2.drawStr(1, 30, "STM32F103C8T6");
     u8g2.drawCircle(50, 35, 25, U8G2_DRAW_ALL);
     static uint16_t counter =0;
     u8g2.drawStr(1, 51, std::to_string(counter).c_str() );
     counter++;

     if (fps) {
     u8g2.setFont(u8g2_font_ncenB08_tr);
     std::string str = std::to_string(fps);
     u8g2.drawStr(110, 60, str.c_str());
     }
     uint32_t start_ts = HAL_GetTick();
     u8g2.sendBuffer();
     uint32_t end_ts = HAL_GetTick();
     uint32_t delta_msec = end_ts -start_ts;
     printf("%u\n\r", delta_msec);


}

