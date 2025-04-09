#!/bin/sh
gst-launch-1.0 v4l2src device=/dev/video0 ! jpegdec ! gdkpixbufoverlay offset-x=100 offset-y=100 overlay-width=500 location=back.jpg ! openh264enc ! hlssink2 playlist-root=. target-duration=2 
