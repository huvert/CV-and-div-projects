#include "PD.h"
#include "menu.h"

// -- Parameters --
// Used for advanced tuning. See accompanied PDF for more info.
#define derivationTime 5                // How often D is updated (Default value: 5)
#define minSpeed 50                     // Minimum net-positive Speed
#define powerFactor 3.0f                // Default: 1.0f ... 3.0f (Raising this value further may result in damaging the vehichle and is done on own responsibility.)  
#define slopeFactor 0.02f               // Default: 0.02f

PD_controller::PD_controller()          // Constructor. Sets default values.
{
    kP = 1.0f;
    kD = 0.0f;
    setPoint = 127;
    e = 0;
    D = 0;
    prev_e = 0;
    dTime = 0;
}

float PD_controller::setParameters(float& kp, float& kd, uint8_t& sp)
{
    kP = kp;
    kD = kd;
    setPoint = sp;
}

/*  Takes Process Value (PV) as input   Range: 0 ... 255
    Returns Modulated Value MV.         Range: -255 ... 255 */
int PD_controller::calculate(uint8_t& PV)
{
    calcDeviation(PV);
    int MV = 0;
    MV += calcP();
    if (kD > 0) {
        MV += calcD();
    }
    MV = constrain(MV, -255, 255);
    return(MV);
}

void PD_controller::calcDeviation(uint8_t& PV){
    e = setPoint - PV;
}

int PD_controller::calcP(){
    return(e * kP);
}

int PD_controller::calcD()
{
    if ((derivationTime + dTime) < millis()) {
        D = (kD * (e - prev_e)) / derivationTime;
        prev_e = e;
        dTime = millis();
    }
    return(D);
}




/*  - refSpeed:  Sets NetPositiveSpeed reference speed.  Range: (50 ... 255)
    - 5th Argument is a functionpointer and should be pointing to reading of PV.  */
SteeringPD::SteeringPD(float kp, float kd, uint8_t sp, int refSpeed, uint8_t(*ReadSensorFunction)())
{
    PD.setParameters(kp, kd, sp);
    getPV = ReadSensorFunction;
    NPS_maxSpeed = refSpeed;
}

void SteeringPD::setParameters(float kp, float kd, uint8_t sp, int maxSpeed)
{
    PD.setParameters(kp, kd, sp);
    NPS_maxSpeed = maxSpeed;
}

/*  Function does:
    - Calculate PD and update motorspeeds.
    - Checks if tape is missing and corrects itself.
    - Will return as false if vehichle has lost its course (aka: tapes been missing for too long) */
bool SteeringPD::runSteeringPD_andCheckCourse()
{
    uint8_t PV = getPV();                     // Reads Sensor and stores value to PV.
    int mv = PD.calculate(PV);
    int NPS = calcNetPositiveSpeed();
    float leftSpeed = round(NPS - ((mv / 255.0) * NPS * powerFactor));
    float rightSpeed = round(NPS + ((mv / 255.0) * NPS * powerFactor));
    leftSpeed = constrain(leftSpeed, -400, 400);
    rightSpeed = constrain(rightSpeed, -400, 400);
    if (checkTapeMissing(PV)) {
        return(false);
    }
    Driving::driving->motors.setSpeeds(leftSpeed, rightSpeed);
    return(true);
}

/*  - Calculates the NetPositiveSpeed (NPS) of the vehichle.
        - This prevents vehicle from stopping when reaching 0 Deviation.
        - The NPS decreases when deviation increases. And vise versa.
        - This helps the system react to changes and correct the course, while also enabling vehicle
          to run on a higher positive net speed when deviation is low and small to no corrections are needed. */
int SteeringPD::calcNetPositiveSpeed()
{
    int netPositiveSpeed = NPS_maxSpeed - (slopeFactor * e * e);
    if (netPositiveSpeed < minSpeed) {
        netPositiveSpeed = minSpeed;
    }
    return (netPositiveSpeed);
}

/* Function will search for the tape in 2 seconds. Returns as True if no tape was found.   */
bool SteeringPD::checkTapeMissing(uint8_t PV)
{
    if (PV < 10 || PV > 245)  {
        delay(2);
        PV = getPV();       // Reads a second time before (helps avoids setting motors.speeds to 80 when a blipp occurs from the sensor).
        if (PV < 10 || PV > 245) {
            unsigned long tapeTimer = millis();
            Driving::driving->motors.setSpeeds(80, 80);
            while (tapeTimer + 2000 > millis()) {
                PV = getPV();
                if (PV > 15 || PV > 245) {
                    delay(2);
                    PV = getPV();
                    if (PV > 15 || PV > 245) {        // Reads a second time to avoid blipps from sensor.
                        return(false);
                    }
                }
            }
            return(true);
        }
    }
    return(false);
}
