import { ReadlineParser, SerialPort } from "serialport";

import { parentPort } from "worker_threads";

let port;

try {
  port = new SerialPort({
    path: "/dev/ttyACM0",
    baudRate: 115200,
    dataBits: 8,
    parity: "none",
  });
  const parser = new ReadlineParser();
  port.pipe(parser);
  parser.on("data", console.log);
  // port.write("HELLO")
  port.on("error", (err) => {
    console.error(err);
  });
} catch (error) {
  console.error(error);
}

parentPort.on("message", (value) => {
  updateWheelLoop(port, value);
});

export function updateWheelLoop(serial, vector) {
  console.log("updating wheels", vector);
  const zero = arraysEqual(vector, [0, 0, 0, 0]);
  if (wheelLoop) clearInterval(wheelLoop);
  sendCommand(serial, vector);
  if (!zero) {
    wheelLoop = setInterval(() => {
      try {
        sendCommand(serial, vector);
      } catch (err) {
        console.error(err);
      }
    }, 2);
  }
}

export function sendCommand(serial, vector) {
  const cmd =
    "A" + vector[0] + "B" + vector[1] + "C" + vector[2] + "D" + vector[3] + "|";
  if (serial?.writable) {
    serial.write(cmd);
  } else {
    console.log("serial is unwritable", serial);
  }
}
