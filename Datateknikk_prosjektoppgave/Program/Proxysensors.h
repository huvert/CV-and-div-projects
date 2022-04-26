#ifndef Proxysensors_h
#define Proxysensors_h
#include "Turning.h"

class Proxysensor {
private:
    void steerLeft();
    void steerRight();
    void driveForward();
    void calcAll();
    void calcDiff();
    void calcSum();

public:
    int8_t diff;
    uint8_t sum;
    unsigned long proxy_timer;
    Proxysensor();
    bool avoidCollision();
    bool avoidCollision_andTryToDodge();
    Zumo32U4ProximitySensors proxysensors;
};

#endif
