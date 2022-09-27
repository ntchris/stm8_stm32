
/*
#include "main.h"

 #include "u8g2.h"

extern u8g2_t u8g2;

void setup_u8g2_stm32();
void draw_test_screen(uint16_t fps);


#ifdef _cplusplus
extern "C" {
#endif

int my_main(void);

#ifdef _cplusplus
}
#endif

*/

/*
  {
    setup_u8g2_stm32();

    HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_RESET);
    HAL_Delay(300);
    HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_SET);
    uint32_t start_ts = 0;
    uint32_t end_ts = 0;
    uint16_t frame_count = 0;
    const uint16_t time_count = 2000;
    uint16_t fps = 0;
    start_ts = HAL_GetTick();
    while (1) {
        // LL_GPIO_TogglePin(BOARD_LED_GPIO_Port, BOARD_LED_Pin);

        draw_test_screen(fps);
        frame_count++;
        end_ts = HAL_GetTick();
        uint16_t delta_msec = end_ts - start_ts;
        if ( delta_msec >time_count)
        {
               fps = frame_count*1000/delta_msec ;
               frame_count = 0;
               start_ts = HAL_GetTick();
        }


    }

}
*/
