/** Int routine for TWO rotational encoder
 * V0 2013-01-15

 * The calling frequency should not be to low, because otherwise the
 * state machine will be the confused by contact bouncing
 * adding RC-filter may help to keep the calling frequency down

 * duplicated everything to save stack space and increase execution speed

 * A Rotation occured if 0!=EncoderSteps
 * EncoderSteps = 0 must be set by receiving program after decoding
 * Example
// if (EncoderSteps)
// {
// 	LCD_ClearDisplay();
//	printf_tiny("Encoder %d",  EncoderSteps);
//	EncoderSteps=0;
// }
*/

// Encoder and Keys should not overlap in their bits
#define EncoderPort	P0		//select port with encoder
#define SelEncoder 	0b00000011	//select Encoder bits, must be two lower bits
#define SelEncoderState	0b00011100	//select Enocder state bits
#define KeyIncr		0b00000001
#define KeyDecr		0b00000010
#define ShiftEncoderAccel	0x01		//max 7, depends on caling frequency of encoder decode function and required acceleration
#define SelEncoderAccel	0x7F		//depends of ShiftEncoderAccel
#define maxEncoderSteps	100		//avoid overflow
#define minEncoderSteps	-100
#define EncoderAccelStep	0x01		//depends on caling frequency of encoder decode function and required acceleration
#define maxEncoderAccel    0xFF
#define startEncoderDecay	0xAF		//start value for decrementing accelleration, depends on calling frequency

/** State Machine table for rotational encoder
   Format: IN  ACC.0 inc
              ACC.1 dec
              ACC.2 oldstate
              ACC.3 oldstate
              ACC.4 oldstate
          OUT ACC.0 inc, low active!
              ACC.1 dec, low active!
              ACC.2 state
              ACC.3 state
              ACC.4 state
*/

__code unsigned char tblEncoder[] = {
	0b00000011, 0b00000111, 0b00010011, 0b00000011,
	0b00001011, 0b00000111, 0b00000011, 0b00000011,
	0b00001011, 0b00000111, 0b00001111, 0b00000011,
	0b00001011, 0b00000011, 0b00001111, 0b00000001,
	0b00010111, 0b00000011, 0b00010011, 0b00000011,
	0b00010111, 0b00011011, 0b00010011, 0b00000011,
	0b00010111, 0b00011011, 0b00000011, 0b00000010
	};

volatile signed char EncoderStepsA;
volatile signed char EncoderStepsB;

void Encoder()
{
	static unsigned char EncoderStateA;	//stores the encoder state machine A state
 	static unsigned char OldEncoderStateA;
	static unsigned char EncoderAccelA;

	static unsigned char EncoderStateB;	//stores the encoder state machine B state
	static unsigned char OldEncoderStateB;
	static unsigned char EncoderAccelB;

	static unsigned char EncoderDecay;	//common time base


	//construct new state machine input from encoder state and old state
	EncoderStateA &= SelEncoderState;
	EncoderStateA |= (SelEncoder & EncoderPort);
	EncoderStateA = tblEncoder[EncoderStateA];

	//Measure the number of steps, introduce acceleration
	if (KeyIncr == EncoderStateA)
		{
		if ((EncoderStateA == OldEncoderStateA) && (maxEncoderSteps > EncoderStepsA))
			{
			if (maxEncoderAccel > EncoderAccelA)
				{
				EncoderAccelA +=EncoderAccelStep;
				}
			++EncoderStepsA;
			EncoderStepsA += (EncoderAccelA >> ShiftEncoderAccel) & SelEncoderAccel;
			}
		else
			{
			EncoderStepsA = 1;	//change in direction
			OldEncoderStateA = EncoderStateA;
			EncoderAccelA = 0;
			}
		}
	else if (KeyDecr == EncoderStateA)
		{
		if ((EncoderStateA == OldEncoderStateA) && (minEncoderSteps < EncoderStepsA))
			{
			if (maxEncoderAccel > EncoderAccelA)
				{
				EncoderAccelA +=EncoderAccelStep;
				}
			--EncoderStepsA;
			EncoderStepsA -= (EncoderAccelA >> ShiftEncoderAccel) & SelEncoderAccel;
			}
		else
			{
			EncoderStepsA = -1;	//change in direction
			OldEncoderStateA = EncoderStateA;
			EncoderAccelA = 0;
			}
		}

	//construct new state machine input from encoder state and old state
	EncoderStateB &= SelEncoderState;
	EncoderStateB |= (SelEncoder & (EncoderPort>>4));
	EncoderStateB = tblEncoder[EncoderStateB];

	//Measure the number of steps, introduce acceleration
	if (KeyIncr == EncoderStateB)
		{
		if ((EncoderStateB == OldEncoderStateB) && (maxEncoderSteps > EncoderStepsB))
			{
			if (maxEncoderAccel > EncoderAccelB)
				{
				EncoderAccelB +=EncoderAccelStep;
				}
			++EncoderStepsB;
			EncoderStepsB += (EncoderAccelB >> ShiftEncoderAccel) & SelEncoderAccel;
			}
		else
			{
			EncoderStepsB = 1;		//change in direction
			OldEncoderStateB = EncoderStateB;
			EncoderAccelB = 0;
			}
		}
	else if (KeyDecr == EncoderStateB)
		{
		if ((EncoderStateB == OldEncoderStateB) && (minEncoderSteps < EncoderStepsB))
			{
			if (maxEncoderAccel > EncoderAccelB)
				{
				EncoderAccelB +=EncoderAccelStep;
				}
			--EncoderStepsB;
			EncoderStepsB -= (EncoderAccelB >> ShiftEncoderAccel) & SelEncoderAccel;
			}
		else
			{
			EncoderStepsB = -1;	//change in direction
			OldEncoderStateB = EncoderStateB;
			EncoderAccelB = 0;
			}
		}

	if (EncoderDecay)
		{
		--EncoderDecay; 
		}
	else
		{
		EncoderDecay=startEncoderDecay;
		if (EncoderAccelA)
			{
			--EncoderAccelA;			//no new step, decrease acceleration
			}
		if (EncoderAccelB)
			{
			--EncoderAccelB;			//no new step, decrease acceleration
			}
		}

}