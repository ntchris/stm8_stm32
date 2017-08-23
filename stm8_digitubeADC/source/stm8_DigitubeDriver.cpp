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

char STM8_DigitubeDriver::displayBuffer[MAX_DIGIT_COUNT] =
{ '1', '2', '3', '4' };

bool STM8_DigitubeDriver::displayBufferDot[MAX_DIGIT_COUNT] =
{ false, false, false, false }; // add one for ending 0

bool STM8_DigitubeDriver::adcNotReady = false;

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

// so far only trim the leading space
//  ie ____5  to 5
void trim(char*str)
{
   int index = 0;
   while (str[index] == ' ')
   {
      index++;
      if (str[index] == 0) break;
   }
   if (index == 0)
   {
      // no space to trim
      return;
   }
   // now  str[index] is not space ' '
   int newIndex = 0;
   do
   {

      str[newIndex] = str[index];
      if (str[index] == 0)
      {

         break;
      }
      newIndex++;
      index++;
   } while (true);

}

// translate the provide int i to string str ( well it's just char* )
void intToString(int num, char *str, int maxLen)
{
   if (str == 0) return;

   memset(str, ' ', maxLen);
   str[maxLen - 1] = 0;
   if (num < 0)
   {
      str[0] = 0;
      return;
   }

   if (num == 0)
   {
      str[0] = '0';
      str[1] = 0;
      return;
   }

   const int Ten = 10;

   int index = maxLen - 1 - 1; //   maxLen - 1 is the ending 0, so the last char is - 1 -1
   index = maxLen - 1 - 1;
   int left = 0;

   do
   {
      left = num % Ten;
      str[index] = '0' + left;
      num = num / Ten;
      index--;
   } while (index >= 0 && num > 0);

   //trim if 5, then it 's  ___5 need to move 5 to the head
   trim(str);
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

      //set dp
      if (displayBufferDot[currentDigitIndex])
      {
         STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_Dp_Port, Segment_Dp_Pin);
      }
      else
      {
         STM8_DigitubeDriver::stm8_Gpio_Write_Low(Segment_Dp_Port, Segment_Dp_Pin);
      }

      // turn on current digit
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

         STM8_DigitubeDriver::stm8_Gpio_Write_Low(Segment_Dp_Port, Segment_Dp_Pin);

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

   STM8_DigitubeDriver::stm8_TIM4_Interrupt();
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

   memset(displayBuffer, 0, MAX_DIGIT_COUNT);
   memset(displayBufferDot, 0, MAX_DIGIT_COUNT);

   STM8_DigitubeDriver::stm8_Pins_For_DigitubeInit();
   STM8_DigitubeDriver::tim4_Interupt_Init();
   STM8_DigitubeDriver::adcInit();
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

inline void STM8_DigitubeDriver::setDisplayV()
{
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_B_Port, Segment_B_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_C_Port, Segment_C_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_D_Port, Segment_D_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_E_Port, Segment_E_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_F_Port, Segment_F_Pin);

   // every other segment is low
   STM8_DigitubeDriver::stm8_Gpio_Write_Low(Segment_A_Port, Segment_A_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_Low(Segment_G_Port, Segment_G_Pin);

}

inline void STM8_DigitubeDriver::setDisplayA()
{
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_A_Port, Segment_A_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_B_Port, Segment_B_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_C_Port, Segment_C_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_E_Port, Segment_E_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_F_Port, Segment_F_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_G_Port, Segment_G_Pin);
   // every other segment is low

   STM8_DigitubeDriver::stm8_Gpio_Write_Low(Segment_D_Port, Segment_D_Pin);

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
      case 'A':
         STM8_DigitubeDriver::setDisplayA();
         break;
      case 'a':
         STM8_DigitubeDriver::setDisplayA();
         break;
      case 'V':
         STM8_DigitubeDriver::setDisplayV();
         break;

      case 'v':
         STM8_DigitubeDriver::setDisplayV();
         break;

      default:
         STM8_DigitubeDriver::setDisplayDigitEmpty();

   }

}

