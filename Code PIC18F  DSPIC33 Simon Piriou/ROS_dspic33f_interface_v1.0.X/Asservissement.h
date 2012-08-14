


/*---------------------------------------------------------------------------*/
/* Structures		    											     */
/*---------------------------------------------------------------------------*/

struct Bits{
	unsigned bit0:1;
	unsigned bit1:1;
	unsigned bit2:1;
	unsigned bit3:1;
	unsigned bit4:1;
	unsigned bit5:1;
	unsigned bit6:1;
	unsigned bit7:1;
	unsigned bit8:1;
	unsigned bit9:1;
	unsigned bit10:1;
	unsigned bit11:1;
	unsigned bit12:1;
	unsigned bit13:1;
	unsigned bit14:1;
	unsigned bit15:1;
};

volatile struct Bits VOLbits1;
#define PID_CALC_FLAG    VOLbits1.bit1
#define CYCLE1_FLAG      VOLbits1.bit2
#define CYCLE2_FLAG      VOLbits1.bit3

#define ANGLE_OK_FLAG    VOLbits1.bit4
#define DIST_OK_FLAG     VOLbits1.bit5
#define DIST_ENABLE_FLAG VOLbits1.bit6



int TimerPid1 = 0;
int TimerPid2 = 0;

int IcPeriodTmp1;
int IcPeriodTmp2;

long VitesseMes[2];

float PosXdes = 0; // position X voulue
float PosYdes = 0; // position Y voulue
float ThetaDes = 0;// Angle voulu

float ThetaDesRef; // to temporarely store angle (Rad)

int VelDes[2] = {0,0}; // en mm/s : vitesse consigne désirée pour chaque roue
int VelFin[2] = {0,0}; // En Q15 : c'est la référence PID_REF1 et PID_REF2 calculée à partir de VelDes en 1/40 mm/s, comme Kvel

long VitesseCpteur[2];
long Vitesse[2] = {0,0};
int compteurVitesse = 0;
int testPID[2] = {0,0};

int offsetMoteur = 0;

int VitesseMoyenne = 0; // Vitesse désirée du robot (en mm/s)
float Freinage = 0.f; // 1 => pas de frein, 0 => arret

long Kvel[2]; // 32 bits

typedef enum Ordre
{
    ARRETER, // Robot arreté, il est asservi sur sa position actuelle
    AVANCER, // Le Robot avance une certaine distance 
    TOURNER, // Le robot s'aligne à un certain angle
    DEBUG    // Mode DEBUG pour tester le premier étage de l'asserv
    //(il n'y a pas de modulo 2*pi => plusieurs tours sont possibles)
}Ordre;

Ordre ordreEnCours;



// Définition PID route Droite
tPID PIDstruct1;
fractional abcCoefficient1[3] __attribute__ ((section (".xbss, bss, xmemory")));
//fractional controlHistory1[3] __attribute__ ((section (".ybss, bss, ymemory")));
fractional controlHistory1[3] __attribute__ ((space(ymemory),far));
fractional kCoeffs1[] = {0,0,0};
#define KP1 kCoeffs1[0]
#define KI1 kCoeffs1[1]
#define KD1 kCoeffs1[2]
#define PID_OUT1 PIDstruct1.controlOutput
#define PID_MES1 PIDstruct1.measuredOutput
#define PID_REF1 PIDstruct1.controlReference

// Définition PID route Gauche
tPID PIDstruct2;
fractional abcCoefficient2[3] __attribute__ ((section (".xbss, bss, xmemory")));
//fractional controlHistory2[3] __attribute__ ((section (".ybss, bss, ymemory")));
fractional controlHistory2[3] __attribute__ ((space(ymemory),far));
fractional kCoeffs2[] = {0,0,0};
#define KP2 kCoeffs2[0]
#define KI2 kCoeffs2[1]
#define KD2 kCoeffs2[2]
#define PID_OUT2 PIDstruct2.controlOutput
#define PID_MES2 PIDstruct2.measuredOutput
#define PID_REF2 PIDstruct2.controlReference

