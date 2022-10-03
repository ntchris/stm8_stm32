// to support u82g, LCD12864 ST7920 with STM32F103 (blue pill) and STM32F401 (Blackpill)

// LCD_RESET  and CS  are not needed
// CS keep high.   RESET can use resistor and a cap to make it low for a while and keep high
//#define LCD_SPI hspi2
// Pin Config --> RCC --> High Speed Clock (HSE) : Crystal/Ceremic Resonator

// SPI setting for stm32 and LCD12864 - ST7920 to work
// SPI:  transmit only master, NSS disable
// SPI prescaler 64  ( when bluepill is running 72MHZ )
// 8X9
// baudrate =562KB is ok  FPS=17 without DMA
// CPSA : 2 edge (important)
// Clock Polarity , CPOL = HIGH or LOW both work

// SPI prescaler 32 when bluepill is running 64MHZ
// 8X8
// baudrate=1MB, is OK   FPS=22

// SPI prescaler 32 when bluepill is running 72MHZ
// 8X9
// baudrate=1.125MB, is NOT OK

#include <main.h>
#include <U8g2lib.h>  //needed for CPP style u8g2
extern SPI_HandleTypeDef hspi2;

#define LCD_SPI hspi2

// comment out to disable print.   define it to enable, to the print to usart to stlink to PC (check using putty COM##)
// #define DEBUG_PRINT

uint8_t u8x8_byte_3wire_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

uint8_t u8x8_stm32_gpio_and_delay_cb(U8X8_UNUSED u8x8_t *u8x8,
U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int,
U8X8_UNUSED void *arg_ptr);

class U8G2_ST7920_128X64_F_STM32_HW_SPI: public U8G2 {
private:
    // how full is our buffer for DMA
    __IO uint16_t _dma_send_buffer_counter = 0;

    // usually a full screen of data has about 2544 bytes for st7920, it must larger than 2544, otherwise DMA is uselss.
    // because if the buffer is full, the system has to wait for it to free up,
    // it's wasting time, so it defeat the whole point of using DMA.
    const static uint16_t DMA_12864_BUFFER_SIZE_MAX = 2570;

public:
    inline void send_buffer_to_stm32_spi_dma() {
        this->_dma_start_ts = HAL_GetTick();

        HAL_SPI_Transmit_DMA(&LCD_SPI, this->_dma_send_buffer, this->_dma_send_buffer_counter);

    }
    // time counter to check dma speed
    uint32_t _dma_start_ts;
    // buffer to store data and send to dma in batch
    uint8_t _dma_send_buffer[DMA_12864_BUFFER_SIZE_MAX];

    /***
     *  dma transfer finish callback for STM32
     */
     inline void stm32_spi_txCpltCallback(SPI_HandleTypeDef *hspi);

    /**
     *  check_buffer_has_enough_free_space
     *  true: has enough
     *  false: not enough
     * */
    inline bool check_buffer_has_enough_free_space(uint8_t new_data_size) {
        return (_dma_send_buffer_counter + new_data_size) <= DMA_12864_BUFFER_SIZE_MAX;
    }

    inline void store_data_to_buffer(void *data_p, uint16_t len);

    // constructor
    U8G2_ST7920_128X64_F_STM32_HW_SPI(const u8g2_cb_t *rotation = U8G2_R0) :
            U8G2() {

        u8g2_Setup_st7920_s_128x64_f(&u8g2, rotation, u8x8_byte_3wire_hw_spi, u8x8_stm32_gpio_and_delay_cb);

    }
};

