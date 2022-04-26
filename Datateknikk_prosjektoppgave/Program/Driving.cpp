//This library includes mostly all the functions that use the motors of the zumo. This library feeds into menu.cpp to choose what function to run.


#include "Driving.h"
#include "Menu.h"
#include "Encoder.h"
Driving* Driving::driving;  //decleres Driving as a pointer
//Encoder encoder;

int turnSpeed = 100;
bool calibrated = false;            // True if vehichle has already been calibrated.
Zumo32U4LineSensors linesensors;
unsigned int linesensorValues[5];

//Calibrates the linesensors by using the calibrate function from the zumo library while spinning.
void Driving::Calibrate()
{
    if (!calibrated) {
        linesensors.initFiveSensors();
        Menu::menu->lcd.clear();
        Menu::menu->lcd.write("Press A");
        Menu::menu->lcd.gotoXY(0, 1);
        Menu::menu->lcd.write("to calibrate");

        //Waits for button A be pressed, so the user has time to put the zumo on the line
        Menu::menu->ButtonA.waitForButton();
        Menu::menu->lcd.clear();
        Menu::menu->lcd.write("Calibration");
        
        //Car spins for 10 seconds while calibrating
        motors.setSpeeds(100, -100);
        unsigned long timer = millis();
        while (millis() < timer + 10000) {
            linesensors.calibrate();
            delay(1);
        }
        
        motors.setSpeeds(0, 0);
        Menu::menu->lcd.gotoXY(0, 1);
        Menu::menu->lcd.write("is done");
        delay(1000);
        Menu::menu->lcd.clear();
        //A break before line follow starts, so you have time to place the zumo in position
        Menu::menu->lcd.write("A Start");
        Menu::menu->lcd.gotoXY(0, 1);
        Menu::menu->lcd.write("B Stop");
        Menu::menu->ButtonA.waitForButton();
        calibrated = true;
    }
}
//Standard Linefollow
void Driving::Followline(bool speeddisplay)
{
    Calibrate();
    while (!Menu::menu->ButtonB.isPressed()) //The program runs until button B is pressed
        {
        int pos = linesensors.readLine(linesensorValues); //reads the sensors
        
        //Displays either the sensorvalue, or the speed of the zumo.
        if (speeddisplay)
        {
        encoder.showspeed();  
        }
        else
        {
          Menu::menu->lcd.clear();
          Menu::menu->lcd.print(pos);
        }
        delay(1);
        if ((pos == 0) || (pos == 4000)) //If the value is sensor value is 0, a test is run to check if the zumo is outside the line.
        {
            Test(speeddisplay);
        }
        else
        {
            Reglinefollow();
        }
    }
    motors.setSpeeds(0, 0);
    Menu::menu->lcd.clear();
    delay(1000);
}

void Driving::Reglinefollow()
{
    int pos = linesensors.readLine(linesensorValues);
    int basespeed = 150; // Sets a base maxspeed for the zumo.
    int rspeed = basespeed;
    int lspeed = basespeed;
    //Changes the power of the motors dependant on the sensor values
    if (pos > 2250) {
        rspeed = (4000 - pos) / 100;
    }
    else if (pos < 1750) {
        lspeed = (pos) / 100;
    }
    motors.setSpeeds(lspeed, rspeed);
}


// Checks if the zumo has not seen the tape in over 2 second, and turns around if no tape.
// This assumes that the sensor will either return 0 (No line) or 4000 (The sensor thinks everything is the same color as the line) whenever the zumo has driven off the line.
void Driving::Test(bool speeddisplay)
{
    unsigned long timer = millis();
    pos = linesensors.readLine(linesensorValues);
    while ((pos == 0) || (pos == 4000))
    {
      if (speeddisplay == true)
        {
        encoder.showspeed();  
        }
        else
        {
          Menu::menu->lcd.print(pos);
        }
        
        pos = linesensors.readLine(linesensorValues);
        if (millis() > (timer + 2000))
        {
            turning.turnRight(180);
            motors.setSpeeds(150, 150);
            while ((pos == 0) || (pos == 4000))
            {
              if (speeddisplay)
        {
        encoder.showspeed();  
        }
        else
        {
          Menu::menu->lcd.print(pos);
        }
                pos = linesensors.readLine(linesensorValues);
            }
        }
    }
    return;
}

//Basic program that makes the zumo drive in a square
void Driving::Square()
{
    motors.setSpeeds(200, 200);
    delay(500);
    turning.turnLeft(90);
    motors.setSpeeds(200, 200);
    delay(500);
    turning.turnLeft(90);
    motors.setSpeeds(200, 200);
    delay(500);
    turning.turnLeft(90);
    motors.setSpeeds(200, 200);
    delay(500);
    turning.turnLeft(90);
    return;
}

//Basic program that makes it so the zumo drives in a sircle
void Driving::Circle()
{
    turning.turnRight(90);
  motors.setSpeeds(100,200);
  turning.WaitforAngle(0);
  turning.turnLeft(90);
    return;
}

//Basic program where the zumo drives straight forward, then turns to drive back.
void Driving::Straight()
{

    motors.setSpeeds(200, 200);
    delay(1000);
    turning.turnLeft(179);
    motors.setSpeeds(200, 200);
    delay(1000);
    turning.turnRight(180);
    return;
}

