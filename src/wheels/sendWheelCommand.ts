import { Quartet } from "@rphk/constants";
import { SerialPort } from 'serialport';

/** Sends a wheel command message, based on an array of 4 numbers, from a given serial port. */
export function sendWheelCommand(serial: SerialPort, wheelSpeeds: Quartet) {
    const cmd = 'A' + wheelSpeeds[0] + 'B' + wheelSpeeds[1] + 'C' + wheelSpeeds[2] + 'D' + wheelSpeeds[3] + '|';
    if (serial?.writable) {
        serial.write(cmd);
    } else {
        console.log("serial is unwritable", serial);
    }
}
