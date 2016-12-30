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



    STM8_DigitubeDriver::display(1234);
    STM8_DigitubeDriver::display(5);
    STM8_DigitubeDriver::display(12);


    STM8_DigitubeDriver::displayFloat(5.123); // only show 5.12
    STM8_DigitubeDriver::displayFloat(12.34); //
}

#endif





void Delayms(unsigned int ms);




int main()
{

   STM8_DigitubeDriver::stm8_init();
   int i=10000;

   STM8_DigitubeDriver::displayFloat(12.34); //

   while (true)
   {

      //STM8_DigitubeDriver::display(28);



       i++;
       Delayms(10);

       if(i> MAX_NUMBER_TO_DISPLAY+100 )
       {

          i=0;
       }


   };

}
