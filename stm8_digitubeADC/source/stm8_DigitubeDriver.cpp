#include "stm8s.h"

#include "stm8_DigitubeDriver.hpp"

//STM8_DigitubeDriver::Port_Pin STM8_DigitubeDriver::Segment_A;

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

const STM8_DigitubeDriver::Port_Pin STM8_DigitubeDriver::Digit1 =
{ Segment_Digit1_Port, Segment_Digit1_Pin };
const STM8_DigitubeDriver::Port_Pin STM8_DigitubeDriver::Digit2 =
{ Segment_Digit2_Port, Segment_Digit2_Pin };
const STM8_DigitubeDriver::Port_Pin STM8_DigitubeDriver::Digit3 =
{ Segment_Digit3_Port, Segment_Digit3_Pin };
const STM8_DigitubeDriver::Port_Pin STM8_DigitubeDriver::Digit4 =
{ Segment_Digit4_Port, Segment_Digit4_Pin };

//STM8_DigitubeDriver::Port_Pin ArrayPortPin[MAX_Segment]={Segment_A, Segment_A, Segment_A, Segment_A, Segment_A, Segment_A, Segment_A    } ;

inline void STM8_DigitubeDriver::tim4_Interupt_Init(void)
{

}

void STM8_DigitubeDriver::stm8_Gpio_Write_Low(GPIO_TypeDef* GPIOx, GPIO_Pin_TypeDef PortPins)
{
   GPIOx->ODR &= (uint8_t) (~PortPins);
}

void STM8_DigitubeDriver::stm8_Gpio_Write_High(GPIO_TypeDef* GPIOx, GPIO_Pin_TypeDef PortPins)
{
   GPIOx->ODR |= (uint8_t) PortPins;
}

void STM8_DigitubeDriver::gpioInitPushPull(GPIO_TypeDef* GPIOx, GPIO_Pin_TypeDef GPIO_Pin)
{

   GPIOx->CR2 &= (uint8_t) (~(GPIO_Pin));
   GPIOx->DDR |= (uint8_t) GPIO_Pin;  // Output mode
   GPIOx->CR1 |= (uint8_t) GPIO_Pin;  //push pull
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
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_B_Port, Segment_B_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_C_Port, Segment_C_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_D_Port, Segment_D_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_E_Port, Segment_E_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_F_Port, Segment_F_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_G_Port, Segment_G_Pin);

}


inline void STM8_DigitubeDriver::setDisplay6()
{
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_A_Port, Segment_A_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_B_Port, Segment_B_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_C_Port, Segment_C_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_D_Port, Segment_D_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_E_Port, Segment_E_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_F_Port, Segment_F_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_G_Port, Segment_G_Pin);

}


inline void STM8_DigitubeDriver::setDisplay7()
{
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_A_Port, Segment_A_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_B_Port, Segment_B_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_C_Port, Segment_C_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_D_Port, Segment_D_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_E_Port, Segment_E_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_F_Port, Segment_F_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_G_Port, Segment_G_Pin);

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
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_E_Port, Segment_E_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_F_Port, Segment_F_Pin);
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_G_Port, Segment_G_Pin);

}




void STM8_DigitubeDriver::setDisplayDigit(uint8_t digit)
{
   switch (digit)
   {
      case 0:
         STM8_DigitubeDriver::setDisplay0();
         break;
      case 1:
         STM8_DigitubeDriver::setDisplay1();
         break;
      case 2:
         STM8_DigitubeDriver::setDisplay2();
         break;
      case 3:
         STM8_DigitubeDriver::setDisplay3();
         break;
      case 4:
         STM8_DigitubeDriver::setDisplay4();
         break;

      case 5:
         STM8_DigitubeDriver::setDisplay5();
         break;

      case 6:
         STM8_DigitubeDriver::setDisplay6();
         break;

      case 8:
         STM8_DigitubeDriver::setDisplay8();
         break;
      default:
         ;
   }

}

