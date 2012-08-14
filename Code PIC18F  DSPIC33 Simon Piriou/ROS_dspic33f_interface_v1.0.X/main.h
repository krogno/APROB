#include "p33FJ128MC802.h"
#include <dsp.h>
#include <qei.h>
#include <timer.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define VERSION 'v1.0.0'
#define CLOCK_FREQ_10 10 // quartz de 10 MHz
//#define USE_UART
#define BAUDRATE2       19200UL
#define BRG_DIV2        16
#define BRGH2           1

#include "Constantes.h"
#include "Hardware_profile.h"
#include "Odometrie.h"
#include "Asservissement.h"

#define UART_BUFFER_SIZE 64
//#ifdef USE_UART
  //  #include <uart.h>
//#else
  //  #include "i2c.h"
//#endif


/*---------------------------------------------------------------------------*/
/* Status bits			    											     */
/*---------------------------------------------------------------------------*/

_FBS(BSS_NO_FLASH & BWRP_WRPROTECT_OFF); // Pas de segment de boot et pas de protection en écriture
_FOSCSEL(FNOSC_FRC);			// Primary (XT, HS, EC) Oscillator
_FOSC(FCKSM_CSECMD & OSCIOFNC_OFF  & POSCMD_XT);
_FGS(GSS_OFF & GCP_OFF & GWRP_OFF); // Pas de protection du code (Code Protect: Disabled; Write Protect: Disabled)
_FWDT(FWDTEN_OFF)	// WatchDog inactif (configurable en software)


/*---------------------------------------------------------------------------*/
/* Macros		    											     */
/*---------------------------------------------------------------------------*/
#define FLOAT2INT(x) ((x)>=0?(int)((x)+0.5):(int)((x)-0.5))

#define InitTextIO()      InitUART2()
#define PutChar(c)        UART2PutChar(c)
#define PutHex(h)         UART2PutHex(h)
#define PrintString(s)    UART2PrintString(s)
#define PrintBuffer(s,l)  UART2PrintBuffer(s,l)

#define mDataRdyUSART() U2STAbits.URXDA
#define mTxFullUSART()   U2STAbits.UTXBF
//#define max(a,b) ((a) > (b) ? (a) : (b))


/*---------------------------------------------------------------------------*/
/* Variables		    											     */
/*---------------------------------------------------------------------------*/

long Blink = 0; // blink index


char uartBuffer[UART_BUFFER_SIZE];
int uartNextOut = 0;
int uartCommande = 0;

volatile unsigned char Cycle1;
volatile unsigned char Cycle2;


#define	CICLE1_TMO 5 //10		// premier cycle : Xms (odometrie + Angle)
#define	CICLE2_TMO 2

volatile int RtTimer; // timer en s

/*---------------------------------------------------------------------------*/
/* Prototypes		    											     */
/*---------------------------------------------------------------------------*/

void DelayN1ms(int n);
void DelayN10us(int n);
void ISR_Settings(void);
void _ISR _T1Interrupt(void);


/*---------------------------------------------------------------------------*/
/* Fonctions		    											     */
/*---------------------------------------------------------------------------*/

void DelayN1ms(int n)
{
    int ms;
    for (ms = 0; ms < n; ms ++)
    {
        DelayN10us(100);
    }
}

void DelayN10us(int n)
{
    int DelayCount;
    for (DelayCount = 0; DelayCount < (57 * n); DelayCount ++);
}


/****************************************************************************
  Function:
    void ISR_Settings( void )

  Description:
    Configuration des interruptions pour le programme
	- QEI 1 et 2
	- Timer 1, 2 et 4

  Precondition:
 	None

  Parameters:
    None

  Return Values:
    None

  Remarks:
    None
  ***************************************************************************/
