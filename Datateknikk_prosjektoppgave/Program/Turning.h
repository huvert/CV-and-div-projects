#ifndef Turning_h
#define Turning_h
#include "Music.h"

class Turning
{
private:
    const int32_t turnAngle45 = 0x20000000;
    const int32_t turnAngle90 = turnAngle45 * 2;
    const int32_t turnAngle1 = (turnAngle45 + 22) / 45;
    int turnSpeed = 100;

public:
    void turnSensorSetup();
    void turnSensorReset();
    void turnSensorUpdate();
    void turnLeft(int degrees);
    void turnRight(int degrees);
    int Checkangle();
    void WaitforAngle(int degrees);
};
#endif
