String command = "";

void serialLoop() {
  if (Serial.available()) {
    command = Serial.readStringUntil('\n');
    String subCommand[2];

    subCommand[0] = command.substring(0, 1);
    Serial.print("subCommand 0 : ");
    Serial.println(subCommand[0]);

    subCommand[1] = command.substring(1, command.length());
    Serial.print("subCommand 1 : ");
    Serial.println(subCommand[1]);

  }
}
