/****************************************************************************************************************************
  RP2040W-Server.ino
  For RP2040W with CYW43439 WiFi.

  Based on and modified from Gil Maimon's ArduinoWebsockets library https://github.com/gilmaimon/ArduinoWebsockets
  to support STM32F/L/H/G/WB/MP1, nRF52, SAMD21/SAMD51, RP2040 boards besides ESP8266 and ESP32


  The library provides simple and easy interface for websockets (Client and Server).

  Built by Khoi Hoang https://github.com/khoih-prog/Websockets2_Generic
  Licensed under MIT license
 *****************************************************************************************************************************/
/****************************************************************************************************************************
  RP2040W Websockets Server : Minimal RP2040W Websockets Server

  This sketch:
        1. Connects to a WiFi network
        2. Starts a websocket server on port 8080
        3. Waits for connections
        4. Once a client connects, it wait for a message from the client
        5. Sends an "echo" message to the client
        6. closes the connection and goes back to step 3

  Hardware:
        For this sketch you only need a RP2040W board.

  Originally Created  : 15/02/2019
  Original Author     : By Gil Maimon
  Original Repository : https://github.com/gilmaimon/ArduinoWebsockets

*****************************************************************************************************************************/

#include "defines.h"

// Select the IP address according to your local network
IPAddress serverIP(172, 20, 10, 5);

#define WEBSOCKETS_PORT 8080

int status = WL_IDLE_STATUS;

#include <WebSockets2_Generic.h>

using namespace websockets2_generic;

WebsocketsServer server;
WebsocketsClient client;
bool clientConnected = false;
unsigned long lastAlive = 0;     // timestamp of last alive message from client
unsigned long lastPingTime = 0;  // timestamp of last alive message from client

long interval = 3000;

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("Local IP Address: ");
  Serial.println(ip);
}

void checkStatus() {
  if (millis() - lastAlive > 3000 && millis() - lastPingTime > 200 && clientConnected)  // The client sends a ping every 2 seconds, so timeout on 5
  {
    Serial.println("Client inactive for 3 seconds, sending a ping");
    client.send("0");
    lastPingTime = millis();
  }
  if (millis() - lastAlive > 5000 && clientConnected)  // The client sends a ping every 2 seconds, so timeout on 5
  {

    Serial.print(String(millis()));
    Serial.print(" - ");
    Serial.println(String(lastAlive));

    // phone has lost connection here
    client.close();
    clientConnected = false;
    Serial.println("Connection closed: Client inactive for 5 seconds. Stopping motors.");
    handleMove(0, 0);  // this stops any currently looping movement
  }
}

void setup() {

  Serial.begin(115200);
  delay(1000);
  Serial.println("HERE WE GO");
  while (!Serial && millis() < 5000)
    ;

  Serial.println("\nStarting RP2040W-Server on " + String(BOARD_NAME));
  Serial.println(WEBSOCKETS2_GENERIC_VERSION);

  ///////////////////////////////////

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");

    // don't continue
    while (true)
      ;
  }

  Serial.print(F("Connecting to SSID: "));
  Serial.println(ssid);

  status = WiFi.begin(ssid, pass);

  delay(1000);

  // attempt to connect to WiFi network
  while (status != WL_CONNECTED) {
    delay(500);

    // Connect to WPA/WPA2 network
    status = WiFi.status();
  }

  printWifiStatus();

  ///////////////////////////////////

  server.listen(WEBSOCKETS_PORT);

  Serial.print(server.available() ? "WebSockets Server Running and Ready on " : "Server Not Running on ");
  Serial.println(BOARD_NAME);
  Serial.print("IP address: ");
  Serial.print(WiFi.localIP());  //You can get IP address assigned to SAMD
  Serial.print(", Port: ");
  Serial.println(WEBSOCKETS_PORT);  // Websockets Server Port
}

bool handleMove(int x, int y)  // x is left/right, -1 is left, 1 is right. y is forward/backward - y:1 is forward, y:-1 is backward.
{
  // commands sent are stateful, so carry on looping last command until another is sent.
  return true;
}

bool handleColor(String hue) {
  // set the color of any car LEDs to the hue of the user's avatar
  return true;
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
  Serial.println(message.data());
  lastAlive = millis();
  handleCommand(message.data());
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
    client = server.accept();
    if (client.available()) {
      client.onMessage(onMessagesCallback);
      client.onEvent(onEventsCallback);
      Serial.println("Client connected.");
      clientConnected = true;
      lastAlive = millis();
      // phone is connected here
    }
  }
  client.poll();
  checkStatus();
}
