// to support u82g, LCD12864 ST7920 with STM32F103 (blue pill) and STM32F401 (Blackpill)


#include <stm32_u8g2_lcd12864.h>
#include <stdint.h>
#include <u8g2.h>
#include <main.h>
#include <string>
#include <string.h>


U8G2_ST7920_128X64_F_STM32_HW_SPI u8g2;

// needed by u8x8_cad_st7920_spi
void u8g_hw_port_delay_ns(uint16_t ns) {
    __NOP();
}


uint8_t u8x8_stm32_gpio_and_delay_cb(U8X8_UNUSED u8x8_t *u8x8,
U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int,
U8X8_UNUSED void *arg_ptr) {
    switch (msg) {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
        break;
    case U8X8_MSG_DELAY_NANO:
        // needed by u8x8_cad_st7920_spi
        u8g_hw_port_delay_ns(arg_int);
        break;
    case U8X8_MSG_DELAY_10MICRO:
        break;
    case U8X8_MSG_DELAY_100NANO:
        break;
    case U8X8_MSG_DELAY_MILLI:
        // HAL_Delay(arg_int);
        break;
    case U8X8_MSG_GPIO_DC:
        // no need for LCD12864 ST7920, no DC pin.
        break;
    case U8X8_MSG_GPIO_CS1: // Set the spi chip select line level. always high.
        break;
    case U8X8_MSG_GPIO_RESET:
        // no need
        break;
    }
    return 1;
}

inline void U8G2_ST7920_128X64_F_STM32_HW_SPI::stm32_spi_txCpltCallback(SPI_HandleTypeDef *hspi) {

#ifdef DEBUG_PRINT
    uint32_t dma_end_ts = HAL_GetTick();

    unsigned int time_dma_used_ms = dma_end_ts - this->_dma_start_ts;

    printf("dma sent %u bytes, used  %ums\n\r", this->_dma_send_buffer_counter, time_dma_used_ms);
#endif

    this->_dma_send_buffer_counter = 0;


}

// don't add inline for this
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {

    u8g2.stm32_spi_txCpltCallback(hspi);

}

inline void U8G2_ST7920_128X64_F_STM32_HW_SPI::store_data_to_buffer(void *data_p, uint16_t len) {
    memcpy(&(this->_dma_send_buffer[this->_dma_send_buffer_counter]), data_p, len);
    this->_dma_send_buffer_counter += len;
}

inline void cache_spi_data_to_buffer_for_dma_u8g2_callback(void *arg_ptr, uint8_t arg_int) {

    // when DMA is still sending the buffer, don't modify the buffer with new data.
    while (HAL_DMA_GetState( LCD_SPI.hdmatx) != HAL_DMA_STATE_READY) {
#ifdef DEBUG_PRINT
        printf("DMA working on buffer, wait\n\r");
#endif
        __NOP();
    }

    // when buffer is full, we cannot store more,
    // DMA is not working now ( see above code, we are here only because DMA is not working),
    // we have to send everything we have now.
    if ( !u8g2.check_buffer_has_enough_free_space(arg_int)) {
#ifdef DEBUG_PRINT
        printf("buffer full!! waiting...\r\n");
#endif
           u8g2.send_buffer_to_stm32_spi_dma();
           while(!u8g2.check_buffer_has_enough_free_space(arg_int))
           {
                 // we just sent it using DMA, now wait for it to finish and free up the buffer room
               __NOP();
           }
    }
    // buffer is not full and has enough space to store new data
    u8g2.store_data_to_buffer(arg_ptr, arg_int);
}

inline uint8_t u8x8_byte_3wire_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    switch (msg) {
    case U8X8_MSG_BYTE_SEND:
        cache_spi_data_to_buffer_for_dma_u8g2_callback(arg_ptr, arg_int);
        break;
    case U8X8_MSG_BYTE_INIT:
        break;

    case U8X8_MSG_BYTE_START_TRANSFER:
        // no need, always pull high

        break;
    case U8X8_MSG_BYTE_END_TRANSFER:
         // add this to the right place in u8g2 libs
        //         u8x8_byte_EndTransfer(u8x8);
        // perfect place for DMA to start transfer the huge amount of data we cached.
        u8g2.send_buffer_to_stm32_spi_dma();

        break;
    default:
        return 0;
    }
    return 1;
}

void reset_lcd() {
    HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_RESET);
    HAL_Delay(30);
    HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_SET);
    HAL_Delay(100);

}
void setup_u8g2_stm32() {
    // c style def   u8g2_Setup_st7920_s_128x64_f(&u8g2, U8G2_R0, u8x8_byte_3wire_hw_spi, u8x8_stm32_gpio_and_delay_cb);
    reset_lcd();
    u8g2.initDisplay();
    u8g2.setPowerSave(0);
    u8g2.clearBuffer();
}

void draw_test_screen(uint16_t fps = 0) {

    /* c style draw
     u8g2_ClearBuffer (&u8g2);
     u8g2_SetFont(&u8g2, u8g2_font_ncenB08_tr);
     u8g2_DrawStr(&u8g2, 0, 12, "LCD12864 ST7920");
     u8g2_SetFont(&u8g2, u8g2_font_ncenB10_tr);
     u8g2_DrawStr(&u8g2, 1, 30, "STM32F103C8T6");
     u8g2_DrawCircle(&u8g2, 50, 35, 25, U8G2_DRAW_ALL);
     u8g2_SetFont(&u8g2, u8g2_font_ncenB08_tr);
     std::string str = std::to_string(fps);
     u8g2_DrawStr(&u8g2, 110, 60, str.c_str());

     u8g2_SendBuffer(&u8g2);
     // end of c style draw
     */

    u8g2.clearBuffer();
    // u8g2.drawBox(5, 5, 118, 56);
    /*
     uint32_t start_ts = HAL_GetTick();
     u8g2.sendBuffer();
     uint32_t end_ts = HAL_GetTick();
     uint delta_msec = end_ts - start_ts;
     printf("%u\n\r", delta_msec);
     */

    //u8g2.setDrawColor(1);
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(0, 12, "LCD12864 ST7920 u8g2");
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(1, 30, "STM32F103C8T6");
    u8g2.drawCircle(50, 35, 25, U8G2_DRAW_ALL);
    static uint16_t counter = 0;
    u8g2.drawStr(1, 51, std::to_string(counter).c_str());
    counter++;

    if (fps) {
        u8g2.setFont(u8g2_font_ncenB08_tr);
        std::string str = std::to_string(fps);
        u8g2.drawStr(110, 60, str.c_str());
    }

    uint32_t start_ts = HAL_GetTick();
    u8g2.sendBuffer();
    uint32_t end_ts = HAL_GetTick();
    unsigned int delta_msec = end_ts - start_ts;
    printf("sendbuffer used %u ms\r\n", delta_msec);
    // not using dma fps 16
    // no delay    fps 23
    //HAL_Delay(10);  // same fps 23 , the same as no DMA, meaning DMS helps to improve the performace, saved 10ms for system to work on other stuffs.
     HAL_Delay(20);  // fps 22 !
    //HAL_Delay(30);   //  fps 18
    // HAL_Delay(40);   // fps 15

}

