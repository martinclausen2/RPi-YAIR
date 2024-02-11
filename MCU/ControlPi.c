/** Handels communication with Pi and controls hardware
 */

#ifdef LPC922

#define Pin_PowerSwitch	P1_6
#define Pin_AmpOff	P1_7
#define Pin_PiOn		P1_4
#define Pin_PiOff		P1_3	//inverted Pi On

#else

#define Pin_PowerSwitch	P2_2
#define Pin_AmpOff	P2_3
#define Pin_PiOn		P2_5
#define Pin_PiOff		P2_4	//inverted Pi On

#endif


__bit AmpOff; 
__bit PiOn;
__bit PowerSwitch;

// link stdio to UART
int  putchar(int c) 
{
	SBUF = c;
	while (0==TI){}
	TI = 0;
	return c;
}

void WaitSeconds(unsigned char seconds)
{
	// use all interupt sources to create delay
	unsigned long j;
	for(j=(unsigned long)seconds*(unsigned long)4556; j; j--)
		{
		PCON=0b00000001;				//go idel, wake up by any int
		}
}

void ToggelPower()
{
	PiOn=!PiOn;

	AmpOff=!PiOn;

	//switch monitor synchronusly
	PowerSwitch=PiOn;

	if (PiOn)
		{					// Power up
		Pin_PiOn=PiOn;
		Pin_PiOff=!PiOn;
		P1_0=PiOn;
		P1_1=PiOn;
		Pin_PowerSwitch=PowerSwitch;
		WaitSeconds(DelayStartup);
		Pin_AmpOff=AmpOff;
		}
	else
		{					// Power down
		Pin_AmpOff=AmpOff;
		printf_tiny("shutdown\n");
		WaitSeconds(DelayShutdown);
		Pin_PowerSwitch=PowerSwitch;
		P1_0=PiOn;
		P1_1=PiOn;
		Pin_PiOn=PiOn;
		Pin_PiOff=!PiOn;
		}
}

void ToggelMute()
{
	if (PiOn)			//do not switch the amp on if the pi is off
		{
		AmpOff=!AmpOff;
		Pin_AmpOff=AmpOff;
		}
}