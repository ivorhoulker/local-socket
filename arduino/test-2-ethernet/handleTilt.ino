void handleTilt(OSCMessage & msg, int addrOffset) {
  Serial.print("TILT: ");
  long val[msg.size()];
  for (int i = 0; i < msg.size(); i++) {
    if (msg.isInt(i)) {
      val[i] = msg.getInt(i);
    }
    Serial.print(val[i]);
    Serial.print("\t");
  }
  Serial.println();
  //TODO: Handle the tilt command'
  //bundleOUT.add("/confirm/tilt").add(val[0]);

}
