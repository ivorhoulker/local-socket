import { Server } from "socket.io"
import { Worker } from 'worker_threads';
import dotenv from "dotenv"
import express from "express";
import http from "http"
import os from "os";
import path from "path"
import { wait } from './helpers/wait';

export const __dirname = path.resolve();
const local = getLocalIp();
console.log({ local })

dotenv.config({ path: path.resolve(__dirname, '.env') })
const app = express();
app.get("/", (req, res) => {
  res.send(`Hello from Ivor's app! This shows it exists.`);
});


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



const wheelWorker = new Worker('./wheelWorker.ts', {
  workerData: {
    path: './wheelWorker.ts'
  }
});

wheelWorker.postMessage([2, 2, -2, -2])
await wait(3000)
wheelWorker.postMessage([0, 0, 0, 0])


io.sockets.on("connection", (socket) => {
  console.log("socket connected", socket.id)
  socket.on("handshake", (callback) => {
    callback("hi from the server")
  })
  socket.on("move", async (data, callback) => {
    console.log("move data", data)
    wheelWorker.postMessage(data)
    callback()
  })
  socket.on("disconnect", () => {
    wheelWorker.postMessage([0, 0, 0, 0])
  })
})

function getLocalIp() {
  let localIp = "127.0.0.1";
  const interfaces = os.networkInterfaces();
  Object.keys(interfaces).forEach((interfaceName) => {
    for (const iface of interfaces[interfaceName] ?? []) {
      // Ignore IPv6 and 127.0.0.1
      if (iface.family !== "IPv4" || iface.internal !== false) {
        continue;
      }
      // Set the local ip to the first IPv4 address found and exit the loop
      localIp = iface.address;
      return;
    }
  });
  return localIp;
}
