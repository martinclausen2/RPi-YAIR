#!/bin/bash
#
# Autostart mpd
#

# sleep 15

export MPD_HOST=notshared@localhost

mpc -q clear
mpc -q load radio
mpc -q volume 70
mpc -q play 4

