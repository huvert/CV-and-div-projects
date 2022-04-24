
#include "Dispenser.h"      // Also includes SocketIoClient.h, websockets and esp servo.
#include <WiFiMulti.h>      // Used for internetconnection

#define SERVER_IP "109.189.14.16"
#define SERVER_PORT 4000

const char* SSID = "wifi";
const char* PASSWORD = "password";

WiFiMulti WiFiMulti;                      //Declare an instance of the WiFiMulti library
Dispenser dispenser = Dispenser(10, 30);  // arguments is the range of the sonic sensor (distance)


void setup() {
  /* Basic arduino setup*/
  Serial.begin(9600);
  Serial.setDebugOutput(true);  //Set debug to true (during ESP32 booting)
  Serial.println();
  Serial.println();
  Serial.println();
  for (uint8_t t = 4; t > 0; t--) { //More debugging
    Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  // Wifi setup
  WiFiMulti.addAP(SSID, PASSWORD);
  Serial.println("Connecting to wifi");
  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.println(".");
    delay(100);
  }
  Serial.println("Connected to WiFi successfully!"); //When we have connected to a WiFi hotspot

  // dispenser setup
  dispenser.init(SERVER_IP, SERVER_PORT);

  /* NOTE:  you can add more eventlisteners to the system as showed below.
            But the server you are connecting to has to use the same identifiers for it to work.
  Dispenser.websocket.on("identifier", function*)
  Dispenser.webSocket.on("event", socket_event);
  */
}


void loop() {
  // Runs the dispenser ESP program
  dispenser.dispenserProgram();
}
