import { SerialPort } from "serialport";
type Triplet = [number, number, number]
export function sendLightCommand(serial: SerialPort, color: Triplet, speed?: number) {
    const cmd = `C${color[0]}S${color[1]}L${[color[2]]}T${speed || 3}|\n`;
    if (serial?.writable) {
        serial.write(cmd);
    } else {
        throw new Error("Serial is not writable, arduino may be disconnected")
    }
}