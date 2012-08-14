#include "USB/usb.h"
#include "USB/usb_function_cdc.h"
#include "HardwareProfile.h"
#//include <usart.h> // Marche pas, probleme avec PIR1bits.RCIF
//#include "i2c.h"

/** CONFIGURATION **************************************************/

        #pragma config PLLDIV   = 5         // (20 MHz crystal on PICDEM FS USB board)
        #pragma config CPUDIV   = OSC1_PLL2   
        #pragma config USBDIV   = 2         // Clock source from 96MHz PLL/2
        #pragma config FOSC     = HSPLL_HS
        #pragma config FCMEN    = OFF
        #pragma config IESO     = OFF
        #pragma config PWRT     = OFF
        #pragma config BOR      = ON
        #pragma config BORV     = 3
        #pragma config VREGEN   = ON      //USB Voltage Regulator
        #pragma config WDT      = OFF
        #pragma config WDTPS    = 32768
        #pragma config MCLRE    = ON
        #pragma config LPT1OSC  = OFF
        #pragma config PBADEN   = OFF
        #pragma config CCP2MX   = ON
        #pragma config STVREN   = ON
        #pragma config LVP      = OFF
//      #pragma config ICPRT    = OFF       // Dedicated In-Circuit Debug/Programming
        #pragma config XINST    = OFF       // Extended Instruction Set
        #pragma config CP0      = OFF
        #pragma config CP1      = OFF
//      #pragma config CP2      = OFF
//      #pragma config CP3      = OFF
        #pragma config CPB      = OFF
//      #pragma config CPD      = OFF
        #pragma config WRT0     = OFF
        #pragma config WRT1     = OFF
//      #pragma config WRT2     = OFF
//      #pragma config WRT3     = OFF
        #pragma config WRTB     = OFF       // Boot Block Write Protection
        #pragma config WRTC     = OFF
//      #pragma config WRTD     = OFF
        #pragma config EBTR0    = OFF
        #pragma config EBTR1    = OFF
//      #pragma config EBTR2    = OFF
//      #pragma config EBTR3    = OFF
        #pragma config EBTRB    = OFF



/** I N C L U D E S **********************************************************/

#include "GenericTypeDefs.h"
#include "Compiler.h"
#include "usb_config.h"
#include "USB/usb_device.h"
#include "USB/usb.h"

#include "HardwareProfile.h"


#define MAX_COMMAND_LENGTH              42
#define BOARD_VERSION                   "v1.01"


// *****************************************************************************
// *****************************************************************************
// Data Structures
// *****************************************************************************
// *****************************************************************************

typedef struct _COMMAND
{
    char        buffer[MAX_COMMAND_LENGTH];
    BYTE        index;
    BYTE        command;
    BYTE        escFirstChar;
    //struct
    //{
        BYTE    reading             : 1;
        BYTE    escNeedFirstChar    : 1;
        BYTE    escNeedSecondChar   : 1;
    //};
} COMMAND;


typedef enum _COMMANDS
{
    COMMAND_NO_COMMAND,
    COMMAND_FW,
    COMMAND_SENSOR, // sensor a b ...
    COMMAND_BLINK, // blink 6:? 7:?
    COMMAND_HELP, // TODO
    COMMAND_UART, // uart => read, uart c => write
    COMMAND_ENCODER,
    COMMAND_I2C, // TODO
    COMMAND_PWM, // pwm 1:? 2:?
    COMMAND_RESET, // TODO
    COMMAND_UNKNOWN
} COMMANDS;


/*const int sevenSeg[16] = {
0b10000000, // 0 dec 0001 0001 bin
0b10011110, // 1 dec 1101 0111 bin
0b01001000, // 2 dec 0011 0010 bin
0b00001100, // 3 dec 1001 0010 bin
0b00010110, // 4 dec 1101 0100 bin
0b00100100, // 5 dec 1001 1000 bin
0b00100000, // 6 dec 0001 1000 bin
0b10001110, // 7 dec 1101 0011 bin
0b00000000, // 8 dec 0001 0000 bin
0b00000100, // 9 dec 1001 0000 bin
0b00000010, // "A" dec 0101 0000 bin
0b00110000, // "b" dec 0001 1100 bin
0b11100000, // "C" dec 0011 1001 bin
0b00011000, // "d" dec 0001 0110 bin
0b01100000, // "E" dec 0011 1000 bin
0b01100010 // "F" dec 0111 1000 bin
};*/



/** V A R I A B L E S ********************************************************/
#pragma udata
//char USB_In_Buffer[64];
char USB_Out_Buffer[CDC_DATA_OUT_EP_SIZE]; // UART

//char USB_Out_Buffer[CDC_DATA_OUT_EP_SIZE];
char RS232_Out_Data[64];//CDC_DATA_IN_EP_SIZE]; // USB

unsigned char  NextUSBOut;
//unsigned char    NextUSBOut;
//char RS232_In_Data;
unsigned char    LastRS232Out;  // Number of characters in the buffer
unsigned char    RS232cp;       // current position within the buffer
unsigned char RS232_Out_Data_Rdy = 0;
USB_HANDLE  lastTransmission;

BOOL stringPrinted;
volatile BOOL buttonPressed;
volatile BYTE buttonCount;


