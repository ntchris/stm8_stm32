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



float ADC1_GetConversionValue(void)
{

    static uint16_t temph = 0;
    uint8_t templ = 0;

    if ((ADC1->CR2 & ADC1_CR2_ALIGN) != 0) /* Right alignment */
    {
        /* Read LSB first */
        templ = ADC1->DRL;
        /* Then read MSB */
        temph = ADC1->DRH;

        temph = (uint16_t)(templ | (uint16_t)(temph << (uint8_t)8));
    }

    static float fvalue = temph*3.3/1024.0;
    return  fvalue ;

}


int main()
{

   STM8_DigitubeDriver::stm8_init();


   while (true)
   {
      ADC1->CR1 |= ADC1_CR1_ADON;
      Delayms(500);

      float adcValue =0;
      adcValue = ADC1_GetConversionValue();


      STM8_DigitubeDriver::displayFloat(adcValue);
      Delayms(1500);

      //STM8_DigitubeDriver::displayCurrent(0.512345);



   };

}
