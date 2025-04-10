I got this tiny MediaTek RSB-3810 board which has some interesting
hardware accelerated video features.

I plugged in the power & ethernet & the ethernet light came on so I did
an arp-scan. My network was cluttered with a bunch of junk & I couldn't
figure out which one the board was, so I deicded I'd turn my laptop into a
"home router" & plugged the ethernet directly into my laptop.

These are the NICs my laptop has:

![](images/nics.jpg)

The board is plugged into enp0s25. This was the script that I was running
for the "home router" configuration:

```sh
# assign our ethernet port 10.20.30.1
ip addr add 10.20.30.1/24 dev enp0s25 >/dev/null 2>&1
ip link set up dev enp0s25

# mask packets from the RSB-3810 as if they were coming from this machine
# otherwise, your home router won't know where to send packets
iptables -t nat -A POSTROUTING -j MASQUERADE

# allow our machine to send packets from other hosts, to other hosts(routing)
sysctl net.ipv4.ip_forward=1

# configure & run the dhcp server to provide an IP to the RSB-3810
(cat > /tmp/dhcpd.conf) << "EOF"
subnet 10.20.30.0 netmask 255.255.255.0 {
	range 10.20.30.2 10.20.30.40;
	option routers 10.20.30.1;
	option domain-name-servers 8.8.8.8;
}
EOF

dhcpd enp0s25 -cf /tmp/dhcpd.conf -d
```

![](images/headquarters.jpg)
![](images/dhcpd.jpg)

In this case, I saw that the board was asigned `10.20.30.2`.
I was able to log in over SSH - the username & password were both `ubuntu`.

![](images/mission-accomplished.jpg)

![](images/neofetch.jpg)


I plugged the webcam in & ran `v4l2-ctl --list-devices`

![](images/v4l2-devices.jpg)

I ran `ffplay udp://0.0.0.0:2000` on my laptop & ran the following on the board:

```
gst-launch-1.0 v4l2src device=/dev/video5 ! jpegdec ! x264enc ! udpsink host=10.20.30.1 port=2000
```

![](images/ffplay.jpg)

looks about right - let's add an image on top:

```
gst-launch-1.0 v4l2src device=/dev/video0 ! jpegdec ! gdkpixbufoverlay location=0.png overlay-width=1000 overlay-height=1000 ! x264enc ! udpsink host=10.20.30.1 port=2000
```

When I ran that, nothing came out on the other end & gstreamer wrote:

```
WARNING: from element /GstPipeline:pipeline0/GstUDPSink:udpsink0: Attempting to send a UDP packets larger than maximum size (94235 > 65507)
Additional debug info:
../gst/udp/gstmultiudpsink.c(684): gst_multiudpsink_send_messages (): /GstPipeline:pipeline0/GstUDPSink:udpsink0:
Reason: Error sending message: Message too long
```

For simplicity's sake, I cut the bitrate in half:

```
gst-launch-1.0 v4l2src device=/dev/video0 ! jpegdec ! gdkpixbufoverlay location=0.png overlay-width=1000 overlay-height=1000 ! x264enc bitrate=1024 ! udpsink host=10.20.30.1 port=2000
```

![](images/apple.jpg)

That's fun & all but I want to be able to move it around in realtime.