// Définition PID Angle
tPID AnglePIDstruct;
fractional AngleabcCoefficient[3] __attribute__ ((section (".xbss, bss, xmemory")));
fractional AnglecontrolHistory[3] __attribute__ ((space(ymemory),far));
fractional AngleKCoeffs[] = {0,0,0};
#define ANGLE_KP AngleKCoeffs[0]
#define ANGLE_KI AngleKCoeffs[1]
#define ANGLE_KD AngleKCoeffs[2]
#define ANGLE_PID_DES AnglePIDstruct.controlReference
#define ANGLE_PID_MES AnglePIDstruct.measuredOutput
#define ANGLE_PID_OUT AnglePIDstruct.controlOutput		// PID output

// Définition PID Distance
tPID DistPIDstruct;
fractional DistabcCoefficient[3] __attribute__ ((section (".xbss, bss, xmemory")));
fractional DistcontrolHistory[3] __attribute__ ((space(ymemory),far));
fractional DistKCoeffs[] = {0,0,0};
#define DIST_KP DistKCoeffs[0]
#define DIST_KI DistKCoeffs[1]
#define DIST_KD DistKCoeffs[2]
#define DIST_PID_DES DistPIDstruct.controlReference	// desired distance
#define DIST_PID_MES DistPIDstruct.measuredOutput	// measured distance
#define DIST_PID_OUT DistPIDstruct.controlOutput	// PID output



void arreterRobot()
{
    VitesseMoyenne = 0;
    Freinage = 0.f;
    ordreEnCours = ARRETER;
}


void InitPid1(void)
{
    //Initialize the PID data structure: PIDstruct
    //Set up pointer to derived coefficients
    PIDstruct1.abcCoefficients = &abcCoefficient1[0];
    //Set up pointer to controller history samples
    PIDstruct1.controlHistory = &controlHistory1[0];
    // Clear the controler history and the controller output
    PIDInit(&PIDstruct1);
    //Derive the a,b, & c coefficients from the Kp, Ki & Kd
    PIDCoeffCalc(&kCoeffs1[0], &PIDstruct1);
}

void InitPid2(void)
{
    //Initialize the PID data structure: PIDstruct
    //Set up pointer to derived coefficients
    PIDstruct2.abcCoefficients = &abcCoefficient2[0];
    //Set up pointer to controller history samples
    PIDstruct2.controlHistory = &controlHistory2[0];
    // Clear the controler history and the controller output
    PIDInit(&PIDstruct2);
    //Derive the a,b, & c coefficients from the Kp, Ki & Kd
    PIDCoeffCalc(&kCoeffs2[0], &PIDstruct2);
}

void InitPidAngle(void)
{
    //Initialize the PID data structure: PIDstruct
    //Set up pointer to derived coefficients
    AnglePIDstruct.abcCoefficients = &AngleabcCoefficient[0];
    //Set up pointer to controller history samples
    AnglePIDstruct.controlHistory = &AnglecontrolHistory[0];
    // Clear the controler history and the controller output
    PIDInit(&AnglePIDstruct);
    //Derive the a,b, & c coefficients from the Kp, Ki & Kd
    PIDCoeffCalc(&AngleKCoeffs[0], &AnglePIDstruct);
}

void InitPidDist(void)
{
    //Initialize the PID data structure: PIDstruct
    //Set up pointer to derived coefficients
    DistPIDstruct.abcCoefficients = &DistabcCoefficient[0];
    //Set up pointer to controller history samples
    DistPIDstruct.controlHistory = &DistcontrolHistory[0];
    // Clear the controler history and the controller output
    PIDInit(&DistPIDstruct);
    //Derive the a,b, & c coefficients from the Kp, Ki & Kd
    PIDCoeffCalc(&DistKCoeffs[0], &DistPIDstruct);
}

void initAsservissement()
{
    // Le format Q15 varie de -32768 a 32767. La vitesse max est de 0.75 m/s
    // => on choisi, pour v=0.5 m/s, VitesseMes = 20000, soit 1/40 mm/s comme unité
    Kvel[R] = 0.653*4000000;
    Kvel[L] = 0.653*4000000;

    VelFin[R] = 0;
    VelFin[L] = 0;

    offsetMoteur = 0;

    PosXdes = 0; // position X voulue
    PosYdes = 0; // position Y voulue
    ThetaDes = 0;// Angle voulu

    arreterRobot();

    KP1=Q15(0.05);
    KI1=Q15(0.01);
    KD1=Q15(0);
    KP2=Q15(0.05);
    KI2=Q15(0.01);
    KD2=Q15(0);

    ANGLE_KP=Q15(0.5);
    ANGLE_KI=Q15(0.1);
    ANGLE_KD=Q15(0);

    DIST_KP=Q15(0.5);
    DIST_KI=Q15(0.1);
    DIST_KD=Q15(0);

    DIST_PID_DES=0;
    ANGLE_PID_DES=0;

    InitPid1();
    InitPid2();
    InitPidAngle();
    InitPidDist();
}



