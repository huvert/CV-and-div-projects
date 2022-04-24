#ifndef DISPENSER_H
#define DISPENSER_H

#include <SocketIoClient.h>   //Import the Socket.io library, this also imports all the websockets
#include <ESP32Servo.h>       //Import the servomotor library

#define DISP_ECHOPIN 5
#define DISP_TRIGPIN 4
#define DISP_FSRPIN 34        //pressuresensor
#define DISP_SERVOPIN 2       //servomotor
#define DISP_SONIC_INTERVAL 5 // time(in ms) between every reading on the sonic sensor
#define DISP_ARRAYLENGTH 40
#define DISP_FSRMIN 0
#define DISP_FSRMAX 167       //when resolution for analogread is set to 8-bit, range 0-255. Calibrated value.


class Dispenser
{
  public:
    SocketIoClient websocket;

    //------- variables -------
    char outerArr[DISP_ARRAYLENGTH];  // Used as a temporary datacontainer for sending data to socketIo
    int bottleLevel;                  // Last updated value of level in %

    //------- functions -------
    Servo Myservo;
    Dispenser(float sonicMIN, float sonicMAX);
    void init(const char* server_ip, const int server_port);
    void dispenserProgram();
    void setParameters(float sonicMIN, float sonicMAX);
    void sendValues();
    void textWrapper(char* outerArr, String text);

  private:
    //------- variables -------
    int var;    // switch case variabel
    unsigned long previousTime;
    // variables ultrasonic-sensor
    float sonicMin;
    float sonicMax;
    bool trigPinState;
    int long duration;
    int distance;
    unsigned long previousSonicTime;
    float sonicCm;

    // variables FSR-sensor
    String FrsValueStr;
    int frsValue;

    //------- functions -------
    float sonicListen();
    int readFsr();
    String updateSensorValues();
};

// This function needs to be outside the class for it to work properly.
// used '_functionname' to reduce the probability for redefinition.
void _maintenance(const char* payload, size_t length);

#endif
