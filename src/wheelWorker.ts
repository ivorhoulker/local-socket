import { ReadlineParser, SerialPort } from "serialport";

import { parentPort } from "worker_threads";

let port: SerialPort
type Vector4 = [number, number, number, number]
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

let wheelLoop: NodeJS.Timeout;

function updateWheelLoop(serial: SerialPort, vector: Vector4) {
  console.log('updating wheels', vector);
  const zero = arraysEqual(vector, [0, 0, 0, 0])
  if (wheelLoop) clearInterval(wheelLoop);
  sendCommand(serial, vector)
  if (!zero) {
    wheelLoop = setInterval(() => {
      try {
        sendCommand(serial, vector)
      } catch (err) {
        console.error(err);
      }
    }, 2);
  }
}

function sendCommand(serial: SerialPort, vector: Vector4) {
  const cmd = 'A' + vector[0] + 'B' + vector[1] + 'C' + vector[2] + 'D' + vector[3] + '|';
  if (serial?.writable) {
    serial.write(cmd);
  } else {
    console.log("serial is unwritable", serial)
  }
}

const arraysEqual = <T>(a: T[], b: T[]) => {
  if (a === b) return true;
  if (a == null || b == null) return false;
  if (a.length !== b.length) return false;
  for (let i = 0; i < a.length; ++i) {
    if (a[i] !== b[i]) return false;
  }
  return true;
};