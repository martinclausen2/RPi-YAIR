/** Handels keys reading
 */

// Encoder and Keys should not overlap in their bits
#define KeysPort		P0		//select port with keys
#define SelKeys		0b11001100	//select keys within key port
#define KeyPowerMute	0b00000100
#define KeyA		0b00001000
#define KeyB		0b01000000
#define KeyC		0b10000000

#define KeyPressShort	5

volatile unsigned char KeyState;		//stores the keys state
volatile unsigned char OldKeyState;
volatile unsigned char KeyPressDuration;

void WDT_RTC_isr(void) __interrupt (10) __using(isrregisterbank)	//int from internal RTC to read keys
{
	timerFlag=1;
	//Get KeysState and invert it
	KeyState = ~KeysPort & SelKeys;
	//decode keystate and measure time, no release debouncing, so do not call to often
	if (KeyState)					//Key pressed?
		{
		if (0 == OldKeyState)			//Is this key new?
			{
			OldKeyState = KeyState;		// then store it
			KeyPressDuration = 0;		//tick zero
			}
		else if (OldKeyState == KeyState) 
			{				//same key is pressed
			if (0xFF != KeyPressDuration)	//count ticks while same key is being pressed
				++KeyPressDuration;
			}
		else
			{				//we have a new key without an release of the old one
			OldKeyState = KeyState;		// store new one
			KeyPressDuration=0;		// reset time
			}
		}
	RTCCON = RTCen;					//Reset int flag manually
}