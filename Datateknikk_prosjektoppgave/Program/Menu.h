#ifndef Menu_h
#define Menu_h
#include "Driving.h"

class Menu
{
private:
    int program;
    int buttonpressed;
    bool showspeed;
                                        
public:
    Driving driving;                    
    void Initialize();
    int WaitforButton();
    int chooseprogram();
    void DisplayProgram();
    void Runprogram();
    Zumo32U4LCD lcd;
    Zumo32U4ButtonA ButtonA;
    Zumo32U4ButtonB ButtonB;
    Zumo32U4ButtonC ButtonC;
    static Menu* menu; //Declaring a static
};
#endif
