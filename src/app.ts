import { ReadlineParser, SerialPort } from "serialport";

import { Server } from "socket.io"
// import { Worker } from 'worker_threads';
import dotenv from "dotenv"
import express from "express";
import { getLocalIp } from "./helpers/getLocalIp";
import http from "http"
import path from "path"
import { sendWheelCommand } from "./wheels/sendWheelCommand";

export const __dirname = path.resolve(); // for if __dirname is not present
dotenv.config({ path: path.resolve(__dirname, '.env') }) // ensure .env variables are loaded from the correct place

let local = getLocalIp();
let port: SerialPort
let httpServer: http.Server


const app = express();
app.get("/", (req, res) => {
  res.send(`Hello from the node app on the raspberry pi at ${local}! This shows it exists.`);
});



/** Start the server, the socket, and listeners. */
async function startServer() {
  httpServer = http.createServer(app);
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
    },
  })
  io.sockets.on("connection", (socket) => {
    console.log("socket connected", socket.id)
    socket.on("handshake", (callback) => {
      callback("hi from the server")
    })
    socket.on("move", async (data, callback) => {
      console.log("move data", data)
      sendWheelCommand(port, data)
      callback()
    })
    socket.on("disconnect", (reason) => {
      console.warn("disconnected", reason)
      sendWheelCommand(port, [0, 0, 0, 0])
      if (reason === "transport close") {
        //the network status probably changed
        console.warn("network interrupted?")
        // httpServer?.close()
        // startServer()
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


//check every second whether the ip has changed, and if so, restart the server
setInterval(() => {
  try {
    const newIp = getLocalIp()
    if (local !== newIp) {
      //ip has changed
      console.log("NEW LOCAL IP ADDRESS: ", newIp)
      local = newIp
      httpServer?.close()
      if (newIp !== "127.0.0.1") startServer() // 127.0.0.1 would mean no connections can be made, so no point starting
    }
  } catch (err) {
    console.error(err);
  }
}, 1000);