int afficheur7seg;
int getIRvalue;
int secondes;

unsigned int blink[4];





// *****************************************************************************
// *****************************************************************************
// Macros
// *****************************************************************************
// *****************************************************************************

#define IsNum(c)            ((('0' <= c) && (c <= '9')) ? TRUE : FALSE)
#define UpperCase(c)        (('a'<= c) && (c <= 'z') ? c - 0x20 : c)
#define SkipWhiteSpace()    { while (commandInfo.buffer[commandInfo.index] == ' ') commandInfo.index++; }


/** VECTOR REMAPPING ***********************************************/
	
	#define REMAPPED_RESET_VECTOR_ADDRESS           0x1000
	#define REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS	0x1008
	#define REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS	0x1018

	

	extern void _startup (void);        // See c018i.c in your C18 compiler dir
	#pragma code REMAPPED_RESET_VECTOR = REMAPPED_RESET_VECTOR_ADDRESS
	void _reset (void)
	{
	    _asm goto _startup _endasm
	}


	




/** DECLARATIONS ***************************************************/
#pragma code

COMMAND             commandInfo;

/** P R I V A T E  P R O T O T Y P E S ***************************************/
static void InitializeSystem(void);
void USBDeviceTasks(void);
void YourHighPriorityISRCode();
void YourLowPriorityISRCode();
void USBCBSendResume(void);
void BlinkUSBStatus(void);
void UserInit(void);

void InitializeUSART(void);
void putcUSART(char c);
void putsUSART(char* str);
unsigned char getcUSART();

//void afficher7seg(int secondes);

BYTE    GetCommand( void );
void    GetOneWord( char *buffer );
//void    InitializeAnalogMonitor( void );;
void    InitializeCommand( char* str );
void    MonitorUser( char oneChar, char* str );
void    RedoCommandPrompt( void );
void    ReplaceCommandLine( void );


/*void afficher7seg(int secondes)
{
	static int afficheur = 0;

	int buff;
	int b = 128;

	if(afficheur)
	{
		buff = sevenSeg[secondes%10];
	}
	else
	{
		buff = sevenSeg[secondes/10]|1;
	}

	RCLKPin = 0;
	while(b) {
       if(buff & b)
           SERPin = 1;
       else
           SERPin = 0;

       SRCLKPin = 1;
  
		//Delay10TCYx(10);//48);
       SRCLKPin = 0;
        b >>= 1;
   }
	// send shift register data to the storage register
	RCLKPin = 1;

	afficheur ^= 1;
}*/

/********************************************************************
 * Function:        static void InitializeSystem(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        InitializeSystem is a centralize initialization
 *                  routine. All required USB initialization routines
 *                  are called from here.
 *
 *                  User application initialization routine should
 *                  also be called from here.                  
 *
 * Note:            None
 *******************************************************************/
static void InitializeSystem(void)
{
    #if (defined(__18CXX) & !defined(PIC18F87J50_PIM))
        ADCON1 |= 0x0F;                 // Default all pins to digital
    #endif


//	The USB specifications require that USB peripheral devices must never source
//	current onto the Vbus pin.  Additionally, USB peripherals should not source
//	current on D+ or D- when the host/hub is not actively powering the Vbus line.
//	When designing a self powered (as opposed to bus powered) USB peripheral
//	device, the firmware should make sure not to turn on the USB module and D+
//	or D- pull up resistor unless Vbus is actively powered.  Therefore, the
//	firmware needs some means to detect when Vbus is being powered by the host.
//	A 5V tolerant I/O pin can be connected to Vbus (through a resistor), and
// 	can be used to detect when Vbus is high (host actively powering), or low
//	(host is shut down or otherwise not supplying power).  The USB firmware
// 	can then periodically poll this I/O pin to know when it is okay to turn on
//	the USB module/D+/D- pull up resistor.  When designing a purely bus powered
//	peripheral device, it is not possible to source current on D+ or D- when the
//	host is not actively providing power on Vbus. Therefore, implementing this
//	bus sense feature is optional.  This firmware can be made to use this bus
//	sense feature by making sure "USE_USB_BUS_SENSE_IO" has been defined in the
//	HardwareProfile.h file.    
    #if defined(USE_USB_BUS_SENSE_IO)
    tris_usb_bus_sense = INPUT_PIN; // See HardwareProfile.h
    #endif
    
//	If the host PC sends a GetStatus (device) request, the firmware must respond
//	and let the host know if the USB peripheral device is currently bus powered
//	or self powered.  See chapter 9 in the official USB specifications for details
//	regarding this request.  If the peripheral device is capable of being both
//	self and bus powered, it should not return a hard coded value for this request.
//	Instead, firmware should check if it is currently self or bus powered, and
//	respond accordingly.  If the hardware has been configured like demonstrated
//	on the PICDEM FS USB Demo Board, an I/O pin can be polled to determine the
//	currently selected power source.  On the PICDEM FS USB Demo Board, "RA2" 
//	is used for	this purpose.  If using this feature, make sure "USE_SELF_POWER_SENSE_IO"
//	has been defined in HardwareProfile.h, and that an appropriate I/O pin has been mapped
//	to it in HardwareProfile.h.
    #if defined(USE_SELF_POWER_SENSE_IO)
    tris_self_power = INPUT_PIN;	// See HardwareProfile.h
    #endif
    
    UserInit();

    USBDeviceInit();	//usb_device.c.  Initializes USB module SFRs and firmware
    					//variables to known states.

	#if defined(USB_INTERRUPT)
	   if(USB_BUS_SENSE && (USBGetDeviceState() == DETACHED_STATE))
	   {
	       USBDeviceAttach();
	   }
    #endif
}//end InitializeSystem



