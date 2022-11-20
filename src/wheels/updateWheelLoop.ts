import { Quartet, arraysEqual } from '@rphk/constants';

import { SerialPort } from 'serialport';
import { sendWheelCommand } from "./sendWheelCommand";

let wheelLoop: NodeJS.Timeout;

/** Loops a wheel command - not currently used as the micro-controller stuff handles looping. */
export function updateWheelLoop(serial: SerialPort, vector: Quartet) {
    console.log('updating wheels', vector);
    const zero = arraysEqual(vector, [0, 0, 0, 0])
    if (wheelLoop) clearInterval(wheelLoop);
    sendWheelCommand(serial, vector)
    if (!zero) {
        wheelLoop = setInterval(() => {
            try {
                sendWheelCommand(serial, vector)
            } catch (err) {
                console.error(err);
            }
        }, 2);
    }
}

