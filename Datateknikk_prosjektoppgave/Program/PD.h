#ifndef PD_h
#define PD_h
#include "Proxysensors.h"

class PD_controller
{
private:
    void calcDeviation(uint8_t& PV);
    int calcP();
    int calcD();

public:
    float kP, kD;                                                     // Parameters
    uint8_t setPoint;                                                 // Parameter:   Set Point
    int e;                                                            // Deviation
    int D;                                                            // Stores calculated D (P�drag)
    int prev_e;
    unsigned long dTime;
    PD_controller();
    float setParameters(float& kp, float& kd, uint8_t& sp);
    int calculate(uint8_t& PV);
};



/*  - Class used specificly to Steering vehichle: Zumo32U4.
    - Handles both steering and speed based on deviation.
    - 4th Argument is a functionPointer which should be pointed to reading of PV with Range: 0 ... 255
    - Note that this class does not have a default constructor. This is because punctionpointer has to be specified in constructor.
    - See accomoning PDF for more info. */
class SteeringPD : PD_controller
{
private:
    int calcNetPositiveSpeed();
    bool checkTapeMissing(uint8_t PV);

public:
    int NPS_maxSpeed;
    uint8_t(*getPV)();                              
    ::PD_controller PD;
    SteeringPD(float kp, float kd, uint8_t sp, int refSpeed, uint8_t(*ReadSensorFunction)());
    void setParameters(float kp, float kd, uint8_t sp, int maxSpeed);
    bool runSteeringPD_andCheckCourse();
};
#endif
