/** Setup MCU for I-radio with 89LPC922 / 89LPC93X
 *  V1 2014-10-09
 */

//Disable or Enable RTC with int on external high frequ crystal and clear int flag
#define RTCen	0b01100011
#define RTCdis    0b01100000

void InitMCU()
{
	//Init MCU core
	AUXR1=0b10000000;		//lower power consumption below 8 MHz
	DIVM=0x02;		//run CPU at quard speed
	PCONA=0b01111101;		//Disable EEPROM, analog voltage comperators, ADC, I2C, SPI, CCU
				//enable RTC, UART

	//Init port pins
	//Write 1 to port pin if internal control (SPI, UART, ... ) of the port pin is required 

	//P0.0 Quasi-Bi Encoder0 A
	//P0.1 Quasi-Bi Encoder0 B
	//P0.2 Quasi-Bi Key0 A
	//P0.3 Quasi-Bi key0 B
	//P0.4 Quasi-Bi Encoder1 A
	//P0.5 Quasi-Bi Encoder1 B
	//P0.6 Quasi-Bi Key1 A
	//P0.7 Quasi-Bi key1 B
	P0M1  = 0b00000000;
	P0M2  = 0b00000000;
	//apply save start-up configuration
	P0    = 0b11111111;

#ifdef LPC922
	//P1.0 Push-Pull TxD
	//P1.1 Input RxD
	//P1.2 Open-Drain SCL
	//P1.3 Open-Drain SDA Pi Off (inverted Pi On)
	//P1.4 Push-Pull Pi On
	//P1.5 Input (/Reset) /RC5 input
	//P1.6 Push-PUll Pwr Sw
	//P1.7 Push-Pull Amp Off
	P1M1 = 0b00101110;
	P1M2 = 0b11011101;
	//apply save start-up configuration
	P1   = 0b10101000;	//everything off
#else
	//P1.0 Push-Pull TxD
	//P1.1 Input RxD
	//P1.2 Open-Drain SCL
	//P1.3 Open-Drain SDA
	//P1.4 Quasi-Bi NC
	//P1.5 /Reset
	//P1.6 Quasi-Bi NC
	//P1.7 Quasi-Bi /RC5 input
	P1M1 = 0b00001110;
	P1M2 = 0b00001101;
	//apply save start-up configuration
	P1   = 0b11110000;	//communication off

	//P2.0 Quasi-Bi NC
	//P2.1 Quasi-Bi NC
	//P2.2 Push-PUll Pwr Sw
	//P2.3 Push-Pull Amp Off
	//P2.4 Quasi-Bi  Pi Off (inverted Pi On)
	//P2.5 Push-Pull Pi On
	//P2.6 Quasi-Bi NC
	//P2.7 Quasi-Bi NC
	P2M1 = 0b00000000;
    	P2M2 = 0b00101100;
	//apply save start-up configuration
	P2   = 0b11011011;	//everything off
#endif

    	//P3 is used for the resonator

	//Setup UART
    	// Seriellen Port einstellen
    	// Mode 1, Baud Rate Generator
    	// rate = CCLK/((BRGR1, BRGR0)+16)
    	//      = 57600 Baud
    	BRGR1 = 0;
         BRGR0 = 10;
         SCON = 0b01010000;	//Mode 1, do receive data 
    	// after enable of baud rate generator BRGRX can not be writen to anymore!
    	BRGCON = 0b00000011;

	//Init RTC of P89LPC93X
	// config RTC as Timer with 6*10^6/4/128/10=1172 reload => 10Hz int source
    	RTCH = 0x04; 
    	RTCL = 0x94;
    	RTCCON = RTCdis;
	//enable when configured

    	//switch WTD off or we will be burried under ints
    	WDCON &= 0b11111011; 	//set WD_RUN=0

	//Setup Timer 0 and Timer 1

    	// config Timer 0 for RC5 receiver
    	// cascaded for encoder decoding
    	TMOD = 0b00000010;	//Init T0 (auto reload timer)
    	TCON = 0b00010000;	//gibt T0 frei
    	TH0 = 256-166;		//T0 high Byte => Reload-Wert
        		        		//clk source is PCLK=CCLK/2
	//Init Int
    	IEN0 = 0b11000010;	//Interupt internal RTC (Bit 6) enable
    				//T0 enable
    	RTCCON = RTCen;
}