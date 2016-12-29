#include "stm8_DigitubeDriver.hpp"

#include <stdio.h>

//STM8_DigitubeDriver::Port_Pin STM8_DigitubeDriver::Segment_A;
/*
 const STM8_DigitubeDriver::Port_Pin STM8_DigitubeDriver::Segment_A =
 { Segment_A_Port, Segment_A_Pin };
 const STM8_DigitubeDriver::Port_Pin STM8_DigitubeDriver::Segment_B =
 { Segment_B_Port, Segment_B_Pin };
 const STM8_DigitubeDriver::Port_Pin STM8_DigitubeDriver::Segment_C =
 { Segment_C_Port, Segment_C_Pin };
 const STM8_DigitubeDriver::Port_Pin STM8_DigitubeDriver::Segment_D =
 { Segment_D_Port, Segment_D_Pin };
 const STM8_DigitubeDriver::Port_Pin STM8_DigitubeDriver::Segment_E =
 { Segment_E_Port, Segment_E_Pin };
 const STM8_DigitubeDriver::Port_Pin STM8_DigitubeDriver::Segment_F =
 { Segment_F_Port, Segment_F_Pin };
 const STM8_DigitubeDriver::Port_Pin STM8_DigitubeDriver::Segment_G =
 { Segment_G_Port, Segment_G_Pin };
 const STM8_DigitubeDriver::Port_Pin STM8_DigitubeDriver::Segment_Dp =
 { Segment_Dp_Port, Segment_Dp_Pin };
 */
const STM8_DigitubeDriver::Port_Pin STM8_DigitubeDriver::Digit1 =
{ Segment_Digit1_Port, Segment_Digit1_Pin };
const STM8_DigitubeDriver::Port_Pin STM8_DigitubeDriver::Digit2 =
{ Segment_Digit2_Port, Segment_Digit2_Pin };
const STM8_DigitubeDriver::Port_Pin STM8_DigitubeDriver::Digit3 =
{ Segment_Digit3_Port, Segment_Digit3_Pin };
const STM8_DigitubeDriver::Port_Pin STM8_DigitubeDriver::Digit4 =
{ Segment_Digit4_Port, Segment_Digit4_Pin };

const STM8_DigitubeDriver::Port_Pin STM8_DigitubeDriver::ArrayDigitPortPin[MAX_DIGIT_COUNT] =
{ STM8_DigitubeDriver::Digit1, STM8_DigitubeDriver::Digit2, STM8_DigitubeDriver::Digit3,
      STM8_DigitubeDriver::Digit4 };
uint8_t STM8_DigitubeDriver::currentDigitIndex = 0;

char STM8_DigitubeDriver::displayBuffer[MAX_DIGIT_COUNT  ] =
{ '1', '2', '3', '4'  };

#define TIM4_IT_UPDATE      ((uint8_t)0x01)

inline void STM8_DigitubeDriver::tim4_Interupt_Init(void)
{
   // prescaler
   TIM4->PSCR = (uint8_t) (TIM4_PRESCALER_2);  // bigger , slower

   // The auto reload count
   TIM4->ARR = (uint8_t) (0xff);

   //Enable
   TIM4->CR1 |= TIM4_CR1_CEN;
   // Enable source
   TIM4->IER |= (uint8_t) TIM4_IT_UPDATE;

   enableInterrupts();

}

void Delay8us(void)
{
   asm("nop");

}

void Delayms(unsigned int ms)
{
   unsigned int i;

   for (i = 0; i < ms; i++)
   {
      int j = 0;
      for (j = 0; j < 90; j++)
      {
         Delay8us();
      }

   }

}

