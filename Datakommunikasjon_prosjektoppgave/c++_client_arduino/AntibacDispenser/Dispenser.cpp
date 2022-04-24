#include "Dispenser.h"

int maintenanceState = 0;

// Constructor (explained as a "function" which is named the same as class), 
// for setting variables when calling the class in the code. 
Dispenser::Dispenser(float sonicMIN, float sonicMAX)
{
  int var = 0;    // switch case variable
  unsigned long previousTime = 0;
  unsigned long previousSonicTime = 0;
  setParameters(sonicMIN, sonicMAX);
}

//setting parametres from class call
void Dispenser::setParameters(float sonicMIN, float sonicMAX)
{
  sonicMin = sonicMIN;
  sonicMax = sonicMAX;
}

void Dispenser::init(const char* server_ip, const int server_port)
{
  Myservo.attach(DISP_SERVOPIN);
  pinMode(DISP_ECHOPIN, INPUT);
  pinMode(DISP_TRIGPIN , OUTPUT);
  pinMode(DISP_FSRPIN, INPUT);
  Myservo.write(90);

  websocket.on("maintenance", _maintenance);
  websocket.begin(server_ip, server_port);

  textWrapper(outerArr, "esp#Antibac");
  websocket.emit("join-room", outerArr);
}

//function that ties together the functions for the dispenser, by using switch case
void Dispenser::dispenserProgram()
{
  websocket.loop();
  switch (var) {
    // 0: starting the function ultraSonic, which waits for an object within range
    case 0:
      sonicListen();
      if (sonicCm <= sonicMax && sonicCm >= sonicMin) {
        var = 1;
        previousTime = millis();
      }

      // checking for maintenance stopcommand from website.
      if(maintenanceState == 1){
        Serial.println("jobber i andre if i case 0, gir var = 5");
        var = 5;
      }
      break;

    // 1: setting servo to 90 degrees and waits before updating previousTime, and moves on to next step
    case 1:
      Myservo.write(0);
      if (millis() - previousTime >= 2000) {
        var = 2;
        previousTime = millis();
      }
      break;

    // 2: setting servo to 0 degrees and waits before updating previousTime, and moves on to next step
    case 2:
      Myservo.write(90);
      if (millis() - previousTime >= 2000) {
        var = 3;
        previousTime = millis();
      }
      break;

    // 3: stays in this case if object is still within range, if not; moves on to the next step
    case 3:
      sonicListen();
      if (sonicCm <= sonicMin || sonicCm >= sonicMax) {
        var = 4;
      }
      else {
        var = 3;
      }
      break;

    // 4: finishing step that reads the current values and sends them to server, before moving back to case 0.
    case 4:
      sendValues();
      var = 0;
      break;

    // 5: waiting step, waits if 'maintenance stopcommand' is received from server, moves back to case 0 
    // when 'startcommand' is received
    case 5:
      if(maintenanceState == 0){
         var = 0;
      }
      else{
        var = 5;
      }
      break;

    // default step
    default:
      var = 0;
      break;
  }
}


// function that returnes TRUE/FALSE, 1/0
//      1 = stop program for maintenance
//      0 = run as normal
void _maintenance(const char* payload, size_t length){

  String statusAsString(payload);
  int statusMaintenance = statusAsString.toInt();
  if (statusMaintenance == 0){
    maintenanceState = 1;
  }
  else if (statusMaintenance == 1){
    maintenanceState = 0;
  }
  else {
    Serial.print("[ERROR] maintenance payload not recognized");
  }
}


// wrapping values into a string and rewriting it to a char-array which is the only format the server accepts, by
// using the function textWrapper, then transfers the char-array holding the values from ESP32 to server
void Dispenser::sendValues() {
  textWrapper(outerArr, updateSensorValues());
  websocket.emit("res-data", outerArr);
}


// sending a pulse on trig pin recording to the interval set, calculating distance in cm 
// by using the difference in time of sent and reflected pulse
float Dispenser::sonicListen()
{
  if (millis() - previousSonicTime >= DISP_SONIC_INTERVAL) {
    trigPinState = ! trigPinState;
    digitalWrite(DISP_TRIGPIN, trigPinState);
    previousSonicTime = millis();

    if (trigPinState == false) {
      float echoTime = pulseIn(DISP_ECHOPIN, HIGH);
      sonicCm = (echoTime / 2) / 29.1;
    }
  }
  return sonicCm;
}


// reading levelvalue and returning it as a percentage value
int Dispenser::readFsr()
{
  analogReadResolution(8);
  bottleLevel = analogRead(DISP_FSRPIN);
  bottleLevel = map(bottleLevel, DISP_FSRMIN, DISP_FSRMAX, 0, 100); //mapping analogsignal to percentage
  return bottleLevel;
}


//reading sensorvalues and writing them to string format
String Dispenser::updateSensorValues()
{
  int fsrReading = readFsr();
  String frsValue = String(fsrReading);
  return frsValue;
}


// takes a string as argument and wrapping it in "..." and writing it to a char array
void Dispenser::textWrapper(char* outerArr, String text) {
  text = "\"" + text + "\""; // returns: "text"
  char cnt[DISP_ARRAYLENGTH];
  text.toCharArray(cnt, DISP_ARRAYLENGTH);
  for (int i = 0; i < DISP_ARRAYLENGTH; ++i) {
    outerArr[i] = cnt[i];
  }
}