/******************************************************************************
 * Function:        void UserInit(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This routine should take care of all of the demo code
 *                  initialization that is required.
 *
 * Note:            
 *
 *****************************************************************************/
void UserInit(void)
{
	unsigned char i;
    //Initialize all of the debouncing variables
    buttonCount = 0;
    buttonPressed = FALSE;
    stringPrinted = TRUE;

	//mInit74HC595();
    //Initialize all of the LED pins
    mInitAllLEDs();

    //Initialize the pushbuttons
    //mInitAllSwitches();


	
    InitializeUSART();
    //OpenUSART(USART_TX_INT_OFF  &
    //          USART_RX_INT_OFF  &
    //          USART_ASYNCH_MODE &
    //          USART_EIGHT_BIT   &
    //          USART_CONT_RX     &
    //          USART_BRGH_HIGH,
    //          25 );
                
    

	// Initialize the arrays
	for (i=0; i<sizeof(USB_Out_Buffer); i++)
    {
		USB_Out_Buffer[i] = 0;
    }

	NextUSBOut = 0;
	LastRS232Out = 0;
	lastTransmission = 0;
}//end UserInit


/********************************************************************
 * Function:        void BlinkUSBStatus(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        BlinkUSBStatus turns on and off LEDs 
 *                  corresponding to the USB device state.
 *
 * Note:            mLED macros can be found in HardwareProfile.h
 *                  USBDeviceState is declared and updated in
 *                  usb_device.c.
 *******************************************************************/
void BlinkUSBStatus(void)
{
    static WORD led_count=0;
    
    if(led_count == 0)led_count = 10000U;
    led_count--;

    #define mLED_Both_Off()         {mLED_1_Off();mLED_2_Off();}
    #define mLED_Both_On()          {mLED_1_On();mLED_2_On();}
    #define mLED_Only_1_On()        {mLED_1_On();mLED_2_Off();}
    #define mLED_Only_2_On()        {mLED_1_Off();mLED_2_On();}

    if(USBSuspendControl == 1)
    {
        if(led_count==0)
        {
            mLED_1_Toggle();
            if(mGetLED_1())
            {
                mLED_2_On();
            }
            else
            {
                mLED_2_Off();
            }
        }//end if
    }
    else
    {
        if(USBDeviceState == DETACHED_STATE)
        {
            mLED_Both_Off();
        }
        else if(USBDeviceState == ATTACHED_STATE)
        {
            mLED_Both_On();
        }
        else if(USBDeviceState == POWERED_STATE)
        {
            mLED_Only_1_On();
        }
        else if(USBDeviceState == DEFAULT_STATE)
        {
            mLED_Only_2_On();
        }
        else if(USBDeviceState == ADDRESS_STATE)
        {
            if(led_count == 0)
            {
                mLED_1_Toggle();
                mLED_2_Off();
            }//end if
        }
        else if(USBDeviceState == CONFIGURED_STATE)
        {
            if(led_count==0)
            {
                mLED_1_Toggle();
                if(mGetLED_1())
                {
                    mLED_2_Off();
                }
                else
                {
                    mLED_2_On();
                }
            }//end if
        }//end if(...)
    }//end if(UCONbits.SUSPND...)

}//end BlinkUSBStatus




/******************************************************************************
 * Function:        void InitializeUSART(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This routine initializes the UART to 19200
 *
 * Note:            
 *
 *****************************************************************************/
void InitializeUSART(void)
{
	    unsigned char c;

        //UART_TRISRx=1;				// RX
        //UART_TRISTx=0;				// TX
        TXSTA = 0x24;       	// TX enable BRGH=1
        RCSTA = 0x90;       	// Single Character RX
        SPBRG = 0x71;
        SPBRGH = 0x02;      	// 0x0271 for 48MHz -> 19200 baud
        BAUDCON = 0x08;     	// BRG16 = 1
        c = RCREG;				// read 
}//end InitializeUSART

#define mDataRdyUSART() PIR1bits.RCIF
#define mTxRdyUSART()   TXSTAbits.TRMT


/******************************************************************************
 * Function:        void putcUSART(char c)
 *
 * PreCondition:    None
 *
 * Input:           char c - character to print to the UART
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Print the input character to the UART
 *
 * Note:
 *
 *****************************************************************************/
void putcUSART(char c)
{
    #if defined(__18CXX)
	    TXREG = c;
    #else
        UART2PutChar(c);
    #endif
}


void putsUSART(char* str)
{
    while(*str != 0)
    {
        putcUSART(*str);
        ++str;
        while(!mTxRdyUSART());
    }
}


/******************************************************************************
 * Function:        void putcUSART(char c)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          unsigned char c - character to received on the UART
 *
 * Side Effects:    None
 *
 * Overview:        Print the input character to the UART
 *
 * Note:
 *
 *****************************************************************************/
