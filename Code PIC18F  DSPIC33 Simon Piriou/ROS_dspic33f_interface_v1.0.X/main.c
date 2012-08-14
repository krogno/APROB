/* 
 * File:   main.c
 * Author: spiriou
 *
 * Created on 7 mars 2012, 02:09
 */

#include "main.h"
#include "OrdreUART.h"
#include "OrdreI2C.h"
/*
 * 
 */
int main(int argc, char** argv)
{
    // Initialise le microcontroleur
    Settings();
    DelayN1ms(30);
    // Initialise l'UART
    InitTextIO();
    // Initialise les interruptions
    ISR_Settings();

    initAsservissement();
    initOdometrie();

    // Configure tous les moteurs à l'arret
    OC1RS = 0;
    OC2RS = 0;
    OC3RS = 0;
    OC4RS = 0;

    // Active les modules pour les PWM
    OC1CONbits.OCM = 0b110;
    OC2CONbits.OCM = 0b110;
    OC3CONbits.OCM = 0b110;
    OC4CONbits.OCM = 0b110;

    uartNextOut = 0;
    uartCommande = 0;

    Cycle1 = 0;
    Cycle2 = 0;
    CYCLE1_FLAG = 0;
    CYCLE2_FLAG = 0;

    // Temporisation de 1 seconde
    RtTimer = 10;
    // Boucle Principale
    while(1)
    {
        if(PID_CALC_FLAG)
        {
            Pid1();
            Pid2();

            compteurVitesse ++;
            if(compteurVitesse == 10)
            {
                Vitesse[L] = VitesseCpteur[L]/400; // 400 = 10 échantillons x 40 pour le 1/40 mm/s => Le résultat est en mm/s
                Vitesse[R] = VitesseCpteur[R]/400;
                compteurVitesse = 0;
                VitesseCpteur[L] = 0;
                VitesseCpteur[R] = 0;
            }
            PID_CALC_FLAG = 0;
        }

        if(CYCLE1_FLAG)
        {
            Odometrie();
            if(ordreEnCours != DEBUG)
            {
                Orientation();
            }
            CYCLE1_FLAG=0;
        }

        if(CYCLE2_FLAG)
        {
            Navigation();
            CYCLE2_FLAG=0;
        }

        // blink LED
        if (RtTimer <= 0)
        {
            RtTimer = 10;
            LED_BLINK = !LED_BLINK;
        }



 
        if( uartCommande )
        {
            GererCommande();
        }
    }
    return (EXIT_SUCCESS);
}

