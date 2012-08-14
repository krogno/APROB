/****************************************************************************
  Function:
    void Odometrie( void )

  Description:
    Fonction qui calcule la position X et Y du robot
	Variables en entrée :
	- POS1CNT
	- POS2CNT

	Variables en sortie :
	- CosPrev : Cosinus de l'angle du robot
	- SinPrev : Sinus de l'angle du robot
	- PosXmes : Position X du robot (mm)
	- PosYmes : Position Y du robot (mm)
	- Space	  : Distance parcourue par le robot (mm)

  Precondition:
 	None

  Parameters:
    None

  Return Values:
    None

  Remarks:
    None
  ***************************************************************************/

int Dist[2];
double Ksp[2];
float ThetaMes; // Angle mesuree (en radian)
float PosXmes; // position X mesuree (en mm)
float PosYmes; // position Y mesuree (en mm)

float Space;
float longueurAxe;
float longueurSemiAxe;
float CosAngle;		// Cosinus de l'angle
float SinAngle;		// Sinus de l'angle

void initOdometrie()
{
    // En mm (1 cran en mm)
    Ksp[R] = 0.653;//0.101243;
    //0.668250;//0.668250;//0.71807; //0.673198
    Ksp[L] = 0.653;//0.101242;
    //0.660250;//0.671179;//0.71807; //il faut moins
    longueurAxe = 185.3f;//183.000;//185.000;

    longueurSemiAxe = longueurAxe/2;

    PosXmes = 0.f;
    PosYmes = 0.f;
    ThetaMes = 0.f;

    CosAngle=cosf(ThetaMes);
    SinAngle=sinf(ThetaMes);
}

void Odometrie()
{
        float CosAngleQ;		// Cosinus de l'angle
	float SinAngleQ;		// Sinus de l'angle
	float DeltaDist;                // Distance parcourue par le robot
	float DeltaTheta;		// delta rotation angle
	float DeltaPosX;		// delta space on X axis
	float DeltaPosY;		// delta space on Y axis
	float SaMinusSb;
	float SrPlusSl;
	float Radius;
        float Distf[2];

	static int AncienRoueGauche = 0;
	static int AncienRoueDroite = 0;
	int i1, i2;

	i1=POS1CNT;
	i2=POS2CNT;

	if(AncienRoueGauche > i2)
	{
		if(AncienRoueGauche - i2 > ODO_MAX_2)
		{
			Dist[L]= i2 + (ODO_MAX -AncienRoueGauche);//probleme
		}
		else
		{
			Dist[L]= i2 - AncienRoueGauche;
		}
	}
	else
	{
		if(i2 - AncienRoueGauche > ODO_MAX_2)
		{
			Dist[L]= i2 - (ODO_MAX + AncienRoueGauche);//probleme
		}
		else
		{
			Dist[L]= i2 - AncienRoueGauche;
		}
	}


	if(AncienRoueDroite > i1)
	{
		if(AncienRoueDroite - i1 > ODO_MAX_2)
		{
			Dist[R]= i1 + (ODO_MAX -AncienRoueDroite);//probleme
		}
		else
		{
			Dist[R]= i1 - AncienRoueDroite;
		}
	}
	else
	{
		if(i1 - AncienRoueDroite > ODO_MAX_2)
		{
			Dist[R]= i1 - (ODO_MAX + AncienRoueDroite);//probleme
		}
		else
		{
			Dist[R]= i1 - AncienRoueDroite;
		}
	}


	Distf[R]=(float)(Dist[R])*Ksp[R];	// distance parcourue par la roue droite en mm
	Distf[L]=(float)(Dist[L])*Ksp[L];	// distance parcourue par la roue gauche en mm
	// les constantes Ksp sont determinées avec des tests


	SaMinusSb=Distf[R]-Distf[L];

	SrPlusSl=Distf[R]+Distf[L];

	if (fabs(SaMinusSb) <= DIST_MIN) // Ligne droite
	{
		DeltaDist=Distf[R];
		DeltaPosX=DeltaDist*CosAngle;
		DeltaPosY=DeltaDist*SinAngle;
	}
	else if (fabs(SrPlusSl) <= DIST_MIN) // Tourne sur son axe
	{
		DeltaTheta=SaMinusSb/longueurAxe;
		ThetaMes=fmodf((ThetaMes+DeltaTheta),TWOPI);//current orient. in 2PI range
		CosAngle=cosf(ThetaMes);
		SinAngle=sinf(ThetaMes);
		DeltaPosX=0;
		DeltaPosY=0;
		DeltaDist=0;
	}
	else // trajectoire quelconque
	{
		DeltaTheta=SaMinusSb/longueurAxe;
		ThetaMes=fmodf((ThetaMes+DeltaTheta),TWOPI);//current orient. in 2PI range
		CosAngleQ=cosf(ThetaMes);
		SinAngleQ=sinf(ThetaMes);
		DeltaDist=SrPlusSl/2;
		Radius = (longueurSemiAxe)*(SrPlusSl/SaMinusSb);

		DeltaPosX=Radius*(SinAngleQ-SinAngle);
		DeltaPosY=Radius*(CosAngle-CosAngleQ);

		CosAngle=CosAngleQ;
		SinAngle=SinAngleQ;
	}

	Space += DeltaDist; // Distance parcourue
	PosXmes += DeltaPosX;
	PosYmes += DeltaPosY;

	AncienRoueGauche = i2;
	AncienRoueDroite = i1;
}