unsigned char getcUSART ()
{
	char  c;

	if (RCSTAbits.OERR)  // in case of overrun error
	{                    // we should never see an overrun error, but if we do,
		RCSTAbits.CREN = 0;  // reset the port
		c = RCREG;
		RCSTAbits.CREN = 1;  // and keep going.
	}
	else
		c = RCREG;
// not necessary.  EUSART auto clears the flag when RCREG is cleared
//	PIR1bits.RCIF = 0;    // clear Flag

	return c;
}



// ******************************************************************************************************
// ************** USB Callback Functions ****************************************************************
// ******************************************************************************************************
// The USB firmware stack will call the callback functions USBCBxxx() in response to certain USB related
// events.  For example, if the host PC is powering down, it will stop sending out Start of Frame (SOF)
// packets to your device.  In response to this, all USB devices are supposed to decrease their power
// consumption from the USB Vbus to <2.5mA each.  The USB module detects this condition (which according
// to the USB specifications is 3+ms of no bus activity/SOF packets) and then calls the USBCBSuspend()
// function.  You should modify these callback functions to take appropriate actions for each of these
// conditions.  For example, in the USBCBSuspend(), you may wish to add code that will decrease power
// consumption from Vbus to <2.5mA (such as by clock switching, turning off LEDs, putting the
// microcontroller to sleep, etc.).  Then, in the USBCBWakeFromSuspend() function, you may then wish to
// add code that undoes the power saving things done in the USBCBSuspend() function.

// The USBCBSendResume() function is special, in that the USB stack will not automatically call this
// function.  This function is meant to be called from the application firmware instead.  See the
// additional comments near the function.

/******************************************************************************
 * Function:        void USBCBSuspend(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Call back that is invoked when a USB suspend is detected
 *
 * Note:            None
 *****************************************************************************/
void USBCBSuspend(void)
{
	//Example power saving code.  Insert appropriate code here for the desired
	//application behavior.  If the microcontroller will be put to sleep, a
	//process similar to that shown below may be used:
	
	//ConfigureIOPinsForLowPower();
	//SaveStateOfAllInterruptEnableBits();
	//DisableAllInterruptEnableBits();
	//EnableOnlyTheInterruptsWhichWillBeUsedToWakeTheMicro();	//should enable at least USBActivityIF as a wake source
	//Sleep();
	//RestoreStateOfAllPreviouslySavedInterruptEnableBits();	//Preferrably, this should be done in the USBCBWakeFromSuspend() function instead.
	//RestoreIOPinsToNormal();									//Preferrably, this should be done in the USBCBWakeFromSuspend() function instead.

	//IMPORTANT NOTE: Do not clear the USBActivityIF (ACTVIF) bit here.  This bit is 
	//cleared inside the usb_device.c file.  Clearing USBActivityIF here will cause 
	//things to not work as intended.	
	

    #if defined(__C30__)
        USBSleepOnSuspend();
    #endif
}

/******************************************************************************
 * Function:        void USBCBWakeFromSuspend(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The host may put USB peripheral devices in low power
 *					suspend mode (by "sending" 3+ms of idle).  Once in suspend
 *					mode, the host may wake the device back up by sending non-
 *					idle state signalling.
 *					
 *					This call back is invoked when a wakeup from USB suspend 
 *					is detected.
 *
 * Note:            None
 *****************************************************************************/
void USBCBWakeFromSuspend(void)
{
	// If clock switching or other power savings measures were taken when
	// executing the USBCBSuspend() function, now would be a good time to
	// switch back to normal full power run mode conditions.  The host allows
	// a few milliseconds of wakeup time, after which the device must be 
	// fully back to normal, and capable of receiving and processing USB
	// packets.  In order to do this, the USB module must receive proper
	// clocking (IE: 48MHz clock must be available to SIE for full speed USB
	// operation).
}

/********************************************************************
 * Function:        void USBCB_SOF_Handler(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The USB host sends out a SOF packet to full-speed
 *                  devices every 1 ms. This interrupt may be useful
 *                  for isochronous pipes. End designers should
 *                  implement callback routine as necessary.
 *
 * Note:            None
 *******************************************************************/
void USBCB_SOF_Handler(void)
{
    // No need to clear UIRbits.SOFIF to 0 here.
    // Callback caller is already doing that.

    //This is reverse logic since the pushbutton is active low
    //if(buttonPressed == sw2)
    //{
    //    if(buttonCount != 0)
    //    {
    //        buttonCount--;
    //    }
    //    else
     //   {
            //This is reverse logic since the pushbutton is active low
     //       buttonPressed = !sw2;

            //Wait 100ms before the next press can be generated
      //      buttonCount = 100;
     //   }
   // }
   // else
    ////{
       // if(buttonCount != 0)
        //{
         //   buttonCount--;
       // }
    //}
}

/*******************************************************************
 * Function:        void USBCBErrorHandler(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The purpose of this callback is mainly for
 *                  debugging during development. Check UEIR to see
 *                  which error causes the interrupt.
 *
 * Note:            None
 *******************************************************************/
