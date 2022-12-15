
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

char packetBuffer[CUSTOM_PACKET_MAX_SIZE];

void ethernetLoop() {

  OSCBundle bundleIN;
  int size;
  if ( (size = Udp.parsePacket()) > 0) {
    while (size--) {
      bundleIN.fill(Udp.read(packetBuffer,CUSTOM_PACKET_MAX_SIZE));
    }
    if (!bundleIN.hasError()) {
      // handle incoming messages
      bundleIN.route("/move", handleMove);
      bundleIN.route("/tilt", handleTilt);
      bundleIN.route("/color", handleColor);

    } else if (debugMode) {
      Serial.print("bundleIN error code: ");
      Serial.println(bundleIN.getError());
    }
    
  }
  for(int i=0;i<UDP_TX_PACKET_MAX_SIZE;i++) packetBuffer[i] = 0;
}
