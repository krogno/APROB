/*######################################################################
 *                          CONSTANTES MODIFIABLES
 *######################################################################
 */

//Numero de l'interruption utilisee sur front montant de la voie A de l encodeur gauche (0-> pin 2)
#define INTERRUPTION_GAUCHE 0
//Pin a laquelle est reliee la voie B de l encodeur gauche (pour avoir le sens de rotation)
#define PIN_SENS_GAUCHE  4
//Changer la valeur de cette constante en cas de decompte dans le mauvais sens
#define SENS_GAUCHE true

#define INTERRUPTION_DROIT 1 //(pin 3 sur arduino Uno)
#define PIN_SENS_DROIT 5
#define SENS_DROIT true

//Pin utilisees pour piloter les moteurs via la carte SaberTooth en mode analogique
#define PIN_MOTEUR_GAUCHE 9
#define PIN_MOTEUR_DROIT 10

//Periode d asservissement (en ms)
//Doit etre suffisament importante pour que de nombreux crans aient ete decomptes (10 serait  un debut)
//et etre suffisament grande pour que les calculs / emissions de message / reception de message aient le temps de se faire
#define PERIODE 20

//Constantes du PID de vitesse
#define K_VITESSE_P 0.1
#define K_VITESSE_I 0.0
#define K_VITESSE_D 0.0
#define SATURATION_I 500 //seuil de saturation du terme integral
//il permet de limiter les oscillations instables
/*######################################################################
 *                          VARIABLES GLOBALES
 *######################################################################
 */

//Nombre de crans decomptes a gauche et a droite respectivement
//Ici j ai utilise des int (encodes sur 16 bits), si il s avere qu il y a des depassements utiliser des des long (32 bits)
int cransGauche=0;
int cransDroit=0;


int consigneVitesseGauche=0;
int consigneVitesseDroit=0;

//True si les moteurs doivent etre alimentes
//bool go=false; NE PAS UTILISER EN MODE ANALOGIQUE : IL FAUT TOUT LE TEMPS ASSERVIR SINON LE ROBOT PART DANS LES CHOUX

//Instant auquel la boucle d asservissement a ete effectuee pour la derniere fois
unsigned long last_time=0;

/*######################################################################
 *                          SETUP 
 *######################################################################
 */
void setup() 
{
    Serial.begin(9600);
    
    //Une interruption sera lancee a chaque front montant sur les voies A des encodeurs
    attachInterrupt(INTERRUPTION_GAUCHE, InterruptionGauche, RISING);
    attachInterrupt(INTERRUPTION_DROIT, InterruptionDroit, RISING);
    //les voies B des encodeurs sont des entrees
    pinMode(PIN_SENS_DROIT, INPUT);
    pinMode(PIN_SENS_GAUCHE, INPUT);
    
    pinMode(PIN_MOTEUR_DROIT, OUTPUT);
    pinMode(PIN_MOTEUR_GAUCHE, OUTPUT);
    
}
/*######################################################################
 *                          LOOP 
 *######################################################################
 */
void loop() 
{
    
    if(/*go && */millis()-last_time>PERIODE)
    {
        last_time=millis();
        TimerAsservissement();
    }
    else
    {
        /*Il serait possible ici de tester  si il reste beaucoup de temps ou pas avant
         * la prochaine execution de l asservissement et de ne verifier les messages que
         * si on a suffisamment de temps pour ne pas decaler la boucle
         */
        
        TraiterPotentielMessage();
    }
    
}

/*######################################################################
 *                          INTERRUPTIONS
 *######################################################################
 */

//Interruption appelee a chaque front montant sur la voie A de l encodeur gauche
void InterruptionGauche()
{
    switch(digitalRead(PIN_SENS_GAUCHE)==SENS_GAUCHE)
    {
        case true  : cransGauche++; break;
        case false : cransGauche--; break;
    }
}

void InterruptionDroit()
{
    switch(digitalRead(PIN_SENS_DROIT)==SENS_DROIT)
    {
        case true  : cransDroit++; break;
        case false : cransDroit--; break;
    }
}

/*######################################################################
 *                          ASSERVISSEMENT
 *######################################################################
 */

//Fonction appelee periodiquement et realisant l asservissement en vitesse
//(Les timers ne semblent pas exister sur arduino et les deux interruptions sont prises)
inline void TimerAsservissement()
{
    static int old_cransGauche=0;
    static int old_cransDroit=0;
      
    int epsilonVitesseGauche=consigneVitesseGauche-(cransGauche-old_cransGauche);
    int epsilonVitesseDroit=consigneVitesseDroit-(cransDroit-old_cransDroit);
    
    old_cransGauche=cransGauche;
    old_cransDroit=cransDroit;
 
    //Float contenant une valeur de consigne pour les moteurs pouvant etre negative ou positive   
    int consigneGauche=PID_Vitesse(epsilonVitesseGauche);
    int consigneDroit=PID_Vitesse(epsilonVitesseDroit);
    
    //Entiers envoyes aux sorties analogiques (PWM)
    int intConsigneGauche=max(min(128+consigneGauche,255),0);
    int intConsigneDroit=max(min(128+consigneDroit,255),0);
    
    analogWrite(PIN_MOTEUR_GAUCHE,intConsigneGauche);
    analogWrite(PIN_MOTEUR_DROIT,intConsigneDroit);

}

/*######################################################################
 *                          FONCTIONS
 *######################################################################
 */

//La j'ai mis le meme PID pour les deux moteurs pour avoir moins de constantes a regler
//Vous pouvez modifier si ca va pas
 inline float PID_Vitesse(int epsilon)
 {
     static int old_epsilon=0;
     static int integrale=0;
     static int derivee=0;
     
     derivee=epsilon-old_epsilon;
     integrale=max(min(integrale+epsilon,SATURATION_I),-SATURATION_I);
	
     
     old_epsilon=epsilon;
     
     return K_VITESSE_P*epsilon+K_VITESSE_I*integrale+K_VITESSE_D*derivee;
 }

inline void TraiterPotentielMessage()
{
    //Si un message a ete recu sur le port serie
    if(Serial.available()>0)
    {
        switch(Serial.read())
        {
            case 's' : //Stop : arrete l asservissement
                       // go=false; 
                        consigneVitesseGauche=0;
			consigneVitesseDroit=0;
                        break;  
            case 'r' : //Reset : reinitialise les compteurs de cran
                        cransDroit=0;
                        cransGauche=0;
            case 'g' : /* Go : le message de consigne doit etre de type "gVW" ou V et W sont encodes sur 1 octet chacun (caracteres ASCII pour les positifs).
                        * avec (valeur de V) affectee a la consigne de vitesse du moteur gauche et (valeur de W) celle du moteur droit*/
                       // go=true;
                        consigneVitesseGauche=(char)Serial.read();
                        consigneVitesseDroit=(char)Serial.read();
                        break;
            case 'e' : //Renvoie la valeur des encodeurs
                        Serial.write(cransGauche);
                        Serial.write(" ");
                        Serial.write(cransDroit);
                        Serial.println("");
            default : break;
        }
        //On vide le tampon de message
        Serial.flush();
    }
}
