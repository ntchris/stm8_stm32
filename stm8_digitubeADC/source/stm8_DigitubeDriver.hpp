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
#include <string.h>

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

#define ADC_Port  GPIOD
#define ADC_Pin   GPIO_PIN_3


#define AdcChannel (uint8_t)0x04

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

typedef enum
{
   TIM4_PRESCALER_1 = ((uint8_t) 0x00),
   TIM4_PRESCALER_2 = ((uint8_t) 0x01),
   TIM4_PRESCALER_4 = ((uint8_t) 0x02),
   TIM4_PRESCALER_8 = ((uint8_t) 0x03),
   TIM4_PRESCALER_16 = ((uint8_t) 0x04),
   TIM4_PRESCALER_32 = ((uint8_t) 0x05),
   TIM4_PRESCALER_64 = ((uint8_t) 0x06),
   TIM4_PRESCALER_128 = ((uint8_t) 0x07)
} TIM4_Prescaler_TypeDef;

// Connect a 4 digit digitube
static const uint8_t MAX_DIGIT_COUNT = 4;
static const uint8_t MAX_Segment = 7;  // A - G  , Dp doesn't count

// stm8 and 4 digits 7 segments digitube so the max number it can display is 9999
static const int MAX_NUMBER_TO_DISPLAY = 9999;

class STM8_DigitubeDriver
{

   struct Port_Pin
   {
      GPIO_TypeDef *port;
      GPIO_Pin_TypeDef pin;
   };

   /*static const Port_Pin Segment_A, Segment_B, Segment_C, Segment_D, Segment_E, Segment_F,
    Segment_G, Segment_Dp;
    static const Port_Pin ArrayPortPin[MAX_Segment];
    */
   static const Port_Pin Digit1, Digit2, Digit3, Digit4;

   static const Port_Pin ArrayDigitPortPin[MAX_DIGIT_COUNT];
   static uint8_t currentDigitIndex;

   // if it's 1234 then display 1234
   static char displayBuffer[MAX_DIGIT_COUNT ]; // add one for ending 0
   static bool displayBufferDot[MAX_DIGIT_COUNT ]; // add one for ending 0

   //={Segment_A, Segment_A, Segment_A, Segment_A, Segment_A, Segment_A, Segment_A    } ;

   const static int Full_Cycle = 20;
   const static int Duty_PWM = 5; // bigger the brighter

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
   static void setDisplayV();
   static void setDisplayA();

   static void tim4_Interupt_Init(void);
   static void stm8_Pins_For_DigitubeInit(void);
   static void stm8_Gpio_Write_Low(GPIO_TypeDef* GPIOx, GPIO_Pin_TypeDef PortPins);
   static void stm8_Gpio_Write_High(GPIO_TypeDef* GPIOx, GPIO_Pin_TypeDef PortPins);
   static void setOneDisplayDigit(uint8_t digit);
   static void setDisplayDigitEmpty(void);
   static bool checkIfOverflow(const char* str);
   static void adcInit(void);

   //static void display
public:

   static void stm8_TIM4_Interrupt(void);
   static void stm8_init(void);




   static void  displayInt( int  num);

   //static void display(unsigned char * num);
   static void setDisplayBufferOverflow(void);
   static void setDisplayBufferEmpty(void);
   static void displayString(const char * str);
   static void displayFloat(float f);
   static void displayVoltage(float f);
   static void displayCurrent(float f);

 };

