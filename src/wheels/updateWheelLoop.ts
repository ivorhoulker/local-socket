import { SerialPort } from 'serialport';
import { Vector4 } from './type/Vector4';
import { arraysEqual } from '../helpers/arraysEqual';

let wheelLoop: NodeJS.Timeout;

export function updateWheelLoop(serial: SerialPort, vector: Vector4) {
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

export function sendCommand(serial: SerialPort, vector: Vector4) {
    const cmd = 'A' + vector[0] + 'B' + vector[1] + 'C' + vector[2] + 'D' + vector[3] + '|';
    if (serial?.writable) {
        serial.write(cmd);
    } else {
        console.log("serial is unwritable", serial)
    }
}