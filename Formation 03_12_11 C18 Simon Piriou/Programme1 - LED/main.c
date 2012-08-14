#include <p18f4550.h>
#include <delays.h>

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




// Le PIC est cadensé à 48Mhz (quartz à 20Mhz + PLL). C'est la fréquence de l'oscillateur, Fosc.
// La fréquence des cycles d'instructions est de Fosc/4, soit Fcy = 12 MIPS (Mega instructions seconde).
// On a ainsi Tcy = 1/Fcy = 83,334 ns
// Pour attendre 0,5 seconde, il faut donc attendre 6 millions de cycles

// void Delay1TCY ( void ); // Attend 1 cycle d'instruction
// void Delay10TCYx ( unsigned char n ); // Attend 10*n cycles d'instruction, n=0 à 255
// void Delay100TCYx ( unsigned char n ); // Attend 100*n cycles d'instruction
// void Delay1KTCYx ( unsigned char n ); // Attend 1000*n cycles d'instruction
// void Delay10KTCYx ( unsigned char n ); // Attend 10000*n cycles d'instruction

void delay(void) // attend 0,5 secondes (6M cycles)
{
	Delay10KTCYx(200);
	Delay10KTCYx(200);
	Delay10KTCYx(200);
}

void main(void)
{
	int indiceLed = 4;
	// page 122, section 10.4
	TRISD=0b00001111; // Les pins D4 à D7 sont configurés en sortie (0 comme Output), et les autres en entrée (1 comme Input)
	PORTD=0x00;	// On connecte tous les pins du port D à la masse (les leds sont éteintes).

	while(1)
	{
		delay();
		//PORTDbits.RD5 = !PORTDbits.RD5;
		

		PORTD = 1 << indiceLed;
		indiceLed++;
		if(indiceLed == 8)
			indiceLed = 4;
	}
}
