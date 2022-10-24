#include "main.h"
#include <stdio.h>   // for printf
#include<string>

#include <super_rgbled_ws2811.h>
#include<stm32_u8g2_minilcd12864_v3.h>
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
extern TIM_HandleTypeDef htim2;

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
    rgbled.set_color_one_led(0, 80, 170, 0);
    rgbled.set_color_one_led(1, 10, 3, 2);
    rgbled.set_color_one_led(2, 4, 5, 2);

    rgbled.rgb_led_on();
}

extern SPI_HandleTypeDef hspi2;
#define LCD_SPI hspi2

U8G2_ST7567_128X64_F_STM32_HW_SPI my_u8g2(U8G2_R2, & LCD_SPI);


// for encoder , note, the pins must be pull up externally
static uint16_t g_value ;
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{

    g_value = __HAL_TIM_GET_COUNTER(htim);


}

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
    //HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
    HAL_TIM_Encoder_Start_IT(&htim2, TIM_CHANNEL_ALL);
    uint16_t old_value=0 ;

    while (1) {

        bool enc_btn = HAL_GPIO_ReadPin(BTN_ENC_GPIO_Port, BTN_ENC_Pin);

        my_u8g2.draw_test_screen(fps);
        HAL_Delay(10);   // no delay , 36FPS ,  delay(20),  21FPS
        if (enc_btn) {
            my_u8g2.drawStr(20, 40, "enc btn");

        }
        std::string value_str = std::to_string(g_value);
        my_u8g2.drawStr(80, 40, value_str.c_str()   );

         if ( g_value!=old_value)
        {
            printf("%u \n\r", g_value);
            old_value = g_value;
        }




        uint32_t start_ts = HAL_GetTick();
        my_u8g2.sendBuffer();
        uint32_t end_ts = HAL_GetTick();
        unsigned int delta_msec = end_ts - start_ts;
        //printf("sendbuffer used %u ms\r\n", delta_msec);


/*
    frame_count++;
    end_ts = HAL_GetTick();
    uint32_t delta_msec = end_ts - start_ts;
    if (delta_msec > time_count) {
        fps = frame_count * 1000 / delta_msec;
        frame_count = 0;
        start_ts = HAL_GetTick();
    }
    */
}

}