void USBCBErrorHandler(void)
{
    // No need to clear UEIR to 0 here.
    // Callback caller is already doing that.

	// Typically, user firmware does not need to do anything special
	// if a USB error occurs.  For example, if the host sends an OUT
	// packet to your device, but the packet gets corrupted (ex:
	// because of a bad connection, or the user unplugs the
	// USB cable during the transmission) this will typically set
	// one or more USB error interrupt flags.  Nothing specific
	// needs to be done however, since the SIE will automatically
	// send a "NAK" packet to the host.  In response to this, the
	// host will normally retry to send the packet again, and no
	// data loss occurs.  The system will typically recover
	// automatically, without the need for application firmware
	// intervention.
	
	// Nevertheless, this callback function is provided, such as
	// for debugging purposes.
}


/*******************************************************************
 * Function:        void USBCBCheckOtherReq(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        When SETUP packets arrive from the host, some
 * 					firmware must process the request and respond
 *					appropriately to fulfill the request.  Some of
 *					the SETUP packets will be for standard
 *					USB "chapter 9" (as in, fulfilling chapter 9 of
 *					the official USB specifications) requests, while
 *					others may be specific to the USB device class
 *					that is being implemented.  For example, a HID
 *					class device needs to be able to respond to
 *					"GET REPORT" type of requests.  This
 *					is not a standard USB chapter 9 request, and 
 *					therefore not handled by usb_device.c.  Instead
 *					this request should be handled by class specific 
 *					firmware, such as that contained in usb_function_hid.c.
 *
 * Note:            None
 *******************************************************************/
void USBCBCheckOtherReq(void)
{
    USBCheckCDCRequest();
}//end


/*******************************************************************
 * Function:        void USBCBStdSetDscHandler(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The USBCBStdSetDscHandler() callback function is
 *					called when a SETUP, bRequest: SET_DESCRIPTOR request
 *					arrives.  Typically SET_DESCRIPTOR requests are
 *					not used in most applications, and it is
 *					optional to support this type of request.
 *
 * Note:            None
 *******************************************************************/
void USBCBStdSetDscHandler(void)
{
    // Must claim session ownership if supporting this request
}//end


/*******************************************************************
 * Function:        void USBCBInitEP(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is called when the device becomes
 *                  initialized, which occurs after the host sends a
 * 					SET_CONFIGURATION (wValue not = 0) request.  This 
 *					callback function should initialize the endpoints 
 *					for the device's usage according to the current 
 *					configuration.
 *
 * Note:            None
 *******************************************************************/
void USBCBInitEP(void)
{
    CDCInitEP();
}

/********************************************************************
 * Function:        void USBCBSendResume(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The USB specifications allow some types of USB
 * 					peripheral devices to wake up a host PC (such
 *					as if it is in a low power suspend to RAM state).
 *					This can be a very useful feature in some
 *					USB applications, such as an Infrared remote
 *					control	receiver.  If a user presses the "power"
 *					button on a remote control, it is nice that the
 *					IR receiver can detect this signalling, and then
 *					send a USB "command" to the PC to wake up.
 *					
 *					The USBCBSendResume() "callback" function is used
 *					to send this special USB signalling which wakes 
 *					up the PC.  This function may be called by
 *					application firmware to wake up the PC.  This
 *					function will only be able to wake up the host if
 *                  all of the below are true:
 *					
 *					1.  The USB driver used on the host PC supports
 *						the remote wakeup capability.
 *					2.  The USB configuration descriptor indicates
 *						the device is remote wakeup capable in the
 *						bmAttributes field.
 *					3.  The USB host PC is currently sleeping,
 *						and has previously sent your device a SET 
 *						FEATURE setup packet which "armed" the
 *						remote wakeup capability.   
 *
 *                  If the host has not armed the device to perform remote wakeup,
 *                  then this function will return without actually performing a
 *                  remote wakeup sequence.  This is the required behavior, 
 *                  as a USB device that has not been armed to perform remote 
 *                  wakeup must not drive remote wakeup signalling onto the bus;
 *                  doing so will cause USB compliance testing failure.
 *                  
 *					This callback should send a RESUME signal that
 *                  has the period of 1-15ms.
 *
 * Note:            This function does nothing and returns quickly, if the USB
 *                  bus and host are not in a suspended condition, or are 
 *                  otherwise not in a remote wakeup ready state.  Therefore, it
 *                  is safe to optionally call this function regularly, ex: 
 *                  anytime application stimulus occurs, as the function will
 *                  have no effect, until the bus really is in a state ready
 *                  to accept remote wakeup. 
 *
 *                  When this function executes, it may perform clock switching,
 *                  depending upon the application specific code in 
 *                  USBCBWakeFromSuspend().  This is needed, since the USB
 *                  bus will no longer be suspended by the time this function
 *                  returns.  Therefore, the USB module will need to be ready
 *                  to receive traffic from the host.
 *
 *                  The modifiable section in this routine may be changed
 *                  to meet the application needs. Current implementation
 *                  temporary blocks other functions from executing for a
 *                  period of ~3-15 ms depending on the core frequency.
 *
 *                  According to USB 2.0 specification section 7.1.7.7,
 *                  "The remote wakeup device must hold the resume signaling
 *                  for at least 1 ms but for no more than 15 ms."
 *                  The idea here is to use a delay counter loop, using a
 *                  common value that would work over a wide range of core
 *                  frequencies.
 *                  That value selected is 1800. See table below:
 *                  ==========================================================
 *                  Core Freq(MHz)      MIP         RESUME Signal Period (ms)
 *                  ==========================================================
 *                      48              12          1.05
 *                       4              1           12.6
 *                  ==========================================================
 *                  * These timing could be incorrect when using code
 *                    optimization or extended instruction mode,
 *                    or when having other interrupts enabled.
 *                    Make sure to verify using the MPLAB SIM's Stopwatch
 *                    and verify the actual signal on an oscilloscope.
 *******************************************************************/