inline void STM8_DigitubeDriver::stm8_TIM4_Interrupt(void)
{

   static uint8_t count = 0;
   const uint8_t PWM_LOOP = STM8_DigitubeDriver::Full_Cycle / STM8_DigitubeDriver::Duty_PWM;
   static bool isOff = false;

   static const STM8_DigitubeDriver::Port_Pin *currentDigit = &ArrayDigitPortPin[currentDigitIndex];
   if (count == 0)
   {
      // turn ON
      // set the digit (only one) for digitube to show
      STM8_DigitubeDriver::setOneDisplayDigit(
            STM8_DigitubeDriver::displayBuffer[currentDigitIndex]);

      // turn on only the current digit
      STM8_DigitubeDriver::stm8_Gpio_Write_Low(currentDigit->port, currentDigit->pin);
      isOff = false;
      count++;
      return;
   }
   else
   {
      // should be off. counting PWM counter
      count++;
      //is current digit off ?
      if (!isOff)
      {
         //set digit off
         STM8_DigitubeDriver::stm8_Gpio_Write_High(currentDigit->port, currentDigit->pin);
         isOff = true;
      }
      if (count >= PWM_LOOP)
      {
         count = 0;
         //========================
         // refresh digits
         currentDigitIndex++;
         if (currentDigitIndex >= MAX_DIGIT_COUNT)
         {
            currentDigitIndex = 0;
         }
         currentDigit = &ArrayDigitPortPin[currentDigitIndex];
      }

   }
}

INTERRUPT_HANDLER(TIM4_UPD_OVF_IRQHandler, 23)
{
   // clear interrupt flag otherwise it reenter this again and again
   TIM4->SR1 = (uint8_t) (~TIM4_IT_UPDATE);
   static bool isReady = true;

   if (!isReady)
   {
      //last process is NOT ready yet ???

      return;
   }

   isReady = false;

   STM8_DigitubeDriver::stm8_TIM4_Interrupt();
   isReady = true;
}

inline void STM8_DigitubeDriver::stm8_Gpio_Write_Low(GPIO_TypeDef* GPIOx, GPIO_Pin_TypeDef PortPins)
{
   GPIOx->ODR &= (uint8_t) (~PortPins);
}

inline void STM8_DigitubeDriver::stm8_Gpio_Write_High(GPIO_TypeDef* GPIOx,
      GPIO_Pin_TypeDef PortPins)
{
   GPIOx->ODR |= (uint8_t) PortPins;
}

void STM8_DigitubeDriver::gpioInitPushPull(GPIO_TypeDef* GPIOx, GPIO_Pin_TypeDef GPIO_Pin)
{

   GPIOx->CR2 &= (uint8_t) (~(GPIO_Pin));
   GPIOx->DDR |= (uint8_t) GPIO_Pin;  // Output mode
   GPIOx->CR1 |= (uint8_t) GPIO_Pin;  //push pull
}

void STM8_DigitubeDriver::stm8_init(void)
{
   STM8_DigitubeDriver::stm8_Pins_For_DigitubeInit();
   STM8_DigitubeDriver::tim4_Interupt_Init();
}

void STM8_DigitubeDriver::stm8_Pins_For_DigitubeInit(void)
{

   STM8_DigitubeDriver::gpioInitPushPull(Segment_A_Port, Segment_A_Pin);
   STM8_DigitubeDriver::gpioInitPushPull(Segment_B_Port, Segment_B_Pin);
   STM8_DigitubeDriver::gpioInitPushPull(Segment_C_Port, Segment_C_Pin);
   STM8_DigitubeDriver::gpioInitPushPull(Segment_D_Port, Segment_D_Pin);
   STM8_DigitubeDriver::gpioInitPushPull(Segment_E_Port, Segment_E_Pin);
   STM8_DigitubeDriver::gpioInitPushPull(Segment_F_Port, Segment_F_Pin);
   STM8_DigitubeDriver::gpioInitPushPull(Segment_G_Port, Segment_G_Pin);
   STM8_DigitubeDriver::gpioInitPushPull(Segment_Dp_Port, Segment_Dp_Pin);
   STM8_DigitubeDriver::gpioInitPushPull(Segment_Digit1_Port, Segment_Digit1_Pin);
   STM8_DigitubeDriver::gpioInitPushPull(Segment_Digit2_Port, Segment_Digit2_Pin);
   STM8_DigitubeDriver::gpioInitPushPull(Segment_Digit3_Port, Segment_Digit3_Pin);
   STM8_DigitubeDriver::gpioInitPushPull(Segment_Digit4_Port, Segment_Digit4_Pin);

   // default set all common cathode digit1-4 high to turn all off
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_Digit1_Port, Segment_Digit1_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_Digit2_Port, Segment_Digit2_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_Digit3_Port, Segment_Digit3_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_Digit4_Port, Segment_Digit4_Pin);

}

