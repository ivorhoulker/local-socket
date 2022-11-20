# Node or Python implementations of socket server on local network

I was going to do it in python, but then decided to make a test in node first so I know the code is not the problem. Node seems more convenient to share types on the socket. The python implementation of socket.io is also not directly compatible with the latest js version.

To connect the pi to an iphone via usb hotspot local network, you need `usbmuxd` - I found out [here](https://support.speedify.com/article/565-tethered-iphone-linux). There is a similar package for ubuntu.

The default local ip of the pi seems to be `172.20.10.2` and the iphone is `172.20.10.1` and it acts as the DNS server. Tested this is as consistent when unplugging/replugging.

# Installing from scratch

You need a way to connect to the iphone hotspot: (https://support.speedify.com/article/565-tethered-iphone-linux)[https://support.speedify.com/article/565-tethered-iphone-linux]

Basically run: `sudo apt install usbmuxd`
You'll have to 'trust' the computer and put your passcode in on the phone.

`pm2` is used to run the process on start up, for reliable restarts and easy log access. To set it up manually:

# PM2 Setup

On a fresh pi, run `pm2 ecosystem` to generate a config file, edit and delete the contents, replace it with:

```
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

Then

```
mkdir app
cd app
git clone #this repo into a folder called socket
cd socket
yarn
yarn build-ts

pm2 kill # stop any old processes
pm2 start
pm2 save
pm2 log  #to check it works
```

Make sure to delete any yarn files in the directories above. The current pis have a yarn workspace active in the root! This was a bad idea. Sorry.
ls -a to see hidden files. Delete .yarnrc and .yarnrc.yml or any other yarn-like stuff.

Node incompatible error? Might need to change node version. E.g. `nvm install 16`.

No yarn? `npm i -g yarn`

## Updating many cars at once

Download [angry ip scanner](https://angryip.org/) and save yourself from having to listen to/find the local IPs of the cars manually. Use it to scan and identify all the cars - their host names are Greek heroes (Hector, Achilles, Prometheus, Theseus and Jason so far).

Download [csshX](https://formulae.brew.sh/formula/csshx) so you can ssh into multiple cars simultaneously to do the updates.

```bash
csshX pi@192.168.0.1 pi@192.168.0.2 pi@192.168.0.3 # but  use the local ips of the cars
# enter password when prompted, it should fill in all screens (type in red box at bottom)
```

Example I usually use:

```bash
csshX pi@10.0.1.61 pi@10.0.1.62

```

Once ssh'ed into all cars, follow the same procedure as below for updating one at a time.

## Testing

```bash
pm2 kill #kill pm2 temporarily while you test
cd app/socket && git pull # get latest version
yarn # install
yarn build-ts && yarn start # run the script directly to attach to the process
```
