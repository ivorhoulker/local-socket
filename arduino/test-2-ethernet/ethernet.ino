
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
}; // you can find this written on the board of some Arduino Ethernets or shields

void ethernetSetup() {
  Ethernet.init(17);
  Ethernet.begin(mac, ip);
  Udp.begin(inPort);
  Serial.print("Local IP : ");
  Serial.println(Ethernet.localIP());
  Serial.print("OSC PORT : ");
  Serial.println(inPort);
}

void ethernetLoop() {
  static int32_t sequenceNumber = 0;
  OSCBundle bundleIN;
  currentMillis = millis();
  int size;
  if ( (size = Udp.parsePacket()) > 0)
  {
    while (size--)
      bundleIN.fill(Udp.read());
    if (!bundleIN.hasError())
    {
      // handle incoming message
      bundleIN.route("/command" , handleCommand);
      Serial.println("GOT BUNDLE");

      // send back confirmation
      OSCBundle bundleOut;
      bundleOut.add("/got/command").add(sequenceNumber++);
      Udp.beginPacket(Udp.remoteIP(), outPort);
      bundleOut.send(Udp);
      Udp.endPacket();


    } else {
      if (debugMode) {
        Serial.println("BUNDLE IN ERROR");
      }
    }
  }

}
void handleCommand(OSCMessage & msg, int addrOffset) {

  long val[3];
  for (int i = 0; i < 3; i++) {
    if (msg.isInt(i)) {
      val[i] = msg.getInt(i);
    }
    if (val[i] > 255) {
      val[i] = 255;
    }
    if (val[i] < 0) {
      val[i] = 0;
    }
    Serial.print(val[i]);
    Serial.print("\t");
  }
  Serial.println();
}