/****************************************************************************
  Function:
    void Pid1( void )

  Description:
    Fonction qui asservie la roue 1 (droite) en vitesse
	Variables utiles :
	- POS1CNT (valeur du compteur de la roue droite)
	- TimerPid1 (temps entre deux calculs de l'asservissement)
	- VelFin[R] Vitesse désirée en Q15, 1/40 mm/s

	Cette fonction fournie une consigne pour le moteur droit : PWM

  Precondition:
 	None

  Parameters:
    None

  Return Values:
    None

  Remarks:
    None
  ***************************************************************************/
void Pid1(void)
{
	int IcIndxTmp;
	int PWM;

	static int AncienRoueDroite = 0;
	int i1;

	i1=POS1CNT;
	IcPeriodTmp1=TimerPid1;
	TimerPid1 = 0;

	if(AncienRoueDroite > i1)
	{
		if(AncienRoueDroite - i1 > ODO_MAX_2)
		{
			IcIndxTmp= i1 + (ODO_MAX -AncienRoueDroite);//probleme
		}
		else
		{
			IcIndxTmp= i1 - AncienRoueDroite;
		}
	}
	else
	{
		if(i1 - AncienRoueDroite > ODO_MAX_2)
		{
			IcIndxTmp= i1 - (ODO_MAX + AncienRoueDroite);//probleme
		}
		else
		{
			IcIndxTmp= i1 - AncienRoueDroite;
		}
	}

	

	if (IcIndxTmp)	// Le moteur tourne
        {
            VitesseMes[R] = (Kvel[R]*IcIndxTmp)/IcPeriodTmp1;
            VitesseCpteur[R] += VitesseMes[R];
        }
        else
            VitesseMes[R]=0;


        PID_REF1 = VelFin[R];
	PID_MES1 = VitesseMes[R];
	PID(&PIDstruct1);


	AncienRoueDroite = i1;

	PWM = (PID_OUT1) + offsetMoteur;	// 3 : >>15 (int) <<12 (x4096)

        testPID[1] = PWM;
	//PWM = -700;
	// +200 pour que le moteur démarre lorsque la consigne est très faible
	if(PWM<0)
	{
		OC2RS = 0;
		OC1RS = min(-PWM, TMR2_VALUE);
	}
	else
	{
                OC1RS = 0;
		OC2RS = min(PWM, TMR2_VALUE);
		
	}
}



/****************************************************************************
  Function:
    void Pid2( void )

  Description:
    Fonction qui asservie la roue 2 (gauche) en vitesse
	Variables utiles :
	- POS2CNT (valeur du compteur de la roue gauche)
	- TimerPid2 (temps entre deux calculs de l'asservissement)
	- VelFin[L] Vitesse désirée en Q15, 1/40 mm/s

	Cette fonction fournie une consigne pour le moteur gauche : PWM

  Precondition:
 	None

  Parameters:
    None

  Return Values:
    None

  Remarks:
    None
  ***************************************************************************/
void Pid2(void)
{
	int IcIndxTmp;
	int PWM;

	static int AncienRoueGauche = 0;
	int i2;

	i2=POS2CNT;
	IcPeriodTmp2=TimerPid2;
	TimerPid2 = 0;

	if(AncienRoueGauche > i2)
	{
		if(AncienRoueGauche - i2 > ODO_MAX_2)
		{
			IcIndxTmp= i2 + (ODO_MAX -AncienRoueGauche);//probleme
		}
		else
		{
			IcIndxTmp= i2 - AncienRoueGauche;
		}
	}
	else
	{
		if(i2 - AncienRoueGauche > ODO_MAX_2)
		{
			IcIndxTmp= i2 - (ODO_MAX + AncienRoueGauche);//probleme
		}
		else
		{
			IcIndxTmp= i2 - AncienRoueGauche;
		}
	}

        if (IcIndxTmp)	// Le moteur tourne
        {
            //VitesseMes[L] = (Kvel[L]*IcIndxTmp)/IcPeriodTmp1;
            VitesseMes[L] = (Kvel[L]*IcIndxTmp)/IcPeriodTmp2;
            VitesseCpteur[L] += VitesseMes[L];
        }
        else
            VitesseMes[L]=0;


        PID_REF2 = VelFin[L];

	PID_MES2 = VitesseMes[L];
	PID(&PIDstruct2);


	AncienRoueGauche = i2;

	PWM = (PID_OUT2) + offsetMoteur;	// 3 : >>15 (int) <<12 (x4096)

        testPID[0] = PWM;
	//PWM = -700;
	// +200 pour que le moteur démarre lorsque la consigne est très faible
	if(PWM<0)
	{
		OC4RS = 0;
		OC3RS = min(-PWM, TMR2_VALUE);
	}
	else
	{
                OC3RS = 0;
		OC4RS = min(PWM, TMR2_VALUE);

	}
}






