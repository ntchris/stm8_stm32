// to support u82g, LCD12864 ST7567 with STM32F103 (blue pill) and STM32F401 (Blackpill)
#include <stdio.h>   // for printf
#include<string>
#include <stdint.h>
#include <u8g2.h>
#include <u8x8.h>

#include <stm32_u8g2_minilcd12864_v3.h>

SPI_HandleTypeDef *g_lcd_spi;


#define delay_20nanosec()  __NOP();__NOP();
#define delay_40nanosec()  __NOP();__NOP();__NOP();
#define delay_60nanosec()  __NOP();__NOP();__NOP();__NOP();
#define delay_100nanosec()  __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
#define delay_200nanosec()  delay_100nanosec(); delay_100nanosec();
#define delay_300nanosec()  delay_100nanosec(); delay_200nanosec();
#define delay_400nanosec()  delay_200nanosec(); delay_200nanosec();
#define delay_500nanosec()  delay_200nanosec(); delay_300nanosec();

extern void delay_us(uint32_t time);

/*
 void delay_us(uint32_t time) {
 uint32_t i = 8 * time;
 while (i--)
 ;
 }
 */

uint8_t u8x8_stm32_gpio_and_delay_cb(U8X8_UNUSED u8x8_t *u8x8,
U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int,
U8X8_UNUSED void *arg_ptr) {
    switch (msg) {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
        break;
    case U8X8_MSG_DELAY_NANO:
        for (int i = 0; i < arg_int / 40; i++) {
            delay_40nanosec();
        }

        break;
    case U8X8_MSG_DELAY_10MICRO:
        delay_us(arg_int * 10ul);
        break;
    case U8X8_MSG_DELAY_100NANO:
        delay_100nanosec()
        ;
        break;
    case U8X8_MSG_DELAY_MILLI:
        // needed for LCD RESET
        HAL_Delay(arg_int);
        break;
    case U8X8_MSG_GPIO_DC:

        break;
    case U8X8_MSG_GPIO_CS1:
        // never gets called ??!
        // printf("U8X8_MSG_GPIO_CS1 here!!!!\n\r");
        break;
    case U8X8_MSG_GPIO_RESET:
        HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, (GPIO_PinState) arg_int);
        break;
    }
    return 1;
}



inline uint8_t u8x8_byte_4wire_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    switch (msg) {
    case U8X8_MSG_BYTE_SEND:

        // cache_spi_data_to_buffer_for_dma_u8g2_callback(arg_ptr, arg_int);
        while (HAL_SPI_STATE_READY != HAL_SPI_GetState(g_lcd_spi)) {
            __NOP();
        }
        HAL_SPI_Transmit(g_lcd_spi, (uint8_t*) arg_ptr, arg_int, 1000000);  // non DMA
        break;
    case U8X8_MSG_BYTE_INIT:
        break;
    case U8X8_MSG_BYTE_SET_DC:
        // DC, Data/Command PIN is necessary for ST7567 LCD
        HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, arg_int ? GPIO_PIN_SET : GPIO_PIN_RESET);

        break;
    case U8X8_MSG_BYTE_START_TRANSFER:
#ifdef LCD_CS_Pin
        HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
        u8x8->gpio_and_delay_cb(u8x8, U8X8_MSG_DELAY_NANO, u8x8->display_info->post_chip_enable_wait_ns, NULL);
#endif
        break;
    case U8X8_MSG_BYTE_END_TRANSFER:
#ifdef LCD_CS_Pin
        HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, (GPIO_PinState) (u8x8->display_info->chip_disable_level));
#endif

        break;
    default:
        return 0;
    }
    return 1;
}

/*
void reset_lcd() {
    HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_RESET);
    HAL_Delay(200);
    HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_SET);

}
*/

void U8G2_ST7567_128X64_F_STM32_HW_SPI::setup_u8g2_stm32() {
    // c style def   u8g2_Setup_st7920_s_128x64_f(&u8g2, U8G2_R0, u8x8_byte_3wire_hw_spi, u8x8_stm32_gpio_and_delay_cb);
    // reset_lcd();
    this->initDisplay();
    this->setPowerSave(0);
    this->setContrast(150);

    //my_u8g2.setContrast( 255);
    this->clearBuffer();
}

// constructor
U8G2_ST7567_128X64_F_STM32_HW_SPI::U8G2_ST7567_128X64_F_STM32_HW_SPI(const u8g2_cb_t *rotation,
        SPI_HandleTypeDef *lcd_spi) {
    g_lcd_spi = lcd_spi;
    u8g2_Setup_st7567_jlx12864_f(&u8g2, U8G2_R2, u8x8_byte_4wire_hw_spi, u8x8_stm32_gpio_and_delay_cb); // 168 seems best

}

SPI_HandleTypeDef* U8G2_ST7567_128X64_F_STM32_HW_SPI::get_spi() {
    return g_lcd_spi;
}
void U8G2_ST7567_128X64_F_STM32_HW_SPI::draw_test_screen(uint8_t fps = 0) {

    this->clearBuffer();
    this->setFont(u8g_font_ncenB08);
    this->drawStr(1, 9, "LCD12864 ST7567 u8g2");

    this->setFont(u8g_font_helvR08);

    this->drawStr(1, 21, "STM32F103C8T6");
    this->drawCircle(64, 35, 25, U8G2_DRAW_ALL);
    static uint16_t counter = 0;
    this->drawStr(1, 63, std::to_string(counter).c_str());
    counter++;

    if (fps) {
        //this->setFont(u8g_font_courR08r);
        std::string str = std::string("fps ") + std::to_string(fps);
        this->drawStr(98, 63, str.c_str());
       // printf("fps is %u\n\r", fps);
    }

}
