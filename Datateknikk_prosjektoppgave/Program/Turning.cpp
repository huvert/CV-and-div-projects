//This library has taken a lot of inspiration from an existing library from the internet. Some parts are straight copy (The settings in the sensorsetup, and the calculations),
//while some parts are modified by us, or made completely new.
//https://github.com/pvcraven/zumo_32u4_examples/tree/master/TurnExample

//This library uses the gyro on the zumo to controll turns, and to check how to angle of the zumo has changed between updates.
#include <Wire.h>
#include "Menu.h"

L3G gyro;

extern uint32_t turnAngle;
extern int16_t turnRate;
Turning turning;

uint32_t turnAngle = 0;
// turnRate is the angle speed to the gyroscope in the unit 0.07 degrees per second.
int16_t turnRate;

// This is the average sensor reading of the Z axis of the gyroscope during calibration.
int16_t gyroOffset;

// This variable shows the time that has went between the sensor reading of the gyroscope
uint16_t gyroLastUpdate = 0;


void Turning::turnSensorSetup()
{
  Wire.begin();
  gyro.init();

// The gyroscope options are initialized.
//The options are gathered from https://www.pololu.com/file/0J731/L3GD20H.pdf
// The options are written as binary, since thats the format used in the data sheet.
  // low-pass filter cutoff 100 Hz, Initialize x,y, og z axis, og  low-pass filter on 100HZ, and output data on 800 Hz
  gyro.writeReg(L3G::CTRL1, 0b11111111);

  // Sets the range of the readings to 2000
  gyro.writeReg(L3G::CTRL4, 0b00100000);

  // Turns of the high-pass filter
  gyro.writeReg(L3G::CTRL5, 0b00000000);

  Menu::menu->lcd.clear();
  Menu::menu->lcd.print("Gyro cal");
  Menu::menu->lcd.gotoXY(0,1);
  Menu::menu->lcd.print("No move");


  // Calibrates the gyroscope.
  int32_t total = 0;
  for (uint16_t i = 0; i < 1024; i++)
  {
    // When STATUS == 8, new information has arrived in the sensor that is ready to be read.
    // Therefore whenever new info is available, the sensor is read.
    // This information can be found on page 42 of the data sheet 
    while(!gyro.readReg(L3G::STATUS_REG) & 0x08);
    gyro.read();

    // Add the z data from the gyro to a total which afterwards the average is added to gyrooffset
    total += gyro.g.z;
  }
  gyroOffset = total / 1024;
  Menu::menu->lcd.clear();
}

// Resets turnAngle to 0, this is used to change the initial degree, used in turns and other functions.
void Turning::turnSensorReset()
{
  gyroLastUpdate = micros();
  turnAngle = 0;
}

//Reads the sensorvalues of the gyroscope and updates the angle (variable angle), this code is a straight up copy from the internet.
void Turning::turnSensorUpdate()
{
  // Read the measurements from the gyro.
  gyro.read();
  turnRate = gyro.g.z - gyroOffset;

  // dt is how much time has passed since last update
  uint16_t m = micros();
  uint16_t dt = m - gyroLastUpdate;
  gyroLastUpdate = m;

  // Multiply dt by turnRate in order to get an estimation of how
  // much the robot has turned since the last update.
  // (angular change = angular velocity * time)
  int32_t d = (int32_t)turnRate * dt;

  // The units of d are gyro digits times microseconds.  We need
  // to convert those to the units of turnAngle, where 2^29 units
  // represents 45 degrees.  The conversion from gyro digits to
  // degrees per second (dps) is determined by the sensitivity of
  // the gyro: 0.07 degrees per second per digit.
  //
  // (0.07 dps/digit) * (1/1000000 s/us) * (2^29/45 unit/degree)
  // = 14680064/17578125 unit/(digit*us)
  turnAngle += (int64_t)d * 14680064 / 17578125;
}


//Turns to the left, maximum degrees is 179 since the variable angle goes from -180 to 179, therefore if you enter something over it, the zumo will spin forever.
void Turning::turnLeft(int degrees)
{
  Driving::driving->motors.setSpeeds(0,0);
  delay(10); //delay for at motoren ikke endrer seg for fort.
  turnSensorReset();
  Driving::driving->motors.setSpeeds(-turnSpeed, turnSpeed);
  int angle = 0;
  do {
    delay(1);
    turnSensorUpdate();
    angle = (((int32_t)turnAngle >> 16) * 360) >> 16;
  } while (angle < degrees);
  Driving::driving->motors.setSpeeds(0, 0);
}

//Turns to the left, maximum degrees is 180 since the variable angle goes from -180 to 179, therefore if you enter something over it, the zumo will spin forever.
void Turning::turnRight(int degrees) {
  Driving::driving->motors.setSpeeds(0,0);
  delay(10); //delay for at motoren ikke endrer seg for fort.
  turnSensorReset();
  Driving::driving->motors.setSpeeds(turnSpeed, -turnSpeed);
  int angle = 0;
  do {
    delay(1);
    turnSensorUpdate();
    angle = (((int32_t)turnAngle >> 16) * 360) >> 16;
  } while (angle > -degrees);
  Driving::driving->motors.setSpeeds(0, 0);
}

// Returns how much the angle has changed since last time turnSensorReset has been run.
int Turning::Checkangle() {
  turnSensorUpdate();
    int angle = (((int32_t)turnAngle >> 16) * 360) >> 16;
    return angle;
}

// This is the same as the funcitons to turn, except without setting any speed on the motors. This is used to make the zumo drive in sircles and halfsircles.
void Turning::WaitforAngle(int degrees) {
  turnSensorReset();
  int angle = 0;
  unsigned long timer = millis();
  if (degrees == 0) //This is used when you want the zumo to drive a full sircle
  {
  do {
    delay(1);
    turnSensorUpdate();
    angle = (((int32_t)turnAngle >> 16) * 360) >> 16;
  } while ((angle != degrees) || ( millis() < (timer + 1000))); //A small timer to make it so the car does not instantly break the loop since the angle starts at 0.
}
  else
  do {
    delay(1);
    turnSensorUpdate();
    angle = (((int32_t)turnAngle >> 16) * 360) >> 16;
  } while (angle > -degrees);
}
