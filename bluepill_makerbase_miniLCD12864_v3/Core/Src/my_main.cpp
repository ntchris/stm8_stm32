#include "main.h"
#include <stdio.h>   // for printf
#include<string>

#include <super_rgbled_ws2811.h>
#include <stm32_u8g2_minilcd12864_v3.h>
#include <simple_menu.h>
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

void set_lcd_backlight_default() {
    rgbled.rgb_led_off();
    HAL_Delay(200);
    rgbled.set_color_one_led(0, 80, 170, 0);
    rgbled.set_color_one_led(1, 10, 3, 2);
    rgbled.set_color_one_led(2, 4, 5, 2);

    rgbled.rgb_led_on();
}

void set_lcd_backlight(uint8_t r, uint8_t g, uint8_t b) {
    static uint8_t old_r, old_g, old_b;
    bool changed = false;
    if (old_r != r) {
        old_r = r;
        changed = true;
    }
    if (old_g != g) {
        changed = true;
        old_g = g;
    }
    if (old_b != b) {
        changed = true;
        old_b = b;
    }

    if (changed) {
        rgbled.rgb_led_on(r, g, b);

    }

}

extern SPI_HandleTypeDef hspi2;
#define LCD_SPI hspi2

U8G2_ST7567_128X64_F_STM32_HW_SPI my_u8g2(U8G2_R2, & LCD_SPI);
Simple_Menu_Item_Value menu(&my_u8g2);

static uint32_t g_start_time = 0;
static uint16_t g_on_time = 0;
void beeper_on(uint16_t on_time = 100) {

    HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
    g_start_time = HAL_GetTick();
    g_on_time = on_time;

}

void beeper_off() {

    HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
    g_start_time = 0;
    g_on_time = 0;

}

void check_and_turn_beeper_off() {
    if (g_start_time == 0) {
        return;
    }
    uint32_t now = HAL_GetTick();
    if ((now - g_start_time) > g_on_time) {
        HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
        g_start_time = 0;
        g_on_time = 0;
    }

}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
    __IO static uint16_t old_value;
    // for encoder , note, the pins must be pull up internally or externally
    __IO static uint16_t g_value;

    const uint8_t STEP = 4;

    beeper_on(10);

    g_value = __HAL_TIM_GET_COUNTER(htim);

    // printf("g: %u , old: %u\n\r", g_value, old_value);
    if (g_value >= (STEP + old_value)) {
        old_value = g_value;
        menu.active_item_next();

    } else if ((g_value + STEP) <= old_value) {
        old_value = g_value;
        menu.active_item_prev();
    } else {
        // delta, +1, +2, +3, ignore.

    }

}

// Rotary Encoder Button Click Handler
// External Interrupt ISR Handler CallBackFun
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    // debounce
    // can you press the button 10+ times in a second ?
    const uint8_t TIME_DELTA_DEBOUNCE = 90;
    if (GPIO_Pin == BTN_ENC_Pin) {

        static uint32_t last_ts = 0;
        uint32_t now_ts = HAL_GetTick();
        if ((now_ts - last_ts) < TIME_DELTA_DEBOUNCE) {
            last_ts = now_ts;
            return;
        }
        // debounce done.

        last_ts = now_ts;
        beeper_on(100);


        // button handler put here
        menu.click_on_current_menu_item();

    }
}

/*
 not useful

 void test_menu_OK_CANCEL() {
 my_u8g2.setFont(u8g2_font_6x10_tf);
 my_u8g2.setFontRefHeightAll(); // this will add some extra space for the text inside the buttons
 my_u8g2.userInterfaceMessage("Title1", "Title2", "Title3", " Ok \n Cancel ");
 }
 */
void init_rotary_encoder() {

    uint16_t v = htim2.Init.Period / 2;
    __HAL_TIM_SET_COUNTER(&htim2, v);
    HAL_TIM_Encoder_Start_IT(&htim2, v);
}

uint8_t get_fps() {

    static uint32_t fps_start_ts = HAL_GetTick();
    ;
    uint32_t fps_end_ts = 0;
    static uint16_t frame_count = 0;
    static uint8_t fps = 0;
    unsigned int delta_msec;
    const uint16_t Time_Count = 2000;

    frame_count++;
    fps_end_ts = HAL_GetTick();
    delta_msec = fps_end_ts - fps_start_ts;
    if (delta_msec > Time_Count) {
        fps = frame_count * 1000 / delta_msec;

        fps_start_ts = HAL_GetTick();
        frame_count = 0;
    }
    return fps;
}

void get_rgb_from_menu_and_set_rgb() {
    uint8_t r, g, b;
    r = menu.get_menuitem_value(0);
    g = menu.get_menuitem_value(1);
    b = menu.get_menuitem_value(2);
    set_lcd_backlight(r, g, b);
}

extern "C" int my_main(void) {

    printf("about to do main while()\n\r");
    set_lcd_backlight_default();

    my_u8g2.setup_u8g2_stm32();

    // uint32_t start_ts = 0;
    //uint32_t end_ts = 0;

    init_rotary_encoder();

    uint8_t Default_RGB = 100;
    Item_Value mi_red = Item_Value("Red", Default_RGB, 2);
    Item_Value mi_green = Item_Value("Green", Default_RGB, 2);
    Item_Value mi_blue = Item_Value("Blue", Default_RGB, 2);
    menu.add_menu_item(&mi_red);
    menu.add_menu_item(&mi_green);
    menu.add_menu_item(&mi_blue);
    menu.set_active_item(1);
    menu.set_font(u8g2_font_helvR08_tf, u8g2_font_ncenB08_tf);
    while (1) {

        check_and_turn_beeper_off();

        get_rgb_from_menu_and_set_rgb();
        //   bool enc_btn = HAL_GPIO_ReadPin(BTN_ENC_GPIO_Port, BTN_ENC_Pin);
        uint8_t fps = get_fps();

        my_u8g2.draw_test_screen(fps);

        HAL_Delay(20);   // no delay , 36FPS ,  delay(20),  21FPS

        //my_u8g2.drawStr(20, 50, std::to_string(g_btn).c_str());

        // std::string value_str =std::to_string( menu.get_active_menuitem_value() );
        // my_u8g2.drawStr(80, 50, value_str.c_str());

        menu.show_menu(6, 34);
        //test_menu_OK_CANCEL();
        // start_ts = HAL_GetTick();
        my_u8g2.sendBuffer();
        // end_ts = HAL_GetTick();
        // unsigned int delta_msec = end_ts - start_ts;
        // printf("sendbuffer used %u ms\r\n", delta_msec);

    }

}

