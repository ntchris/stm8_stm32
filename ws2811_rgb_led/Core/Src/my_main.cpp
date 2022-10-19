
#include "main.h"
#include <stdio.h>   // for printf
#include <super_rgbled_ws2811.h>

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

// the order is 1: LCD Screen backlight , 2: rotary left ,  3: rotary right.
// one small thing to pay attention:
// after remove /disable RGB data, the rgb chip remain the previous state, meaning:
// if previously we light up all 3 leds (LED_COUNT 3) ,
// but later we change the code : LED_COUNT 2, to only control 2 LEDs.
// so the 2nd chip won't send anything to the 3rd chip, so the 3rd chip remain the same and the
// 3rd LED remains the same. until power cycle.
#define LED_COUNT 3

/**
 * @brief  The real and my application entry point since STM32 IDE doesn't generate main.cpp.
 * @retval int
 */
extern "C" int my_main(void) {

    static Super_RgbLed_WS2811 rgbled(RGB_DATA_GPIO_Port, RGB_DATA_Pin, LED_COUNT);
    //rgbled.light_rgb_led(255, 255, 255);
    rgbled.rgb_led_off( );
    HAL_Delay(500);
    rgbled.rgb_led_on(255, 255, 255 );
    HAL_Delay(500);
    //rgbled.self_test_3_different_color();
    //rgbled.rgb_led_on(100, 100, 30 );
    //rgbled.self_test_keep_sending_0_code();
    //rgbled.self_test_keep_sending_1_code();

    //rgbled.self_test_keep_changing_dim();
    //rgbled.self_test_keep_sending_white_FFFFFF();
    //rgbled.self_test_keep_sending_OFF_000000();
    /*
     rgbled.rgb_led_on(255, 255, 255 );

     */
    //rgbled.self_test_3_different_color();
    while (1) {

        //rgbled.self_test_keep_changing_color();
        //rgbled.rgb_led_on(128 , 128, 128 );
     rgbled.self_test_all();



    }

}
