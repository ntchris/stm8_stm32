// to support u82g, LCD12864 ST7567 with STM32F103 (blue pill) and STM32F401 (Blackpill)
// using DMA to do SPI data transfer to LCD screen.
// st7567 is pretty fast already ( u8g2 send buffer only takes 8ms or 4ms (depends on SPI speed setting 2MBit or 1MBit)  ,
// so DMA in my opinion is totally optional

// LCD_RESET  may not be needed to connect to MCU, but it's very useful when debugging.
// DC - Data Command Pin  is needed
// CS - Chip Select can be kept low, not need to connect to MCU pin.

// Pin Config --> RCC --> High Speed Clock (HSE) : Crystal/Ceremic Resonator

// SPI setting

// SPI:  transmit only master, NSS disable
// SPI prescaler 64  ( when bluepill is running 72MHZ  // 8X9)
// SPI speed
// SPI prescaler 32, speed 1.125MBit/s  OK   36FPS  , 8ms
// SPI prescaler 16, speed 2.25MBit/s  OK   41FPS   , 4-5ms   <==== choose this
// SPI prescaler 8, speed 4.5MBit/s  OK   45FPS   2ms
// SPI prescaler 4, speed 9MBit/s  OK   47FPS   1-2 ms.
// SPI prescaler 2, speed 18MBit/s  OK   47FPS   1-2 ms.

// CPSA : 2 edge (important)
// Clock Polarity , CPOL = HIGH or LOW both work

// about DMA:
// since ST7567 need DC - Data Command Pin, so using DMA is much more difficult, we cannot blindly transfer data
// to SPI, we must handle the DC pin in the right moment. otherwise the Data/Command will be totally
// messed up.

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





// [ packet1   dc0, len=10, 1234567890 ,   packet2 dc1, len=1, 1,  packet3 dc2, len=5, 12345  ]

// the length data type we put in the buffer. so far I can see it's always <= 128.
// but for other unknown type of larger screen  it's possible 255 can't hold.
// for DC = command, data is always short.
// for DC = data , it's always 128 and follow by a different DC. ( so they don't add up like 128 + 128 etc )
typedef  uint8_t Buffer_Length_Type;
// typedef  uint16_t Buffer_Length_Type;

struct DMA_DATA_PACKET
{
      uint8_t  dc;
      void *p;
      // if we have larger single packet size > 255 (but not likely, u8g2 said something about 255 being the max size),
      // this won't work, must change to uint16_t
      uint8_t len;
};


const static uint16_t BUFFER_SIZE_MAX = 1500;

class DMA_BUFFER {
public:

    uint8_t data_buffer[ BUFFER_SIZE_MAX ];
    __IO uint16_t store_counter;
    __IO uint16_t send_counter;
    __IO bool dma_working;
   uint16_t get_packet_size(DMA_DATA_PACKET *packet_p )
   {
        uint16_t size = sizeof(packet_p->dc) + sizeof(packet_p->len );
        size +=packet_p->len;
        return size;

   }
   // add ok true,  not ok false
  //  bool  add_packet( DMA_DATA_PACKET *);
   bool add_packet_combine(DMA_DATA_PACKET *packet_p) ;

   // constructor
   DMA_BUFFER();

};

class U8G2_ST7567_128X64_F_STM32_HW_SPI_DMA: public U8G2 {
private:

    /**
     *  check_buffer_has_enough_free_space
     *  true: has enough
     *  false: not enough
     * */

    __IO static uint32_t _dma_start_ts;


    // send one packet from the buffer to DMA, one packet only. different packet has different DC - Data Command
    // so they cannot be sent as one buffer.
    static void send_packet_to_dma();
public:

    // constructor
    U8G2_ST7567_128X64_F_STM32_HW_SPI_DMA(const u8g2_cb_t *rotation, SPI_HandleTypeDef *lcd_spi);

    SPI_HandleTypeDef* get_spi();

    void draw_test_screen(uint16_t fps);
    void setup_u8g2_stm32();
    static uint8_t u8x8_byte_4wire_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

   static inline void send_buffer_to_stm32_spi_dc_dma();

    static uint8_t u8x8_stm32_gpio_and_delay_cb(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg,
            U8X8_UNUSED uint8_t arg_int,
            U8X8_UNUSED void *arg_ptr);

    inline static void cache_spi_data_and_dc_to_buffer_for_dma_u8g2_cb(uint8_t dc, void *arg_ptr,
            uint8_t arg_int) ;
    inline static   void  stm32_spi_txCpltCallback(SPI_HandleTypeDef *hspi) ;

    // use our own sendBuffer() so we can control when to send DMA.
    void sendBuffer();
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
