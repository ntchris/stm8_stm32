

#include "main.h"
#include <stdio.h>   // for printf
 #include "u8g2.h"



void setup_u8g2_stm32();
void draw_test_screen(uint16_t fps);



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



/**
 * @brief  The real and my application entry point since STM32 IDE doesn't generate main.cpp.
 * @retval int
 */
  extern "C"
int my_main(void) {

    setup_u8g2_stm32();


    uint32_t start_ts = 0;
    uint32_t end_ts = 0;
    uint16_t frame_count = 0;
    const uint16_t time_count = 2000;
    uint16_t fps = 0;
    start_ts = HAL_GetTick();
    while (1) {

        draw_test_screen(fps);
        frame_count++;
        end_ts = HAL_GetTick();
        uint32_t delta_msec = end_ts - start_ts;
        if ( delta_msec >time_count)
        {
               fps = frame_count*1000/delta_msec ;
               frame_count = 0;
               start_ts = HAL_GetTick();
        }

        HAL_Delay(20);
    }

}
