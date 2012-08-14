#include "main.h"
#include <timers.h>


void YourHighPriorityISRCode();


#pragma code REMAPPED_HIGH_INTERRUPT_VECTOR = REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS
	void Remapped_High_ISR (void)
	{
	     _asm goto YourHighPriorityISRCode _endasm
	}

	#pragma code REMAPPED_LOW_INTERRUPT_VECTOR = REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS
	void Remapped_Low_ISR (void)
	{
	     _asm goto YourLowPriorityISRCode _endasm
	}

	
	#pragma code HIGH_INTERRUPT_VECTOR = 0x08
	void High_ISR (void)
	{
	     _asm goto REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS _endasm
	}
	#pragma code LOW_INTERRUPT_VECTOR = 0x18
	void Low_ISR (void)
	{
	     _asm goto REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS _endasm
	}

	#pragma code
	
	
	//These are your actual interrupt handling routines.
	#pragma interrupt YourHighPriorityISRCode
	void YourHighPriorityISRCode()
	{
		//Check which interrupt flag caused the interrupt.
		//Service the interrupt
		//Clear the interrupt flag
		//Etc.
		static int cnt1 = 0;
                static unsigned int ledBlink[4];
		static int cnt2 = 0;
                static int servo = 0;
		
		if(PIR1bits.TMR1IF)
		{
			WriteTimer1(0xC567);// 100 Hz   //A81B); // set start value of timer, set interrupt at every 15 ms.
			PIR1bits.TMR1IF = 0;
			
			//cnt1++;
                        ledBlink[0]++;
                        ledBlink[1]++;
                        ledBlink[2]++;
                        ledBlink[3]++;
                        
                        if( (ledBlink[0] >= blink[0]) && (blink[0] != 0) )
                        {
                            mLED_1_Toggle();
                            ledBlink[0] = 0;
                        }
                        if( (ledBlink[1] >= blink[1]) && (blink[1] != 0) )
                        {
                            mLED_2_Toggle();
                            ledBlink[1] = 0;
                        }
                         if( (ledBlink[2] >= blink[2]) && (blink[2] != 0) )
                        {
                            mLED_3_Toggle();
                            ledBlink[2] = 0;
                        }
                        if( (ledBlink[3] >= blink[3]) && (blink[3] != 0) )
                        {
                            mLED_4_Toggle();
                            ledBlink[3] = 0;
                        }

			//if(cnt1 >= 100)//100)//20)//65) // c'est 66,666667
			//{
			//	cnt1 = 0;
                          //      CCPR1L++;
                           //     if(CCPR1L >45)
                             //       CCPR1L = 14;
			//}
		}
		else
		if(PIR1bits.TMR2IF)
		{
			PIR1bits.TMR2IF = 0; // 20kHz
			cnt2++;
                        if(cnt2 >= servo)
                        {
                            //mLED_3_Off();
                        }
			if(cnt2 >= 500) //400: 50 fois par seconde
			{
                            //mLED_3_Toggle();
                            //mLED_3_On();
                            //servo = CCPR1L;
                            cnt2 = 0;
			}
		}
		else
		{
        #if defined(USB_INTERRUPT)
	        USBDeviceTasks();
        #endif
		}
	}

	#pragma interruptlow YourLowPriorityISRCode
	void YourLowPriorityISRCode()
	{
		//Check which interrupt flag caused the interrupt.
		//Service the interrupt
		//Clear the interrupt flag
		//Etc.	
	}



#pragma code

