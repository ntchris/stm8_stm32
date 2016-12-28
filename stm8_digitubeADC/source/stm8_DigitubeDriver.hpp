/**
 *  Work with Common Cathode 7 Seg digitube
 *
 *  Stm8 Pin configuration
 *  Seven Segment DigiTube Pin# and Def    STM8 PIN
 *  -----------------------------------------------
 *    11   Seg A                             PD5
 *    7    Seg B                             PC7
 *    4    Seg C                             PC3
 *    2    Seg D                             PA2
 *    1    Seg E                             PA3
 *    10   Seg F                             PD6
 *    5    Seg G                             PC4
 *    3    Seg Dp (dot)                      PA1
 *    12   Digit1  (Common Cathode)          PD4    (Low = ON , High = OFF )
 *    9    Digit2  (Common Cathode)          PB4    (Low = ON , High = OFF )
 *    8    Digit3  (Common Cathode)          PB5    (Low = ON , High = OFF )
 *    6    Digit4  (Common Cathode)          PD2    (Low = ON , High = OFF )
 *
 *
 */
#include "stm8s.h"

typedef enum
{
   GPIO_PIN_0 = ((uint8_t) 0x01), /*!< Pin 0 selected */
   GPIO_PIN_1 = ((uint8_t) 0x02), /*!< Pin 1 selected */
   GPIO_PIN_2 = ((uint8_t) 0x04), /*!< Pin 2 selected */
   GPIO_PIN_3 = ((uint8_t) 0x08), /*!< Pin 3 selected */
   GPIO_PIN_4 = ((uint8_t) 0x10), /*!< Pin 4 selected */
   GPIO_PIN_5 = ((uint8_t) 0x20), /*!< Pin 5 selected */
   GPIO_PIN_6 = ((uint8_t) 0x40), /*!< Pin 6 selected */
   GPIO_PIN_7 = ((uint8_t) 0x80), /*!< Pin 7 selected */
   GPIO_PIN_LNIB = ((uint8_t) 0x0F), /*!< Low nibble pins selected */
   GPIO_PIN_HNIB = ((uint8_t) 0xF0), /*!< High nibble pins selected */
   GPIO_PIN_ALL = ((uint8_t) 0xFF) /*!< All pins selected */
} GPIO_Pin_TypeDef;

#define Segment_A_Port  GPIOD
#define Segment_A_Pin   GPIO_PIN_5

#define Segment_B_Port  GPIOC
#define Segment_B_Pin   GPIO_PIN_7

#define Segment_C_Port  GPIOC
#define Segment_C_Pin   GPIO_PIN_3

#define Segment_D_Port  GPIOA
#define Segment_D_Pin   GPIO_PIN_2

#define Segment_E_Port  GPIOA
#define Segment_E_Pin   GPIO_PIN_3

#define Segment_F_Port  GPIOD
#define Segment_F_Pin   GPIO_PIN_6

#define Segment_G_Port  GPIOC
#define Segment_G_Pin   GPIO_PIN_4

#define Segment_Dp_Port  GPIOA
#define Segment_Dp_Pin   GPIO_PIN_1

#define Segment_Digit1_Port  GPIOD
#define Segment_Digit1_Pin   GPIO_PIN_4

#define Segment_Digit2_Port  GPIOB
#define Segment_Digit2_Pin   GPIO_PIN_4

#define Segment_Digit3_Port  GPIOB
#define Segment_Digit3_Pin   GPIO_PIN_5

#define Segment_Digit4_Port  GPIOD
#define Segment_Digit4_Pin   GPIO_PIN_2

typedef enum
{
   GPIO_MODE_IN_FL_NO_IT = (uint8_t) 0x00, /*!< Input floating, no external interrupt */
   GPIO_MODE_IN_PU_NO_IT = (uint8_t) 0x40, /*!< Input pull-up, no external interrupt */
   GPIO_MODE_IN_FL_IT = (uint8_t) 0x20, /*!< Input floating, external interrupt */
   GPIO_MODE_IN_PU_IT = (uint8_t) 0x60, /*!< Input pull-up, external interrupt */
   GPIO_MODE_OUT_OD_LOW_FAST = (uint8_t) 0xA0, /*!< Output open-drain, low level, 10MHz */
   GPIO_MODE_OUT_PP_LOW_FAST = (uint8_t) 0xE0, /*!< Output push-pull, low level, 10MHz */
   GPIO_MODE_OUT_OD_LOW_SLOW = (uint8_t) 0x80, /*!< Output open-drain, low level, 2MHz */
   GPIO_MODE_OUT_PP_LOW_SLOW = (uint8_t) 0xC0, /*!< Output push-pull, low level, 2MHz */
   GPIO_MODE_OUT_OD_HIZ_FAST = (uint8_t) 0xB0, /*!< Output open-drain, high-impedance level,10MHz */
   GPIO_MODE_OUT_PP_HIGH_FAST = (uint8_t) 0xF0, /*!< Output push-pull, high level, 10MHz */
   GPIO_MODE_OUT_OD_HIZ_SLOW = (uint8_t) 0x90, /*!< Output open-drain, high-impedance level, 2MHz */
   GPIO_MODE_OUT_PP_HIGH_SLOW = (uint8_t) 0xD0 /*!< Output push-pull, high level, 2MHz */
} GPIO_Mode_TypeDef;

// Connect a 4 digit digitube
static const uint8_t MAX_DIGIT = 4;
static const uint8_t MAX_Segment = 7;  // A - G  , Dp doesn't count

class STM8_DigitubeDriver
{

   struct Port_Pin
   {
      GPIO_TypeDef *port;
      GPIO_Pin_TypeDef pin;
   };

   static const Port_Pin Segment_A, Segment_B, Segment_C, Segment_D, Segment_E, Segment_F,
         Segment_G, Segment_Dp;
   static const Port_Pin Digit1, Digit2, Digit3, Digit4;

   const static Port_Pin ArrayPortPin[MAX_Segment];
   //={Segment_A, Segment_A, Segment_A, Segment_A, Segment_A, Segment_A, Segment_A    } ;

   static void tim4_Interupt_Init(void);

   static void gpioInitPushPull(GPIO_TypeDef* GPIOx, GPIO_Pin_TypeDef GPIO_Pin);

   static void setDisplay0();
   static void setDisplay1();
   static void setDisplay2();
   static void setDisplay3();
   static void setDisplay4();
   static void setDisplay5();
   static void setDisplay6();
   static void setDisplay7();
   static void setDisplay8();
   static void setDisplay9();

public:

   static void stm8_Pins_For_DigitubeInit(void);

   static void stm8_Gpio_Write_Low(GPIO_TypeDef* GPIOx, GPIO_Pin_TypeDef PortPins);
   static void stm8_Gpio_Write_High(GPIO_TypeDef* GPIOx, GPIO_Pin_TypeDef PortPins);
   //set the 7 segment digit, so current enabled digitN show the digit
   static void setDisplayDigit(uint8_t digit);
};

