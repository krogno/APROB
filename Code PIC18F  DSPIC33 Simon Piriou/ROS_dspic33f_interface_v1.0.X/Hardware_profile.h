

/*---------------------------------------------------------------------------*/
/* Parametres clock			    											     */
/*---------------------------------------------------------------------------*/

#ifdef CLOCK_FREQ_10
    #define FCY 40000000
/*
CLOCK_FREQ = 10 MHz
Fosc = CLOCK_FREQ*M/(N1*N2) = 10M*32/(2*2)=80Mhz
Fcy = Fosc / 2 = 40
*/
// TCY = 1 / FCY = 25 ns
#else
    #define FCY 39628800
/*
CLOCK_FREQ = 7,3728 MHz
Fosc = CLOCK_FREQ*M/(N1*N2) = 7.3728M*43/(2*2)=79,2576Mhz
Fcy = Fosc / 2 = 39.6288
*/
// TCY = 1 / FCY = 25.2341731266 ns
#endif

#define TCY 1/((float)(FCY)
#define _ISR_PSV __attribute__((interrupt, auto_psv))
#define RC6Tick(x) (Delay1KTCYx(4*x),Delay100TCYx(4*x),Delay10TCYx(3*x))
#define	DELAY100MS Delay10KTCYx(100)	// 100ms
const long Tcy = 1000000/(float)(FCY)* 100000000;


/*---------------------------------------------------------------------------*/
/* UART clock : U2BRG register value and baudrate mistake calculation									     */
/*---------------------------------------------------------------------------*/
#if defined (__C30__)
    #define BAUDRATEREG2        ((FCY/BAUDRATE2/BRG_DIV2)-1)
#endif

#if defined (__C30__)
    #define BAUD_ACTUAL         (FCY/BRG_DIV2/(BAUDRATEREG2+1))
#endif

#define BAUD_ERROR              ((BAUD_ACTUAL > BAUDRATE2) ? BAUD_ACTUAL-BAUDRATE2 : BAUDRATE2-BAUD_ACTUAL)
#define BAUD_ERROR_PERCENT      ((BAUD_ERROR*100+BAUDRATE2/2)/BAUDRATE2)

#if defined (__C30__)
	#if (BAUD_ERROR_PERCENT > 2)
	    #error UART frequency error is worse than 2%
	#elif(BAUD_ERROR_PERCENT > 1)
	    #warning UART frequency error is worse than 1%
	#endif
#endif


/****************************************************************************
  Function:
    void Settings( void )

  Description:
    Configuration des pins pour le programme
	- QEI 1 et 2
	- OC 1, 2, 3 et 4 pour les moteurs

  Precondition:
 	None

  Parameters:
    None

  Return Values:
    None

  Remarks:
    None
  ***************************************************************************/
