// to support u82g, LCD12864 ST7567 with STM32F103 (blue pill) and STM32F401 (Blackpill)

// LCD_RESET  may not be needed to connect to MCU, but it's very useful when debugging.
// DC - Data Command Pin  is needed
// CS - Chip Select can be kept low, not need to connect to MCU pin.



// Pin Config --> RCC --> High Speed Clock (HSE) : Crystal/Ceremic Resonator

// SPI setting

// SPI:  transmit only master, NSS disable
// SPI prescaler 64  ( when bluepill is running 72MHZ  // 8X9)
// SPI speed
// SPI prescaler 32, speed 1.125MBit/s  OK   36FPS
// SPI prescaler 16, speed 2.25MBit/s  OK   42FPS
// SPI prescaler 8, speed 4.5MBit/s  OK   45FPS


// Clock Polarity , CPOL = LOW
// CPSA : 1 edge
// if choose incorrectly, LCD won't work when connect probe (multimeter or oscilloscope) to any NSS/CLK/Data pins


#include <main.h>


#ifndef LCD_DC_GPIO_Port
#error "please define user lable LCD_DC , LCD_RESET , for gpio pin"
#endif

#ifndef LCD_DC_Pin
#error "please define user lable LCD_DC , LCD_RESET for gpio pin"
#endif








#include <U8g2lib.h>  //needed for CPP style u8g2

// must define LCD_DC and LCD_RESET as user label
// LCD_DC_Port   LCD_DC_Pin  LCD_SPI





uint8_t u8x8_byte_4wire_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

uint8_t u8x8_stm32_gpio_and_delay_cb(U8X8_UNUSED u8x8_t *u8x8,
U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int,
U8X8_UNUSED void *arg_ptr);

class U8G2_ST7567_128X64_F_STM32_HW_SPI: public U8G2 {
private:
    // how full is our buffer for DMA
    //   __IO uint16_t _dma_send_buffer_counter = 0;


public:

    // constructor
    U8G2_ST7567_128X64_F_STM32_HW_SPI(const u8g2_cb_t *rotation  , SPI_HandleTypeDef *lcd_spi );

    SPI_HandleTypeDef * get_spi();


    void draw_test_screen( uint8_t fps  ) ;
    void  setup_u8g2_stm32(   ) ;

};
    /*


    inline void send_buffer_to_stm32_spi_dma() {
        this->_dma_start_ts = HAL_GetTick();

        HAL_SPI_Transmit_DMA(&LCD_SPI, this->_dma_send_buffer, this->_dma_send_buffer_counter);

    }
    // time counter to check dma speed
    uint32_t _dma_start_ts;
    // buffer to store data and send to dma in batch
    uint8_t _dma_send_buffer[DMA_12864_BUFFER_SIZE_MAX];
*/
    /***
     *  dma transfer finish callback for STM32
     */
     //inline void stm32_spi_txCpltCallback(SPI_HandleTypeDef *hspi);

    /**
     *  check_buffer_has_enough_free_space
     *  true: has enough
     *  false: not enough
     * */
    /*
    inline bool check_buffer_has_enough_free_space(uint8_t new_data_size) {
        return (_dma_send_buffer_counter + new_data_size) <= DMA_12864_BUFFER_SIZE_MAX;
    }

    inline void store_data_to_buffer(void *data_p, uint16_t len);
*/



/*
 *
 *
class U8G2_ST7567_JLX12864_F_4W_HW_SPI : public U8G2 {
  public: U8G2_ST7567_JLX12864_F_4W_HW_SPI(const u8g2_cb_t *rotation, uint8_t cs, uint8_t dc, uint8_t reset = U8X8_PIN_NONE) : U8G2() {
    u8g2_Setup_st7567_jlx12864_f(&u8g2, rotation, u8x8_byte_arduino_hw_spi, u8x8_gpio_and_delay_arduino);
    u8x8_SetPin_4Wire_HW_SPI(getU8x8(), cs, dc, reset);
  }
};

 * class U8G2_ST7567_ENH_DG128064_F_3W_HW_SPI : public U8G2 {
  public: U8G2_ST7567_ENH_DG128064_F_3W_HW_SPI(const u8g2_cb_t *rotation, uint8_t cs, uint8_t reset = U8X8_PIN_NONE) : U8G2() {
    u8g2_Setup_st7567_enh_dg128064_f(&u8g2, rotation, u8x8_byte_arduino_3wire_hw_spi, u8x8_gpio_and_delay_arduino);
    u8x8_SetPin_3Wire_HW_SPI(getU8x8(), cs, reset);
    */
