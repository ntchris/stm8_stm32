#include <super_rgbled_ws2811.h>
#include<stdio.h>

uint32_t Super_RgbLed_WS2811::_pin_unset;

#ifdef DEBUG_RGB
void Super_RgbLed_WS2811::print_rgb_data(uint8_t *rgb_bits) {
    for (int i = 0; i < BIT8; i++) {
        printf("%u ", rgb_bits[i]);
    }
    printf("\n\r");
}

void Super_RgbLed_WS2811::self_test_keep_sending_1_code() {
    while (1) {
        this->send_1_code_to_ic();
    }
}

void Super_RgbLed_WS2811::self_test_keep_sending_0_code() {
    while (1) {
        this->send_0_code_to_ic();

    }
}

void Super_RgbLed_WS2811::self_test_keep_on_and_off() {
    while (1) {
        this->rgb_led_off();
        HAL_Delay(1000);
        this->rgb_led_on(255, 255, 255);

        HAL_Delay(1000);
    }
}

void Super_RgbLed_WS2811::self_test_chang_color() {

    this->rgb_led_off();
    HAL_Delay(500);

    this->rgb_led_on(128, 0, 0);
    HAL_Delay(500);

    this->rgb_led_on(0, 128, 0);
    HAL_Delay(500);
    this->rgb_led_on(0, 0, 128);
    HAL_Delay(500);
    this->rgb_led_on(128, 128, 128);
    HAL_Delay(500);

    this->rgb_led_on(128, 128, 0);
    HAL_Delay(500);
    this->rgb_led_on(128, 0, 128);
    HAL_Delay(500);
    this->rgb_led_on(0, 128, 128);
    HAL_Delay(500);

    this->rgb_led_on(128, 128, 128);
    HAL_Delay(500);
    this->rgb_led_on(255, 128, 128);
    HAL_Delay(500);
    this->rgb_led_on(128, 255, 128);
    HAL_Delay(500);
    this->rgb_led_on(128, 128, 255);

    HAL_Delay(500);
    this->rgb_led_on(128, 255, 255);
    HAL_Delay(500);
    this->rgb_led_on(255, 128, 255);
    HAL_Delay(500);
    this->rgb_led_on(255, 255, 128);
    HAL_Delay(500);

    this->rgb_led_on(255, 255, 255);

    HAL_Delay(500);

    this->rgb_led_on(0xFF0000);
    HAL_Delay(500);
    this->rgb_led_on(0x00FF00);
    HAL_Delay(500);
    this->rgb_led_on(0x0000FF);
    HAL_Delay(500);
    // this->rgb_led_on(0xFF00FF);
    // HAL_Delay(500);
    this->rgb_led_on(0xFFFFFF);
    HAL_Delay(800);

}

void Super_RgbLed_WS2811::self_test_keep_changing_dim() {

        this->rgb_led_on(0x888888);
        uint8_t dim = 180;
        while (1) {

            dim--;

            printf("dim %u\n\r", dim);
            this->rgb_led_dim_all(dim);
            HAL_Delay(30);
            if (dim==0)
            {
                break;
            }
        }

}

void Super_RgbLed_WS2811::self_test_keep_sending_white_FFFFFF() {
    while (1) {
        this->rgb_led_on(0xFFFFFF);
    }

}

void Super_RgbLed_WS2811::self_test_keep_sending_OFF_000000() {
    while (1) {
        this->rgb_led_on(0);
    }

}
void Super_RgbLed_WS2811::self_test_3_different_color() {

    set_color_one_led(0, 240,140,140);
    set_color_one_led(1, 20,188,20);
    set_color_one_led(2, 20,20,120);
    send_rgb_data_to_led();
/*
    HAL_Delay(600);

    for (int i=0;i<255;i++)
    {// set 1st LED
    set_color_one_led(i%this->_led_count, 10*i, 20*i, 40*i);

    // set 3rd LED


    // send signal to ws2811 IC to make rgb turn on the right color
    send_rgb_data_to_led();
    HAL_Delay(50);
    }
*/
}

void Super_RgbLed_WS2811::self_test_all()
{
     this->rgb_led_on(128, 128, 128);

      HAL_Delay(300);
      this->rgb_led_off( );

      //this->self_test_keep_sending_0_code();
      //HAL_Delay(800);
      //this->self_test_keep_sending_1_code();
      HAL_Delay(800);
      this->self_test_chang_color();

      this->self_test_keep_changing_dim();

      this->self_test_3_different_color();
      HAL_Delay(800);



}
#endif

// 1000us = 1ms
void delay_us(uint32_t time) {
    uint32_t i = 5 * time;
    while (i--) {
        __NOP();
    }
}

inline void Super_RgbLed_WS2811::send_0_code_to_ic() {
    //  T0H 0 code, high voltage time 220ns~380ns
    //  T0L 0 code, low voltage time 580ns~1us
    // cannot use HAL_GPIO, too slow.

    this->_gpio_port->BSRR = this->_gpio_pin; // Set the Pin PA8
    // test 0  no delay at all, works,   from rise to starts to drop 110nanos. drop to 70% of low, 200nanos.

    // test 1 from rise to starts to drop: 220ns,   drop to 70% of low 270ns
    // delay_100nano();    // works

    // test 2 from 70% high to starts to drop: 200ns,   drop to 70% of low 260ns
    delay_200nanosec();  // works
    delay_20nanosec();
    // test 3 from 70% high to starts to drop: 300ns.
    // delay_300nano();  // works but maybe too long.  ( < 380ns )

    //GPIOA->BSRR = (uint32_t) RGB_DATA_Pin << 16u;
    this->_gpio_port->BSRR = this->_pin_unset;
    // due to other overhead, this delay need to be shorter.
    // without any delay, low is 420nanosec, so we need about 200-400nanosec more

    delay_300nanosec();
    delay_60nanosec();



}