/****************************************************************************
  Function:
    void  Orientation( void )

  Description:
    Fonction qui gère l'asservissment en angle du robot

  Precondition:
 	None

  Parameters:
    None

  Return Values:
    None

  Remarks:
    None
  ***************************************************************************/
void Orientation(void) // Asservissement en angle du robot, et determination des vitesses à appliquer aux moteurs
{
	int DeltaVel;	// difference de vitesse entre les deux roues qu'il faut pour corriger le robot
	int RealVel;	// vitesse du centre du robot en tenant compte des phases de deceleration
	float Error;

        
        // L'erreur maximale en angle est PI.
        // Donc on rammene l'erreur sur l'intervale [-pi, pi] pour qu'apres une division par pi,
        // on recupere un nombre entre 0 et 1 pour le PID.
        if (ThetaDes < 0) ThetaDes += TWOPI; // La valeur de l'angle fournie par atan2 est entre -pi et pi
        if (ThetaMes < 0) ThetaMes += TWOPI;
        Error = ThetaMes - ThetaDes;

        if (Error > PI)
        {
                Error -= TWOPI;
        }
        else if (Error < -PI)
        {
                Error += TWOPI;
        }

        if (ordreEnCours == TOURNER)
        {
                if (fabsf(Error) < MIN_THETA_ERR)
                {
                    arreterRobot();
                    //ANGLE_OK_FLAG = 1;	// target ok (Flag à 1 uniquement quand l'angle est bon, sinon 0)
                }
        }
        

        ANGLE_PID_DES = 0; // On regarde l'erreur et non l'angle
        ANGLE_PID_MES = Q15(Error/PI);
        PID(&AnglePIDstruct);

        // ANGLE_PID_OUT vaut 1 en "Q15" et 32 767 en "signed int" au maximum.
        // Pour avoir un bon comportement du robot, dont la vitesse normale est entre 100 et 200,
        // on veut une différence de vitesse entre les deux roues de l'ordre de 100 mm/s.
        // 32 768/1024 = 32. Donc on multiplie par 32 768 (2^15) pour obtenir un entier signé,
        // et on divise par 64 (2^5) pour obtenir un nombre aux alentours de 500,
        // soit (ANGLE_PID_OUT >> 15) << 5 = ANGLE_PID_OUT >> 5
        DeltaVel = (ANGLE_PID_OUT >> 7); // => 256 max (en mm/s)

        if(ordreEnCours == TOURNER)
        {
                if(DeltaVel > VitesseMoyenne)
                        DeltaVel = VitesseMoyenne;
                else if(DeltaVel < -VitesseMoyenne)
                        DeltaVel = -VitesseMoyenne;

                VelDes[R] = + DeltaVel;
                VelDes[L] = - DeltaVel;
        }
        else
        {
            RealVel = VitesseMoyenne * Freinage;
            VelDes[R] = RealVel + DeltaVel;
            VelDes[L] = RealVel - DeltaVel;
        }

        // Pour les tests, on ne regarde pas l'angle
        // DeltaVel = 0;

	

	if (VelDes[R] > VITESSE_MAX)
	{
		VelDes[R] = VITESSE_MAX;
		VelDes[L] = VITESSE_MAX  - (DeltaVel << 1); // MAX_ROT_SPEED  - 2*DeltaVel, pour conserver la difference de vitesse
	}
	else if (VelDes[R] < -VITESSE_MAX)
	{
		VelDes[R] = -VITESSE_MAX;
		VelDes[L] = -VITESSE_MAX - (DeltaVel << 1);
	}
	else if (VelDes[L] > VITESSE_MAX)
	{
		VelDes[L] = VITESSE_MAX;
		VelDes[R] = VITESSE_MAX + (DeltaVel << 1);
	}
	else if (VelDes[L] < -VITESSE_MAX)
	{
		VelDes[L] = -VITESSE_MAX;
		VelDes[R] = -VITESSE_MAX + (DeltaVel << 1);
	}


	// gestion des rampes de vitesse
	VelFin[R] = VelDes[R]*40; // VelDes en mm/s et VelFin en 1/40 mm/s => x40
	/*if (VelFin[R] != PID_REF1)
	{
		if (PID_REF1>=0 && VelFin[R] > PID_REF1)
		{
			Ramp1 = Q15( ACC);
			RAMP_T_FLAG1 = 1;
		}
		if (PID_REF1>=0 && VelFin[R] < PID_REF1)
		{
			Ramp1 = Q15(-DEC);
			RAMP_T_FLAG1 = 0;
		}
		if (PID_REF1< 0 && VelFin[R] > PID_REF1)
		{
			Ramp1 = Q15( DEC);
			RAMP_T_FLAG1 = 1;
		}
		if (PID_REF1< 0 && VelFin[R] < PID_REF1)
		{
			Ramp1 = Q15(-ACC);
			RAMP_T_FLAG1 = 0;
		}
		RAMP_FLAG1 = 1; // acceleration ramp start
	}*/

	VelFin[L] = VelDes[L]*40; // VelDes en mm/s et VelFin en 1/40 mm/s => x40
	/*if (VelFin[L] != PID_REF2)
	{
		if (PID_REF2>=0 && VelFin[L] > PID_REF2)
		{
			Ramp2 = Q15( ACC);
			RAMP_T_FLAG2 = 1;
		}
		if (PID_REF2>=0 && VelFin[L] < PID_REF2)
		{
			Ramp2 = Q15(-DEC);
			RAMP_T_FLAG2 = 0;
		}
		if (PID_REF2< 0 && VelFin[L] > PID_REF2)
		{
			Ramp2 = Q15( DEC);
			RAMP_T_FLAG2 = 1;
		}
		if (PID_REF2< 0 && VelFin[L] < PID_REF2)
		{
			Ramp2 = Q15(-ACC);
			RAMP_T_FLAG2 = 0;
		}
		RAMP_FLAG2 = 1; // acceleration ramp start
	}*/
}





