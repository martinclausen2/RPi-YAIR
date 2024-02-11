# RPi-YAIR
Raspberry Pi YAIR (Yet Another Internet Radio)

### Einleitung

Ja, es gibt sie: Internetradios. Zum Kaufen. Zum Selbst bauen. Aus Routern. Aus dem Raspberry Pi. Aber schlussendlich gefiel mir keines so gut, dass es mich vom Selbstbau abgehalten hätte ...
### Features für den Nutzer

* Intuitive Bedienung über zwei Drehencoder mit Tastfunktion
  * Dynamische Schrittweitenanpassung für die schnelle und genaue Einstellung der Lautstärke und Sender
* Fernbedienbar via IR-Fernbedieung oder Netzwerk / WLAN via Smartphone
* Sehr niedriger Standby-Stromverbrauch
* Lesen der Musik via Netzwerk oder empfang via BT

### Technische Eigenschaften

* Basis Raspberry Pi Zero (nur headless) oder besser, 512MB, WLAN und BT

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

Die Software kann per #define in Quellcode für LPC922 oder LPC93X kompiliert werden. Die zusätzlichen Funktionsblöcke der LPC93X werden nicht verwendet.

Bei Bedarf könnte ich MCUs programmieren. Alternativ ist der Code sicher leicht auf einen AVR zu portieren. 

## Raspberry Pi Setup

