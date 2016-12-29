#include "stm8_DigitubeDriver.hpp"






void Delayms(unsigned int ms);


int main()
{

   STM8_DigitubeDriver::stm8_init();
   int i=10000;


   while (true)
   {
      STM8_DigitubeDriver::display(i);

       //i++;
       Delayms(10);

       if(i> MAX_NUMBER_TO_DISPLAY+100 )
       {

          i=0;
       }


   };

}
