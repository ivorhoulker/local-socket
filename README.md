# Node or Python implementations of socket server on local network

I was going to do it in python, but then decided to make a test in node first so I know the code is not the problem.

To connect the pi to an iphone via usb hotspot local network, you need `usbmuxd` - I found out [here](https://support.speedify.com/article/565-tethered-iphone-linux). There is a similar package for ubuntu.

The default local ip of the pi seems to be `172.20.10.2` and the iphone is `172.20.10.1` and it acts as the DNS server. Tested this is as consistent when unplugging/replugging.

T

## Updating many cars at once

Download [angry ip scanner](https://angryip.org/) and save yourself from having to listen to/find the local IPs of the cars manually. Use it to scan and identify all the cars - their host names are Greek heroes (Hector, Achilles, Prometheus, Theseus and Jason so far).

Download [csshX](https://formulae.brew.sh/formula/csshx) so you can ssh into multiple cars simultaneously to do the updates.

```bash
csshX pi@192.168.0.1 pi@192.168.0.2 pi@192.168.0.3 # but  use the local ips of the cars
# enter password when prompted, it should fill in all screens (type in red box at bottom)
```

For convenience, you can assign static ips on your router and assign them (e.g.) to the cars' wifi mac addresses. The command to ssh into Prometheus and Hector on the Rooftop Productions network is:

```bash
csshX pi@10.0.1.11 pi@10.0.1.12 #my router is set to non-standard local ips so they're shorter to type.

```

Once ssh'ed into all cars, follow the same procedure as below for updating one at a time.

## Updating one car at a time

Once you ssh in:

```bash
cd presence # in the top directory presence
git pull # ivor's github ssh key is already in the car, no password needed
yarn # install new packages if necessary, wait for yarn to finish
pm2 restart car # if pm2 is not running already, see below for installing from scratch

```

# Installing from scratch

`pm2` is used to run the process on start up, for reliable restarts and easy log acccess. To set it up manually:

```bash
#in the top directory presence
cd presence
yarn # wait for install
pm2 start yarn --name car -- car-start # starts new process named car - use car to keep it consistent
pm2 save #save currently running process to always run on start
pm2 status #see current status of processes
pm2 log #access logs of currently running process

```

## Testing

```bash
pm2 kill #kill pm2 temporarily while you test
git pull # get latest version
yarn # install
yarn run car-start # run the script directly to attach to the process
```
