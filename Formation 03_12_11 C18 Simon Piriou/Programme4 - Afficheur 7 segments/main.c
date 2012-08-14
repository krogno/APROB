#include <p18f4550.h>
#include <timers.h>
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


int afficheur7seg;
int secondes;
const int Afficheur7segments[16] = {
0b10000000, // 0
0b10011110, // 1
0b01001000, // 2
0b00001100, // 3
0b00010110, // 4
0b00100100, // 5
0b00100000, // 6
0b10001110, // 7
0b00000000, // 8
0b00000100, // 9
0b00000010, // "A"
0b00110000, // "b"
0b11100000, // "C"
0b00011000, // "d"
0b01100000, // "E"
0b01100010  // "F"
};

#define RCLKPin  LATBbits.LATB1 // Pin connected to RCLK -- (latch pin) of 74HC595
#define SRCLKPin LATBbits.LATB2 // Pin connected to SRCLK -- shift register clock pin -- of 74HC595
#define SERPin   LATBbits.LATB0 // Pin connected to SER -- serial data input -- of 74HC595
#define Init74HC595()  LATB &= 0b11111000; TRISB &= 0b11111000;
#define max(a,b) ((a) > (b) ? (a) : (b))
#define min(a,b) ((a) < (b) ? (a) : (b))

void afficher7seg(int);
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
		WriteTimer1(0x81CF); // 20ms*1500 = 30000 => 65536- 30000 = 0x81CF (interruption dans 20ms)
		PIR1bits.TMR1IF = 0;
		cnt++;
		afficheur7seg = 1; // rafraichit l'afficheur 7 segment
		
		if(cnt == 25) // Au bout de 0,5 secondes
		{ 
			PORTDbits.RD4 = !PORTDbits.RD4;
		}
		else if(cnt == 50) // Au bout de 1 seconde
		{
			cnt = 0;
			PORTDbits.RD4 = !PORTDbits.RD4;
			secondes ++;
			if(secondes == 100)
				secondes = 0;
		}
	}
}

#pragma interruptlow low_isr 
void low_isr (void) 
{ 
/* ISR code goes here */ 
}


void delay(void) // Attend une seconde
{
	Delay10KTCYx(200);
	Delay10KTCYx(200);
	Delay10KTCYx(200);
}


void afficher7seg(int valeur)
{
	static int afficheur = 0;

	int buff;
	int index = 128;

	if(afficheur)
	{
		buff = Afficheur7segments[valeur%10]|1;
	}
	else
	{
		buff = Afficheur7segments[valeur/10];
	}

	RCLKPin = 0;
	while(index) {
       if(buff & index)
           SERPin = 1;
       else
           SERPin = 0;

       SRCLKPin = 1;
       SRCLKPin = 0;
       index >>= 1;
   }
	RCLKPin = 1;
	afficheur ^= 1;
}


void main(void)
{
	// page 122, section 10.4
	TRISD=0b00001111; // Les pins D4 à D7 sont configurés en sortie (0 comme Output), et les autres en entrée (1 comme Input)
	PORTD=0x00;	// On connecte tous les pins du port D à la masse (les leds sont éteintes).
	

	Init74HC595();

	OpenTimer1(TIMER_INT_ON & T1_8BIT_RW & T1_SOURCE_INT & T1_PS_1_8 & T1_OSC1EN_OFF & T1_SYNC_EXT_OFF);
	RCONbits.IPEN = 1; //Enable priority levels on interrupts
	RCONbits.SBOREN = 0; //Disable BOR
	PIR1bits.TMR1IF = 0;
	INTCON = 0b10000000;
	IPR1bits.TMR1IP = 1;
	PIE1bits.TMR1IE = 1;

	secondes = 0;
	WriteTimer1(0x81CF); // 20ms*1500 = 30000 => 65536- 30000 = 0x81CF (interruption dans 20ms)
	
	while(1)
	{
		if(afficheur7seg)
		{
			afficher7seg(min(99,secondes));
			afficheur7seg = 0;
		}
		
		// On peut faire autre chose ici, mais il ne faut surtout pas mettre de delay.
	}
}
