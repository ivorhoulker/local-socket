void handleMove(OSCMessage & msg, int addrOffset) {
  Serial.print("MOVE: ");
  long val[2];
  for (int i = 0; i < 2; i++) {
    if (msg.isInt(i)) {
      val[i] = msg.getInt(i);
    }
    Serial.print(val[i]);
    if (i < 1) Serial.print(", ");
  }
  Serial.println();
  //TODO: Handle the move command
  //bundleOUT.add("/confirm/move").add(val[0]);
}