inline void Super_RgbLed_WS2811::send_1_code_to_ic() {
    //uint16_t time_T1H_100ns =1;  // T1H 1 code, high voltage time 580ns~1us
    //uint16_t time_T1L_100ns = 2;  // T1L 1 code, low voltage time 580ns~1us

    HAL_GPIO_WritePin(this->_gpio_port, this->_gpio_pin, GPIO_PIN_SET);
    // delay 300nano works too.
    delay_500nanosec();  // 700 nano sec

    //this->_gpio_port->BSRR = (uint32_t) this->_gpio_pin << 16u;
    this->_gpio_port->BSRR  =  this->_pin_unset;
    // due to other overhead, this delay need to be shorter.
    // without any delay ,  low time is about 800 nanosec
    delay_40nanosec(); //  low time is about 840 nanosec

    //delay_100nanosec();   // lowtime is about 900nanosec
    //delay_500nanosec();  //  works too, but too long. low time is 1200nanosec
    //delay_500nanosec(); // even delay 1000 nanos works.

}

void Super_RgbLed_WS2811::send_reset_code_to_ic() {
    uint16_t RESET_DELAY_US = 400;
    HAL_GPIO_WritePin(this->_gpio_port, this->_gpio_pin, GPIO_PIN_RESET);
    delay_us(RESET_DELAY_US);

}

// for R, or G, or B, for each LED need to call this function 3 times.
inline void Super_RgbLed_WS2811::send_one_color_to_ic(uint8_t rgb) {

    for (uint8_t i = 0; i < BIT8; i++) {

        if (rgb & 0b10000000 >> i) {
            // send 1 code
            this->send_1_code_to_ic();

        } else {
            // send 0 code
            this->send_0_code_to_ic();

        }

    }
}

void Super_RgbLed_WS2811::set_color_all(uint8_t r, uint8_t g, uint8_t b) {
    for (uint16_t i = 0; i < this->_led_count; i++) {
        this->_all_rgb_led_data_p[i].base_r = r;
        this->_all_rgb_led_data_p[i].base_g = g;
        this->_all_rgb_led_data_p[i].base_b = b;
        this->_all_rgb_led_data_p[i].dim_r = r;
        this->_all_rgb_led_data_p[i].dim_g = g;
        this->_all_rgb_led_data_p[i].dim_b = b;
    }
}

void Super_RgbLed_WS2811::set_color_all(uint32_t rgb) {
    uint8_t r = rgb / 0x10000 & 0x0000FF;
    uint8_t g = rgb / 0x100 & 0x0000FF;
    uint8_t b = rgb & 0x0000FF;
    this->set_color_all(r, g, b);
}

void Super_RgbLed_WS2811::set_color_one_led(uint16_t index, uint8_t r, uint8_t g, uint8_t b) {
    if (index >= _led_count) {
        return;
    }
    One_Led_Data *one_rgb_led = &this->_all_rgb_led_data_p[index];
    one_rgb_led->base_r = r;
    one_rgb_led->dim_r = r;
    one_rgb_led->base_g = g;
    one_rgb_led->dim_g = g;
    one_rgb_led->base_b = b;
    one_rgb_led->dim_b = b;
}

uint8_t float_to_uint8(float f) {
    if (f > 255) {
        f = 255;
    }
    return f;
}

void Super_RgbLed_WS2811::rgb_led_dim_all(uint8_t dim) {
    // if dim is 150, can set it brighter, not an error.

    if (dim >= 150) {
        dim = 200;
    }
    for (uint16_t i = 0; i < this->_led_count; i++) {
        One_Led_Data *one_led = &this->_all_rgb_led_data_p[i];

        float dim_f = float(dim) / 100.0;
        // don't change the base color
        float r = one_led->base_r * dim_f;
        one_led->dim_r = float_to_uint8(r);

        float g = one_led->base_g * dim_f;
        one_led->dim_g = float_to_uint8(g);

        float b = one_led->base_b * dim_f;
        one_led->dim_b = float_to_uint8(b);
    }
    this->send_rgb_data_to_led();

}

void Super_RgbLed_WS2811::send_rgb_data_to_led() {
    // note , do not add print or other stuff here, can mess up the time.
    // loop for each and every RGB led data
    for (uint16_t i = 0; i < this->_led_count; i++) {

        One_Led_Data *one_led = &this->_all_rgb_led_data_p[i];

        this->send_one_color_to_ic(one_led->dim_r);
        this->send_one_color_to_ic(one_led->dim_g);
        this->send_one_color_to_ic(one_led->dim_b);
    }
    // after send data for all LEDs, we send the reset code
    this->send_reset_code_to_ic();

}

void Super_RgbLed_WS2811::rgb_led_on(uint8_t r, uint8_t g, uint8_t b) {
    this->set_color_all(r, g, b);
    this->send_rgb_data_to_led();

}

void Super_RgbLed_WS2811::rgb_led_on(uint32_t color) {
    this->set_color_all(color);
    this->send_rgb_data_to_led();

}

void Super_RgbLed_WS2811::rgb_led_on( ) {
    this->rgb_led_dim_all(100);

}

void Super_RgbLed_WS2811::rgb_led_off() {

    this->rgb_led_dim_all(0);

}
