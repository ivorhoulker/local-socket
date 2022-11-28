import { SerialPort } from 'serialport';
import { Triplet } from '@rphk/constants';

/** Sends a light command message, based on an array of 3 numbers: hue, saturation, lightness. */
export function sendLightCommand(serial: SerialPort, color: Triplet, speed?: number) {
  const cmd = `C${color[0]}S${color[1]}L${[color[2]]}T${speed || 3}|\n`;
  if (serial?.writable) {
    serial.write(cmd);
  } else {
    throw new Error('Serial is not writable, arduino may be disconnected');
  }
}
