/**
 *  helper class to let stm32F103 for stm32F401 to handle neopixel RGB LED WS2811 IC.
 *  using NON-DMA method.
 *  pay attention to the stm32 's system clock.
 *  the custom delay function works well for 72MHz
 *  for different system clock, ...delay need change
 */

#ifndef  __super_rgbled_ws2811__
#define  __super_rgbled_ws2811__

// only when debug and do self test,  uncomment next line.
#define DEBUG_RGB

// a lot of stuffs are defined in side main.h , at least indirectly.
#include <main.h>
// regardless, we need to use main.h, otherwise, how do we run it ??

// for uint8_t
#include <stdint.h>
// for printf
#include<stdio.h>

#define delay_20nanosec()  __NOP();__NOP();
#define delay_40nanosec()  __NOP();__NOP();__NOP();
#define delay_60nanosec()  __NOP();__NOP();__NOP();__NOP();
#define delay_100nanosec()  __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
#define delay_200nanosec()  delay_100nanosec(); delay_100nanosec();
#define delay_300nanosec()  delay_100nanosec(); delay_200nanosec();
#define delay_400nanosec()  delay_200nanosec(); delay_200nanosec();
#define delay_500nanosec()  delay_200nanosec(); delay_300nanosec();
//  asm("NOP");

#define BIT8 8

class Super_RgbLed_WS2811 {

    // GPIO low value, so it's easier to use, no shifting bit everytime
    static uint32_t _pin_unset;
    struct One_Led_Data {
        // no matter how we dim, we don't change the base value
        uint8_t base_r, dim_r;
        uint8_t base_g, dim_g;
        uint8_t base_b, dim_b;
    };

    One_Led_Data *_all_rgb_led_data_p = 0;

    GPIO_TypeDef *_gpio_port;
    uint16_t _gpio_pin;

    // how many RGB LED do we have, each RGBLED is controlled by one WS2811 ic.
    uint16_t _led_count;

    void send_0_code_to_ic();
    void send_1_code_to_ic();
    void send_reset_code_to_ic();
    // send one R or G or B data ( 8bits, or uint8_t ) to IC.
    void send_one_color_to_ic(uint8_t rgb);


public:
    // constructor
    Super_RgbLed_WS2811(GPIO_TypeDef *gpio_port, uint16_t gpio_pin, uint16_t led_count) {
        this->_gpio_port = gpio_port;
        this->_gpio_pin = gpio_pin;
        this->_led_count = led_count;
        this->_all_rgb_led_data_p = new One_Led_Data[led_count];

        this->_pin_unset = (uint32_t) this->_gpio_pin << 16u;
    }

    void rgb_led_on(uint8_t r, uint8_t g, uint8_t b);
    //  HTML color values, ie RED = #ff0000 , GREEN = #00ff00
    void rgb_led_on(uint32_t color);

    // dim the current value to dim percentage, 0: off  50: 50%  darker, 100: 100%, no change, 150: stronger
    // dim doesn't change the base color.  if we dim it to 0% and dim again to 100%, it's OK
    // if dim change the base color, once it's dim to 0 , it will never be dim to 100% again
    void rgb_led_dim_all(uint8_t dim);
    // dim to zero, but we don't lose any already set RGB data
    void rgb_led_off();
    // change dim value back to 100%
    void rgb_led_on();
    // set color for all RGB Leds, using r, g, b values.
    void set_color_all(uint8_t r, uint8_t g, uint8_t b);
    // set color for all RGB Leds. using HTML RGB value, like 0xFF0000 = RED
    void set_color_all(uint32_t rgb);

    void set_color_one_led(uint16_t index, uint32_t color);
    void set_color_one_led(uint16_t index, uint8_t r, uint8_t g, uint8_t b);

    // send all data to RGB IC (for all RGB LEDs)
    void send_rgb_data_to_led();

#ifdef DEBUG_RGB
    void print_led_data(struct One_Led_Data *rgb_led_data);
    void print_rgb_data(uint8_t *rgb_bits);

    // keep sending 0 code so oscilloscope can easily check it, high short, low long
    void self_test_keep_sending_0_code();
    // keep sending 1 code so oscilloscope can easily check it, high long, low long
    void self_test_keep_sending_1_code();

    // so we know it's working well.
    // when it couldn't send some correct data, the RGBLED just remains the same color,
    // so we don't know it's sending the correct data at all
    void self_test_keep_on_and_off();
    void self_test_chang_color();
    void self_test_keep_changing_dim();
    void self_test_keep_sending_white_FFFFFF();
    void self_test_keep_sending_OFF_000000();
    void self_test_3_different_color();
    void self_test_all();
#endif // DEBUG_RGB
};

#endif // __super_rgbled_ws2811__
