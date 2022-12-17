
/****************************************************************************************************************************
  RP2040-Ethernet_AdvancedWebServer.ino
  From: https://github.com/khoih-prog/WebSockets2_Generic
  For RP2040 with Ethernet module/shield.
  Based on and modified from Gil Maimon's ArduinoWebsockets library https://github.com/gilmaimon/ArduinoWebsockets
  to support STM32F/L/H/G/WB/MP1, nRF52, RP2040, SAMD21/SAMD51, SAM DUE, Teensy boards besides ESP8266 and ESP32
  The library provides simple and easy interface for websockets (Client and Server).

  Built by Khoi Hoang https://github.com/khoih-prog/Websockets2_Generic
  Licensed under MIT license
 *****************************************************************************************************************************/

#include "defines.h"
#include <Ethernet_Generic.h>
#include <WebSockets2_Generic.h>
#include <EthernetWebServer.h>
#include <ArduinoJson.h>

using namespace websockets2_generic;

#define WEBSOCKETS_PORT 8080
#define USE_THIS_SS_PIN 17

// Server
EthernetWebServer server(80);
IPAddress serverIP(192, 168, 8, 101);
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
WebsocketsServer SocketsServer;

// Client
WebsocketsClient client;
bool clientConnected = false;
unsigned long lastAlive = 0; // timestamp of last alive message from client

// JSON
StaticJsonDocument<128> jDocRx; // https://arduinojson.org/v6/assistant/#/step1 to determine size

// declare functions first

void sendError(String clientCallbackId, String command, String errorMessage)
{
  StaticJsonDocument<128> response;
  response["clientCallbackId"] = clientCallbackId;
  response["command"] = command;
  response["error"]["message"] = errorMessage;
  char buff[128];
  size_t len = serializeJson(response, buff);
  client.send(buff, len);
}

void sendSuccess(String clientCallbackId, String command, int value)
{
  StaticJsonDocument<128> response;
  response["clientCallbackId"] = clientCallbackId;
  response["command"] = command;
  response["data"]["value"] = value;
  char buff[128];
  size_t len = serializeJson(response, buff);
  client.send(buff, len);
}

void initSerial()
{
  Serial.begin(115200);
  while (!Serial && millis() < 5000)
    ;
  Serial.println("\nStarting RP2040 Ethernet Websocket Server on " + String(BOARD_NAME));
  Serial.println(WEBSOCKETS2_GENERIC_VERSION);
}

void initEthernet()
{
  pinMode(USE_THIS_SS_PIN, OUTPUT);
  digitalWrite(USE_THIS_SS_PIN, HIGH);
  Ethernet.init(USE_THIS_SS_PIN);
  Ethernet.begin(mac, serverIP);
}

void initSocketServer()
{
  Serial.print("WebSockets Server IP address: ");
  Serial.println(Ethernet.localIP());
  SocketsServer.listen(WEBSOCKETS_PORT);

  Serial.print(SocketsServer.available() ? "Websocket server running and ready on " : "ERROR: Server not running on ");
  Serial.println(BOARD_NAME);
  Serial.print("IP address: ");
  Serial.print(Ethernet.localIP());
  Serial.print(", Port: ");
  Serial.println(WEBSOCKETS_PORT); // Websockets Server Port

  server.begin();
}

void setup()
{
  initSerial();
  initEthernet();
  initSocketServer();
}

void loop()
{
  server.handleClient();
  if (!clientConnected)
  {
    client = SocketsServer.accept();
    clientConnected = true;
    lastAlive = millis();
  }

  if (((millis() - lastAlive) > 5000) && clientConnected) // The client sends a ping every 2 seconds, so timeout on 5
  {
    client.close();
    clientConnected = false;
    Serial.println("connection closed: client inactive");
  }

  if (client.available())
  {
    WebsocketsMessage msg = client.readNonBlocking();
    deserializeJson(jDocRx, msg.data()); // Deserialize message data into jDocRx object

    String command = jDocRx["command"];  // The message contains a command to determine the command to be handled
    String clientCallbackId = jDocRx["clientCallbackId"]; // The messages contains an id to determine the kind of message and destination

    if (command)
    {
      // client sends "ping" command every 2 seconds to keep alive, but set alive for any command sent anyway
      lastAlive = millis(); // Timestamp the last alive message from client
    }
    if (command == "move")
    {
      float x = jDocRx["data"]["x"]; // left is -1, right is 1, 0 is no turning
      float y = jDocRx["data"]["y"]; // fiorward is 1, backward is -1, 0 is no movement
      //handle car movement
      Serial.print("x: ");
      Serial.print(x);
      Serial.print(", y: ");
      Serial.println(y);
      //if success:
      sendSuccess(clientCallbackId, command, 1);
      //if there's an error, like the command says forward but sensors say you can't move forward, send an error:
      //sendError(clientCallbackId, command, "Can't move forward");
    }
    if (command == "testError")
    {
      sendError(clientCallbackId, command, "Test error");
    }

    if (command != "ping") {
      Serial.print("Got message: ");
      Serial.println(msg.data());
    }
  }
}
