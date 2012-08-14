#include <p18f4550.h>
#include <timers.h>

// Page 288 (section 25.1)
#pragma config PLLDIV   = 5       // (20 MHz input)
#pragma config CPUDIV   = OSC1_PLL2
#pragma config USBDIV   = 2       // Clock source from 96MHz PLL/2
#pragma config FOSC     = HSPLL_HS //HS oscillator, PLL enabled, HS used by USB
#pragma config VREGEN   = ON

#pragma config LVP      = OFF
#pragma config WDT      = OFF

#pragma config FCMEN    = OFF
#pragma config IESO     = OFF
#pragma config PWRT     = OFF
#pragma config BOR      = ON
#pragma config BORV     = 3


#pragma config WDTPS    = 32768
#pragma config MCLRE    = ON
#pragma config LPT1OSC  = OFF
#pragma config PBADEN   = OFF
#pragma config CCP2MX   = ON
#pragma config STVREN   = ON
#pragma config XINST    = OFF       // Extended Instruction Set
#pragma config CP0      = OFF
#pragma config CP1      = OFF
#pragma config CP2      = OFF
#pragma config CPB      = OFF
#pragma config CPD      = OFF
#pragma config WRT0     = OFF
#pragma config WRT1     = OFF
#pragma config WRT2     = OFF
#pragma config WRTB     = ON       // Boot Block Write Protection
#pragma config WRTC     = OFF
#pragma config WRTD     = OFF
#pragma config EBTR0    = OFF
#pragma config EBTR1    = OFF
#pragma config EBTR2    = OFF
#pragma config EBTRB    = OFF





void high_isr(void);
void low_isr(void);

#pragma code _HIGH_INTERRUPT_VECTOR = 0x000008
void _high_ISR (void)
{
    _asm goto high_isr _endasm
}

#pragma code _LOW_INTERRUPT_VECTOR = 0x000018
void _low_ISR (void)
{
    _asm goto low_isr _endasm
}

#pragma code // Retourne dans la section du code

#pragma interrupt high_isr
void high_isr (void)
{
	static int cnt = 0;
		
	if(PIR1bits.TMR1IF)
	{
		// Prescale de 8 => le timer est incrémenté à 12M/8 = 1,5 Mhz
		WriteTimer1(0x81CF); // 20ms*1500 = 30000 => 65536- 30000 = 0x81CF (interruption dans 20ms)
		PIR1bits.TMR1IF = 0;
		cnt++;
		if(cnt > 25) // toute les 0,5 secondes (25*20ms = 0,5 secondes)
		{
			PORTDbits.RD4 = !PORTDbits.RD4;
			cnt = 0;
		}
	}
}

#pragma interruptlow low_isr 
void low_isr (void) 
{ 
/* ISR code goes here */ 
}



void delay(void) // Attend 0,5 seconde
{
	Delay10KTCYx(200);
	Delay10KTCYx(200);
	Delay10KTCYx(200);
}


void main(void)
{
	// page 122, section 10.4
	TRISD=0b00001111; // Les pins D4 à D7 sont configurés en sortie (0 comme Output), et les autres en entrée (1 comme Input)
	PORTD=0x00;	// On connecte tous les pins du port D à la masse (les leds sont éteintes).
	

	OpenTimer1(TIMER_INT_ON & T1_8BIT_RW & T1_SOURCE_INT & T1_PS_1_8 & T1_OSC1EN_OFF & T1_SYNC_EXT_OFF);
	RCONbits.IPEN = 1; //Enable priority levels on interrupts
	RCONbits.SBOREN = 0; //Disable BOR
	PIR1bits.TMR1IF = 0;
	INTCON = 0b10000000;
	IPR1bits.TMR1IP = 1;
	PIE1bits.TMR1IE = 1;

	while(1)
	{
		delay();
		PORTDbits.RD7 = !PORTDbits.RD7;
	}
}
