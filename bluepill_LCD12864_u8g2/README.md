
A few key items list:

dev board: stm32F103C8T6 blue pill
API u8g2, LCD communication: hardware SPI
LCD: LCD12864 ST7920  , a regular one, 5V version.
It works with STM32 bluepill device (which is 3.3V )

HW connection:
To make it less dangerous I lower the 5V supply to LCD be 4.8V and also use a diode to further lower the power to LCD by 0.7V.
so the 3.3V SPI signal should be better for a 4.2V powered LCD.


Always disconnect LCD power first, then disconnect stm32 later. because
AN4899
Application note
STM32 microcontroller GPIO hardware settings
and low-power consumption
page 19



Using STM32CUBEIDE, not Arduino IDE.
C++ style of u8g2

LCD Pin connection:
CS (Chip Select) : can be always pulled HIGH for LCD12864 ST7920. so MCU doesn't deal with it.
DC : not available, ignore
SPI - in the example using SPI2 - SCK PB13 and MOSI PB15 only.
RESET , optional, low for a bit to reset, then pull high,  or connect to MCU, so MCU can reset it everytime it reboot.



Path setting:
need to add C++ include and source path for u8g2. ( not included )
see attached IDE setting picture


Key step to make it work:


class U8G2_ST7920_128X64_F_STM32_HW_SPI: public U8G2 {
    private :

public:

    U8G2_ST7920_128X64_F_STM32_HW_SPI(const u8g2_cb_t *rotation = U8G2_R0) : U8G2() {

        u8g2_Setup_st7920_s_128x64_f(&u8g2, rotation, u8x8_byte_3wire_hw_spi, u8x8_stm32_gpio_and_delay_cb);

    }
};

uint8_t u8x8_byte_3wire_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    switch (msg) {
    case U8X8_MSG_BYTE_SEND:
        HAL_SPI_Transmit(&LCD_SPI, (uint8_t*) arg_ptr, arg_int, 10000);
		

about speed:
without DMA, just using HAL_SPI_Transmit()
each call takes about 43ms,  screen refresh rate is about 16-17FPS.
Bluepill is working on 72MHZ ,  SPI speed setting is about 562K (bottle neck)
// SPI prescaler 64  ( when bluepill is running 72MHZ )
// 8X9
// baudrate =562KB is ok  FPS=17

If lower bluepill speed a little, SPI prescaler can be 32, so SPI speed can be higher, so FPS can be about 22


		