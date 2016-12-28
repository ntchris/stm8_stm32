

#include "stm8s.h"

#include "stm8_DigitubeDriver.hpp"

void STM8_DigitubeDriver::gpioInitPushPull(GPIO_TypeDef* GPIOx, GPIO_Pin_TypeDef GPIO_Pin  )
{
   GPIOx->CR2 &= (uint8_t)(~(GPIO_Pin));
   GPIOx->DDR |= (uint8_t)GPIO_Pin;  // Output mode
   GPIOx->CR1 |= (uint8_t)GPIO_Pin;  //push pull
}

void STM8_DigitubeDriver::tim4_Interupt_Init(void)
{


}


void STM8_DigitubeDriver::stm8_Gpio_Write_Low(GPIO_TypeDef* GPIOx, GPIO_Pin_TypeDef PortPins)
{
    GPIOx->ODR &= (uint8_t)(~PortPins);
}


void STM8_DigitubeDriver::stm8_Gpio_Write_High(GPIO_TypeDef* GPIOx, GPIO_Pin_TypeDef PortPins)
{
    GPIOx->ODR |= (uint8_t)PortPins;
}


void STM8_DigitubeDriver::stm8_Pins_For_DigitubeInit(void)
{
   STM8_DigitubeDriver::gpioInitPushPull(Segment_A_Port,   Segment_A_Pin );
   STM8_DigitubeDriver::gpioInitPushPull(Segment_B_Port,   Segment_B_Pin );
   STM8_DigitubeDriver::gpioInitPushPull(Segment_C_Port,   Segment_C_Pin );
   STM8_DigitubeDriver::gpioInitPushPull(Segment_D_Port,   Segment_D_Pin );
   STM8_DigitubeDriver::gpioInitPushPull(Segment_E_Port,   Segment_E_Pin );
   STM8_DigitubeDriver::gpioInitPushPull(Segment_F_Port,   Segment_F_Pin );
   STM8_DigitubeDriver::gpioInitPushPull(Segment_G_Port,   Segment_G_Pin );
   STM8_DigitubeDriver::gpioInitPushPull(Segment_Dp_Port,   Segment_Dp_Pin );
   STM8_DigitubeDriver::gpioInitPushPull(Segment_Digit1_Port,   Segment_Digit1_Pin );
   STM8_DigitubeDriver::gpioInitPushPull(Segment_Digit2_Port,   Segment_Digit2_Pin );
   STM8_DigitubeDriver::gpioInitPushPull(Segment_Digit3_Port,   Segment_Digit3_Pin );
   STM8_DigitubeDriver::gpioInitPushPull(Segment_Digit4_Port,   Segment_Digit4_Pin );


   // default set all common cathode digit1-4 high to turn all off
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_Digit1_Port,   Segment_Digit1_Pin );
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_Digit2_Port,   Segment_Digit2_Pin );
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_Digit3_Port,   Segment_Digit3_Pin );
   STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_Digit4_Port,   Segment_Digit4_Pin );

}
