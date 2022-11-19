import { ReadlineParser, SerialPort } from 'serialport';

import { parentPort } from 'worker_threads';
import { updateWheelLoop } from 'wheels/updateWheelLoop';

let port: SerialPort
try {
    port = new SerialPort({ path: '/dev/ttyACM0', baudRate: 115200, dataBits: 8, parity: "none" })
    const parser = new ReadlineParser()
    port.pipe(parser)
    parser.on('data', console.log)
    // port.write("HELLO")
    port.on("error", (err) => {
        console.error(err)
    })


} catch (error) {
    console.error(error)
}

parentPort.on("message", (value) => {
    updateWheelLoop(port, value)
})