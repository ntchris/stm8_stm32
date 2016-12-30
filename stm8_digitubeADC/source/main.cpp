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


}

#endif





void Delayms(unsigned int ms);




int main()
{

   STM8_DigitubeDriver::stm8_init();
   int i=10000;


   while (true)
   {

      STM8_DigitubeDriver::displayString("1..2");

       i++;
       Delayms(10);

       if(i> MAX_NUMBER_TO_DISPLAY+100 )
       {

          i=0;
       }


   };

}
