# RPi-YAIR
Raspberry Pi YAIR (Yet Another Internet Radio)

Diverted from [Raspberry Pi YAIR - Yet Another Internet Radio](https://www.mikrocontroller.net/articles/Raspberry_Pi_YAIR_(Yet_Another_Internet_Radio) )

German parts to be translated later. Details on one particular hardware setup to be found under articel cited above.

### Einleitung

Ja, es gibt sie: Internetradios. Zum Kaufen. Zum Selbst bauen. Aus Routern. Aus dem Raspberry Pi. Aber schlussendlich gefiel mir keines so gut, dass es mich vom Selbstbau abgehalten hätte ...
### Features für den Nutzer

* Intuitive Bedienung über zwei Drehencoder mit Tastfunktion
  * Dynamische Schrittweitenanpassung für die schnelle und genaue Einstellung der Lautstärke und Sender
* Fernbedienbar via IR-Fernbedieung oder Netzwerk / WLAN via Smartphone
* Sehr niedriger Standby-Stromverbrauch
* Lesen der Musik via Netzwerk oder empfang via BT

### Hardware Setup

* Basis Raspberry Pi Zero (nur headless) oder besser, 512MB, WLAN und BT
* I2S Audio DAC connection please see [External-Non-USB-Audio-DAC-ES9023,-PCM5102,-etc.](https://github.com/MiczFlor/RPi-Jukebox-RFID/wiki/External-Non-USB-Audio-DAC-ES9023,-PCM5102,-etc.)
* Microcontroller connnection please see [Raspberry Pi YAIR - Yet Another Internet Radio](https://www.mikrocontroller.net/articles/Raspberry_Pi_YAIR_(Yet_Another_Internet_Radio) )

## MCU

Die Software besteht aus folgenden Teilen:

* Hauptschleife
  * Setzt Zustände der State-Machines in Befehle, die über den UART gesendet werden, um
  * Steuert die Stromversorgung von Raspberry Pi, Monitor und Audio-Verstärker

* Interrupt-Routinen
  * RC5 Decoder, ausgelöst durch Timer 0 bei 4,5kHz, jeden 9. Aufruf wird auch der Drehencoder ausgewertet
  * Einlesen der Tasten, ausgelöst durch Timer aus RTC der MCU bei 10Hz

* Initialisierung der MCU
  * Core Funktionen, hauptsächliche Stromsparfunktionen
  * I/O Pins
  * Interne RTC als 10Hz Taktgeber: 6*10^6/4/128/10
  * UART: 57600 Bit/s Mode 1
  * Watchdog wird abgeschaltet
  * Interrupts freischalten

* Auswertung Drehencoder

* RC5 Decoder

Der RC5 Decoder beruht auf einer State-Machine, die schrittweise die einzelnen Bits erkennt und dann in Adresse und Daten zerlegt. Nach der Erkennung eines kompletten Kommandos wird steht rCounter auf dem Wert 12 und das Hauptprogramm beginnt mit der Decodierung (Aufruf "DecodeRemote()").

Die Software kann per #define in Quellcode für LPC922 oder LPC93X kompiliert werden. Die zusätzlichen Funktionsblöcke der LPC93X werden nicht verwendet. Als Kompiler ist SDCC zu verwenden.

Bei Bedarf könnte ich MCUs programmieren. Alternativ ist der Code sicher leicht auf einen AVR, STM32, oder ... zu portieren. 

## Raspberry Pi Setup

### Step by Step Instructions

Please follow the follwoing steps to steup the raspberry pi

* deploy raspberry pi os bookworm on sdcard
* setup the configuration 'sudo raspi-config'
  * setup system options
    * WLAN
    * Hostname
  * Interface
    * SSH
    * Serail Port - enable serial port, but not (!) console
  * Localisation Options
    * Timezone
    * Keyboard
    * WLAN Country
  * Advanced Options
    * Expand Filesystem
* update repos `sudo apt-get update`
* update software `sudo apt-get dist-upgrade`
* install software `sudo apt-get install mpd mpc wakeonlan git`
* Reboot Rpi
* Adjust configuration files from RPi folder and changes dummy credentials, consider to modifiy the following files on your traget system in case your installation has other specifics.  
  * `/boot/firmware/cmdline.txt`
    * remove `console=tty1`
  * `/boot/firmware/config.txt`
    * remove `dtparam=audio=on` by addin a hash in fromt of the line
    * add `dtoverlay=hifiberry-dac`
  * If you decide to adjust the start volue, you need to do it in `receiver.c`, complie again, and in `startmpd.sh`
* Copy adjusted configuration files to RPi
* Install BT audio receiver
  * `git clone https://github.com/nicokaiser/rpi-audio-receiver.git`
  * `cd rpi-audio-receiver`
  * Choose the features as needed when running `./install.sh`
* create folder to mount music folder from NAS
  * `mkdir /media/music`
  * `chmod 555 /media/music`
* Make playlist folder accessable `sudo chmod 777 /var/lib/mpd/playlists`
  * Deploy there at least `radio.m3u`, this is used for playback at startup (entry 4)
* stop automated concole activation
  * details may depend on your sprecific RPi model and configuration
	* use `systemctl status` to check for any serial-getty services running and deactivte them using stop, disable, mask
  * `sudo systemctl stop serial-getty@ttyAMA0.service`
  * `sudo systemctl disable serial-getty@ttyAMA0.service`
  * `sudo systemctl mask serial-getty@ttyAMA0.service`
  * `sudo systemctl stop serial-getty@tty1.service`
  * `sudo systemctl disable serial-getty@tty1.service`
  * `sudo systemctl mask serial-getty@tty1.service`
  * `sudo systemctl stop serial-getty@ttyS0.service`
  * `sudo systemctl disable serial-getty@ttyS0.service`
  * `sudo systemctl mask serial-getty@ttyS0.service`
* Activate services
  * `sudo systemctl enable mpd`
  * `sudo systemctl enable wol`
  * `sudo systemctl enable startmpd`
  * `sudo systemctl enable receiver`
  * `sudo systemctl daemon-reload`
* Compile software to receive data from MCU
  * `gcc receiver.c -o receiver`
  * `chmod 755 receiver`
  * If you test the software on console, make sure to delete temp file in `/tmp/mpdlist` before tring to start the service
### Debugging
* Review audio interfaces by
  * `cat /proc/asound/modules`
  * `cat /proc/asound/cards`
* Review service status
  * `systemctl status`
  * `systemctl [status servicename]`
* Review system logs
  * `journalctl -rm -n 1000`
* Inspect data from MCU
  * install minicom `sudo apt-get install minicom`
  * `mincom --device /dev/tytyS0 --baudrate 57600`
