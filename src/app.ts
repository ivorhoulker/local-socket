import { ReadlineParser, SerialPort } from "serialport";

import { Server } from "socket.io"
// import { Worker } from 'worker_threads';
import dotenv from "dotenv"
import express from "express";
import { getLocalIp } from "./helpers/getLocalIp";
import http from "http"
import path from "path"
import { sendWheelCommand } from "./wheels/sendWheelCommand";
import { wait } from "@rphk/constants";

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
  const error = await new Promise<void | Error>((resolve, reject) => {
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
  if (error instanceof Error) console.error(error)

  console.log(`server running on http://${local}:1337`)

  const io = new Server(httpServer, {
    cors: {
      origin: `*`,
      methods: ['GET', 'POST'],
      credentials: false,
    },
    pingInterval: 250, //trying a really short ping interval to speed up the car's auto stop in case of phone connection issue
    pingTimeout: 501 // ditto
  })
  io.sockets.on("connection", (socket) => {
    console.log("socket connected", socket.id)
    socket.on("handshake", (callback) => {
      callback(`Connected to local socket server on ${local}:1337`)
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
      }
    })
  })

}
await startServer()

async function startSerialPort() {
  try {
    port = new SerialPort({
      path: "/dev/ttyACM0",
      baudRate: 115200,
      dataBits: 8,
      parity: "none",
      endOnClose: true
    });

    const parser = new ReadlineParser();
    port.pipe(parser);
    parser.on("data", console.log); //this will log any data received from the port, assuming it can be parsed with the ReadlineParser
    // port.write("HELLO") // you could write any arbitrary data to the port in this way
    port.on("error", async (error) => {
      console.error(error, "Closing and retrying.");
      if (port?.isOpen) port.close()
      port = null
      await wait(500) // wait half a second before retrying serial port connection on port error
      startSerialPort()
    });
  } catch (error) {
    console.warn(error)
    // console.error(error, " while connecting. Closing and retrying.");
    // if (port) port.close()
    // port = null
    // await wait(500) // wait half a second before retrying serial port connection on connection error
    // startSerialPort()
  }
}

await startSerialPort()



/** check every second whether the ip has changed, and if so, restart the server */
setInterval(() => {
  try {
    const newIp = getLocalIp()
    if (local !== newIp) {
      //ip has changed
      console.log("NEW LOCAL IP ADDRESS: ", newIp)
      local = newIp
      httpServer?.close()
      if (port) sendWheelCommand(port, [0, 0, 0, 0])
      if (newIp !== "127.0.0.1") startServer() // 127.0.0.1 would mean no connections can be made, so no point starting
    }
  } catch (err) {
    console.error(err);
  }
}, 500); // the frequency to check the ip, 500 = every half a second