inline void STM8_DigitubeDriver::setDisplay0()
{
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_A_Port, Segment_A_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_B_Port, Segment_B_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_C_Port, Segment_C_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_D_Port, Segment_D_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_E_Port, Segment_E_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_F_Port, Segment_F_Pin);

   // every other segment is low
   STM8_DigitubeDriver::stm8_Gpio_Write_Low(Segment_G_Port, Segment_G_Pin);

}

inline void STM8_DigitubeDriver::setDisplay1()
{
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_B_Port, Segment_B_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_C_Port, Segment_C_Pin);

   // every other segment is low
   STM8_DigitubeDriver::stm8_Gpio_Write_Low(Segment_A_Port, Segment_A_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_Low(Segment_D_Port, Segment_D_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_Low(Segment_E_Port, Segment_E_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_Low(Segment_F_Port, Segment_F_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_Low(Segment_G_Port, Segment_G_Pin);

}

inline void STM8_DigitubeDriver::setDisplay2()
{
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_A_Port, Segment_A_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_B_Port, Segment_B_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_D_Port, Segment_D_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_E_Port, Segment_E_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_G_Port, Segment_G_Pin);

   // every other segment is low

   STM8_DigitubeDriver::stm8_Gpio_Write_Low(Segment_C_Port, Segment_C_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_Low(Segment_F_Port, Segment_F_Pin);

}

inline void STM8_DigitubeDriver::setDisplay3()
{
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_A_Port, Segment_A_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_B_Port, Segment_B_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_C_Port, Segment_C_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_D_Port, Segment_D_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_G_Port, Segment_G_Pin);

   // every other segment is low

   STM8_DigitubeDriver::stm8_Gpio_Write_Low(Segment_E_Port, Segment_E_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_Low(Segment_F_Port, Segment_F_Pin);

}

inline void STM8_DigitubeDriver::setDisplay4()
{
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_B_Port, Segment_B_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_C_Port, Segment_C_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_F_Port, Segment_F_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_G_Port, Segment_G_Pin);

   // every other segment is low

   STM8_DigitubeDriver::stm8_Gpio_Write_Low(Segment_A_Port, Segment_A_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_Low(Segment_E_Port, Segment_E_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_Low(Segment_D_Port, Segment_D_Pin);

}

inline void STM8_DigitubeDriver::setDisplay5()
{
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_A_Port, Segment_A_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_C_Port, Segment_C_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_D_Port, Segment_D_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_F_Port, Segment_F_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_G_Port, Segment_G_Pin);
   // every other segment is low
   STM8_DigitubeDriver::stm8_Gpio_Write_Low(Segment_B_Port, Segment_B_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_Low(Segment_E_Port, Segment_E_Pin);

}

inline void STM8_DigitubeDriver::setDisplay6()
{
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_A_Port, Segment_A_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_C_Port, Segment_C_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_D_Port, Segment_D_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_E_Port, Segment_E_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_F_Port, Segment_F_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_G_Port, Segment_G_Pin);
   // every other segment is low
   STM8_DigitubeDriver::stm8_Gpio_Write_Low(Segment_B_Port, Segment_B_Pin);

}

inline void STM8_DigitubeDriver::setDisplay7()
{
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_A_Port, Segment_A_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_B_Port, Segment_B_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_C_Port, Segment_C_Pin);

   // every other segment is low
   STM8_DigitubeDriver::stm8_Gpio_Write_Low(Segment_D_Port, Segment_D_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_Low(Segment_E_Port, Segment_E_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_Low(Segment_F_Port, Segment_F_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_Low(Segment_G_Port, Segment_G_Pin);
}

inline void STM8_DigitubeDriver::setDisplay8()
{
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_A_Port, Segment_A_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_B_Port, Segment_B_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_C_Port, Segment_C_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_D_Port, Segment_D_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_E_Port, Segment_E_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_F_Port, Segment_F_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_G_Port, Segment_G_Pin);

}

inline void STM8_DigitubeDriver::setDisplay9()
{
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_A_Port, Segment_A_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_B_Port, Segment_B_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_C_Port, Segment_C_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_D_Port, Segment_D_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_F_Port, Segment_F_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_G_Port, Segment_G_Pin);

   // every other segment is low

   STM8_DigitubeDriver::stm8_Gpio_Write_Low(Segment_E_Port, Segment_E_Pin);

}

void STM8_DigitubeDriver::setOneDisplayDigit(unsigned char digit)
{
   switch (digit)
   {
      case '0':
         STM8_DigitubeDriver::setDisplay0();
         break;
      case '1':
         STM8_DigitubeDriver::setDisplay1();
         break;
      case '2':
         STM8_DigitubeDriver::setDisplay2();
         break;
      case '3':
         STM8_DigitubeDriver::setDisplay3();
         break;
      case '4':
         STM8_DigitubeDriver::setDisplay4();
         break;

      case '5':
         STM8_DigitubeDriver::setDisplay5();
         break;

      case '6':
         STM8_DigitubeDriver::setDisplay6();
         break;
      case '7':
         STM8_DigitubeDriver::setDisplay7();
         break;

      case '8':
         STM8_DigitubeDriver::setDisplay8();
         break;
      case '9':
         STM8_DigitubeDriver::setDisplay9();
         break;
      default:
         STM8_DigitubeDriver::setDisplayEmpty();

   }

}


void STM8_DigitubeDriver::setDisplayEmpty()
{
     STM8_DigitubeDriver::stm8_Gpio_Write_Low(Segment_A_Port, Segment_A_Pin);
     STM8_DigitubeDriver::stm8_Gpio_Write_Low(Segment_B_Port, Segment_B_Pin);
     STM8_DigitubeDriver::stm8_Gpio_Write_Low(Segment_C_Port, Segment_C_Pin);
     STM8_DigitubeDriver::stm8_Gpio_Write_Low(Segment_D_Port, Segment_D_Pin);
     STM8_DigitubeDriver::stm8_Gpio_Write_Low(Segment_E_Port, Segment_E_Pin);
     STM8_DigitubeDriver::stm8_Gpio_Write_Low(Segment_F_Port, Segment_F_Pin);
     STM8_DigitubeDriver::stm8_Gpio_Write_Low(Segment_G_Port, Segment_G_Pin);
}



void STM8_DigitubeDriver::display(int num)
{

   if (num > MAX_NUMBER_TO_DISPLAY)
   {

      STM8_DigitubeDriver::displayOverflow();
   }
   else if (num < 0)
   {
      STM8_DigitubeDriver::displayOverflow();

   }

   memset(displayBuffer, 0, MAX_DIGIT_COUNT );

   if (num == 0)
   {
      displayBuffer[MAX_DIGIT_COUNT-1] = '0' + 0;
      return;
   }

   int number = num;
   const int Ten=10;
   int divide = Ten;
   int index = MAX_DIGIT_COUNT -1 ;
   static int left=0;


   do
   {
      left = number % Ten;
      displayBuffer[ index ] = '0' + left;
      number = number /Ten;
       index--;
   } while (index >=0  && number>0 );
}

void STM8_DigitubeDriver::display(unsigned char * num)
{
   if (!num) return;
   memcpy(displayBuffer, num, MAX_DIGIT_COUNT);

}

void STM8_DigitubeDriver::displayOverflow(void)
{
   STM8_DigitubeDriver::setDisplay8();
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_Dp_Port, Segment_Dp_Pin);

}
