import { SerialPort } from 'serialport';
import { Vector4 } from './type/Vector4';
import { arraysEqual } from '../helpers/arraysEqual';

let wheelLoop: NodeJS.Timeout;

export async function updateWheelLoop(serial: SerialPort, vector: Vector4) {
    console.log('updating wheels', vector);
    const zero = arraysEqual(vector, [0, 0, 0, 0])

    if (zero) {
        sendCommand(serial, vector)
    }
    //only loop if vector is not zero
    if (!zero) {
        sendCommand(serial, vector)
        wheelLoop = setInterval(() => {
            try {
                sendCommand(serial, vector)
            } catch (err) {
                console.error(err);
            }
        }, 2);
    }
    if (zero && wheelLoop) {
        clearInterval(wheelLoop);
    }
}

export async function sendCommand(serial: SerialPort, vector: Vector4) {
    const cmd = 'A' + vector[0] + 'B' + vector[1] + 'C' + vector[2] + 'D' + vector[3] + '|';
    serial && serial.write(cmd);
}