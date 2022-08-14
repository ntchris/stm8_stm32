/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/Blink
*/


const unsigned long SerialSpeed = 115200;


uint16_t *flashSizeF103Addr = (uint16_t*) 0x1FFFF7E0;
uint16_t *flashSizeF401Addr = (uint16_t*) 0x1FFF7A22;
uint16_t flashSize = *flashSizeF103Addr;


uint16_t g_chipId = 0;
uint16_t get_chipId()
{
  if ( g_chipId)
  {
    return g_chipId;
  }
  uint32_t *deviceIdAddr = (uint32_t*) 0xE0042000;
  g_chipId = *deviceIdAddr;
  return g_chipId;
  
}

// the setup function runs once when you press reset or power the board
void setup() {



  if ( g_chipId)
  {
    // stm32F401
    flashSize = *flashSizeF401Addr;
  }
  else
  { // stm32F103
    flashSize = *flashSizeF103Addr;
  }

  Serial.begin(SerialSpeed);

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {


  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)

  Serial.println("Hello World from Stm32CubeIDE!\r\n");
  String message = "Device Id " + String(g_chipId) + " flash size is " + String(flashSize);
  Serial.println(message);


  delay(800);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(500);                       // wait for a second

}
