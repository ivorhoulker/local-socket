/****************************************************************************************************************************
  RP2040-Ethernet_AdvancedWebServer.ino
  From: https://github.com/khoih-prog/WebSockets2_Generic
  For RP2040 with Ethernet module/shield.
  Based on and modified from Gil Maimon's ArduinoWebsockets library https://github.com/gilmaimon/ArduinoWebsockets
  to support STM32F/L/H/G/WB/MP1, nRF52, RP2040, SAMD21/SAMD51, SAM DUE, Teensy boards besides ESP8266 and ESP32
  The library provides simple and easy interface for websockets (Client and Server).

  Built by Khoi Hoang https://github.com/khoih-prog/Websockets2_Generic
  Licensed under MIT license

  From (https://arduino-pico.readthedocs.io/en/latest/multicore.html)

  By adding a setup1() and loop1() function to your sketch you can make use of the second core.
  Anything called from within the setup1() or loop1() routines will execute on the second core.

  Functions:
  1) Pausing Cores
  - void rp2040.idleOtherCore();
  - void rp2040.resumeOtherCore();

  2) Communicating Between Cores
  - void rp2040.fifo.push(uint32_t);
  - bool rp2040.fifo.push_nb(uint32_t);
  - bool rp2040.fifo.push_nb(uint32_t);
  - bool rp2040.fifo.pop_nb(uint32_t *dest);
  - int rp2040.fifo.available();
 *****************************************************************************************************************************/

#include <Arduino.h>

#include "defines.h"
#include <Ethernet_Generic.h>
#include <WebSockets2_Generic.h>
#include <EthernetWebServer.h>
#include <ArduinoJson.h>
// #include "pico/stdlib.h"
// #include "pico/multicore.h"

using namespace websockets2_generic;

// Env
#define THIS_VEHICLE_ID "Jimbob"
#define THIS_VEHICLE_TYPE "SMALL_CAR"


#define WEBSOCKETS_PORT 8080
#define USE_THIS_SS_PIN 17

// Server
EthernetWebServer server(80);
IPAddress serverIP(172, 20, 10, 2);
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

// IPAddress serverGateway(192, 168, 48, 220);
// IPAddress serverSubnet(255, 255, 255, 0);
WebsocketsServer SocketsServer;

// Client
WebsocketsClient client;
bool clientConnected = false;
unsigned long lastAlive = 0;  // timestamp of last alive message from client

// JSON
StaticJsonDocument<192> receivedMessage;  // https://arduinojson.org/v6/assistant/#/step1 to determine size

// Handlers
// TODO: implement the handlers below, and return false if there's an error or condition that prevents them from running
// can change bool to int error codes if different types of error need reporting

bool handleMove(int x, int y)  // x is left/right, -1 is left, 1 is right. y is forward/backward - y:1 is forward, y:-1 is backward.
{
  // commands sent are stateful, so carry on looping last command until another is sent.
  return true;
}

bool handleTilt(int amount)  // positive is up, negative is down, 0 is stop.
{
  // commands sent are stateful, so carry on looping last command until another is sent.
  return true;
}

bool handleResetTilt() {
  // animate the tilt back to level position
  return true;
}

bool handleColor(String hue) {
  // set the color of any car LEDs to the hue of the user's avatar
  return true;
}

bool handleEmoji(String emojiName)  // emojiName can currently be 'happy' | 'sad' | 'yes' | 'no'
{
  // display an emoji on LED Matrix - need to decide what emoji names to implement
  return true;
}

// Car to client signalling
// Maybe need more stuff for outdoor cars

void signalRfidDistance(float distance) {
  StaticJsonDocument<128> message;
  message["signal"] = "rfid";
  message["data"]["distance"] = distance;
  sendToClient(message);
}

// Socket
void sendToClient(StaticJsonDocument<192> json) {
  char buff[192];
  size_t len = serializeJson(json, buff);
  client.send(buff, len);
}

void initSerial() {
  Serial.begin(115200);
  while (!Serial && millis() < 5000)
    ;
  Serial.println("\nStarting RP2040 Ethernet Websocket Server on " + String(BOARD_NAME));
  Serial.println(WEBSOCKETS2_GENERIC_VERSION);
}

void initEthernet() {
  pinMode(USE_THIS_SS_PIN, OUTPUT);
  digitalWrite(USE_THIS_SS_PIN, HIGH);
  Ethernet.init(USE_THIS_SS_PIN);
  Ethernet.begin(mac);  //, serverGateway, serverSubnet
}

void initSocketServer() {
  Serial.print("WebSockets Server IP address: ");
  Serial.println(Ethernet.localIP());
  for (byte i = 0; i < 6; i++) {
    Serial.print(mac[i], HEX);
    if (i < 5) Serial.print(":");
  }
  Serial.println("- mac address");
  Serial.print("hostName: ");
  Serial.println(hostName);


  SocketsServer.listen(WEBSOCKETS_PORT);

  Serial.print(SocketsServer.available() ? "Websocket server running and ready on " : "ERROR: Server not running on ");
  Serial.println(BOARD_NAME);
  Serial.print("IP address: ");
  Serial.print(Ethernet.localIP());
  Serial.print(", Port: ");
  Serial.println(WEBSOCKETS_PORT);    // Websockets Server Port
  Serial.println(String(HOST_NAME));  // Host name

  server.begin();
}

