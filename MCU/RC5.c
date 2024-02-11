/* Function to decode RC5 commands
 * V1 2014-10-09
 */

#define EncoderReload	8	//divides 4,5kHz down to 500Hz

#define RC5address	0

#ifdef LPC922
#define _RC5inp		P1_5	//define input pin
#else
#define _RC5inp		P1_7	//define input pin
#endif

__data volatile unsigned char rCommand;   	//Bitte erhalten! Wenn Befehl fertig: auswerten
__data volatile unsigned char rAddress;   	//Bitte erhalten! Wenn Befehl fertig: auswerten
__data volatile unsigned char rCounter;   	//Bitte erhalten!

__bit volatile RTbit;			//Togglebit von RC5

//State Machine zur Decodierung des RC5-Fernbedieungscodes
__code unsigned char tblRemote[] =
{1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0,
 8, 0, 9, 0, 10, 0, 11, 0, 12, 0, 13, 0, 14,
 0, 15, 0, 16, 0, 17, 0, 18, 0, 19, 0, 20, 0,
 20, 21, 0, 22, 0, 23, 26, 24, 26, 25, 26, 0,
 27, 0, 28, 0, 29, 31, 30, 31, 0, 31, 0, 32,
 0, 33, 35, 34, 35, 43, 36, 43, 37, 0, 38, 40,
 39, 40, 0, 40, 0, 41, 0, 42, 35, 34, 35, 44,
 0, 45, 48, 46, 48, 47, 48, 0, 49, 0, 50, 0, 34, 43};

void T0_isr(void) __interrupt(1) __using(isrregisterbank)	//int from Timer 0 to read RC5 state
{
	__data static unsigned char rc5state;
	__bit static Rbit;				//Dedektiertes Bit von RC5
	__data static unsigned char EncoderCounter;

	rc5state=rc5state<<1;
	rc5state|=!_RC5inp;
	rc5state=tblRemote[rc5state];
	if (33==rc5state)				//Startsequenz erkannt?
		{
		rCounter=0;			//alles zurÃ¼cksetzen
		rAddress=0;
		rCommand=0;
		}
	else if ((42==rc5state) || (50==rc5state))	 //Erkanntes Bit einorden
		{
		if (42==rc5state)			//Nutzbit 1 erkannt?
			{
			Rbit=1;
			}
		else if (50==rc5state)		//Nutzbit 0 erkannt?
			{
			Rbit=0;
			}
   			++rCounter;			//Da neues Bit ...
		if (1==rCounter)
 				{
 				RTbit=Rbit;
 				}
		else if (7>rCounter)		//Adressbit
			{
 				rAddress=rAddress<<1;
 				rAddress|=Rbit;
 				}
		else				//Commandbit
			{
 				rCommand=rCommand<<1;
 				rCommand|=Rbit;
 				}
		}
	if (EncoderCounter)
		{
		EncoderCounter--;
		}
	else
		{
		EncoderCounter=EncoderReload;
		Encoder();
		}
}

void DecodeRemote()
{
	__bit static RTbitold;				//Togglebit des letzten Befehls von RC5

	if (12==rCounter)
		{
		if (RC5address==rAddress)
			{
			if (RTbit ^ RTbitold)	//Neue Taste erkannt
				{
				switch (rCommand)
					{
					case 1:
						if(0==PiOn)
							{
							ToggelPower();
							}
						break;
					case 12:			//Standby
						ToggelPower();
						break;
					case 13:			//Mute
						ToggelMute();
						break;
	  				}
				}
			if (1 == PiOn)
				{
				printf_tiny("RC5 %d %d\n", rCommand, RTbit ^ RTbitold);
				}
	  		RTbitold=RTbit;		//Togglebit speichern
	  		}
		rCounter=0;			//Nach Erkennung zurücksetzen
		}
}