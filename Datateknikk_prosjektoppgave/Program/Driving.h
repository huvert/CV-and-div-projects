
#ifndef Driving_h
#define Driving_h
#include "PD.h"
#include "Encoder.h"
 
class Driving
{
private:
    int pos; 

public:
    Turning turning;
    Encoder encoder;
    void Calibrate();
    void Followline(bool speeddisplay);
    void Reglinefollow();
    void Test(bool speeddisplay);
    void Square();
    void Circle();
    void Straight();
    void Zigzag();
    void runPD(float kp, float kd, int refSpeed, bool enableProxy, bool enableMusic, bool speeddisplay);
    void runProxy();
    Zumo32U4Motors motors;
    static Driving* driving; //lager en static

};

uint8_t getLineSensorReading();

#endif
