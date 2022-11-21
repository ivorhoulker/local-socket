# Local Socket

> A local socket process to run on cars/robots to receive controls from a phone running the Presence app.

The code is written in typescript, and transpiled to javascript (a `dist` directory is created on build). The typescript definitions and constants come from a package published from a separate monorepo, `@rphk/constants`, which is installed like any regular npm package. To expand functionality, you will want to look at the socket listeners and emitters in `app.ts`.

```javascript
socket.on("move", (data, callback) => {
  sendWheelCommand(port, data);
  callback();
});
```

When the car receives a 'move' command via the server, it sends a wheel command to the serial port (via `sendWheelCommand`) - data is an array of four numbers. You can see the type of `data` by hovering over it. The events on the socket are typed, so if you wanted to add a new event, let's say, "tilt", you would also need the `@rphk/constants` package server type to be updated to specify that event and the data it takes.

You could still add new commands to send over serial, and trigger them manually (e.g. on start up) to test, and decide what data they need, before asking Ivor to add them to the server/frontend and `@rphk/constants`, so that you can add a listener for that event to trigger the command.

For example, I want to add a tilt command. I write a `sendTiltCommand` function. It needs to know whether to tilt upward or downward, and what speed to tilt. So I decide I will need a number between -5 and 5 as the data. Positive tilts up, negative tilts down. The higher the number, the faster. I call my function with hardcoded parameters, just after the socket connects, to confirm it works. I tell Ivor to add this functionality to the project. He updates `@rphk/constants`. Then I run `yarn install @rphk/constants@latest` and now the socket knows about this event. I can write my

```javascript
socket.on("tilt", (data, callback) => {
  sendTiltCommand(port, data);
  callback();
});
```

and the compiler will be happy.

# Installation

## Installing from scratch

To connect the pi to an iphone via usb hotspot local network, you need `usbmuxd` - I found out [here](https://support.speedify.com/article/565-tethered-iphone-linux). There is a similar package for ubuntu.

On a pi, run:

```bash
sudo apt install usbmuxd
```

(remember, this is only for pi os, ubuntu/debian needs a different package - check the link above)
You'll have to 'trust' the computer and put your passcode in on the phone.

You'll need node and npm (node package manager). It's best to use nvm (node version manager) for easy switching between versions.
Check it's installed:

```bash
nvm -v
```

It should show a version number.
If not, install it:

```bash
curl -o- https://raw.githubusercontent.com/creationix/nvm/v0.39.2/install.sh | bash
```

Our process can run with node 16, among others. Let's install that:

```bash
nvm install 16
```

This installs version 16 and sets it as the active version.

I like using yarn, and my projects usually have yarn.lock files. Here, it is assumed you will use yarn to run everything.

```bash
npm i -g yarn
```

### PM2 Setup

`pm2` is used to run the process on start up, for reliable restarts and easy log access. To set it up manually:

```bash
npm install -g pm2
```

Run `pm2 ecosystem` to generate a config file. Open it with vim or nano:

```bash
nano pm2.config.js # or whatever file it said it created, make sure you're in the right directory.
```

Set the contents as follows:

```bash
module.exports = {
  apps: [
    {
      name: "socket",
      cwd: "./app/socket",
      script: "yarn",
      args: "start",
    },
  ],
};

```

Save and exit.
Then:

```bash
mkdir app
cd app
git clone https://github.com/ivorhoulker/local-socket.git socket
cd socket
yarn
yarn build-ts

pm2 kill # stop any old processes, probably not necessary
pm2 start
pm2 save
pm2 log  # to check it works - exit with ctrl+c
```

### Common errors

Can't find the nvm command or some command isn't recognised? Restart the pi if you're sure you already installed it.

Some kind of yarn berry error? Make sure to delete any yarn files in the directories above. The presence pis have a yarn workspace active in the root! This was a bad idea. Sorry.
`ls -a` to see hidden files. Delete .yarnrc and .yarnrc.yml.

Node incompatible error? Might need to change node version. E.g. `nvm install 16` or whatever version it tells you in the error.

No yarn command? `npm i -g yarn`

Can't connect to the socket? Check if the board is on the right ip. The default local ip of the pi seems to be `172.20.10.2` when connected to iphone (which itself is `172.20.10.1` and acts as the DNS server.) Tested this is as consistent when unplugging/replugging. Otherwise use angry ip scanner to find it on your local network. Check the ip in react native is correct.

### Updating

Ssh into the pi and pull from the git repo, build, then restart the pm2 process:

```bash
cd app/socket
git pull
yarn
yarn build-ts
pm2 restart socket
```

Note that `git pull` will not override any local changes you made to files on the pi! You may want to `git stash` your local changes first.

### Updating many cars at once

Download [angry ip scanner](https://angryip.org/) and save yourself from having to listen to/find the local IPs of the cars manually. Use it to scan and identify all the cars - their host names are Greek heroes (Hector, Achilles, Prometheus, Theseus and Jason so far). Once you find them, reserve their ips on your router.

Download [csshX](https://formulae.brew.sh/formula/csshx) so you can ssh into multiple cars simultaneously to do the updates.

```bash
csshX pi@192.168.0.1 pi@192.168.0.2 pi@192.168.0.3 # but  use the local ips of the cars
# enter password when prompted, it should fill in all screens (type in red box at bottom)
```

Example I usually use:

```bash
csshX pi@10.0.1.61 pi@10.0.1.62

```

Once ssh'ed into all cars, follow the same procedure as above.

### Testing

You might want to attach directly to the process for testing. Kill pm2 and run it directly with yarn.

```bash
pm2 kill #kill pm2 temporarily while you test
cd app/socket && git pull # get latest version
yarn # install
yarn build-ts && yarn start # run the script directly to attach to the process
```