//Program where the zumo drives in halfsircles to simulate driving zig-zag between obstacles
void Driving::Zigzag() 
{
      turning.turnRight(90);
  motors.setSpeeds(100,200);
  turning.WaitforAngle(180);
  motors.setSpeeds(200,100);
  turning.WaitforAngle(180);
  motors.setSpeeds(100,200);
  turning.WaitforAngle(180);
  motors.setSpeeds(200,100);
  turning.WaitforAngle(180);
  turning.turnLeft(90);
  return;
  }


// =========================================================================

// --------------------          PD         --------------------------------

// =========================================================================


uint8_t getLineSensorReading() {
    uint8_t pv = map(linesensors.readLine(linesensorValues), 0, 4000, 0, 255);
    return(pv);
}

/*  Arguments:
    - kp:           P-forsterkning
    - kd:           D-forsterkning
    - refSpeed:     reference speed used in steering PID
    - enableProxy:  true: Enables proxysensors. This enables vehichle to emergency brake.
    - music         true: Enables music to play while driving.
    - speeddisplay true: Speedometer on the lcd screen, false: sensorvalue in range 0-4000 on lcd screen
    Function will create a SteeringPD object and run vehichle in PD mode.
    When leaving the function the SteeringPD object will be destructed which will free up memory.   
    */
        /*  States:
            - 0     -> Stop
            - 1     -> Normal Mode
            - 2     -> Turn 180*
            - 3     -> Manual drive forward
            - 4     -> Drive slow while searching for route */
void Driving::runPD(float kp, float kd, int refSpeed, bool enableProxy, bool enableMusic, bool speeddisplay) {
    Menu::menu->driving.Calibrate();
    bool flag, flag_2;
    unsigned long timerPD;
    int PD_state = 1;
    uint8_t(*PV)();                                         // Oppretter function pointer
    PV = getLineSensorReading;                              // Peker functionpointer til function for lesing av sensor.
    SteeringPD steering(kp, kd, 127, (int)refSpeed, PV);    
    Proxysensor proxy;                                      // This is for the emergency brake.
    Music music;                                            // This is for playing "Take on Me"

    do {
      if (speeddisplay)
        {
        encoder.showspeed();  
        }
        else
        {
          Menu::menu->lcd.clear();
          int pos = linesensors.readLine(linesensorValues);
          Menu::menu->lcd.print(pos);
        }
        // Manual Exit 
        if (Menu::menu->ButtonB.getSingleDebouncedPress()) {
            motors.setSpeeds(0, 0);
            Menu::menu->lcd.clear();
            Menu::menu->lcd.print("Exit");
            delay(4000);
            PD_state = 0;
        }
        if (enableMusic) {
            music.playTune();
        }
        if(!enableProxy || !proxy.avoidCollision()) {           // If proxysensors disabled OR sensor does not indicate collision .. 
            switch (PD_state)
            {
                // Normal Mode
            case 1:
                if (!steering.runSteeringPD_andCheckCourse()) {         // stops vehichle if it has lost its course.
                    motors.setSpeeds(0, 0);
                    PD_state++;
                }
                break;
                // Turn 180*
            case 2:
                Menu::menu->driving.turning.turnRight(180);
                delay(500);
                timerPD = millis();
                motors.setSpeeds(80, 80);
                PD_state++;
                break;
                // Manual drive (1 sek)
            case 3:
                if (timerPD + 1000 < millis()) {
                    steering.setParameters(5.0f, 1.0f, 80, 50);
                    Menu::menu->driving.turning.turnSensorReset();            // Resetter vinkelen (angle) til 0
                    PD_state++;
                }
                break;
                // Drive slow and search for new route.
            case 4:
                flag = steering.runSteeringPD_andCheckCourse();
                if (!flag) {
                    motors.setSpeeds(0, 0);
                    Menu::menu->lcd.clear();
                    Menu::menu->lcd.print("Truly");
                    Menu::menu->lcd.gotoXY(0, 1);
                    Menu::menu->lcd.print("Lost");
                    delay(2000);
                    flag_2 = false;
                    PD_state = 0;   // New Route not found -> Exit PD.
                }
                else if (flag) {
                    if (Menu::menu->driving.turning.Checkangle() > 60 || Menu::menu->driving.turning.Checkangle() < -60) {      // Checks for turn
                        timerPD = millis();
                        flag_2 = true;
                    }
                    if (flag_2 && timerPD + 1000 > millis()) {       // Waits a time before switching back to old parameters.
                        steering.setParameters(kp, kd, 127, refSpeed);
                        PD_state = 1;
                    }
                }
                break;
            }
        }
    } while (PD_state != 0);
}

// =========================================================================

// --------------------          Proxy         -----------------------------

// =========================================================================

/*  Function will navigate with only 1 proxy sensor.
    If a object is sensed it will try to evaluate which side it has to navigate to (Left or Right)
    It will then try to avoid it. 
    NOTE: This would work better with 3 Porxysensors instead of 1.
    However this is not possible as we are using 5 linesensors (Look Zumo32U4 datasheet for more info).
    - When a collision is detected vehichle will turn 45* to the left. It will repeat this untill there is no object in the way.
    */
void Driving::runProxy() {
    Music music;
    Proxysensor proxy;
    do {
        if (proxy.avoidCollision_andTryToDodge()) {
            turning.turnLeft(45);        
        }
        music.playTune();
    } while (!Menu::menu->ButtonB.isPressed());
    motors.setSpeeds(0, 0);
    Menu::menu->lcd.clear();
    Menu::menu->lcd.print("Bye :D");
    delay(3000);
}
