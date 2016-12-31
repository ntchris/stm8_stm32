#include "stm8_DigitubeDriver.hpp"

#define TESTBUILD

#ifdef TESTBUILD
void doTest()
{

   STM8_DigitubeDriver::displayString("12.34");

   STM8_DigitubeDriver::displayString("13..4");
   STM8_DigitubeDriver::displayString("....");

   STM8_DigitubeDriver::displayString("...1");
   STM8_DigitubeDriver::displayString("1..2");

   STM8_DigitubeDriver::displayInt(1234);
   STM8_DigitubeDriver::displayInt(5);
   STM8_DigitubeDriver::displayInt(12);

   STM8_DigitubeDriver::displayFloat(5.123); // only show 5.12
   STM8_DigitubeDriver::displayFloat(12.34); //
   STM8_DigitubeDriver::displayVoltage(9.8765);
   STM8_DigitubeDriver::displayInt(0);

}

#endif

void Delayms(unsigned int ms);

int main()
{

   STM8_DigitubeDriver::stm8_init();
   Delayms(500);

   ADC1->CR1 |= ADC1_CR1_ADON;

   while (true)
   {
      STM8_DigitubeDriver::displayADC();
      //STM8_DigitubeDriver::displayFloat(1.234);
      ADC1->CR1 |= ADC1_CR1_ADON;

      Delayms(500);


   };

}
