
char str[64];
int UART_Index;

#define SkipWhiteSpace()    { while (uartBuffer[UART_Index] == ' ') UART_Index++; }



void GetOneWord( char *buffer )
{
    SkipWhiteSpace();

    while ((uartBuffer[UART_Index] != 0) &&
           (uartBuffer[UART_Index] != ' '))
    {
        *buffer++ = uartBuffer[UART_Index++];
    }
    *buffer = 0;
}



void GererCommande()
{
    char commande[16];
    UART_Index = 0;

    GetOneWord(commande);

    if (!strncmp( commande, "enc", 3 ))
    {
        sprintf(str, "%d %d>", (int)POS2CNT, (int)POS1CNT ); // POS1CNT = droite, POS2CNT = gauche
        UART2PrintString(str);
    }
    else if (!strncmp( commande, "pos", 3 ))
    {
        sprintf(str, "%f %f %f>", (float)PosXmes, (float)PosYmes, (float)ThetaMes*RAD2DEG );
        UART2PrintString(str);
    }
    else if (!strncmp( commande, "vit", 3 ))
    {
        sprintf(str, "%ld %ld>", Vitesse[L], Vitesse[R] );
        UART2PrintString(str);
    }
    else if (!strncmp( commande, "cmd", 3 ))
    {
        sprintf(str, "%d>", ordreEnCours );
        UART2PrintString(str);
    }
    else if (!strncmp( commande, "bug", 3 ))
    {
        sprintf(str, "TM:%f TD:%f %f\r\nO:%d XM:%f YM:%f\r\nXD:%f YD:%f>", ThetaMes*RAD2DEG, ThetaDes*RAD2DEG, ThetaDesRef*RAD2DEG, ordreEnCours, (float)PosXmes, (float)PosYmes, (float)PosXdes, (float)PosYdes);
        UART2PrintString(str);
    }
    else if (!strncmp( commande, "tan", 3 ))
    {
        arreterRobot();
        GetOneWord(commande);
        PosXdes=atof(commande);
        GetOneWord(commande);
        PosYdes=atof(commande);
        
        sprintf(str, "XD:%f YD:%f TD:%f %f>", (float)PosXdes, (float)PosYdes, atan2f(PosYdes,PosXdes)*RAD2DEG, atan2f(PosXdes,PosYdes)*RAD2DEG);
        UART2PrintString(str);
    }
    else if (!strncmp( commande, "off", 3 ))
    {
        arreterRobot();
        GetOneWord(commande);
        offsetMoteur=atof(commande);

        sprintf(str, "offset:%d>", offsetMoteur);
        UART2PrintString(str);
    }


    

    else if (!strncmp( commande, "goto", 4 )) // avance à une position à une certaine vitesse
    {
        GetOneWord(commande);
        VitesseMoyenne = atoi(commande); // en mm

        if(VitesseMoyenne == 0)
        {
            arreterRobot();
            sprintf(str, "NACK>");
            UART2PrintString(str);
        }
        else
        {
            GetOneWord(commande);
            PosXdes=atof(commande);
            GetOneWord(commande);
            PosYdes=atof(commande);

            DIST_ENABLE_FLAG = 1;

            ordreEnCours = AVANCER;
            sprintf(str, "V:%d XD:%f YD:%f>", VitesseMoyenne, PosXdes, PosYdes);
            UART2PrintString(str);
        }
    }
    
    else if (!strncmp( commande, "frwd", 4 )) // avance tout droit d'une certaine distance et vitesse
    {
        GetOneWord(commande);
        VitesseMoyenne = atoi(commande); // en mm
        if(VitesseMoyenne == 0)
        {
            arreterRobot();
            sprintf(str, "NACK>");
            UART2PrintString(str);
        }
        else
        {
            GetOneWord(commande);
            PosXdes=PosXmes+(atof(commande)*cos(ThetaDes));
            PosYdes=PosYmes+(atof(commande)*sin(ThetaDes));

            DIST_ENABLE_FLAG = 1;

            ordreEnCours = AVANCER;
            sprintf(str, "O:%d, V:%d XD:%f YD:%f>", ordreEnCours, VitesseMoyenne, PosXdes, PosYdes);
            UART2PrintString(str);
        }
    }


    else if (!strncmp( commande, "move", 4 )) // avance tout droit à une certaine vitesse
    {
        GetOneWord(commande);
        VitesseMoyenne = atoi(commande); // en mm/s
        if(VitesseMoyenne == 0)
        {
            arreterRobot();
            sprintf(str, "NACK>");
            UART2PrintString(str);
        }
        else
        {
            DIST_ENABLE_FLAG = 0;
            ordreEnCours = AVANCER;
            sprintf(str, "V:%d>", VitesseMoyenne);
            UART2PrintString(str);
        }
    }

    else if (!strncmp( commande, "speed", 5 )) // pilote la vitesse des roues
    {
        arreterRobot();
        GetOneWord(commande);
        VelFin[L] = atoi(commande)*40;
        GetOneWord(commande);
        VelFin[R] = atoi(commande)*40;

        DIST_ENABLE_FLAG = 0;
        ordreEnCours = DEBUG;
        sprintf(str, "VL:%d VR:%d>", VelFin[L], VelFin[R]);
        UART2PrintString(str);
    }





    else if (!strncmp( commande, "turn", 4 ))
    {
        arreterRobot();
        GetOneWord(commande);
        VitesseMoyenne = atoi(commande); // en mm/s
        if(VitesseMoyenne == 0)
        {
            sprintf(str, "NACK>");
            UART2PrintString(str);
        }
        else
        {
            GetOneWord(commande);
            ThetaDes = atof(commande)*DEG2RAD; // en mm/s
            ThetaDesRef = ThetaDes;
            PIDInit(&AnglePIDstruct);
            VitesseMoyenne = 0;
            ordreEnCours = TOURNER;
            ANGLE_OK_FLAG = 0;
            DIST_ENABLE_FLAG = 0;

            sprintf(str, "%f %f>", ThetaMes, ThetaDes);
            UART2PrintString(str);
        }
    }
	
    
    
    else if (!strncmp( commande, "spidg", 5 ))
    {
        arreterRobot();

        GetOneWord(commande); // parametres en float sur 4 octets
        KP2 = Q15(atof(commande));
        GetOneWord(commande); // parametres en float sur 4 octets
        KI2 = Q15(atof(commande));
        GetOneWord(commande); // parametres en float sur 4 octets
        KD2 = Q15(atof(commande));
        InitPid2();
        sprintf(str, "ACK>");
        UART2PrintString(str);
    }
    else if (!strncmp( commande, "gpidg", 5 ))
    {
        sprintf(str, "%d %d %d>", KP2, KI2, KD2 );
        UART2PrintString(str);
    }
    else if (!strncmp( commande, "spidd", 5 ))
    {
        GetOneWord(commande); // parametres en float sur 4 octets
        KP1 = Q15(atof(commande));
        GetOneWord(commande); // parametres en float sur 4 octets
        KI1 = Q15(atof(commande));
        GetOneWord(commande); // parametres en float sur 4 octets
        KD1 = Q15(atof(commande));
        InitPid1();
        sprintf(str, "ACK>");
        UART2PrintString(str);
    }
    else if (!strncmp( commande, "gpidd", 5 ))
    {
        sprintf(str, "%d %d %d>", KP1, KI1, KD1 );
        UART2PrintString(str);
    }
    else if (!strncmp( commande, "spida", 5 ))
    {
        GetOneWord(commande);
        ANGLE_KP = Q15(atof(commande));
        GetOneWord(commande);
        ANGLE_KI = Q15(atof(commande));
        GetOneWord(commande);
        ANGLE_KD = Q15(atof(commande));
        InitPidAngle();
        sprintf(str, "ACK>");
        UART2PrintString(str);
    }
    else if (!strncmp( commande, "gpida", 5 ))
    {
        sprintf(str, "%d %d %d>", ANGLE_KP, ANGLE_KI, ANGLE_KD );
        UART2PrintString(str);
    }
    
    else if (!strncmp( commande, "spidn", 5 ))
    {
        GetOneWord(commande);
        DIST_KP = Q15(atof(commande));
        GetOneWord(commande);
        DIST_KI = Q15(atof(commande));
        GetOneWord(commande);
        DIST_KD = Q15(atof(commande));
        InitPidDist();
        sprintf(str, "ACK>");
        UART2PrintString(str);
    }
    else if (!strncmp( commande, "gpidn", 5 ))
    {
        sprintf(str, "%d %d %d>", DIST_KP, DIST_KI, DIST_KD );
        UART2PrintString(str);
    }



    
    else if (!strncmp( commande, "sprmo", 5 ))
    {
        //sprintf(str, "%s ", commande);
        GetOneWord(commande); // parametres en float sur 4 octets
        Ksp[L] = atof(commande);
        Kvel[L] = Ksp[L]*1000000;
        //sprintf(str, "%s<%s> ", str, commande);

        GetOneWord(commande); // parametres en float sur 4 octets
        Ksp[R] = atof(commande);
        Kvel[R] = Ksp[R]*1000000;
        //sprintf(str, "%s<%s> ", str, commande);
        
        GetOneWord(commande); // parametres en float sur 4 octets
        longueurAxe = atof(commande);
        longueurSemiAxe = longueurAxe/2;
        sprintf(str, "ACK>");
        UART2PrintString(str);
    }
    else if (!strncmp( commande, "gprmo", 5 ))
    {
        sprintf(str, "%f %f %f %ld %ld>", Ksp[L], Ksp[R], longueurAxe, Kvel[L], Kvel[R] );
        UART2PrintString(str);
    }













    else if (!strncmp( commande, "tpid", 4 ))
    {
        //sprintf(str, "%d %d>", (int)IcPeriodTmp2, (int)IcPeriodTmp1 ); // POS1CNT = droite, POS2CNT = gauche
        sprintf(str, "%d %d>", (int)testPID[0], (int)testPID[1] );

        UART2PrintString(str);
    }



    
    else if (!strncmp( commande, "fw", 2 ))
    {
        sprintf(str, _VERSION );
        UART2PrintString(str);
    }
    else if (!strncmp( commande, "reset", 6 ))
    {
        sprintf(str, "Reset dans 1 seconde>" );
        UART2PrintString(str);
        DelayN1ms(1000);
        asm("RESET");
    }
    else
    {
        sprintf(str, "unknow command>");
        UART2PrintString( str );
    }
    
    uartNextOut = 0;
    uartCommande = 0;
}