void STM8_DigitubeDriver::setDisplayDigitEmpty()
{
   STM8_DigitubeDriver::stm8_Gpio_Write_Low(Segment_A_Port, Segment_A_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_Low(Segment_B_Port, Segment_B_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_Low(Segment_C_Port, Segment_C_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_Low(Segment_D_Port, Segment_D_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_Low(Segment_E_Port, Segment_E_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_Low(Segment_F_Port, Segment_F_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_Low(Segment_G_Port, Segment_G_Pin);
}

/*
 void STM8_DigitubeDriver::display(int num)
 {

 if (num > MAX_NUMBER_TO_DISPLAY)
 {

 STM8_DigitubeDriver::setDisplayBufferOverflow();
 return;
 }
 else if (num < 0)
 {
 STM8_DigitubeDriver::setDisplayBufferOverflow();
 return;
 }

 memset(displayBuffer, 0, MAX_DIGIT_COUNT);

 if (num == 0)
 {
 displayBuffer[MAX_DIGIT_COUNT - 1] = '0' + 0;
 return;
 }

 int number = num;
 const int Ten = 10;

 int index = MAX_DIGIT_COUNT - 1;
 static int left = 0;

 do
 {
 left = number % Ten;
 displayBuffer[index] = '0' + left;
 number = number / Ten;
 index--;
 } while (index >= 0 && number > 0);
 }
 */

void STM8_DigitubeDriver::displayInt(int num)
{
   char buffer[MAX_DIGIT_COUNT + 1];

   if (num > MAX_NUMBER_TO_DISPLAY)
   {

      STM8_DigitubeDriver::setDisplayBufferOverflow();
      return;
   }

   intToString(num, buffer, MAX_DIGIT_COUNT + 1);
   STM8_DigitubeDriver::displayString(buffer);

}

/*
 void STM8_DigitubeDriver::display(unsigned char * num)
 {
 if (!num)
 {
 //just display empty buffer

 return;
 }
 memcpy(STM8_DigitubeDriver::displayBuffer, num, MAX_DIGIT_COUNT);

 }
 */

void STM8_DigitubeDriver::setDisplayBufferEmpty(void)
{
   for (int i = 0; i < MAX_DIGIT_COUNT; i++)
   {
      displayBuffer[i] = 0; // means empty, not '0'
      displayBufferDot[i] = false;
   }

}

void STM8_DigitubeDriver::setDisplayBufferOverflow(void)
{
   for (int i = 0; i < MAX_DIGIT_COUNT; i++)
   {
      displayBuffer[i] = '8';
      displayBufferDot[i] = true;
   }

}

int stringIndexOf(const char *string, char targetC)
{
   if (string == 0)
   {
      return -1;
   }

   int index = 0;
   while (string[index] != targetC)
   {
      if (string[index] == 0)
      {
         return -1;
      }

      index++;

   }
   return index;

}

//  if m_maxDisplayDigits is 4,  then 8.8.8.8 is not overflow
//  88888 is overflow
bool STM8_DigitubeDriver::checkIfOverflow(const char* str)
{

   // null string, empty
   if (str == 0)
   {
      return false;
   }

   //deal with special cases first
   if (str[0] == 0)
   {
      return false;
   }

   uint8_t digitCount = 0;

   int strlength = strlen(str);

   if (strlength <= MAX_DIGIT_COUNT)
   {
      return false;
   }
   int dotIndex = stringIndexOf(str, '.');

   if (dotIndex < 0 && strlength > MAX_DIGIT_COUNT)
   {
      //there is no dot but more chars than MAX_DIGIT_COUNT, so it has to be overflow
      return true;
   }

   //it has dot and it it's more then MAX_DIGIT_COUNT, need to do some calculation
   for (int i = 0; i < strlength; i++)
   {
      char c = str[i];
      if (c == '.')
      {
         //if previous char not exist , +1
         if (i == 0)
         {
            digitCount++;
         }
         else if (str[i - 1] == '.')
         {
            //if previous char is . +1,
            digitCount++;

         }
         else
         {
            //if prevous char is other, don't +1

         }
      }
      else
      {
         digitCount++;
      }

      if (digitCount > MAX_DIGIT_COUNT)
      {
         return true;
      }
   }

   return false;

}

void STM8_DigitubeDriver::displayString(const char * str)
{
   // make a copy of the string so we don't trim the original string
   //String trimstr = str;
   //trimstr.trim();

   //check if string is longer than the installed digitube digits
   //debugPrint("display " , str);
   memset(displayBuffer, 0, MAX_DIGIT_COUNT);
   memset(displayBufferDot, 0, MAX_DIGIT_COUNT);

   // null string, empty
   if (str == 0)
   {
      return;
   }

   //deal with special cases first
   if (str[0] == 0)
   {
      return;
   }
   else
   {
      if (checkIfOverflow(str))
      {
         STM8_DigitubeDriver::setDisplayBufferOverflow();
         return;
      }

      // find the end of the number string
      int endIndex = strlen(str) - 1;

      int strIndex = endIndex;
      //the index for display character buffer (digit buffer)
      int bufferIndex = MAX_DIGIT_COUNT - 1;

      // the dot buffer index
      int bufferDotIndex = MAX_DIGIT_COUNT - 1;

      for (strIndex = endIndex; strIndex >= 0; strIndex--)
      {

         //process from the end of number
         char c = str[strIndex];

         if (c == 0) break;

         if (c == '.')
         {
            if (displayBufferDot[bufferDotIndex] == true)
            {
               // this is the 2nd dot
               bufferDotIndex--;
               displayBufferDot[bufferDotIndex] = true;
               bufferIndex--;

            }
            else
            {
               displayBufferDot[bufferDotIndex] = true;
            }

         }
         else
         {
            displayBuffer[bufferIndex] = c;
            bufferIndex--;
            bufferDotIndex--;
         }
         //safety guard
         if (bufferIndex < 0 || bufferDotIndex < 0)
         {
            break;
         }

      }
   }

}

// let's don't make this too complicated!!
void floatToString(float f, char *str, int maxLen)
{
   memset(str, ' ', maxLen);
   str[maxLen] = 0;

   // part1
   int intPart = (int) f;

   if (intPart > MAX_NUMBER_TO_DISPLAY)
   {
      // the integer part is already bigger than the Max
      STM8_DigitubeDriver::setDisplayBufferOverflow();
      return;
   }
   intToString(intPart, str, maxLen);

   //deal with fraction part

   // let's don't make this too complicated!!
   const int MaxFloatDigits = 2;
   char strF[MaxFloatDigits + 1];
   memset(strF, 0, MaxFloatDigits + 1);
   int floatPart_2_digits;
   floatPart_2_digits = int((f - intPart) * 100);

   //if (floatPart_2_digits > 0)
   {

      intToString(floatPart_2_digits, strF, MaxFloatDigits + 1);
      strcat(str, ".");
   }

   // append a 0 if the float part has only one digit, ie  5.07,  so float part is 7, must append a 0 before 7, otherwise we have 5.7
   if (floatPart_2_digits < 10)
   {
      strcat(str, "0");
   }
   strcat(str, strF);
   trim(str);
}

void STM8_DigitubeDriver::displayFloat(float f)
{
   const int stringSize = 3 * MAX_DIGIT_COUNT;
   char string[stringSize];
   if (f > MAX_NUMBER_TO_DISPLAY)
   {
      setDisplayBufferOverflow();
      return;
   }

   //don't make this too complicated, limit the string for float just 3
   const int MaxFloatLen = MAX_DIGIT_COUNT;
   floatToString(f, string, MaxFloatLen);
   STM8_DigitubeDriver::displayString(string);

}

void STM8_DigitubeDriver::displayVoltage(float volt)
{
   const int stringSize = 3 * MAX_DIGIT_COUNT;
   char string[stringSize];

   
   floatToString(volt, string, MAX_DIGIT_COUNT);
   //strcat(string, "V");
   
   STM8_DigitubeDriver::displayString(string);
}

void STM8_DigitubeDriver::displayCurrent(float amp)
{
   const int stringSize = 3 * MAX_DIGIT_COUNT;
   char currentString[stringSize];

   floatToString(amp, currentString, MAX_DIGIT_COUNT - 1);
   strcat(currentString, "A");
   STM8_DigitubeDriver::displayString(currentString);
}

void STM8_DigitubeDriver::adcInit(void)
{

   // =============================  input floating ==============

   ADC_Port->CR2 &= (uint8_t) (~(ADC_Pin));  // reset

   ADC_Port->DDR &= (uint8_t) ~ADC_Pin;  // input mode

   // ADC_Port->CR1 &= (uint8_t)(~ ADC_Pin ); // Floating
   STM8_DigitubeDriver::stm8_Gpio_Write_Low(ADC_Port, ADC_Pin);

   uint8_t ADC1_ALIGN_RIGHT = (uint8_t) 0x08;

   //ADC1_ConversionConfig
   /* Clear the ADC1 channels */
   ADC1->CSR &= (uint8_t) (~ADC1_CSR_CH);

   /* Select the ADC1 channel */
   ADC1->CSR |= (uint8_t) (AdcChannel );

   /* Clear the align bit */
   ADC1->CR2 &= (uint8_t) (~ADC1_CR2_ALIGN);
   /* Configure the data alignment */
   ADC1->CR2 |= (uint8_t) (ADC1_ALIGN_RIGHT);
   ADC1->CSR |= (uint8_t) (AdcChannel );

   // continuous mode
   // ADC1->CR1 |= (uint8_t)ADC1_CR1_CONT;
   ADC1->CR1 &= (uint8_t) (~ADC1_CR1_CONT);

   //

   // ====================================
   //  prescaler config

   /* Clear the SPSEL bits */
   ADC1->CR1 &= (uint8_t) (~ADC1_CR1_SPSEL);
   /* Select the prescaler division factor according to ADC1_PrescalerSelection values */
   const uint8_t ADC1_PRESSEL_FCPU_D18 = (uint8_t) 0x70;
//   const uint8_t ADC1_PRESSEL_FCPU_D2 = (uint8_t) 0x00;
   ADC1->CR1 |= (uint8_t) (ADC1_PRESSEL_FCPU_D18);

   // =====================================
   //   ADC1_ExternalTriggerConfig
   ADC1->CR2 &= (uint8_t) (~ADC1_CR2_EXTSEL); // clear
   //ADC1->CR2 |= (uint8_t)(ADC1_CR2_EXTTRIG); // enable
   //uint8_t ADC1_EXTTRIG_GPIO  = (uint8_t)0x10;
   //ADC1->CR2 |= (uint8_t)(ADC1_EXTTRIG_GPIO);

   //enable ADC interrupt
   ADC1->CSR |= ADC1_CSR_EOCIE;

   //
   ADC1->CR3 &= ~ADC1_CR3_DBUF; // disable buffer
   //enable !
   ADC1->CR1 |= ADC1_CR1_ADON;
   enableInterrupts();

}


float adc_compensate(float adcv)
{
	//unfortunately the adc value is not accurate, 
	//with the current resistors:
	//12.5V input, actual adc input  1.99V , adc measured 1.72  (less  1.99-1.72=0.27V)
	//4.15V input, actual adc line input 0.65V, adc measure 0.65 (the same)
	//3.26V input, actual adc line input 0.51V, measure 0.495
 	float delta=(adcv-0.63);
	
	float compensatedAdc=adcv;
	if( delta>0) 
	{ 
      compensatedAdc = delta /1.72*0.33 + adcv;
	}
    return compensatedAdc;
}

float ADC1_GetConversionValue(void)
{

   uint16_t temph = 0;
   uint8_t templ = 0;

   if ((ADC1->CR2 & ADC1_CR2_ALIGN) != 0) /* Right alignment */
   {
      /* Read LSB first */
      templ = ADC1->DRL;
      /* Then read MSB */
      temph = ADC1->DRH;

      temph = (uint16_t) (templ | (uint16_t) (temph << (uint8_t)8));
   }

   float fvalue = temph * 3.3 / 1023.0;
   
   fvalue = adc_compensate(fvalue);
   
   uint8_t ADC1_FLAG_EOC = (uint8_t) 0x80; /**< EOC falg */
   //clear EOC bit
   ADC1->CSR &= (uint8_t) (~ADC1_FLAG_EOC);

   return fvalue;

}

void STM8_DigitubeDriver::displayADC(void)
{
   /*
    //disable ADC now
    uint8_t ADC1_FLAG_EOC = (uint8_t) 0x80; //  < EOC falg

    float adcValue = 0;
    adcValue = ADC1_GetConversionValue();

    STM8_DigitubeDriver::displayFloat(adcValue);
    // clear end of conversion bit
    */
}


// return current battery percent for this 12V type battery.
float getBattery12VPercent( float voltage)
{
	const float minV=11.6, fullV =13.15;
	//11.6V means 0%,  13.15V meas full
	
	if (voltage >= fullV ) 
	{
	  return 100;
	}
	
	if (voltage <= minV ) 
	{
	  return 0;
	}
	
	
	float percent = (voltage-minV)/(fullV-minV) *100;
	int percentInt=percent;
	return percentInt;
	
}

INTERRUPT_HANDLER(ADC1_EOC_IRQHandler, 22)
{
   STM8_DigitubeDriver::stm8_ADC_Interrupt();
}


//average, and display voltage, and percentage!
inline void processAdcInfo(float voltageResult)
{
	
	
   static bool switchVolt_Percent=true;

   static float voltageToDisplay=0;
   static int currentIndex=0;

   const int MAX_AVG = 40;
   
   static float avg[MAX_AVG];
   
   
   avg[currentIndex] = voltageResult;
   currentIndex++;
   
   //have enough values to do average.
   if(currentIndex>=MAX_AVG)
   {
	   currentIndex=0;
	   float avgValue=0;
       for(int i=0;i<MAX_AVG;i++)
       {
	      avgValue= avg[i]+avgValue;
       }
	   //avg value ready, update the voltageToDisplay
       voltageToDisplay=avgValue/MAX_AVG;
	  
       //now we have a new value to display	  
   }  // 
   else if(currentIndex==(MAX_AVG/2))
   {
	  //the firs time enter here, voltage avg value is not ready. dont show
	  if( voltageToDisplay >0) 
      { 
         if(switchVolt_Percent)
        { 
           STM8_DigitubeDriver::displayFloat(voltageToDisplay);
        }
	     else
        {
           int percent = getBattery12VPercent(voltageToDisplay);
           STM8_DigitubeDriver::displayInt(percent);
        }
        switchVolt_Percent=!switchVolt_Percent;
	  }
   }
   
}

inline void STM8_DigitubeDriver::stm8_ADC_Interrupt(void)
{
   
   uint8_t ADC1_FLAG_EOC = (uint8_t) 0x80;  //EOC falg //
   float adcValue = 0.0;
   adcValue = ADC1_GetConversionValue();
   float voltageResult = VoltageTime * adcValue;
   //debug STM8_DigitubeDriver::displayVoltage(adcValue);

   //STM8_DigitubeDriver::displayFloat(voltageResult);
   processAdcInfo(voltageResult);
   
   ADC1->CSR &= (uint8_t) (~ADC1_FLAG_EOC);
   //It's ready
   STM8_DigitubeDriver::adcNotReady = false;
}

void STM8_DigitubeDriver::startADC(void)
{
   if (STM8_DigitubeDriver::adcNotReady)
   {
      return;

   }
   // it's ready
   STM8_DigitubeDriver::adcNotReady = true;
   ADC1->CR1 |= ADC1_CR1_ADON;

}


