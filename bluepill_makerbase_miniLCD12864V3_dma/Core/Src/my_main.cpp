#include "main.h"
#include <stdio.h>   // for printf
#include <stm32_u8g2_minilcd12864v3_dma.h>
#include <super_rgbled_ws2811.h>
/*
 add to this part to main.c to enable printf to usart putty com##
 extern int my_main(void);

 //must have this to print in uart com port
 PUTCHAR_PROTOTYPE {
 // Place your implementation of fputc here
 // e.g. write a character to the EVAL_COM1 and Loop until the end of transmission
 HAL_UART_Transmit(&huart2, (uint8_t*) &ch, 1, 0xFFFF);

 return ch;
 }

 */

void setup_u8g2_stm32();

extern UART_HandleTypeDef huart2;

#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
 set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

//must have this to print in uart com port
PUTCHAR_PROTOTYPE {
    // Place your implementation of fputc here
    // e.g. write a character to the EVAL_COM1 and Loop until the end of transmission
    HAL_UART_Transmit(&huart2, (uint8_t*) &ch, 1, 0xFFFF);

    return ch;
}

#define LED_COUNT 3
static Super_RgbLed_WS2811 rgbled(RGB_DATA_GPIO_Port, RGB_DATA_Pin, LED_COUNT);

void set_lcd_backlight() {
    rgbled.rgb_led_off();
    HAL_Delay(200);
    rgbled.set_color_one_led(0,  80,  170,  0);
    rgbled.set_color_one_led(1, 10, 3, 2);
    rgbled.set_color_one_led(2, 4, 5, 2);

    rgbled.rgb_led_on();
}




extern SPI_HandleTypeDef hspi2;
#define LCD_SPI hspi2

U8G2_ST7567_128X64_F_STM32_HW_SPI_DMA my_u8g2(U8G2_R2, & LCD_SPI );

/**
 * @brief  The real and my application entry point since STM32 IDE doesn't generate main.cpp.
 * @retval int
 */
extern "C" int my_main(void) {

    printf("about to do main while()\n\r");

    set_lcd_backlight();
    my_u8g2.setup_u8g2_stm32();

    uint32_t start_ts = 0;
    uint32_t end_ts = 0;
    uint16_t frame_count = 0;
    const uint16_t time_count = 2000;
    uint16_t fps = 0;
    start_ts = HAL_GetTick();
    my_u8g2.draw_test_screen(fps);
    end_ts = HAL_GetTick();


    while (1) {

       my_u8g2.draw_test_screen(fps);
       HAL_Delay(20);   // no delay , 36FPS ,  delay(20),  21FPS
       // using DMA  26FPS


        frame_count++;
        end_ts = HAL_GetTick();
        uint32_t delta_msec = end_ts - start_ts;
        if (delta_msec > time_count) {
            fps = frame_count * 1000 / delta_msec;
            frame_count = 0;
            start_ts = HAL_GetTick();
        }
    }

}
