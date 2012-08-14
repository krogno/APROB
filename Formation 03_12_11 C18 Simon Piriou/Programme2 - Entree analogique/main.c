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




void delay(void) // Attend 1/6 de secondes
{
	Delay10KTCYx(200);
}

void main(void)
{
	int valeurCapteur = 0;

	// page 122, section 10.4
	TRISD=0b00001111; // Les pins D4 à D7 sont configurés en sortie (0 comme Output), et les autres en entrée (1 comme Input)
	PORTD=0x00;	// On connecte tous les pins du port D à la masse (les leds sont éteintes).

	// initialisation du convertisseur analogique-numerique (page 261, section 21.0)
	TRISAbits.RA0 = 1;
	ADCON0=0b00000001;   // Configure le convertisseur analogique-numerique sur AN0, et activation du module.
	ADCON1=0b00001110;   // Utilise comme reference Vdd et Vss (+5v et 0v), entrée analogique sur AN0.
	ADCON2=0b10101011;   // Configure la vitesse d'aquisition, et la résolution de la mesure (sur 10 bits).

	valeurCapteur = 0;
	delay(); // attend 250ms
	while(1)
	{
		ADCON0bits.GO=1; // lance une acquisition
		while(ADCON0bits.GO); // Attend que la conversion soit finie. (quelques ms)

		valeurCapteur = ADRESL; // recupere les 8 premiers bits.
   	 	valeurCapteur += (ADRESH << 8); // les deux derniers bits sont contenus dans ADRESH. il faut donc les decaler de 8.
		
		delay(); // attend 250ms
		
		
		// Comme il y a seulement 4 leds, on découpe de la sorte :
		//   0-255 => 0
		// 256-511 => 1
		// 512-767 => 2
		// 768-1023 => 3
		PORTD = 1 << ((valeurCapteur/256) + 4);
	}
}