void USBCBSendResume(void)
{
    static WORD delay_count;
    
    //First verify that the host has armed us to perform remote wakeup.
    //It does this by sending a SET_FEATURE request to enable remote wakeup,
    //usually just before the host goes to standby mode (note: it will only
    //send this SET_FEATURE request if the configuration descriptor declares
    //the device as remote wakeup capable, AND, if the feature is enabled
    //on the host (ex: on Windows based hosts, in the device manager 
    //properties page for the USB device, power management tab, the 
    //"Allow this device to bring the computer out of standby." checkbox 
    //should be checked).
    if(USBGetRemoteWakeupStatus() == TRUE) 
    {
        //Verify that the USB bus is in fact suspended, before we send
        //remote wakeup signalling.
        if(USBIsBusSuspended() == TRUE)
        {
            USBMaskInterrupts();
            
            //Clock switch to settings consistent with normal USB operation.
            USBCBWakeFromSuspend();
            USBSuspendControl = 0; 
            USBBusIsSuspended = FALSE;  //So we don't execute this code again, 
                                        //until a new suspend condition is detected.

            //Section 7.1.7.7 of the USB 2.0 specifications indicates a USB
            //device must continuously see 5ms+ of idle on the bus, before it sends
            //remote wakeup signalling.  One way to be certain that this parameter
            //gets met, is to add a 2ms+ blocking delay here (2ms plus at 
            //least 3ms from bus idle to USBIsBusSuspended() == TRUE, yeilds
            //5ms+ total delay since start of idle).
            delay_count = 3600U;        
            do
            {
                delay_count--;
            }while(delay_count);
            
            //Now drive the resume K-state signalling onto the USB bus.
            USBResumeControl = 1;       // Start RESUME signaling
            delay_count = 1800U;        // Set RESUME line for 1-13 ms
            do
            {
                delay_count--;
            }while(delay_count);
            USBResumeControl = 0;       //Finished driving resume signalling

            USBUnmaskInterrupts();
        }
    }
}


/*******************************************************************
 * Function:        void USBCBEP0DataReceived(void)
 *
 * PreCondition:    ENABLE_EP0_DATA_RECEIVED_CALLBACK must be
 *                  defined already (in usb_config.h)
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is called whenever a EP0 data
 *                  packet is received.  This gives the user (and
 *                  thus the various class examples a way to get
 *                  data that is received via the control endpoint.
 *                  This function needs to be used in conjunction
 *                  with the USBCBCheckOtherReq() function since 
 *                  the USBCBCheckOtherReq() function is the apps
 *                  method for getting the initial control transfer
 *                  before the data arrives.
 *
 * Note:            None
 *******************************************************************/
#if defined(ENABLE_EP0_DATA_RECEIVED_CALLBACK)
void USBCBEP0DataReceived(void)
{
}
#endif

/*******************************************************************
 * Function:        BOOL USER_USB_CALLBACK_EVENT_HANDLER(
 *                        USB_EVENT event, void *pdata, WORD size)
 *
 * PreCondition:    None
 *
 * Input:           USB_EVENT event - the type of event
 *                  void *pdata - pointer to the event data
 *                  WORD size - size of the event data
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is called from the USB stack to
 *                  notify a user application that a USB event
 *                  occured.  This callback is in interrupt context
 *                  when the USB_INTERRUPT option is selected.
 *
 * Note:            None
 *******************************************************************/
BOOL USER_USB_CALLBACK_EVENT_HANDLER(USB_EVENT event, void *pdata, WORD size)
{
    switch(event)
    {
        case EVENT_TRANSFER:
            //Add application specific callback task or callback function here if desired.
            break;
        case EVENT_SOF:
            USBCB_SOF_Handler();
            break;
        case EVENT_SUSPEND:
            USBCBSuspend();
            break;
        case EVENT_RESUME:
            USBCBWakeFromSuspend();
            break;
        case EVENT_CONFIGURED: 
            USBCBInitEP();
            break;
        case EVENT_SET_DESCRIPTOR:
            USBCBStdSetDscHandler();
            break;
        case EVENT_EP0_REQUEST:
            USBCBCheckOtherReq();
            break;
        case EVENT_BUS_ERROR:
            USBCBErrorHandler();
            break;
        case EVENT_TRANSFER_TERMINATED:
            //Add application specific callback task or callback function here if desired.
            //The EVENT_TRANSFER_TERMINATED event occurs when the host performs a CLEAR
            //FEATURE (endpoint halt) request on an application endpoint which was 
            //previously armed (UOWN was = 1).  Here would be a good place to:
            //1.  Determine which endpoint the transaction that just got terminated was 
            //      on, by checking the handle value in the *pdata.
            //2.  Re-arm the endpoint if desired (typically would be the case for OUT 
            //      endpoints).
            break;
        default:
            break;
    }      
    return TRUE; 
}




