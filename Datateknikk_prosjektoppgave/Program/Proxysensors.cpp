#include "ProxySensors.h"
#include "Menu.h"

// -- Proxy Sensor Parameters --
#define Prox_forwardSpeed 80
#define Prox_steeringSpeed_low 40
#define Prox_steeringSpeed_high 110

Proxysensor::Proxysensor() {
    proxysensors.initFrontSensor();
    proxy_timer = 0;
}

bool Proxysensor::avoidCollision()
{
    calcAll();
    if (sum >= 12) {
        Driving::driving->motors.setSpeeds(0, 0);
        return(true);
    }
    return(false);
}

/*  - Tries to navigate using 1 proxysensor
    - Returns as true if collision is detected.   */
bool Proxysensor::avoidCollision_andTryToDodge() 
{
    bool flag = false;                
    if (millis() > proxy_timer + 800) {       // This forces vehichle to turn for atleast 800ms before updating the speed again.
        flag = true;                          // Note that Emergency Brake overrides this.
    }
    calcAll();
    if (sum >= 12) {
        // Emergency Brake!
        Driving::driving->motors.setSpeeds(0, 0);
        return(true);
    }
    else if (flag == true) {
        if (sum == 0) {
            // Nothing detected -> No worries 
            driveForward();
        }
        else {
            // Something detected -> Try to manouver
            if (diff >= 1) {
                steerLeft();
            }
            else if (diff <= -1) {
                steerRight();
            }
            else {
                driveForward();
            }
        } 
    }
    return(false);
}

void Proxysensor::steerLeft() {
    Driving::driving->motors.setSpeeds(Prox_steeringSpeed_low, Prox_steeringSpeed_high);
}

void Proxysensor::steerRight() {
    Driving::driving->motors.setSpeeds(Prox_steeringSpeed_high, Prox_steeringSpeed_low);
}

void Proxysensor::driveForward() {
    Driving::driving->motors.setSpeeds(Prox_forwardSpeed, Prox_forwardSpeed);
}

void Proxysensor::calcAll() {
    proxysensors.read();               
    calcDiff();
    calcSum();
}

void Proxysensor::calcDiff() {
    diff = proxysensors.countsFrontWithRightLeds() - proxysensors.countsFrontWithLeftLeds();
}

void Proxysensor::calcSum() {
    sum = proxysensors.countsFrontWithRightLeds() + proxysensors.countsFrontWithLeftLeds();
}