void Settings(void)
{
	#ifdef CLOCK_FREQ_10
	// Configure Oscillator to operate the device at 40 Mhz
	// Fosc= Fin*M/(N1*N2), Fcy=Fosc/2
	// Fosc= 10M*32(2*2)=80Mhz pour 10 MHz en entrée
	PLLFBD=30;					// M=32
	#warning **********************************************************************
	#warning -- compiling for 10MHz oscillator, frenquency = 80.0000 MHz **********
	#else

	// Configure Oscillator to operate the device at 39,6288 Mhz
	// Fosc= Fin*M/(N1*N2), Fcy=Fosc/2
	// Fosc= 7.3728M*43(2*2)=79,2576Mhz pour 7,3728 MHz en entrée
	PLLFBD=41;					// M=43
	#warning **********************************************************************
	#warning -- compiling for 7.3728MHz oscillator, frenquency = 79.2576 MHz **
	#endif

	CLKDIVbits.PLLPOST=0;		// N1=2
	CLKDIVbits.PLLPRE=0;		// N2=2

	// Disable Watch Dog Timer
	RCONbits.SWDTEN=0;

	// Clock switching to incorporate PLL
	__builtin_write_OSCCONH(0x03);		// Initiate Clock Switch to Primary
										// Oscillator with PLL (NOSC=0b011)
	__builtin_write_OSCCONL(0x01);		// Start clock switching


	while (OSCCONbits.COSC != 0b011);	// Wait for Clock switch to occur
	while(OSCCONbits.LOCK!=1) {};		// Wait for PLL to lock


	
	#if defined(__dsPIC33FJ64MC802__) || defined(__dsPIC33FJ128MC802__)
		#warning -- compiling for a 28 pin DSC dsPIC33FJ128MC802 **********************

		/*-----------------------------------------------------------------------*/
		/* Port	A   			    										     */
		/*-----------------------------------------------------------------------*/

		// None

		/*-----------------------------------------------------------------------*/
		/* Port	B   			    										     */
		/*-----------------------------------------------------------------------*/
		//TRISBbits.TRISB5 = 1; // odometrie
		//TRISBbits.TRISB6 = 1; // odometrie

		//TRISBbits.TRISB10 = 1; // odometrie
		//TRISBbits.TRISB11 = 1; // odometrie

		TRISBbits.TRISB2 = 0; // moteur
		TRISBbits.TRISB3 = 0; // moteur

		TRISBbits.TRISB14 = 0; // moteur
		TRISBbits.TRISB15 = 0; // moteur


		// Peripheral PIN selection ***********************************
		// Unlock Registers
		//*************************************************************
		asm volatile ( "mov #OSCCONL, w1 \n"
		"mov #0x45, w2 \n"
		"mov #0x57, w3 \n"
		"mov.b w2, [w1] \n"
		"mov.b w3, [w1] \n"
		"bclr OSCCON, #6 ");

		/*-----------------------------------------------------------------------*/
		// Configure Fonctions
		/*-----------------------------------------------------------------------*/

		// Configuration odometrie
		RPINR14bits.QEA1R = 6; // QEA1 sur RP6
		RPINR14bits.QEB1R = 5; // QEB1 sur RP5
		RPINR16bits.QEA2R = 10; // QEA2 sur RP10
		RPINR16bits.QEB2R = 11; // QEB2 sur RP11

		// Configuration PWM
		RPOR1bits.RP3R = 0b10010; // OC1 sur B2
		RPOR1bits.RP2R = 0b10011; // OC2 sur B3
		RPOR7bits.RP14R = 0b10100; // OC3 sur B14
		RPOR7bits.RP15R = 0b10101; // OC4 sur B15

                // Configuration UART
                RPINR19bits.U2RXR = 9;// RX2 sur RP4
                RPOR4bits.RP8R = 0b00101;// pour TX2 -- 0b00011;// TX1 sur RP7 --


		/*-----------------------------------------------------------------------*/
		// Lock Registers
		/*-----------------------------------------------------------------------*/
		asm volatile ( "mov #OSCCONL, w1 \n"
		"mov #0x45, w2 \n"
		"mov #0x57, w3 \n"
		"mov.b w2, [w1] \n"
		"mov.b w3, [w1] \n"
		"bset OSCCON, #6");
		// *********************************** Peripheral PIN selection

		#warning *********************************************************************



		/*-----------------------------------------------------------------------*/
		// QEI
		/*-----------------------------------------------------------------------*/

		QEI1CON = 0;             // clr CNTERR bit (among others)
		QEI1CONbits.QEIM = 0b111;    // x4 reset by match
		QEI1CONbits.POSRES = 0;  // Index reset disable
		QEI1CONbits.SWPAB = 0;   // 0->dont need to swap a/b phases
		DFLT1CON = 0;            // digital filter set off
		DFLT1CONbits.QEOUT = 0;  // enable digital filter on phase a,b

		MAX1CNT = ODO_MAX;        // counts/rev (used as preset when index pulse seen)
		POS1CNT = 0;


		QEI2CON=QEI1CON; //même config pour les deux codeurs
		QEI2CONbits.SWPAB = 0;   // 0 = pas d'inversion entre les phases A et B
		MAX2CNT=MAX1CNT;
		POS2CNT=POS1CNT;
		DFLT2CON=DFLT1CON;



		/*-----------------------------------------------------------------------*/
		// Timer 2 // Pour les PWM
		/*-----------------------------------------------------------------------*/

		T2CONbits.TON = 0; 		// Disable Timer
		T2CONbits.TCS = 0; 		// Select internal instruction cycle clock
		T2CONbits.TGATE = 0; 	// Disable Gated Timer mode
		T2CONbits.TCKPS = 0b00;	// Select 1:1 Prescaler
		TMR2 = 0x00; 			// Clear timer register
		PR2 = TMR2_VALUE; 		// Load the period value



		/*-----------------------------------------------------------------------*/
		// Timer 4 // Pour l'asservissement en vitesse
		/*-----------------------------------------------------------------------*/

		T4CONbits.TON = 0; 		// Disable Timer
		T4CONbits.TCS = 0; 		// Select internal instruction cycle clock
		T4CONbits.TGATE = 0; 	// Disable Gated Timer mode
		T4CONbits.TCKPS = 0b00;	// Select 1:1 Prescaler
		TMR4 = 0x00; 			// Clear timer register
		PR4 = TMR4_VALUE; 		// Load the period value


		/*---------------------------------------------------- ------------------*/
		// OC (Output Compare) Pour les PWM des moteurs
		/*-----------------------------------------------------------------------*/

		// Initialize Output Compare Module
		OC1CONbits.OCM = 0b000; // Disable Output Compare Module
		OC1R = 0; // Write the duty cycle for the first PWM pulse
		OC1RS = 0; // Write the duty cycle for the second PWM pulse
		OC1CONbits.OCTSEL = 0; // Select Timer 2 as output compare time base
		OC1R = 0; // Load the Compare Register Value
		//OC1CONbits.OCM = 0b110; // Select the Output Compare mode


		// Initialize Output Compare Module
		OC2CONbits.OCM = 0b000; // Disable Output Compare Module
		OC2R = 0; // Write the duty cycle for the first PWM pulse
		OC2RS = 0; // Write the duty cycle for the second PWM pulse
		OC2CONbits.OCTSEL = 0; // Select Timer 2 as output compare time base
		OC2R = 0; // Load the Compare Register Value
		//OC2CONbits.OCM = 0b110; // Select the Output Compare mode


		// Initialize Output Compare Module
		OC3CONbits.OCM = 0b000; // Disable Output Compare Module
		OC3R = 0; // Write the duty cycle for the first PWM pulse
		OC3RS = 0; // Write the duty cycle for the second PWM pulse
		OC3CONbits.OCTSEL = 0; // Select Timer 2 as output compare time base
		OC3R = 0; // Load the Compare Register Value
		//OC3CONbits.OCM = 0b110; // Select the Output Compare mode


		// Initialize Output Compare Module
		OC4CONbits.OCM = 0b000; // Disable Output Compare Module
		OC4R = 0; // Write the duty cycle for the first PWM pulse
		OC4RS = 0; // Write the duty cycle for the second PWM pulse
		OC4CONbits.OCTSEL = 0; // Select Timer 2 as output compare time base
		OC4R = 0; // Load the Compare Register Value
		//OC4CONbits.OCM = 0b110; // Select the Output Compare mode



                /*-----------------------------------------------------------------------*/
		// Blink LED (clignote quand le programme fonctionne)
		/*-----------------------------------------------------------------------*/
                #define LED_BLINK LATBbits.LATB3
                TRISBbits.TRISB3 = 0;
                LED_BLINK = 0;

		/*-----------------------------------------------------------------------*/
		// Timer 1	10ms
		/*-----------------------------------------------------------------------*/
		#ifdef CLOCK_FREQ_10
			#define TMR1_VALUE 50000 // on le met de 1ms à 10ms => de 40000 à 50000 avec scale de 8 (version test, à modifier après)
		#else
			#define TMR1_VALUE 49536 // 39628
		#endif

		OpenTimer1(	T1_ON &
					T1_GATE_OFF &
					T1_PS_1_8 & 		// PreScale de 8 au lieu de T1_PS_1_1
					T1_SYNC_EXT_OFF &
					T1_SOURCE_INT,
					TMR1_VALUE);
	#else
		#warning -- Unsupported device ***********************
	#endif
}
