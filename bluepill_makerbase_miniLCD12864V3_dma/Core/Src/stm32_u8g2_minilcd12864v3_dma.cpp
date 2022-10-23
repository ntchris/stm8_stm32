// to support u82g, LCD12864 ST7567 with STM32F103 (blue pill) and STM32F401 (Blackpill)
#include <stdio.h>   // for printf
#include<string.h>
#include<string>
#include <stdint.h>
#include <stm32_u8g2_minilcd12864v3_dma.h>
#include <u8g2.h>
#include <u8x8.h>

SPI_HandleTypeDef *g_lcd_spi;

#define delay_20nanosec()  __NOP();__NOP();
#define delay_40nanosec()  __NOP();__NOP();__NOP();
#define delay_60nanosec()  __NOP();__NOP();__NOP();__NOP();
#define delay_100nanosec()  __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
#define delay_200nanosec()  delay_100nanosec(); delay_100nanosec();
#define delay_300nanosec()  delay_100nanosec(); delay_200nanosec();
#define delay_400nanosec()  delay_200nanosec(); delay_200nanosec();
#define delay_500nanosec()  delay_200nanosec(); delay_300nanosec();

extern void delay_us(uint32_t time);

/*
 void delay_us(uint32_t time) {
 uint32_t i = 8 * time;
 while (i--)
 ;
 }
 */

__IO uint32_t U8G2_ST7567_128X64_F_STM32_HW_SPI_DMA::_dma_start_ts;

//uint8_t U8G2_ST7567_128X64_F_STM32_HW_SPI_DMA::_dma_send_buffer[U8G2_ST7567_128X64_F_STM32_HW_SPI_DMA::DMA_miniLCDV3_12864_BUFFER_SIZE_MAX];

//#define DEBUG_PRINT

DMA_BUFFER dma_buffer;
DMA_BUFFER::DMA_BUFFER() {
    memset(data_buffer, 0, BUFFER_SIZE_MAX);
    store_counter = 0;
    send_counter = 0;
    dma_working = false;
}

// combine same DC packets
bool DMA_BUFFER::add_packet_combine(DMA_DATA_PACKET *packet_p) {
    static uint8_t prev_dc = 99;
    static Buffer_Length_Type *prev_len_p = 0;
    uint16_t new_size = get_packet_size(packet_p);
    if ((store_counter + new_size) >= BUFFER_SIZE_MAX) {
        return false;
    }
    if (packet_p->dc != prev_dc) {
        // new / different DC

        uint8_t *current_p = &data_buffer[store_counter];
        memcpy(current_p, &packet_p->dc, sizeof(packet_p->dc));
        store_counter += sizeof(packet_p->dc);
        // pointing to len
        current_p = &data_buffer[store_counter];
        prev_len_p = (Buffer_Length_Type*) current_p;
        // set the length
        //memcpy(current_p, &packet_p->len, sizeof(packet_p->len));
        *prev_len_p = packet_p->len;
        store_counter += sizeof(Buffer_Length_Type);
        prev_dc = packet_p->dc;

    } else {
        // same dc ,  we combine the data, adjust the previous length only
#ifdef OVER_CAUCIOUS
        // this could slow things down
        if (sizeof(Buffer_Length_Type) == 1) {
            uint16_t temp = packet_p->len + *prev_len_p;

            if (temp > 255) {
                printf("fatal error, buffer len data type is too small, so it's overflowed\n\r");
                return false;
            }
        }
#endif
        *prev_len_p += packet_p->len;

    }
    // add the new data to buffer
    //printf("new len %u\n\r", *prev_len_p);
    uint8_t *current_p = &data_buffer[store_counter];
    memcpy(current_p, packet_p->p, packet_p->len);
    store_counter += packet_p->len;
    return true;
}

/*
 bool DMA_BUFFER::add_packet(DMA_DATA_PACKET *packet_p) {
 uint16_t new_size = get_packet_size(packet_p);
 if ((store_counter + new_size) >= BUFFER_SIZE_MAX) {
 return false;
 }
 uint8_t *current_p = &data_buffer[store_counter];
 memcpy(current_p, &packet_p->dc, sizeof(packet_p->dc));
 store_counter += sizeof(packet_p->dc);
 current_p = &data_buffer[store_counter];
 memcpy(current_p, &packet_p->len, sizeof(packet_p->len));
 store_counter += sizeof(packet_p->len);
 current_p = &data_buffer[store_counter];
 memcpy(current_p, packet_p->p, packet_p->len);
 store_counter += packet_p->len;
 return true;

 }
 */


