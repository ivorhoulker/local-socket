# Node or Python implementations of socket server on local network

I was going to do it in python, but then decided to make a test in node first so I know the code is not the problem.

To connect the pi to an iphone via usb hotspot local network, you need `usbmuxd` - I found out [here](https://support.speedify.com/article/565-tethered-iphone-linux). There is a similar package for ubuntu.

The default local ip of the pi seems to be `172.20.10.2` and the iphone is `172.20.10.1` and it acts as the DNS server. Tested this is as consistent when unplugging/replugging.

T
