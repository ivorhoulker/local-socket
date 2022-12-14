
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

OSCBundle bundleOUT;

void ethernetLoop() {

  OSCBundle bundleIN;
  int size;
  if ( (size = Udp.parsePacket()) > 0) {
    while (size--) {
      bundleIN.fill(Udp.read());
    }


    if (!bundleIN.hasError()) {

      // handle incoming messages
      bundleIN.route("/move", handleMove);

      //      bundleIN.route("/tilt", handleTilt);
      //      bundleIN.route("/color", handleColor);

    } else if (debugMode) {
//      Serial.print("packetBuffer: ");
//      Serial.println(packetBuffer);
      Serial.print("bundleIN error code: ");
      Serial.println(bundleIN.getError());
    }
    //    if (bundleOUT.size()) {
    //      Udp.beginPacket(Udp.remoteIP(), outPort);
    //      bundleOUT.send(Udp).empty();
    //      Udp.endPacket();
    //    }
  }
}