uint8_t U8G2_ST7567_128X64_F_STM32_HW_SPI_DMA::u8x8_stm32_gpio_and_delay_cb(U8X8_UNUSED u8x8_t *u8x8,
U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int,
U8X8_UNUSED void *arg_ptr) {
    switch (msg) {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
        break;
    case U8X8_MSG_DELAY_NANO:
        for (int i = 0; i < arg_int / 40; i++) {
            delay_40nanosec();
        }

        break;
    case U8X8_MSG_DELAY_10MICRO:
        delay_us(arg_int * 10ul);
        break;
    case U8X8_MSG_DELAY_100NANO:
        delay_100nanosec()
        ;
        break;
    case U8X8_MSG_DELAY_MILLI:
        // needed for LCD RESET
        HAL_Delay(arg_int);
        break;
    case U8X8_MSG_GPIO_DC:
        printf("U8X8_MSG_GPIO_DC\n\r");
        break;
    case U8X8_MSG_GPIO_CS1: // Set the spi chip select line level. always high.
        break;
    case U8X8_MSG_GPIO_RESET:

        HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, (GPIO_PinState) arg_int);

        break;
    }
    return 1;
}

inline void U8G2_ST7567_128X64_F_STM32_HW_SPI_DMA::stm32_spi_txCpltCallback(SPI_HandleTypeDef *hspi) {
    send_packet_to_dma();

}

// don't add inline for this
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {

    U8G2_ST7567_128X64_F_STM32_HW_SPI_DMA::stm32_spi_txCpltCallback(hspi);
}

/*

 */

void U8G2_ST7567_128X64_F_STM32_HW_SPI_DMA::send_packet_to_dma() {
    if (dma_buffer.store_counter == 0) {
        printf("stop, buffer empty\n\r");
        return;
    }
    if (dma_buffer.send_counter >= dma_buffer.store_counter) {
        dma_buffer.send_counter = 0;
        dma_buffer.store_counter = 0;
        dma_buffer.dma_working = false;
#ifdef DEBUG_PRINT
        uint32_t end_ts = HAL_GetTick();

              unsigned int delta_msec = end_ts - _dma_start_ts;
              printf("dma send used %u ms\r\n", delta_msec);
#endif

        return;
    }

    static uint8_t prev_dc = 9;
    uint8_t dc = dma_buffer.data_buffer[dma_buffer.send_counter];
    dma_buffer.send_counter += sizeof(dc);
    Buffer_Length_Type len = dma_buffer.data_buffer[dma_buffer.send_counter];
    dma_buffer.send_counter += sizeof(len);

    if (len == 0) {
        printf("fatal error, len shouldn't be 0\n\r");
    }

    uint8_t *data_p = &dma_buffer.data_buffer[dma_buffer.send_counter];
    if (dc != prev_dc) {
        HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, dc ? GPIO_PIN_SET : GPIO_PIN_RESET);
        prev_dc = dc;
    }
    // delay_40nanosec();
    dma_buffer.send_counter += len;
    HAL_SPI_Transmit_DMA(g_lcd_spi, data_p, len);

}

inline void U8G2_ST7567_128X64_F_STM32_HW_SPI_DMA::send_buffer_to_stm32_spi_dc_dma() {
    while (dma_buffer.dma_working) {
        printf("send_buffer_to_stm32_spi_dc_dma() waiting dma\n\r");

    }

    while (HAL_SPI_STATE_READY != HAL_SPI_GetState(g_lcd_spi)) {
        printf("spi still busy\n\r");
    }
    _dma_start_ts = HAL_GetTick();

    dma_buffer.dma_working = true;
    send_packet_to_dma();

}

inline void U8G2_ST7567_128X64_F_STM32_HW_SPI_DMA::cache_spi_data_and_dc_to_buffer_for_dma_u8g2_cb(uint8_t dc,
        void *arg_ptr, uint8_t arg_int) {

    DMA_DATA_PACKET p;
    p.dc = dc;
    p.p = arg_ptr;
    p.len = arg_int;

    while (dma_buffer.dma_working) {
        printf("cache_spi_data_and_dc_to_buffer_for_dma_u8g2_cb() waiting dma\n\r");
    };

    bool add_ok = dma_buffer.add_packet_combine(&p);
    if (add_ok) {
#ifdef DEBUG_PRINT
       // printf("add dc %u, size %u ok, counter now %u\n\r", dc, p.len, dma_buffer.store_counter);
#endif
        return;
    } else {
        printf("fatal error, buffer full!! has %u, need add %u\n\r", dma_buffer.store_counter, p.len);
        while(1);
    }
}


