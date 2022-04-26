//This library uses the encoder on the zumo to calculate the speed of the zumo


#include "Menu.h"
#include "Encoder.h"

unsigned long updatetimer = 0;
Zumo32U4Encoders encoder;


// returns the distance the zumo has driven, only works in positive direction
int Encoder::checkdistance()
{
  int16_t rightsensor = encoder.getCountsAndResetRight();
  int16_t leftsensor = encoder.getCountsAndResetLeft();
  int sensorvalues = (rightsensor + leftsensor) / 2;
  return sensorvalues;
}

//Returns the average speed every 250 ms.
//Note: if one of the belts drove in reverse, the values returned are going to be wrong.
void Encoder::showspeed()
{
  if (!updatetimer)
  {
    updatetimer = millis();
  }
  if (millis() > (updatetimer + 250))
  {
    float meter = checkdistance() / 7910.43; //Converts sensorvalues to meters
    Serial.print(meter);
     float currentspeed = (1000* meter / (millis() - updatetimer)); //calculates the value to m/s
     updatetimer = millis();
     Menu::menu->lcd.clear();
     Menu::menu->lcd.print(currentspeed);
     Menu::menu->lcd.gotoXY(0,1);
     Menu::menu->lcd.print("m/s");
     
     return;
  }
}
