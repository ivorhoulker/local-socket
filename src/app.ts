import { ReadlineParser, SerialPort } from "serialport";

import { Server } from "socket.io"
// import { Worker } from 'worker_threads';
import dotenv from "dotenv"
import express from "express";
import { getLocalIp } from "./getLocalIp";
import http from "http"
import path from "path"
import { sendCommand } from "./wheels/updateWheelLoop";
import { wait } from './helpers/wait';

export const __dirname = path.resolve();
let local = getLocalIp();
console.log({ local })

dotenv.config({ path: path.resolve(__dirname, '.env') })
const app = express();
app.get("/", (req, res) => {
  res.send(`Hello from Ivor's app! This shows it exists.`);
});
let port: SerialPort

let httpServer: http.Server
async function startServer() {
  const httpServer = http.createServer(app);
  await new Promise<void>((resolve, reject) => {
    try {
      httpServer.listen(
        1337,
        local,
        resolve
      );
    }
    catch (error) {
      reject(error)
    }
  });
  console.log(`server running on http://${local}:1337`)
  const io = new Server(httpServer, {
    cors: {
      origin: `*`,
      methods: ['GET', 'POST'],
      credentials: false,
      // allowedHeaders: ["source"],

    },
  })
  io.sockets.on("connection", (socket) => {
    console.log("socket connected", socket.id)
    socket.on("handshake", (callback) => {
      callback("hi from the server")
    })
    socket.on("move", async (data, callback) => {
      console.log("move data", data)
      sendCommand(port, data)
      callback()
    })
    socket.on("disconnect", (reason) => {
      console.warn("disconnected", reason)
      sendCommand(port, [0, 0, 0, 0])
      if (reason === "transport close") {
        //the network status probably changed
        console.warn("network interrupted?")
        httpServer.close()
        startServer()
      }
    })
  })

}
await startServer()






try {
  port = new SerialPort({
    path: "/dev/ttyACM0",
    baudRate: 115200,
    dataBits: 8,
    parity: "none",
  });
  const parser = new ReadlineParser();
  port.pipe(parser);
  parser.on("data", console.log);
  // port.write("HELLO")
  port.on("error", (err) => {
    console.error(err);
  });
} catch (error) {
  console.error(error);
}

// const wheelWorker = new Worker('./dist/wheelWorker.js');

// wheelWorker.postMessage([2, 2, -2, -2])
// await wait(3000)
// wheelWorker.postMessage([0, 0, 0, 0])





const ipCheckLoop = setInterval(() => {
  try {
    const newIp = getLocalIp()
    if (local !== newIp) {
      //ip change here
      console.log("NEW LOCAL IP ADDRESS: ", newIp)
      local = newIp
      httpServer.close()
      startServer()

    }
  } catch (err) {
    console.error(err);
  }
}, 1000);