// buffer is not full and has enough space to store new data
//store_data_to_buffer(arg_ptr, arg_int);
uint8_t U8G2_ST7567_128X64_F_STM32_HW_SPI_DMA::u8x8_byte_4wire_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int,
        void *arg_ptr) {
    static uint8_t dc = 0;
    switch (msg) {
    case U8X8_MSG_BYTE_SEND:

        //start_ts = HAL_GetTick();

        cache_spi_data_and_dc_to_buffer_for_dma_u8g2_cb(dc, arg_ptr, arg_int);
        //end_ts = HAL_GetTick();
        //   printf("store used %ums for %u bytes \n\r", end_ts - start_ts, arg_int );

        //HAL_SPI_Transmit_DMA(g_lcd_spi, lcd_dma_buffer._dma_buffer, lcd_dma_buffer._counter);
        //HAL_SPI_Transmit_DMA(g_lcd_spi, (uint8_t*)arg_ptr, arg_int);
        break;
    case U8X8_MSG_BYTE_INIT:
        break;
    case U8X8_MSG_BYTE_SET_DC:
        // remember the dc, use it to call cache_spi_data function.
        dc = arg_int;

        break;
    case U8X8_MSG_BYTE_START_TRANSFER:
#ifdef LCD_CS
         HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
          u8x8->gpio_and_delay_cb(u8x8, U8X8_MSG_DELAY_NANO, u8x8->display_info->post_chip_enable_wait_ns, NULL);
#endif
        break;
    case U8X8_MSG_BYTE_END_TRANSFER:
        //   send_buffer_to_stm32_spi_dc_dma();
#ifdef LCD_CS
       HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, (GPIO_PinState) (u8x8->display_info->chip_disable_level));
       u8x8->gpio_and_delay_cb(u8x8, U8X8_MSG_DELAY_NANO, u8x8->display_info->post_chip_disable_wait_ns, NULL);
#endif

        break;
    default:
        return 0;
    }
    return 1;
}

void U8G2_ST7567_128X64_F_STM32_HW_SPI_DMA::setup_u8g2_stm32() {
// c style def   u8g2_Setup_st7920_s_128x64_f(&u8g2, U8G2_R0, u8x8_byte_3wire_hw_spi, u8x8_stm32_gpio_and_delay_cb);
// reset_lcd();
    this->initDisplay();
    this->setContrast(150);
    this->setPowerSave(0);
//my_u8g2.setContrast( 255);
    this->clearBuffer();
}

// constructor
U8G2_ST7567_128X64_F_STM32_HW_SPI_DMA::U8G2_ST7567_128X64_F_STM32_HW_SPI_DMA(const u8g2_cb_t *rotation,
        SPI_HandleTypeDef *lcd_spi) {
    g_lcd_spi = lcd_spi;
    u8g2_Setup_st7567_jlx12864_f(&u8g2, U8G2_R2, u8x8_byte_4wire_hw_spi, u8x8_stm32_gpio_and_delay_cb); // contrast =168 seems best

// store using cmd buffer first.

// set data buffer as sending buffer first (0 size, nothing to send anyway)

}

SPI_HandleTypeDef* U8G2_ST7567_128X64_F_STM32_HW_SPI_DMA::get_spi() {
    return g_lcd_spi;
}

void U8G2_ST7567_128X64_F_STM32_HW_SPI_DMA::sendBuffer() {

    _dma_start_ts = HAL_GetTick();

    U8G2::sendBuffer();

    this->send_buffer_to_stm32_spi_dc_dma();

}

void U8G2_ST7567_128X64_F_STM32_HW_SPI_DMA::draw_test_screen(uint16_t fps = 0) {

    this->clearBuffer();
    this->setFont(u8g2_font_ncenB08_tr);
    this->drawStr(0, 12, "LCD12864 ST7567 u8g2");
    this->setFont(u8g2_font_ncenB08_tr);
    this->drawStr(1, 30, "STM32F103C8T6");
    this->drawCircle(50, 35, 25, U8G2_DRAW_ALL);
    static uint16_t counter = 0;
    this->drawStr(1, 51, std::to_string(counter).c_str());
    counter++;

    if (fps) {
        this->setFont(u8g2_font_ncenB08_tr);
        std::string str = std::to_string(fps);
        this->drawStr(110, 60, str.c_str());
    }

    this->sendBuffer();

}
