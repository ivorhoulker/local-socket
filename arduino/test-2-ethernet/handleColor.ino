void handleColor(OSCMessage & msg, int addrOffset) {
  Serial.print("COLOR: ");
  long val[msg.size()];
  for (int i = 0; i < msg.size(); i++) {
    if (msg.isInt(i)) {
      val[i] = msg.getInt(i);
    }
    Serial.print(val[i]);
    Serial.print("\t");
  }
  Serial.println();
  //TODO: Handle the color command
  bundleOUT.add("/confirm/color").add(val[0]);
}
