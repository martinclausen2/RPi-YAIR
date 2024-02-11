/**
 * Main Program of a RC5, rotational encoder and keyboard interface
 * @file Main.c
 * use a P89LPC922 or P89LPC93x with 6Mhz resonator or quarz
 * communicates at 57600 bit/s
 * 
 * V1 2014-10-08
 */

#define LPC922

#ifdef LPC922
#include <p89lpc922.h>
#else
#include <p89lpc935_6.h>
#endif
#include <stdio.h>

#define DelayStartup	30
#define DelayShutdown	10

void T0_isr(void) __interrupt (1);		//int from Timer 0 to read RC5 state
void WDT_RTC_isr(void) __interrupt (10);	//int from internal RTC to update PWM read keys

#define isrregisterbank	2		//for all isr on the SAME priority level 

__bit timerFlag;

#include "InitMCU.c"
#include "DualEncoder.c"
#include "ControlPi.c"
#include "RC5.c"
#include "Keys.c"

/** Main loop */
void main()
{
	InitMCU();
	AmpOff=1;
	ToggelPower();
	// Infinite loop
	while(1) {

		if(timerFlag)
		{
			DecodeRemote();

			// A Key was pressed if OldKeyState != 0 and Keystate = 0
			// OldKeyState = 0 must be set by receiving program after decoding as a flag
			if (0 == KeyState)
				{
				switch(OldKeyState)
					{
					case KeyPowerMute:
						if (KeyPressShort > KeyPressDuration)
							{
							if (PiOn) 
								{
								ToggelMute();
								}
							else
								{
								ToggelPower();
								}
							}
						else 
							{
							ToggelPower();
							}
					break;
					case KeyA:
					printf_tiny("KeyA %d\n", KeyPressDuration);
					break;
					case KeyB:
					printf_tiny("KeyB %d\n", KeyPressDuration);
					break;
					case KeyC:
					printf_tiny("KeyC %d\n", KeyPressDuration);
					break;
					}
				OldKeyState=0;
				}

			// A Rotation occured if EncoderSteps!= 0
			// EncoderSteps = 0 must be set by receiving program after decoding
			if (EncoderStepsA)
				{
				printf_tiny("EncA %d\n", EncoderStepsA);
				EncoderStepsA = 0;		//ack received steps
				}
			if (EncoderStepsB)
				{
				printf_tiny("EncB %d\n", EncoderStepsB);
				EncoderStepsB = 0;		//ack received steps
				}
		}
		PCON=0b00000001;				//go idel, wake up by any int
	}
}