/****************************************************************************
  Function:
    void InitializeCommand( void )

  Description:
    This function prints a command prompt and initializes the command line
    information.  If available, the command prompt format is:
                    [Volume label]:[Current directory]>

  Precondition:
    None

  Parameters:
    None

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/

void InitializeCommand( char* str )
{
    sprintf(str, "%s> ", str);

    commandInfo.command     = COMMAND_NO_COMMAND;
    commandInfo.index       = 0;
    commandInfo.reading     = TRUE;

    memset( commandInfo.buffer, 0x00, MAX_COMMAND_LENGTH );
}







/****************************************************************************
  Function:
    BYTE GetCommand( void )

  Description:
    This function returns whether or not the user has finished entering a
    command.  If so, then the command entered by the user is determined and
    placed in commandInfo.command.  The command line index
    (commandInfo.index) is set to the first non-space character after the
    command.

  Precondition:
    commandInfo.reading must be valid.

  Parameters:
    None

  Return Values:
    TRUE    - The user has entered a command.  The command is in
                  commandInfo.command.
    FALSE   - The user has not finished entering a command.

  Remarks:
    None
  ***************************************************************************/

BYTE GetCommand( void )
{
    char firstWord[MAX_COMMAND_LENGTH];
    char command[8];

    if (commandInfo.reading)
    {
        return FALSE;
    }
    else
    {
        commandInfo.index = 0;
        GetOneWord( firstWord );
        SkipWhiteSpace();

        if (firstWord[0] == 0)
        {
            commandInfo.command = COMMAND_NO_COMMAND;
            return TRUE;
        }
        sprintf(command, "enc");
        if (!strncmp( firstWord, command, 3 ) && (strlen(firstWord) == 3))
        {
            commandInfo.command = COMMAND_ENCODER;
            return TRUE;
        }
        sprintf(command, "fw");
        if (!strncmp( firstWord, command, 2 ) && (strlen(firstWord) == 2))
        {
            commandInfo.command = COMMAND_FW;
            return TRUE;
        }
        sprintf(command, "sensor");
        if (!strncmp( firstWord, command, 6 ) && (strlen(firstWord) == 6))
        {
            commandInfo.command = COMMAND_SENSOR;
            return TRUE;
        }
        sprintf(command, "blink");
        if (!strncmp( firstWord, command, 5 ) && (strlen(firstWord) == 5))
        {
            commandInfo.command = COMMAND_BLINK;
            return TRUE;
        }
        sprintf(command, "uart");
        if (!strncmp( firstWord, command, 4 ) && (strlen(firstWord) == 4))
        {
            commandInfo.command = COMMAND_UART;
            return TRUE;
        }
        sprintf(command, "pwm");
        if (!strncmp( firstWord, command, 3 ) && (strlen(firstWord) == 3))
        {
            commandInfo.command = COMMAND_PWM;
            return TRUE;
        }
        sprintf(command, "reset");
        if (!strncmp( firstWord, "reset", 5 ) && (strlen(firstWord) == 5))
        {
            commandInfo.command = COMMAND_RESET;
            return TRUE;
        }
        

        commandInfo.command = COMMAND_UNKNOWN;
        return TRUE;
    }
}




/****************************************************************************
  Function:
    void GetOneWord( char *buffer )

  Description:
    This function copies the next word in the command line to the specified
    buffer.  Word deliniation is marked by a space character.  The returned
    word is null terminated.

  Precondition:
    commandInfo.buffer and commandInfo.index are valid

  Parameters:
    *buffer - Pointer to where the word is to be stored.

  Returns:
    None

  Remarks:

  ***************************************************************************/
void GetOneWord( char *buffer )
{
    SkipWhiteSpace();

    while ((commandInfo.buffer[commandInfo.index] != 0) &&
           (commandInfo.buffer[commandInfo.index] != ' '))
    {
        *buffer++ = commandInfo.buffer[commandInfo.index++];
    }
    *buffer = 0;
}





/****************************************************************************
  Function:
    void RedoCommandPrompt( void )

  Description:
    This function is called when the user either removes or inserts media.
    We want to let the user know right away that something has changed, so
    we change the command prompt immediately.  If the user is entering a
    command, we rebuild his command.

  Precondition:
    None

  Parameters:
    None

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/

void RedoCommandPrompt( void )
{
    int i;

    putsUSBUSART( "\r\n> " );

    if (commandInfo.reading)
    {
        for (i = 0; i < commandInfo.index; i++)
        {
            putcUSART( commandInfo.buffer[i] );
        }
    }
}



/****************************************************************************
  Function:
    void MonitorUser( void )

  Description:
    This routine monitors command line input from the user.  The
    UART is polled rather than using interrupts.  If the previous
    command line is still being processed, the characters are
    ignored.  All text input is converted to upper case for
    simplification.  When the user presses the Enter or Return key,
    the command line is considered terminated.  Escape sequences are
    captured and checked.  If the up-arrow is pressed, the previous
    buffered command is displayed, until the oldest command is
    reached.  If the down-arrow is pressed, the next buffered
    command is displayed, until the newest command is displayed.
    If the down-arrow is pressed one additional time, the command
    line is erased.

  Precondition:
    None

  Parameters:
    None

  Returns:
    None

  Remarks:
    Currently, blank entries are added to the old command buffer.
  ***************************************************************************/