void ISR_Settings(void)
{
	IFS3bits.QEI1IF = 0;
	IEC3bits.QEI1IE = 1;
	IFS4bits.QEI2IF = 0;
	IEC4bits.QEI2IE = 1;

	IPC1bits.T2IP = 0x04; // Set Timer 2 Interrupt Priority Level
	IFS0bits.T2IF = 0; // Clear Timer 2 Interrupt Flag
	IEC0bits.T2IE = 1; // Enable Timer 2 interrupt
	T2CONbits.TON = 1; // Start Timer

	IPC6bits.T4IP = 0x05; // Set Timer 4 Interrupt Priority Level
	IFS1bits.T4IF = 0; // Clear Timer 4 Interrupt Flag
	IEC1bits.T4IE = 1; // Enable Timer 4 interrupt
	T4CONbits.TON = 1; // Start Timer

	PR1 = TMR1_VALUE;
	IPC0bits.T1IP = 0x03; // Interuption de priorité 4
	IEC0bits.T1IE = 1; // Activation de l'interuption
	T1CONbits.TON = 1; // 1: start 0: stop

	OC1CONbits.OCM = 0b110;
	OC2CONbits.OCM = 0b110;
	OC3CONbits.OCM = 0b110;
	OC4CONbits.OCM = 0b110;


        // UART Configuration
        IPC7 = 0x4400;	// Mid Range Interrupt Priority level, no urgent reason
	IFS1bits.U2TXIF = 0;	// Clear the Transmit Interrupt Flag
	IEC1bits.U2TXIE = 1;	// Enable Transmit Interrupts
        //IEC1bits.U2TXIE = 0;
	IFS1bits.U2RXIF = 0;	// Clear the Recieve Interrupt Flag
	IEC1bits.U2RXIE = 1;	// Enable Recieve Interrupts
        //IEC1bits.U2RXIE = 0;
	U2MODEbits.UARTEN = 1;	// And turn the peripheral on
	U2STAbits.UTXEN = 1;
}


void InitUART2()
{
	// configure U2MODE
	U2MODEbits.UARTEN = 0;	// Bit15 TX, RX DISABLED, ENABLE at end of func
	//U2MODEbits.notimplemented;	// Bit14
	U2MODEbits.USIDL = 0;	// Bit13 Continue in Idle
	U2MODEbits.IREN = 0;	// Bit12 No IR translation
	U2MODEbits.RTSMD = 0;	// Bit11 Simplex Mode
	//U2MODEbits.notimplemented;	// Bit10
	U2MODEbits.UEN = 0;		// Bits8,9 TX,RX enabled, CTS,RTS not
	U2MODEbits.WAKE = 0;	// Bit7 No Wake up (since we don't sleep here)
	U2MODEbits.LPBACK = 0;	// Bit6 No Loop Back
	U2MODEbits.ABAUD = 0;	// Bit5 No Autobaud (would require sending '55')
	U2MODEbits.URXINV = 0;	// Bit4 IdleState = 1  (for dsPIC)
	U2MODEbits.BRGH = 0;	// Bit3 16 clocks per bit period
	U2MODEbits.PDSEL = 0;	// Bits1,2 8bit, No Parity
	U2MODEbits.STSEL = 0;	// Bit0 One Stop Bit

	// Load a value into Baud Rate Generator.  Example is for 9600.
	// See section 19.3.1 of datasheet.
	//  U2BRG = (Fcy/(16*BaudRate))-1
	//  U2BRG = (37M/(16*9600))-1
	//  U2BRG = 240
	U2BRG = BAUDRATEREG2;//129;// 19200 baud    240;	// 40Mhz osc, 9600 Baud

	// Load all values in for U1STA SFR
	U2STAbits.UTXISEL1 = 0;	//Bit15 Int when Char is transferred (1/2 config!)
	U2STAbits.UTXINV = 0;	//Bit14 N/A, IRDA config
	U2STAbits.UTXISEL0 = 0;	//Bit13 Other half of Bit15
	//U2STAbits.notimplemented = 0;	//Bit12
	U2STAbits.UTXBRK = 0;	//Bit11 Disabled
	U2STAbits.UTXEN = 0;	//Bit10 TX pins controlled by periph
	U2STAbits.UTXBF = 0;	//Bit9 *Read Only Bit*
	U2STAbits.TRMT = 0;	//Bit8 *Read Only bit*
	U2STAbits.URXISEL = 0;	//Bits6,7 Int. on character recieved
	U2STAbits.ADDEN = 0;	//Bit5 Address Detect Disabled
	U2STAbits.RIDLE = 0;	//Bit4 *Read Only Bit*
	U2STAbits.PERR = 0;		//Bit3 *Read Only Bit*
	U2STAbits.FERR = 0;		//Bit2 *Read Only Bit*
	U2STAbits.OERR = 0;		//Bit1 *Read Only Bit*
	U2STAbits.URXDA = 0;	//Bit0 *Read Only Bit*
}




/*******************************************************************************
Function: UART2GetBaudError()

Precondition:
    None.

Overview:
    This routine checks the UART baud rate error percentage and returns it.

Input: None.

Output: Returns the baud rate error in percent.

*******************************************************************************/
char UART2GetBaudError()
{
    unsigned int errorPercent = 0;

	errorPercent = ((BAUD_ERROR*100+BAUDRATE2/2)/BAUDRATE2);
    return (char)errorPercent;
}


