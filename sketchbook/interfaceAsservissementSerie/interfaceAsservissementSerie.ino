#include <digitalWriteFast.h>


/*######################################################################
 *                        PROTOCOLE
 *######################################################################
Consigne envoyable a la carte: 
- chaine de caractere "?\n" -> demande les valeurs decomptees par les encodeurs
 => la carte repond "cransGauche cransDroite\n" e.g. "-102 666\n"
- chaine de caracter "v consigneGauche consigneDroite\n" e.g. "v 254 232"
 -> Envoye les consignes de tension des moteurs gauches et droits (entiers entre -255 et 255)
*/
#define ORDRE_DEMANDE_CRANS  '?'
#define ORDRE_VITESSE        'v'

/*######################################################################
 *                          PINS UTILISEES
 *######################################################################
 */

//Numero de l'interruption utilisee sur front montant de la voie A de l encodeur gauche (0-> pin 2)
#define INTERRUPTION_VOIE_A_GAUCHE 0
#define PIN_VOIE_A_GAUCHE  2
//Pin a laquelle est reliee la voie B de l encodeur gauche (pour avoir le sens de rotation)
#define PIN_VOIE_B_GAUCHE  8

#define INTERRUPTION_VOIE_A_DROITE 1 //(interruption 1 -> pin 3 sur arduino Uno)
#define PIN_VOIE_A_DROITE 3
#define PIN_VOIE_B_DROITE 9

//Pin utilisees pour piloter les moteurs (les PWMs fonctionnent avec les pins 3, 5, 6, 9, 10, et 11)
#define PIN_MOTEUR_GAUCHE_SENS 4
#define PIN_MOTEUR_GAUCHE_PWM 5

#define PIN_MOTEUR_DROITE_SENS 7
#define PIN_MOTEUR_DROITE_PWM 6

/*######################################################################
 *                          VARIABLES GLOBALES
 *######################################################################
 */

//Nombre de crans decomptes a gauche et a droite respectivement (positif ou négatif)
//Ici j ai utilise des long (encodes sur 32 bits) pour ne pas avoir de depassements : avec 32 bits et 512=2^9 crans par tour => environ 2^22 i.e. 4000000 tours avant un overflow
volatile long cransGauche=0;
volatile long cransDroite=0;

/*######################################################################
 *                          SETUP 
 *######################################################################
 */
void setup() 
{
    Serial.begin(115200);
    
    //Une interruption sera lancee a chaque front montant sur les voies A des encodeurs
    attachInterrupt(INTERRUPTION_VOIE_A_GAUCHE, InterruptionGauche, CHANGE);
    attachInterrupt(INTERRUPTION_VOIE_A_DROITE, InterruptionDroit, CHANGE);
    
    //les voies B des encodeurs sont des entrees pour tester le sens de rotation
    pinMode(PIN_VOIE_A_GAUCHE, INPUT);
    pinMode(PIN_VOIE_B_GAUCHE, INPUT);
    pinMode(PIN_VOIE_A_DROITE, INPUT);
    pinMode(PIN_VOIE_B_DROITE, INPUT);
    
    pinMode(PIN_MOTEUR_DROITE_SENS, OUTPUT);
    pinMode(PIN_MOTEUR_GAUCHE_SENS, OUTPUT);
    pinMode(PIN_MOTEUR_DROITE_PWM, OUTPUT);
    pinMode(PIN_MOTEUR_GAUCHE_PWM, OUTPUT);
    
    /*Modification du prescaler du timer 0 à 1
    Ainsi les PWMs generes sur les pins 5 et 6 sont cadences a 62,5kHz
    au lieu de 490Hz par defaut. Cela evite un sifflement desaggreable (ultrason).
    Note: cela altere le timer 0 et donc la mesure du temps.
    Plus d'info:
    http://www.pobot.org/Modifier-la-frequence-d-un-PWM.html
    http://arduino.cc/en/Tutorial/SecretsOfArduinoPWM
    Voir page 134 de la datasheet de l'AVR
    */
    TCCR0B = TCCR0B & 0b11111000 | 0b01;  
    
    //Au demarrage, les moteurs sont hors tension
    MettreAJourTensionsMoteurs(0,0);
}
/*######################################################################
 *                          LOOP 
 *######################################################################
 */
void loop() 
{
  //La boucle consiste à traiter les potentiels messages
  
  //Si une consigne de vitesse a ete recue on charge les nouvelles consignes de vitesse, comme plusieurs messages peuvent s etre accumuler, on met ca dans une boucle
    if(Serial.available()>0)
    {
      char ordre=Serial.read();
      switch(ordre)
      {
        case ORDRE_VITESSE: 
              {
              //Lecture de la consigne de tension moteur gauche
              int tensionGauche=Serial.parseInt();
              //Lecture de la consigne de tension moteur droite              
              int tensionDroite=Serial.parseInt();
              //Prise en compte des consignes
              MettreAJourTensionsMoteurs(tensionGauche, tensionDroite);
              /*
              //Pour debugger la transmission des ordres
              Serial.print("ordre vitesse ");
              Serial.print(tensionGauche);
              Serial.print(" ");
              Serial.print(tensionDroite);
              Serial.print("\n");
              */
              }
              break;
        case ORDRE_DEMANDE_CRANS:
              Serial.print(cransGauche);
              Serial.print(" ");
              Serial.print(cransDroite);
              Serial.print("\n");
              break;
      }
    }
}

/*######################################################################
 *                          INTERRUPTIONS
 *######################################################################
 */

//Interruption appelee a chaque front montant sur la voie A de l encodeur gauche
void InterruptionGauche()
{
    if(digitalReadFast(PIN_VOIE_A_GAUCHE)==digitalReadFast(PIN_VOIE_B_GAUCHE))
      cransGauche++;
    else cransGauche--;
}

void InterruptionDroit()
{
    if(digitalReadFast(PIN_VOIE_A_DROITE)==digitalReadFast(PIN_VOIE_B_DROITE))
      cransDroite++;
    else cransDroite--;
}

/*######################################################################
 *                          FONCTIONS
 *######################################################################
 */

inline void MettreAJourTensionsMoteurs(int tensionGauche, int tensionDroite)
{   
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
        analogWrite(PIN_MOTEUR_DROITE_PWM, -tensionDroite);
      else analogWrite(PIN_MOTEUR_DROITE_PWM, 255);      
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
        analogWrite(PIN_MOTEUR_GAUCHE_PWM, -tensionGauche);
      else analogWrite(PIN_MOTEUR_GAUCHE_PWM, 255);      
    }
}