void MonitorUser( char oneChar, char* str )
{
    // Si une commande est traitée, ignore le caractere
    if (commandInfo.reading)
    {
        if(oneChar == 0x7F) // DEL
        {
            if(commandInfo.index != 0)
            {
                commandInfo.index--;
                sprintf(str, "%c%c%c", 0x08, ' ', 0x08);
            }
        }
        else if ((oneChar == 0x0D) || (oneChar == 0x0A)) // 0x0D = Carriage Return, OxOA = Line Feed
        {
            //sprintf(str, "\r\r\n\r");

            commandInfo.buffer[commandInfo.index]   = 0; // Null terminate the input command
            commandInfo.reading                     = FALSE;
            commandInfo.escNeedFirstChar            = FALSE;
            commandInfo.escNeedSecondChar           = FALSE;


        }
        else if ((0x20 <= oneChar) && (oneChar <= 0x7E)) // Caractere normal
        {
            //oneChar = UpperCase( oneChar ); // To make later processing simpler, MAJUSCULE
            if (commandInfo.index < MAX_COMMAND_LENGTH-1)
            {   
                commandInfo.buffer[commandInfo.index] = oneChar;
                sprintf(str, "%c", oneChar);
                commandInfo.index++;
            }
            else
            {
               sprintf(str, "\0");
            }
        }
    }
}


        /*if (commandInfo.escNeedSecondChar)
        {
            if (commandInfo.escFirstChar == 0x5B)
            {
                //if (oneChar == 0x41)        // Up arrow
                //{
                    /*if (commandBuffer.showing != commandBuffer.oldest)
                    {
                        if (commandBuffer.showing == MAX_BUFFERED_COMMANDS)
                        {
                            commandBuffer.showing = commandBuffer.newest;
                        }
                        else
                        {
                            commandBuffer.showing = (commandBuffer.showing - 1) & (MAX_BUFFERED_COMMANDS-1);
                        }
                    }
                    ReplaceCommandLine();*/
                //}
                //else if (oneChar == 0x42)   // Down arrow
                //{
                    /*if (commandBuffer.showing != MAX_BUFFERED_COMMANDS)
                    {
                        if (commandBuffer.showing != commandBuffer.newest)
                        {
                            commandBuffer.showing = (commandBuffer.showing + 1) & (MAX_BUFFERED_COMMANDS-1);
                            ReplaceCommandLine();
                        }
                        else
                        {
                            EraseCommandLine();
                            commandBuffer.showing = MAX_BUFFERED_COMMANDS;
                        }
                    }
                    else
                    {*/
                      //  EraseCommandLine();
                    //}*/
                //}
      /*      }
            commandInfo.escNeedSecondChar   = FALSE;
        }
        else if (commandInfo.escNeedFirstChar)
        {
            commandInfo.escFirstChar        = oneChar;
            commandInfo.escNeedFirstChar    = FALSE;
            commandInfo.escNeedSecondChar   = TRUE;
        }
        else
        {
            if (oneChar == 0x1B)    // ESC - an escape sequence
            {
                commandInfo.escNeedFirstChar = TRUE;
            }
            else if (oneChar == 0x08)    // Backspace
            {
                if (commandInfo.index > 0)
                {
                    commandInfo.index --;
                    str[0] = 0x08;
                    str[1] = ' ';
                    str[2] = 0x08;
                    
                    putUSBUSART( str, 3 );
                }
            }
            else if ((oneChar == 0x0D) || (oneChar == 0x0A)) // 0x0D = Carriage Return, OxOA = Line Feed
            {
                while(!mUSBUSARTIsTxTrfReady())
                {
                    CDCTxService();
                }
                sprintf(str, "\r\r\n\r");
                putsUSBUSART(str);

                commandInfo.buffer[commandInfo.index]   = 0; // Null terminate the input command
                commandInfo.reading                     = FALSE;
                commandInfo.escNeedFirstChar            = FALSE;
                commandInfo.escNeedSecondChar           = FALSE;

                // Copy the new command into the command buffer
                commandBuffer.showing = MAX_BUFFERED_COMMANDS;
                if (commandBuffer.oldest == MAX_BUFFERED_COMMANDS)
                {
                    commandBuffer.oldest = 0;
                    commandBuffer.newest = 0;
                }
                else
                {
                    commandBuffer.newest = (commandBuffer.newest + 1) & (MAX_BUFFERED_COMMANDS-1);
                    if (commandBuffer.newest == commandBuffer.oldest)
                    {
                        commandBuffer.oldest = (commandBuffer.oldest + 1) & (MAX_BUFFERED_COMMANDS-1);
                    }
                }
                strcpy( &(commandBuffer.lines[commandBuffer.newest][0]), commandInfo.buffer );
            }
            else if ((0x20 <= oneChar) && (oneChar <= 0x7E)) // Caractere normal
            {
                //oneChar = UpperCase( oneChar ); // To make later processing simpler, MAJUSCULE
                if (commandInfo.index < MAX_COMMAND_LENGTH)
                {
                    commandInfo.buffer[commandInfo.index++] = oneChar;
                    putcUSBUSART( oneChar );    // Echo the character
                }
            }
        }
    }
}*/




