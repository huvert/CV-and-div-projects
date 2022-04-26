//This library controlls what program to be run on the zumo.
//It is controlled by using the 3 button on the zumo and works like a scrolling menu


#include "Menu.h"

Menu* Menu::menu; //declares menu as a pointer

//Function to do the start up calibrations and give the user some information
void Menu::Initialize() 
{
    menu = this; //pointer to menu
    lcd.init();
    lcd.clear();
    driving.turning.turnSensorSetup(); //Calibrates the gyro, dont move the car while this is happening
    lcd.write("A/C to");
    lcd.gotoXY(0, 1);
    lcd.write("change");
    delay(1000);
    lcd.clear();
    lcd.write("B to");
    lcd.gotoXY(0, 1);
    lcd.write("run");
    delay(1000);
    program = 1;
}

//Function that waits for one of the button to be pressed, then returns what button which was pressed. A-C = 1-3
int Menu::WaitforButton()
{
    int buttonpressed = 0;
    while (!buttonpressed)
    {
        if ((ButtonA.isPressed()) or (ButtonB.isPressed()) or (ButtonC.isPressed()))
        {
            if (ButtonA.isPressed())
            {
                buttonpressed = 1;
                ButtonA.waitForRelease();
            }
            else if (ButtonB.isPressed())
            {
                buttonpressed = 2;
                ButtonB.waitForRelease();
            }
            else if (ButtonC.isPressed())
            {
                buttonpressed = 3;
                ButtonC.waitForRelease();
            }
        }
    }
    return buttonpressed;
}
//Program that is used to choose what program to be run. Works like a scrolling menu, B to run the program displayed, A goes to the left, C to the right, and it loops around.
int Menu::chooseprogram()
{
    bool chosen = false;
    while (!chosen)
    {
        DisplayProgram();
        int Button = WaitforButton();
        switch (Button) {
        case 1:
            if (program > 1) {

                program -= 1;
            }
            else program = 10;
            break;
        case 2:
            return program;
            chosen = true;
            break;
        case 3:
            if (program < 10) {
                program += 1;
            }
            else
                program = 1;
            break;
        }
    }
    return;
}

//Function that displays on the lcd screen what program is corrently selected in chooseprogram()
void Menu::DisplayProgram()
{
    lcd.clear();
    switch (program) {
    case 1:
        lcd.write("Line");
        lcd.gotoXY(0, 1);
        lcd.write("Follow");
        break;

    case 2:
        lcd.write("Square");
        lcd.gotoXY(0, 1);
        lcd.write("Driving");
        break;

    case 3:
        lcd.write("Circle");
        lcd.gotoXY(0, 1);
        lcd.write("Driving");
        break;

    case 4:
        lcd.write("Straight");
        lcd.gotoXY(0, 1);
        lcd.write("Driving");
        break;

    case 5:
        lcd.write("Zigzag");
        lcd.gotoXY(0, 1);
        lcd.write("Driving");
        break;

    case 6:
        lcd.write("PD");
        lcd.gotoXY(0, 1);
        lcd.write("Normal");
        break;

    case 7:
        lcd.write("PD");
        lcd.gotoXY(0, 1);
        lcd.write("Sunday");
        break;

    case 8:
        lcd.write("PD");
        lcd.gotoXY(0, 1);
        lcd.write("Ludicrous");
        break;

    case 9:
        lcd.write("Sensor");
        lcd.gotoXY(0, 1);
        lcd.write("Show");
        break;
        
    case 10:
    if (showspeed)
    {
      lcd.write("Show");
      lcd.gotoXY(0,1);
      lcd.write("Sensor");
    }
    else if (!showspeed)
    {     
        lcd.write("Show");
        lcd.gotoXY(0, 1);
        lcd.write("Speed");
    }
    break;
    return;
}
}

//The main root of the library, this is run in the void loop in the main program file to choose
//and run different functionalities
void Menu::Runprogram() { 
    program = chooseprogram();
    switch (program) {
    case 1:
        //prog1
        driving.Followline(showspeed);
        break;
    case 2:
        //prog2
        driving.Square();
        break;
    case 3:
        //prog2
        driving.Circle();
        break;
    case 4:
        //prog2
        driving.Straight();
        break;
    case 5:
        //prog2
        driving.Zigzag();
        break;
    case 6:
        // PD Normal
        driving.runPD(3.0f, 1.0f, 100, true, false, showspeed);
        break;
    case 7:
        // PD Sunday
        driving.runPD(3.0f, 1.0f, 80, true, false, showspeed);
        break;

    case 8:
        //PD Lucicrous
        driving.runPD(3.0f, 1.0f, 120, true, true, showspeed);
        break;

    case 9:
        // Sensor Show
        driving.runProxy();
        break;
    case 10:
    // Toggles what to display on the lcd screen while driving on the line
    showspeed = !showspeed;
        break;
    }

    return;
}
