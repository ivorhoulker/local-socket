import { SerialPort } from 'serialport';
import { Vector4 } from './type/Vector4';
import { arraysEqual } from '../helpers/arraysEqual';

let wheelLoop: NodeJS.Timeout;

export async function updateWheelLoop(serial: SerialPort, vector: Vector4) {
    console.log('updating wheels', vector);
    const zero = arraysEqual(vector, [0, 0, 0, 0])
    const wasWheelLoop = !!wheelLoop
    if (wheelLoop) {
        clearInterval(wheelLoop);
    }
    if (wasWheelLoop && zero) {
        const cmd = 'A' + vector[0] + 'B' + vector[1] + 'C' + vector[2] + 'D' + vector[3] + '|';
        serial && serial.write(cmd);
    }

    //only loop if vector is not zero
    if (!zero) {
        wheelLoop = setInterval(() => {
            try {
                const cmd = 'A' + vector[0] + 'B' + vector[1] + 'C' + vector[2] + 'D' + vector[3] + '|';
                serial && serial.write(cmd);
            } catch (err) {
                console.error(err);
            }
        }, 2);
    }

}
