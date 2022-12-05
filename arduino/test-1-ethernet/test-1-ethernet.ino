#include "config.h"
#include <Wire.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>
#include <OSCBundle.h>

/*PICO SPI PIN:
  MOSI  <---> GP19
  MISO  <---> GP16
  SCK <---> GP18
  CS  <---> GP17
*/

EthernetUDP Udp;
long currentMillis = 0;
long previousMillis = 0;

int dd = 101;

//the Arduino's IP
IPAddress ip(192, 168, 8, dd);
IPAddress outIp(192, 168, 8, 255);

//port numbers
const unsigned int inPort = 10000 + dd;
const unsigned int outPort = 20000;
String stepperName[NUM_OF_STEPPER] = { "A"};


bool isSetupDone = false;
void setup() {
  Serial.begin(115200);
  delay(5000);
  ethernetSetup();
}

void setup1() {
  Serial.begin(115200);
  delay(8000);
  isSetupDone = true;
}

void loop() {
  if (isSetupDone) {
    ethernetLoop();
    serialLoop();
  }
}


void loop1() {
  //stepperLoop();
}
