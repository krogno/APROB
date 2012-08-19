
/*######################################################################
 *                        PROTOCOLE
 *######################################################################
Notations :
  - consigne de vitesse d avance : vitesse, egale a 2 fois la moyenne du nombre de crans sur chaque roue codeuse a parcourir par periode d asservissement
  - consigne de vitesse de rotation : omega, egale a la difference du nombre de crans a parcourir de chaque roue codeuse par periode d asservissement.
  
vitesse et omega doivent etre encodees sur 32bits (type long sur arduino) en little endian (l'octet de poids faible en premier e.g. 1 s ecrit 0001 0000 0000 0000)

Consigne envoyable a la carte: 
  - 1 octet : caractere 's' : ordre de ne pas asservir le robot en vitesse, juste d'imposer une tension nulle aux moteurs
  - 1 octet : caractere '?' : demande du nombre de crans decomptes par les roues codeuses
  - 9 octets: caractere 'v' (1 octet), vitesse (4 octets) et omega  (4 octets) : nouvelles consignes de vitesse
      
Messages envoyables par la carte
  - 64 bits composes de cransGauche puis cransDroit (des entiers de 32 bits encodes en little endian)
 */
#define ORDRE_ARRET_MOTEURS  's'
#define ORDRE_DEMANDE_CRANS  '?'
#define ORDRE_VITESSE        'v'

/*######################################################################
 *                          CONSTANTES MODIFIABLES
 *######################################################################
 */

//Numero de l'interruption utilisee sur front montant de la voie A de l encodeur gauche (0-> pin 2)
#define INTERRUPTION_VOIE_A_GAUCHE 0
//Pin a laquelle est reliee la voie B de l encodeur gauche (pour avoir le sens de rotation)
#define PIN_VOIE_B_GAUCHE  4

#define INTERRUPTION_VOIE_A_DROITE 1 //(interruption 1 -> pin 3 sur arduino Uno)
#define PIN_VOIE_B_DROITE 5

//Pin utilisees pour piloter les moteurs (les PWMs fonctionnent avec les pins 3, 5, 6, 9, 10, et 11)
#define PIN_MOTEUR_GAUCHE_SENS 8
#define PIN_MOTEUR_GAUCHE_PWM 9
#define PIN_MOTEUR_DROITE_SENS 7
#define PIN_MOTEUR_DROITE_PWM 10

/*Periode d asservissement (en ms), doit etre suffisament importante pour que de nombreux crans aient ete decomptes (10 serait  un debut) 
et etre suffisament grande pour que les calculs / emissions de message / reception de message aient le temps de se faire.
Mais plus l'asservissement est rapide, mieux c'est.
Notes : 
- Les PWMs generes par l'arduino sont cadencés à 490Hz, soit une période de 2ms, donc il faut au moins 20ms de periode d'asservissement.
- Il est possible de generer des PWMs a plus haute frequence en bidouillant les registres du microcontroleurs via du code pour AVR.
*/
#define PERIODE 40
#define DUREE_MAX_TRAITEMENT_MESSAGES  5

//Constantes du PID de vitesse
#define K_VITESSE_P 0.1
#define K_VITESSE_I 0.0
#define K_VITESSE_D 0.0
#define K_VITESSE_SATURATION_I 500 //seuil de saturation du terme integral
//il permet de limiter les oscillations instables

//Constantes du PID de vitesse de rotation
#define K_OMEGA_P 0.1
#define K_OMEGA_I 0.0
#define K_OMEGA_D 0.0
#define K_OMEGA_SATURATION_I 500 //seuil de saturation du terme integral
//il permet de limiter les oscillations instables

/*######################################################################
 *                          VARIABLES GLOBALES
 *######################################################################
 */

//Nombre de crans decomptes a gauche et a droite respectivement (positif ou négatif)
//Ici j ai utilise des long (encodes sur 32 bits) pour ne pas avoir de depassements
volatile long cransGauche=0;
volatile long cransDroite=0;


long consigneOmega=0;
long consigneVitesse=0;
char ordre;

//Instant auquel la boucle d asservissement a ete effectuee pour la derniere fois
unsigned long last_time=0;

/*######################################################################
 *                          SETUP 
 *######################################################################
 */
void setup() 
{
    Serial.begin(115200);
    
    //Une interruption sera lancee a chaque front montant sur les voies A des encodeurs
    attachInterrupt(INTERRUPTION_VOIE_A_GAUCHE, InterruptionGauche, RISING);
    attachInterrupt(INTERRUPTION_VOIE_A_DROITE, InterruptionDroit, RISING);
    
    //les voies B des encodeurs sont des entrees pour tester le sens de rotation
    pinMode(PIN_VOIE_B_GAUCHE, INPUT);
    pinMode(PIN_VOIE_B_DROITE, INPUT);
    
    pinMode(PIN_MOTEUR_DROITE_SENS, OUTPUT);
    pinMode(PIN_MOTEUR_GAUCHE_SENS, OUTPUT);
    
}
/*######################################################################
 *                          LOOP 
 *######################################################################
 */