void main(void)
{  
    char str[64];
    char param[16];
    char _Version[16] = BOARD_VERSION;
    int connected = 1;
    int i, ii, iii;

    //BYTE numBytesRead;
    InitializeSystem();
    secondes = 0;
    blink[0] = 0;
    blink[1] = 0;
    blink[2] = 0;
    blink[3] = 0;

    OpenTimer1(TIMER_INT_ON & T1_8BIT_RW & T1_SOURCE_INT & T1_PS_1_8 & T1_OSC1EN_OFF & T1_SYNC_EXT_OFF);
    //T1CON = 0b00110001;
    RCONbits.IPEN = 1; //Enable priority levels on interrupts
    RCONbits.SBOREN = 0; //Disable BOR

    TRISCbits.TRISC2 = 0; // PWM configuration
    CCP1CON = 0b00001100;
    CCPR1L = 0x00;
    CCP1CONbits.DC1B = 0x00;

    TRISCbits.TRISC1 = 0; // Sur RC1 (cf CONFIG3H)
    CCP2CON = 0b00001100;
    CCPR2L = 0x00;
    CCP2CONbits.DC2B = 0x00;


    //T2CON = 0b00000111;
    T2CON = 0b00000101;
    PR2 = 0x97;

    PIR1bits.TMR1IF = 0; // Flag d'interruption
    PIR1bits.TMR2IF = 0;
    INTCON = 0b10000000;
    IPR1bits.TMR1IP = 1; // interrupt priority (1 = high, O = low)
    PIE1bits.TMR1IE = 1;
    IPR1bits.TMR2IP = 1;
    PIE1bits.TMR2IE = 1; // TMR2 Match Interrupt Enable bit


    /* Init ADC */
    TRISAbits.RA0 = 1;
    ADCON0=0b00000001;   // ADC port channel 0 (AN0), Enable ADC
    ADCON1=0b00000110;   // Use Internal Voltage Reference (Vdd and Vss), AN0 - 9 analog input
    ADCON2=0b10101011;   // Right justify result, 12 TAD, Select the FRC for 16 MHz

    ADCON0bits.GO=1;
    while(ADCON0bits.GO);

    getIRvalue = 0;


    
    // Message Init
    while(connected)
    {
        //Blink the LEDs according to the USB device status
        BlinkUSBStatus();
        // User Application USB tasks
        if(!((USBDeviceState < CONFIGURED_STATE)||(USBSuspendControl==1)))
        {
            if (RS232_Out_Data_Rdy == 0)  // only check for new USB buffer if the old RS232 buffer is
            {						  // empty.  This will cause additional USB packets to be NAK'd
                    LastRS232Out = getsUSBUSART(RS232_Out_Data,32); //until the buffer is free.
                    if(LastRS232Out > 0)
                    {
                            RS232_Out_Data_Rdy = 1;  // signal buffer full
                            RS232cp = 0;  // Reset the current position
                            //mLED_4_Toggle();
                    }
            }

            //Check if one or more bytes are waiting in the USB transmit
            if(RS232_Out_Data_Rdy && mUSBUSARTIsTxTrfReady())
            {
                if(RS232_Out_Data[RS232cp] == 'a')
                {
                    connected = 0;
                    //mLED_3_Toggle();

                    //sprintf(str, "\r\n***** pic18F4550 %s By SP *****\r\n**********************************\r\n\r\n> ", _Version);
                    sprintf(str, "> ");
                    putsUSBUSART(str);
                }
                
                ++RS232cp;
                if (RS232cp == LastRS232Out)
                        RS232_Out_Data_Rdy = 0;
            }

            CDCTxService();
        }
    }


    commandInfo.command     = COMMAND_NO_COMMAND;
    commandInfo.index       = 0;
    commandInfo.reading     = TRUE;

    memset( commandInfo.buffer, 0x00, MAX_COMMAND_LENGTH );



    //T2CON = 0b00000111;
    //PR2 = 0xFF;


    NextUSBOut = 0;

    // initialise la carte moteur
    //str[0] = 'e';
    //str[1] = '\n';
    //str[2] = 0;
    //putsUSART(str);
   

    while(1)
    {
        //Blink the LEDs according to the USB device status
        BlinkUSBStatus();

        if(!((USBDeviceState < CONFIGURED_STATE)||(USBSuspendControl==1)))
        {
            if (RS232_Out_Data_Rdy == 0)  // only check for new USB buffer if the old RS232 buffer is
            {						  // empty.  This will cause additional USB packets to be NAK'd
                    LastRS232Out = getsUSBUSART(RS232_Out_Data,64); //until the buffer is free.
                    if(LastRS232Out > 0)
                    {
                            RS232_Out_Data_Rdy = 1;  // signal buffer full
                            RS232cp = 0;  // Reset the current position
                            //mLED_3_Toggle();
                    }
            }


            if(mDataRdyUSART())
            {
                if( NextUSBOut < (CDC_DATA_OUT_EP_SIZE - 1))
                {
                    USB_Out_Buffer[NextUSBOut] = getcUSART();
                    //sprintf(str, "%c", USB_Out_Buffer[NextUSBOut] );
                    NextUSBOut++;
                }
                //else
                //{
                    //sprintf(str, "!%c", getcUSART() );
                //}
                //putsUSBUSART(str);
            }

            if(RS232_Out_Data_Rdy && mUSBUSARTIsTxTrfReady())
            {
                MonitorUser(RS232_Out_Data[RS232cp], str);

                if(GetCommand())
                {
                    //SkipWhiteSpace();
                    switch ( commandInfo.command )
                    {
                        case COMMAND_NO_COMMAND:
                            sprintf(str, "\r\nNo command\r\n" );
                            //sprintf(str, "%s\r\n", str );
                            break;

                        case COMMAND_ENCODER:
                            str[0] = 'e';
                            str[1] = 'n';
                            str[2] = 'c';
                            str[3] = '\n';
                            str[4] = 0;
                            putsUSART(str);

                            while(!mDataRdyUSART());
                            USB_Out_Buffer[0] = getcUSART();
                            while(!mDataRdyUSART());
                            USB_Out_Buffer[1] = getcUSART();
                            while(!mDataRdyUSART());
                            USB_Out_Buffer[2] = getcUSART();
                            while(!mDataRdyUSART());
                            USB_Out_Buffer[3] = getcUSART();

                            sprintf(str, "\r\n%d %d\r\n", *(int*)&USB_Out_Buffer[0], *(int*)&USB_Out_Buffer[2]);
                            NextUSBOut = 0;
                            break;


                        case COMMAND_FW:
                            sprintf(str, "\r\n%s\r\n", _Version );
                            break;

                        case COMMAND_SENSOR:
                            //sprintf(str, "%sSensor command ", str);
                            GetOneWord( param );
                            sprintf(str, "\r\n");
                            i=0;
                            while(0x30 <= param[0] && 0x39>=param[0])
                            {
                                i++;
                                ADCON0bits.CHS = (char)(param[0] - 0x30); // Selection entrée
                                ADCON0bits.GO = 1;
                                while(ADCON0bits.GO);   // Convertion finie
                                
                                secondes= (int)ADRESL;
                                secondes += (int)(ADRESH)<<8;

                                sprintf(str, "%s%d ", str, secondes );
                                GetOneWord( param );
                            }
                            if(i>0)
                                sprintf(str, "%s\r\n",str);
                            break;

                        case COMMAND_BLINK:
                            GetOneWord( param );
                            while(0x30 <= param[0] && 0x33>=param[0])
                            {
                                i = 2;
                                ii = param[0]-0x30;
                                while(param[i] != 0)
                                {
                                    param[i-2] = param[i];
                                    i++;
                                }
                                param[i-2] = 0;
                                i = atoi(param);
                                if(i == 0 || i > 100)
                                {
                                    blink[ii] = 0;
                                    LATD = LATD &((1<<ii)^0xFF); // Led Off
                                }
                                else
                                {
                                    blink[ii] = 100/i;
                                    LATD = LATD |((1<<ii)); // Led On
                                }
                                GetOneWord( param );
                            }
                            sprintf(str, "\r\nACK\r\n");
                            break;

                        case COMMAND_PWM:
                            GetOneWord( param );
                            while(0x31 <= param[0] && 0x32>=param[0])
                            {
                                i = 2;
                                ii = param[0]-0x30;
                                while(param[i] != 0)
                                {
                                    param[i-2] = param[i];
                                    i++;
                                }
                                param[i-2] = 0;
                                i = atoi(param);
                                
                                if(ii == 1)
                                {
                                    CCP1CONbits.DC1B = i;
                                    CCPR1L = i>>2;
                                }
                                else
                                {
                                    CCP2CONbits.DC2B = i;
                                    CCPR2L = i>>2;
                                }

                                GetOneWord( param );
                            }
                            sprintf(str, "\r\nACK\r\n");
                            break;
                        case COMMAND_UART:

                            GetOneWord( param );
                            if(param[0]==0 ) // Read
                            {
                                if(NextUSBOut > 0)
                                {
                                    USB_Out_Buffer[NextUSBOut] = 0;
                                    sprintf(str, "\r\n%s\r\n", &USB_Out_Buffer[0]);
                                    NextUSBOut = 0;
                                }
                                else
                                    sprintf(str, "\r\nNACK\r\n");
                            }
                            else
                            {
                                sprintf(str, "%s", param);
                                GetOneWord( param );
                                while(param[0]!=0)
                                {
                                    sprintf(str, "%s %s", str, param);
                                    GetOneWord( param );
                                }
                                sprintf(str, "%s\n", str);
                                putsUSART(str);

                                for(i=0; ; i++)
                                {
                                    while(!mDataRdyUSART());

                                    USB_Out_Buffer[i] = getcUSART();
                                    if(USB_Out_Buffer[i] == '>')
                                    {
                                        USB_Out_Buffer[i] = 0;
                                        break;
                                    }
                                }

                                sprintf(str, "\r\n%s\r\n", USB_Out_Buffer);
                            }
                            break;
                            

                       default:
                            commandInfo.index = 0;
                            GetOneWord( param );
                            sprintf(str, "\r\nNACK <%s>\r\n", param );
                            //sprintf(str, "%sNACK\r\n", str );
                            break;
                    }
                    InitializeCommand(str);
                }

                putsUSBUSART(str);

                RS232cp++;
                if (RS232cp == LastRS232Out)
                        RS232_Out_Data_Rdy = 0;


            }



            CDCTxService();
        }
    }//end while
}//end main