void Navigation(void)	// called after odometry is performed
{
	float DPosX;	// delta PosX
	float DPosY;	// delta PosY
	float Dist;		// distance

	if (DIST_ENABLE_FLAG)
	{
		// distance from goal, i.e.: distance from desired to current position
		DPosX = PosXdes - PosXmes;	   // X component of distance
		DPosY =	PosYdes - PosYmes;	   // Y component of distance
		Dist = sqrtf(powf(DPosX,2) + powf(DPosY,2));// Pythagora's Theorem
	}
	else
	{
		Dist = OBST_THRESHOLD;	// a reference value
		DPosX = Dist * sin(ThetaDesRef);
		DPosY = Dist * cos(ThetaDesRef);
	}

	if ((Dist < MIN_DIST_ERR) && (ordreEnCours == AVANCER))	// goal reached
	{
		DIST_ENABLE_FLAG = 0;	// disable distance computing [24a]
                arreterRobot();
		//DIST_OK_FLAG = 1;	// target ok
		return; // job done
	}
	else
	{// set ThetaDes and VelDecr to reach the goal avoiding obstacles
            if ((DPosX != 0) && (DPosY != 0)) // Si Y=0 et X=0 en meme temps, atan2 n'est pas définie
            {								  // La sortie de atan2 est entre -pi et pi
                    ThetaDes = atan2f(DPosY,DPosX); // DPosX,DPosY);
                    ThetaDesRef = ThetaDes;
                    //PIDInit(&AnglePIDstruct); // efface historique (terme integral et derivé)
            }
	}

	if (Dist < MIN_GOAL_DIST)
	{
            //DIST_PID_DES = 0;		   // the goal is to have dist=0 from target
            //DIST_PID_MES = Q15((float)Dist/(float)MIN_GOAL_DIST);  // measured input
            //PID(&DistPIDstruct);
            Freinage = (float)Dist/(float)MIN_GOAL_DIST;//-Fract2Float(DIST_PID_OUT); // [24d]
	}
	else
	{
            Freinage = 1.f;
	}
}