void loop() 
{
    
    if(millis()-last_time>PERIODE)
    {
      //Boucle d'asservissement
        last_time=millis();
        if(ordre != ORDRE_ARRET_MOTEURS)
          TimerAsservissement();
        else AlimenterMoteurs(0,0);
    }
    if(millis()-last_time < PERIODE-DUREE_MAX_TRAITEMENT_MESSAGES)
    {
      //Si on a suffisament de temps pour ne pas perturber l'asservissement, on traite les messages
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
    if(digitalRead(PIN_VOIE_B_GAUCHE))
      cransGauche++;
    else cransGauche--;
}

void InterruptionDroit()
{
    if(digitalRead(PIN_VOIE_B_DROITE))
      cransDroite++;
    else cransDroite--;
}

/*######################################################################
 *                          ASSERVISSEMENT
 *######################################################################
 */

//Fonction appelee periodiquement et realisant l asservissement en vitesse

inline void TimerAsservissement()
{
    static int old_somme=0;
    static int old_difference=0;
    
    int somme=cransDroite+cransGauche;
    int difference=(cransDroite-cransGauche);
      
    int epsilonOmega=consigneOmega-difference+old_difference;
    int epsilonVitesse=consigneVitesse-somme+old_somme;
    
    old_somme=somme;
    old_difference=difference;
 
    AlimenterMoteurs(CorrectionVitesse(epsilonVitesse), CorrectionOmega(epsilonOmega));
}

/*######################################################################
 *                          FONCTIONS
 *######################################################################
 */
 inline float CorrectionVitesse(int epsilon)
 {
     static int old_epsilon=0;
     static int integrale=0;
     static int derivee=0;
     
     derivee=epsilon-old_epsilon;
     integrale=integrale+epsilon;
     
     //Saturation du terme intégral
     if(integrale >  K_VITESSE_SATURATION_I)
       integrale=K_VITESSE_SATURATION_I;
     else if(integrale <  -K_VITESSE_SATURATION_I)
       integrale=-K_VITESSE_SATURATION_I;
       
     old_epsilon=epsilon;
     
     return K_VITESSE_P*epsilon+K_VITESSE_I*integrale+K_VITESSE_D*derivee;
 }
 
inline float CorrectionOmega(int epsilon)
{
     static int old_epsilon=0;
     static int integrale=0;
     static int derivee=0;
     
     derivee=epsilon-old_epsilon;
     integrale=integrale+epsilon;
     
     //Saturation du terme intégral
     if(integrale >  K_VITESSE_SATURATION_I)
       integrale=K_VITESSE_SATURATION_I;
     else if(integrale <  -K_VITESSE_SATURATION_I)
       integrale=-K_VITESSE_SATURATION_I;
	
     old_epsilon=epsilon;
     
     return K_OMEGA_P*epsilon+K_OMEGA_I*integrale+K_OMEGA_D*derivee;
}

inline void AlimenterMoteurs(float vitesse, float omega)
{
  
    //Calcul des tensions moteurs
    int tensionDroite=vitesse+omega;
    int tensionGauche=vitesse-omega;
    
    //Mise a jours des rapports cycliques et des sens de rotation, en seuillant a 255
    if(tensionDroite >= 0)
    {
      digitalWrite(PIN_MOTEUR_DROITE_SENS, HIGH);
      if(tensionDroite<255)
        analogWrite(PIN_MOTEUR_DROITE_PWM, tensionDroite);
      else analogWrite(PIN_MOTEUR_DROITE_PWM, 255);
    }
    else
    {
      digitalWrite(PIN_MOTEUR_DROITE_SENS, LOW);   
      if(tensionDroite > -255)
        analogWrite(PIN_MOTEUR_DROITE_PWM, 255+tensionDroite);
      else analogWrite(PIN_MOTEUR_DROITE_PWM, 0);      
    }
    
    if(tensionGauche >= 0)
    {
      digitalWrite(PIN_MOTEUR_GAUCHE_SENS, HIGH);
      if(tensionGauche<255)
        analogWrite(PIN_MOTEUR_GAUCHE_PWM, tensionGauche);
      else analogWrite(PIN_MOTEUR_GAUCHE_PWM, 255);
    }
    else
    {
      digitalWrite(PIN_MOTEUR_GAUCHE_SENS, LOW);   
      if(tensionGauche > -255)
        analogWrite(PIN_MOTEUR_GAUCHE_PWM, 255+tensionGauche);
      else analogWrite(PIN_MOTEUR_GAUCHE_PWM, 0);      
    }
}

inline void TraiterPotentielMessage()
{
    //Si une consigne de vitesse a ete recue on charge les nouvelles consignes de vitesse, comme plusieurs messages peuvent s etre accumuler, on met ca dans une boucle
    while(Serial.available()>0)
    {
      char ordre=Serial.read();
      switch(ordre)
      {
        case ORDRE_VITESSE:
              Serial.readBytes((char*) &consigneVitesse, 4);
              Serial.readBytes((char*) &consigneOmega, 4);
              break;
        case ORDRE_DEMANDE_CRANS:
              Serial.write((byte*) &cransDroite, 4);
              Serial.write((byte*) &cransGauche,4);
              break;
      }
    }
}