void checkStatus() {
  if (((millis() - lastAlive) > 5000) && clientConnected)  // The client sends a ping every 2 seconds, so timeout on 5
  {
    Serial.print(String(millis()));
    Serial.print(" - ");
    Serial.println(String(lastAlive));

    // phone has lost connection here
    client.close();
    clientConnected = false;
    Serial.println("Connection closed: client inactive for five seconds. Stopping motors.");
    handleMove(0, 0);  // this stops any currently looping movement
    handleTilt(0);     // this stops any ongoing tilting
  }
}

void setup() {
  initSerial();
  initEthernet();
  initSocketServer();
}

void handleCommand(String str) {
  String cmd = str.substring(0, 1);
  if (cmd == "M") {
    int x = (int)str.charAt(1);
    int y = (int)str.charAt(2);
    handleMove(x, y);
    return;
  } 
   if (cmd == "C") {
    String hue = str.substring(1, 4);
    handleColor(hue);
    return;
  } 
}

void onMessagesCallback(WebsocketsMessage message) {
  lastAlive = millis();
  handleCommand(message.data())
  // StaticJsonDocument<192> response;

  // response["clientCallbackId"] = receivedMessage["clientCallbackId"];
  // response["command"] = receivedMessage["command"];
  // response["data"] = receivedMessage["data"];
  // String command = receivedMessage["command"];
  // deserializeJson(receivedMessage, message.data());

  // if (command == "move") {
  //   float x = receivedMessage["data"]["x"];  // left is -1, right is 1, 0 is no turning - not sure whether to normalize vector, could be ints if we don't
  //   float y = receivedMessage["data"]["y"];  // forward is 1, backward is -1, 0 is no movement
  //   Serial.println("MOVING: x: " + String(x) + ", y: " + String(y));
  //   // handle car movement
  //   bool success = handleMove(x, y);
  //   if (success) {
  //     response["data"]["success"] = true;
  //   } else {
  //     response["error"]["message"] = "Failed to move, something is in the way";
  //   }
  // } else if (command == "tilt") {
  //   float amount = receivedMessage["data"];  // start tilting up is 1, start tilting down is -1, stop is 0
  //   Serial.println("TILTING: " + String(amount));
  //   // handle tilt
  //   bool success = handleTilt(amount);
  //   if (success) {
  //     response["data"]["success"] = true;
  //   } else {
  //     response["error"]["message"] = "Failed to tilt, already at max/min";
  //   }
  // } else if (command == "resetTilt") {
  //   Serial.println("RESETTING TILT");
  //   // handle tilt
  //   bool success = handleResetTilt();
  //   if (success) {
  //     response["data"]["success"] = true;
  //   } else {
  //     response["error"]["message"] = "Failed to reset tilt";
  //   }
  // } else if (command == "color") {
  //   int hue = receivedMessage["data"]["hue"];  // data contains hue, saturation and lightness (brightness), but maybe we only need hue and saturation
  //   int saturation = receivedMessage["data"]["saturation"];
  //   Serial.println("COLOR: hue: " + String(hue) + ", saturation: " + String(saturation));
  //   // handle color
  //   bool success = handleColor(hue, saturation);
  //   if (success) {
  //     response["data"]["success"] = true;
  //   } else {
  //     response["error"]["message"] = "Failed to set color";
  //   }
  // } else if (command == "emoji") {
  //   String emojiName = receivedMessage["data"]["emojiName"];  // emojiName is a string like "happy", "thumbsUp" etc.
  //   Serial.println("EMOJI: " + emojiName);
  //   // handle color
  //   bool success = handleEmoji(emojiName);
  //   if (success) {
  //     response["data"]["success"] = true;
  //   } else {
  //     response["error"]["message"] = "Failed to set emoji";
  //   }
  // } else if (command == "testError") {
  //   response["error"]["message"] = "This is a test error response";
  // } else if (command == "ping") {
  //   // no need to log or respond
  //   Serial.println("PING.");
  //   client.pong();
  //   return;
  // } else {
  //   response["error"]["message"] = "This command is not handled by the socket server";
  //   Serial.print("Got unhandled message: ");
  //   Serial.println(message.data());
  // }
  // sendToClient(response);  // send the response built above
}

void onEventsCallback(WebsocketsEvent event, String data) {
  (void)data;
  if (event == WebsocketsEvent::ConnectionOpened) {
    if (!clientConnected)
      clientConnected = true;

    Serial.println("Connnection Opened");
    lastAlive = millis();
  } else if (event == WebsocketsEvent::ConnectionClosed) {
    if (clientConnected)
      clientConnected = false;

    Serial.println("Connnection Closed");
  } else if (event == WebsocketsEvent::GotPing) {
    if (!clientConnected)
      clientConnected = true;

    Serial.println("Got a Ping!");
    lastAlive = millis();
    client.send("Pong!");
  } else if (event == WebsocketsEvent::GotPong) {
    if (!clientConnected)
      clientConnected = true;

    Serial.println("Got a Pong!");
    lastAlive = millis();
  }
}

void loop() {
  if (!clientConnected) {
    client = SocketsServer.accept();
    if (client.available()) {
      client.onMessage(onMessagesCallback);
      client.onEvent(onEventsCallback);
      Serial.println("Client connected.");
      clientConnected = true;
      lastAlive = millis();
      // phone is connected here, send a welcome message so the phone knows what vehicle it's attached to
      StaticJsonDocument<192> message;
      message["signal"] = "welcome";
      message["data"]["vehicleId"] = THIS_VEHICLE_ID;
      message["data"]["vehicleType"] = THIS_VEHICLE_TYPE;
      sendToClient(message);
    }
  }
  client.poll();
  checkStatus();
}
