#include "stm8_DigitubeDriver.hpp"






void Delayms(unsigned int ms);


int main()
{

   STM8_DigitubeDriver::stm8_init();
   int i;

   //STM8_DigitubeDriver::setDisplayDigit(8);

   while (true)
   {
       STM8_DigitubeDriver::display(i);

       i++;
       Delayms(800);


   };

}