/*******************************************************************************
Function: UART2GetChar()

Precondition:
    UART2Init must be called prior to calling this routine.

Overview:
    This routine waits for a byte to be received.  It then returns that byte.

Input: None.

Output: Byte received.

*******************************************************************************/
char UART2GetChar()
{
    char Temp;

    while(IFS1bits.U2RXIF == 0);

    Temp = U2RXREG;
    IFS1bits.U2RXIF = 0;
    return Temp;
}

/*******************************************************************************
Function: UART2IsPressed()

Precondition:
    UART2Init must be called prior to calling this routine.

Overview:
    This routine checks to see if there is a new byte in UART reception buffer.

Input: None.

Output:
    0 : No new data received.
    1 : Data is in the receive buffer

*******************************************************************************/
char UART2IsPressed()
{
    if(IFS1bits.U2RXIF == 1)
        return 1;
    return 0;
}

/*******************************************************************************
Function: UART2PutChar( char ch )

Precondition:
    UART2Init must be called prior to calling this routine.

Overview:
    This routine writes a character to the transmit FIFO, and then waits for the
    transmit FIFO to be empty.

Input: Byte to be sent.

Output: None.

*******************************************************************************/
void UART2PutChar( char ch )
{
    U2TXREG = ch;
    //while(U2STAbits.TRMT == 0);
    while(mTxFullUSART());
}

/*******************************************************************************
Function: UART2PrintString( char *str )

Precondition:
    UART2Init must be called prior to calling this routine.

Overview:
    This function prints a string of characters to the UART.

Input: Pointer to a null terminated character string.

Output: None.

*******************************************************************************/
void UART2PrintString( char *str )
{
    unsigned char c;

    while( (c = *str++) )
        UART2PutChar(c);
}

void UART2PrintBuffer( char *str, int len )
{
    int i;

    for(i=0; i<len; i++)
        UART2PutChar(*(str++));
}


/*--------------------------------------------------------------------------*
 * Timer 1 interupt                                                         *
 *--------------------------------------------------------------------------*/
void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)	// Timer 1
{
	_T1IF=0;   		// interrupt flag reset
	Blink ++;			// heartbeat LED blink
	// cycle 0 actions
	PID_CALC_FLAG = 1;	// PID1&2 and speed calculation enabled
	Cycle1 ++;
	// cycle 1 actions
	if (Cycle1 >= CICLE1_TMO)
	{
		Cycle1 = 0;
		CYCLE1_FLAG = 1; // it's time to start first cycle actions
		Cycle2 ++;
		// cycle 2 actions
		if (Cycle2 >= CICLE2_TMO)
		{
			Cycle2 = 0;
			CYCLE2_FLAG=1; // it's time to start second cycle actions
			RtTimer --;	   // real time delay
		}
	}
}

void __attribute__((__interrupt__, auto_psv)) _T2Interrupt( void )
{
	IFS0bits.T2IF = 0; // Clear Timer 2 interrupt flag
}

void __attribute__((__interrupt__, auto_psv)) _T4Interrupt( void )
{
	TimerPid1 ++;
	TimerPid2 ++;
	IFS1bits.T4IF = 0; // Clear Timer 2 interrupt flag
}

void __attribute__((__interrupt__,auto_psv)) _QEI1Interrupt(void)
 {
    QEI1CONbits.CNTERR = 0;  // reset count error flag
    IFS3bits.QEI1IF = 0;
 }

void __attribute__((__interrupt__,auto_psv)) _QEI2Interrupt(void)
 {
    QEI2CONbits.CNTERR = 0;  // reset count error flag
    IFS4bits.QEI2IF = 0;
 }

void __attribute__ ((interrupt, no_auto_psv)) _U2RXInterrupt(void)
{
    //OC2RS = OC2RS^(TMR2_VALUE/2);

    if( uartNextOut < (UART_BUFFER_SIZE - 1))
    {
        uartBuffer[uartNextOut] = U2RXREG;
        //U2TXREG = uartBuffer[uartNextOut];
        if(uartBuffer[uartNextOut] == '\n' || uartBuffer[uartNextOut] == '\r')
        {
            uartCommande = 1;
            uartBuffer[uartNextOut] = 0;
            LED_BLINK = !LED_BLINK;

        }
        else
            uartNextOut++;
    }
    else
    {
        uartBuffer[UART_BUFFER_SIZE-1] = '\n';
        uartCommande = 1;
    }
    IFS1bits.U2RXIF = 0;
}

void __attribute__ ((interrupt, no_auto_psv)) _U2TXInterrupt(void) {
	IFS1bits.U2TXIF = 0;
}
