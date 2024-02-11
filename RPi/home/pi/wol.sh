#!/bin/bash
#
# Autostart connection to NAS
#

# sleep 25

wakeonlan -i 192.168.178.255 -p 9 00:1B:21:DD:EC:49

sleep 5

mount -t cifs -o ro,nounix,user='Musik',password='notshared',noauto,x-systemd.automount,x-systemd.idle-timeout=60 //192.168.178.10/Musik /media/music

