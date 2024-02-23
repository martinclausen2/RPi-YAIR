// Program to receive data from MCU using the UART and controling MPC accordingly

// startvolume 70%
// changed to serial0 to allow for BT on full UART

// V5 2016-12-02
// use next / prev instead of abs song number
// V4 2014-10-09
// reduced bouad rate to 57600 to enable lower power consumption of MCU
// V3 2013-03-16
// reduced start volume to 33%
// added start & stop via RC
// V2 2013-01-04
// no mpc meassages during volume changes
// clear mpd before shutdown instead of stop to prevent uncontroled start during boot-up
// V1 2013-01-03
// Added number input via remote
// V0 2012-12-29
// Kompilieren mit: gcc /var/scripte/receiver.c -o /var/scripte/receiver

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>


#define BAUDRATE B57600
char MODEMDEVICE[]= "/dev/serial0";     //ttyAMA0";	// Schnittstelle

char eingang[255]="";

char doubledigit=0;
int laenge=0;				// Play list length
int fd;					// File descriptor
int sender=1;				// no of current play list item
int lautst=70;				// start volume, should correspond to startup value defineed else where

struct	termios newtio={};

unsigned char eingangleer()		//Eingangstring leeren
{
	int i;
	for(i=0; i < laenge; i++){
		eingang[i] = 0;
	}
  laenge=0;
}

unsigned char radioein()		//gewÃ¤hlten Platz in der Playlist abspielen
{
	char befehl[255]="";
	sprintf(befehl,"mpc play %d",sender);
	system(befehl);
}

unsigned char playnext()
{
	system("mpc next");
}

unsigned char playprev()
{
	system("mpc prev");
}

unsigned char lautsetzen()			//LautstÃ¤rke einstellen
{
	char befehl[255]="";
	if (0>lautst)				//limit to usefuil range
		{
		lautst=0;
		}
	else if (100<lautst)
		{
		lautst=100;
		}
	sprintf(befehl,"mpc -q volume %d",lautst);
	system(befehl);
}

unsigned char radioaus()			//stop playback and system shutdwon
{
	system("mpc -q clear");
	system("sudo shutdown -h now");
}

unsigned char auswerten()			//Angekommene Daten auswerten
{ int zeile;
//  char ret;
  char code[8]="";
  int value=0;
  int toggel=0;

  /* Zum Schreiben öffnen */
	sscanf(eingang, "%s %d %d", &code[0], &value, &toggel);
	printf("Received string %s  data %d  toggel %d\n", code, value, toggel);

	if (0==strcmp(code,"shutdown"))
		{
		radioaus();
		}
	else if (0==strcmp(code,"EncA"))
		{
		lautst+=value;
		lautsetzen();
		}
	else if (0==strcmp(code,"EncB"))
		{
		if (value>0)
			{
			playnext();
			}
		else if (value<0)
			{
			playprev();
			}
		}
	else if (0==strcmp(code,"KeyB"))
		{
		if (10>value)
			{
			radioein();
			}
		else
			{
			system("mpc -q stop");
			}
		}
	else if (0==strcmp(code,"RC5"))
		{
		switch (value)
			{
			case 16:
				lautst++;
				lautsetzen();
				break;
			case 17:
				lautst--;
				lautsetzen();
				break;
			}
		if (1==toggel)
			//execute these commands only once for each key pressed
			{
			if(10>value)			//assemble number
				{
				switch (doubledigit)
					{
					case 0:		//single digit
						sender=value;
						radioein();
						break;
					case 1:		//1er digit
						sender+=value;
						doubledigit=0;
						radioein();
						break;
					case 2:		//10er digit
						sender=value*10;
						doubledigit=1;
						break;
					}
				}
			switch (value)
				{
				case 10:
					doubledigit=2;	//activate double digit number input
					break;
				case 32:
					playnext();
					break;
				case 33:
					playprev();
					break;
				case 41:
					system("mpc -q stop");
					break;
				case 43:
					system("mpc -q play");
					break;
				}
			}
		}
	eingangleer();
 }

unsigned char receive()		//Zeichen empfangen
{
    int res;
    unsigned char buffer;

    res = read(fd, &buffer, 1);
    return buffer;
}

int init()			//Schnittstelle parametrieren und öffnen
{
    //O_RDONLY, O_WRONLY or O_RDWR -
    //O_NDELAY (geht weiter, wenn keine Daten da sind und gibt "-1" zurueck)
    // man 2 open fuer mehr Infos - see "man 2 open" for more info
    // O_NOCTTY No ControllTeleType 

    printf("Attempting to open com port.\n");
    fd = open(MODEMDEVICE, O_RDONLY | O_NOCTTY);
    if (fd < 0){
        printf("Error while opening %s.\n", MODEMDEVICE);
        exit(-1);
    }
    memset(&newtio, 0, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;	//setzt die neuen Porteinstellungen
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;
    newtio.c_lflag = 0;         /* set input mode (non-canonical, no echo, ...) */
    newtio.c_cc[VTIME] = 0;     /* inter-character timer unused */
    newtio.c_cc[VMIN] = 1;	/* blocking read until 1 chars received */

    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &newtio);
    return fd;
}

int main(int argc, char** argv)		//Programmstart
{
	char c;

 	init();					//Schnittstelle parametrieren und öffnen
	printf("Ready to receive data.\n");
	while (1)
        {
	    c=receive();		        //Zeichen holen
		if((13==c)||(10==c))		//CR oder LF als Abschluß einer Zeile
			{
			auswerten();
			}
		else if(c>13&&c<128)		//Alles von ASCii 14 bis 127 wird akzetpiert
			{
			eingang[laenge]=c;
			laenge++;
			if (laenge >254)	//Bei 254 Zeichen im Puffer wird automatisch ausgewertet
			{
			auswerten();
			}
        }
    } 
    close (fd);
    return 0;
}
