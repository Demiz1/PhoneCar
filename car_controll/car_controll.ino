#include <Ticker.h>
#include <Servo.h>
#include <ESP8266WiFi.h>
//#include <ESP8266WebServer.h>
#include <ESP8266WebServerSecure.h>
#include <WebSocketsServer.h>
#include "SSD1306Wire.h"
#include "carControlWebPage.h"
#include "carControlAdminWebPage.h"
#include "certificates.h"


const bool verboseLoggBol = true;


//screen---------------------------------------------------------
#define screenPin1 D3
#define screenPin2 D5
SSD1306Wire display(0x3c, screenPin1, screenPin2);

#define display_Init 0
#define display_drive 1

#define maxRowLength 122 //full row of "A"
#define maxRows 5
#define logMaxLength 50
String rows[maxRows];

int screenMode = 0;

//---------------------------------------------------------------


//car------------------------------------------------------------
#define steringServoPin D7
#define driveServoPin D8
#define slowestUpdateTime 100
Servo driveServo;
Servo turnServo;
int drive = 50;
int turn = 50;
unsigned long newValuesTime = 0; //millis we updated the turn and drive values
unsigned long lastestCarWriteTime = 0; //millis we accually wrote the values to the car
Ticker carUpdateInterupt;


//---------------------------------------------------------------


//wifi ----------------------------------------------------------
#define ssid "CaseCar"
String currentSsid;

//ESP8266WebServer caseCarWebServer(80);
BearSSL::ESP8266WebServerSecure caseCarWebServer(443);

WebSocketsServer caseCarWebSocket = WebSocketsServer(81);




#define WEBSOCKETS_SERVER_CLIENT_MAX 50
bool driveRequests[WEBSOCKETS_SERVER_CLIENT_MAX];
int admin;
int clientInControl = -1; //-1 noone in control, 0 - WEBSOCKETS_SERVER_CLIENT_MAX user in control.
void ICACHE_RAM_ATTR updateCarMovement();
#define socketMessage_driveComand     0
#define socketMessage_requestUsers    1
#define socketMessage_takeControl     2
#define socketMessage_giveControl     3
#define socketMessage_wantToDrive     4
#define socketMessage_dontWantToDrive 5
#define socketMessage_iAmAdmin        6
#define socketMessage_whoIsInControl  7
#define socketMessage_whoIsAdmin      8
//#define socketMessage_

//---------------------------------------------------------------

void setup() {
  // put your setup code here, to run once:
  Serial.begin(500000);
  while(!Serial) delay(10);
  Serial.println("Serial port open======================================");

  //init screen
  initDislpay();
  //init car
  carInit();
  //init ap
  initWifi();
}
void loop() {
  // put your main code here, to run repeatedly:
  wifiLoopHandler();
  //update screen
  updateDisplay();
  //update car
  //car have ticker interrupt
}

void displayAndSerialLog(String message){
  Serial.println(message);
  loggScreenApend(message);
}

void displayAndSerialLog(const char* message){
  Serial.println(message);
  loggScreenApend(message);
}
/*
void verboseLogg(String message){
  if(verboseLoggBol){
    displayAndSerialLog(message);
  }
}
*/

void verboseLogg(const char* message){
  if(verboseLoggBol){
    displayAndSerialLog(message);
  }
}

char* IPToCharArr(IPAddress address){
  char chars[5];
  sprintf(chars,"%d.%d.%d.%d", address[0], address[1], address[2], address[3]);
  chars[4] = '\n';
  return chars;
}
