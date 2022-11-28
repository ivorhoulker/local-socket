import { ClientToServerEvents, InterServerEvents, ServerToClientEvents, wait } from '@rphk/constants';
import { ReadlineParser, SerialPort } from 'serialport';

import { Server } from 'socket.io';
import { SocketData } from './types/SocketData';
// import { Worker } from 'worker_threads';
import dotenv from 'dotenv';
import express from 'express';
import { getLocalIp } from './helpers/getLocalIp';
import http from 'http';
import os from 'os';
import path from 'path';
import { sendLightCommand } from './commands/sendLightCommand';
import { sendWheelCommand } from './commands/sendWheelCommand';

export const __dirname = path.resolve(); // for if __dirname is not present
dotenv.config({ path: path.resolve(__dirname, '.env') }); // ensure .env variables are loaded from the correct place
const hostname = os.hostname();

let localIp = getLocalIp();
let port: SerialPort;
let httpServer: http.Server;
let io: Server<ClientToServerEvents, ServerToClientEvents, InterServerEvents, SocketData>;

const app = express();
app.get('/', (req, res) => {
  res.send(`Hello from the node app on the raspberry pi at ${localIp}! This shows it exists.`);
});

/** Start the server, the socket, and listeners. */
async function startServer() {
  httpServer = http.createServer(app);
  const serverStart = await new Promise<void | Error>((resolve, reject) => {
    try {
      httpServer.listen(1337, localIp, resolve);
    } catch (error) {
      reject(error);
    }
  });
  if (serverStart instanceof Error) console.error(serverStart);

  console.log(`server running on http://${localIp}:1337 - host: ${hostname}`);

  io = new Server<ClientToServerEvents, ServerToClientEvents, InterServerEvents, SocketData>(httpServer, {
    cors: {
      origin: `*`,
      methods: ['GET', 'POST'],
      credentials: false,
    },
    pingInterval: 250, //trying a really short ping interval to speed up the car's auto stop in case of phone connection issue
    pingTimeout: 501, // ditto
  });
  io.sockets.on('connection', (socket) => {
    console.log('socket connected', socket.id);
    socket.on('handshake', (callback) => {
      callback(`Connected to ${hostname}, local socket server on ${localIp}:1337`);
    });
    socket.on('command', async (data, callback) => {
      try {
        switch (data.cmd) {
          case 'move':
            sendWheelCommand(port, data.move);
            callback(`received ${data.cmd} command: ${data.move}`);
            break;
          case 'color':
            sendLightCommand(port, data.hsl);
            return callback(`received ${data.cmd} command: ${data.hsl}`);
          default:
            callback(new Error(`command ${data.cmd} not implemented`));
            break;
        }
      } catch (error) {
        callback(error);
      }
    });
    socket.on('disconnect', (reason) => {
      console.warn('disconnected', reason);
      sendWheelCommand(port, [0, 0, 0, 0]);
      if (reason === 'transport close') {
        //the network status probably changed
        console.warn('network interrupted?');
      }
    });
  });
}
await startServer();

async function startSerialPort() {
  try {
    port = new SerialPort({
      path: '/dev/ttyACM0',
      baudRate: 115200,
      dataBits: 8,
      parity: 'none',
      endOnClose: true,
    });

    const parser = new ReadlineParser();
    port.pipe(parser);
    parser.on('data', (data) => {
      if (data && io) {
        io.emit('signal', { signal: 'serial', data, hostname }, (res) => {
          console.log('sent signal', res);
        });
      }
    }); //this will log any data received from the port, assuming it can be parsed with the ReadlineParser
    // port.write("HELLO") // you could write any arbitrary data to the port in this way
    port.on('error', async (error) => {
      console.error(error, 'Closing and retrying.');
      port = null;
      await wait(500); // wait half a second before retrying serial port connection on port error
      startSerialPort();
    });
    port.on('close', async () => {
      console.error('Port just closed. Waiting for end...');
    });
    port.on('end', async () => {
      console.error('Port just ended. Trying again.');
      if (io) {
        io.emit('signal', { signal: 'serial', data: `Serial connection interrupted, retrying...`, hostname }, (res) => {
          console.log('sent signal', res);
        });
      }
      port = null;
      await wait(500); // wait half a second before retrying serial port connection on port error
      startSerialPort();
    });
  } catch (error) {
    console.warn(error);
  }
}
await startSerialPort();

/** check every second whether the ip has changed, and if so, restart the server */
setInterval(() => {
  try {
    const newIp = getLocalIp();
    if (localIp !== newIp) {
      //ip has changed
      console.log('NEW LOCAL IP ADDRESS: ', newIp);
      localIp = newIp;
      httpServer?.close();
      if (port) sendWheelCommand(port, [0, 0, 0, 0]);
      if (newIp !== '127.0.0.1') startServer(); // 127.0.0.1 would mean no connections can be made, so no point starting
    }
  } catch (err) {
    console.error(err);
  }
}, 500); // the frequency to check the ip, 500 = every half a second
