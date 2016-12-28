#include "stm8_DigitubeDriver.hpp"

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

int main()
{

   STM8_DigitubeDriver::stm8_Pins_For_DigitubeInit();

   while (true)
   {
      //all digits on
      STM8_DigitubeDriver::stm8_Gpio_Write_Low(Segment_Digit1_Port, Segment_Digit1_Pin);
      Delay8us();
      STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_Digit1_Port, Segment_Digit1_Pin);
      Delay8us();
      Delay8us();
      Delay8us();

      STM8_DigitubeDriver::stm8_Gpio_Write_Low(Segment_Digit2_Port, Segment_Digit2_Pin);
      Delay8us();
      STM8_DigitubeDriver::stm8_Gpio_Write_High(Segment_Digit2_Port, Segment_Digit2_Pin);
      Delay8us();
      Delay8us();
      Delay8us();

   };

}
