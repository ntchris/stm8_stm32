


/**
 *  Work with Common Cathode 7 Seg digitube
 *
 *  Stm8 Pin configuration
 *  Seven Segment DigiTube Pin# and Def    STM8 PIN
 *  -----------------------------------------------
 *    11   Seg A                             PD5
 *    7    Seg B                             PC7
 *    4    Seg C                             PC3
 *    2    Seg D                             PA2
 *    1    Seg E                             PA3
 *    10   Seg F                             PD6
 *    5    Seg G                             PC4
 *    3    Seg Dp (dot)                      PA1
 *    12   Digit1  (Common Cathode)          PD4    (Low = ON , High = OFF )
 *    9    Digit2  (Common Cathode)          PB4    (Low = ON , High = OFF )
 *    8    Digit3  (Common Cathode)          PB5    (Low = ON , High = OFF )
 *    6    Digit4  (Common Cathode)          PD2    (Low = ON , High = OFF )
 *
 *
 */
#include "stm8s.h"

// Connect a 4 digit digitube
static const  uint8_t  MAX_DIGIT=4;

class STM8_DigitubeDriver
{

  static void tim4_Interupt_Init(void);

public:

   static void digitubeInit(void);




};

