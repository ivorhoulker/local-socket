import { SerialPort } from 'serialport';
import { Vector4 } from './type/Vector4';
import { arraysEqual } from '../helpers/arraysEqual';
import { getWheelCommand } from './wheelCommand';

let wheelLoop: NodeJS.Timeout;

export async function updateWheelLoop(serial: SerialPort, vector: Vector4) {
    if (wheelLoop) {
        clearInterval(wheelLoop);
    }
    console.log('updating wheels', vector);
    //only loop if vector is not zero
    if (!arraysEqual(vector, [0, 0, 0, 0])) {
        wheelLoop = setInterval(() => {
            try {
                const cmd = getWheelCommand(vector);
                serial && serial.write(cmd);
            } catch (err) {
                console.error(err);
            }
        }, 2);
    }
}
