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
#define PIN_VOIE_B_GAUCHE  4

#define INTERRUPTION_VOIE_A_DROITE 1 //(interruption 1 -> pin 3 sur arduino Uno)
#define PIN_VOIE_A_DROITE 3
#define PIN_VOIE_B_DROITE 5

//Pin utilisees pour piloter les moteurs (les PWMs fonctionnent avec les pins 3, 5, 6, 9, 10, et 11)
#define PIN_MOTEUR_GAUCHE_SENS 8
#define PIN_MOTEUR_GAUCHE_PWM 9
//#define REGISTRE_PWM_GAUCHE OCR1A
#define PIN_MOTEUR_DROITE_SENS 7
#define PIN_MOTEUR_DROITE_PWM 10
//#define REGISTRE_PWM_DR OCR1B

/*
Notes : 
Les PWMs generes par l'arduino sont cadencés à 490Hz, soit une période de 2ms, donc il faut au moins 20ms de periode d'asservissement.
a moins de generer des PWMs a plus haute frequence en bidouillant les registres du microcontroleurs via du code pour AVR.
*/

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
    
    //Au demarrage, les moteurs sont hors tension
    MettreAJourTensionsMoteurs(0,0);
    
    /*Par defaut, les PWMs generes par l'arduino sont a 490Hz.
    Pour augmenter la frequence, il faut modifier directement les registres du microcontroleur
    sans passer par le langage arduino.
    Voir page 134 de la datasheet de l'AVR et http://arduino.cc/en/Tutorial/SecretsOfArduinoPWM pour plus d info sur les PWMs
    
    Deux registres permettent de parametrer les PWMs du timer 1 (pin 9 et 10)
    TCCR1A=COM1A1 COM1A0 COM1B1 COM1B0 – – WGM11 WGM10
    TCCR1B=ICNC1 ICES1 – WGM13 WGM12 CS12 CS11 CS10
        
    WGM13 WGM12 WGM11 WGM10=0001 -> specifie un mode phase-correct PWM decoomptant sur 8 bits
    
    COM1A : Compare Output for Channel A, permet de specifier si comment la sortie OSC1A
    de l'AVR (pin 9 de l arduino http://arduino.cc/en/Hacking/PinMapping  ) doit se comporter.
    En mode phase-correct PWM,
    - COM1A1 COM1A0=10 ->Clear OC1A on Compare Match when upcounting. Set OC1A on Compare Match when downcounting. -> generer un PWM
    - COM1A1 COM1A0=11 ->comportement inverse : rapport cyclique oppose (e.g. 25% -> 75%)    
    CS : clock select, pour regler le prescaling -> la frequence des PWMs CS12 CS11 CS10=001 -> prescaling 1 -> environ 30kHz=16MHz/255/2/prescaling
    
    Les registres OCR1A et OCR1B permettent de specifier les rapport cycliques voulus
    (entre 0 et 255, sachant qu'il est possible de les inverser via COM1A et COM1B)
    */
    /*TCCR1A=B101000;
    TCCR1B=B00001001;*/
    
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
              //On ignore l'espace arrivant avant la valeur de tension gauche
              Serial.read();
              //Lecture de la consigne de tension moteur gauche
              int tensionGauche=SerialToInt();
              //Lecture de la consigne de tension moteur droite              
              int tensionDroite=SerialToInt();
              //Prise en compte des consignes
              MettreAJourTensionsMoteurs(tensionGauche, tensionDroite);
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

/*Lit la chaine de caractere dans le buffer du port serie 
et convertit ça en un entier, en s'arretant aux espaces 
et caracteres de fin de ligne '\n'*/
inline int SerialToInt()
{
  char temp;
  int resultat=0;
  boolean negatif=false;
  do
  {
      temp=Serial.read();
      if(temp=='-')
        negatif=true;
      else if(temp>='0' && temp <='9')
        resultat=resultat*10+(temp-'0');
      //Si il y a un autre caractere il faudrait trouver le moyen de generer une erreur
  } while(temp!='\n' && temp!=' ');
  if(negatif)
    return -resultat;
  else
    return resultat;
}

inline void MettreAJourTensionsMoteurs(int tensionGauche, int tensionDroite)
{   
    //Mise a jours des rapports cycliques et des sens de rotation, en seuillant a 255
    if(tensionDroite >= 0)
    {
      digitalWrite(PIN_MOTEUR_DROITE_SENS, HIGH);
      //if(tensionDroite<255)
        analogWrite(PIN_MOTEUR_DROITE_PWM, tensionDroite);
      //else analogWrite(PIN_MOTEUR_DROITE_PWM, 255);
    }
    else
    {
      digitalWrite(PIN_MOTEUR_DROITE_SENS, LOW);   
      //if(tensionDroite > -255)
        analogWrite(PIN_MOTEUR_DROITE_PWM, 255+tensionDroite);
      //else analogWrite(PIN_MOTEUR_DROITE_PWM, 0);      
    }
    
    if(tensionGauche >= 0)
    {
      digitalWrite(PIN_MOTEUR_GAUCHE_SENS, HIGH);
      //if(tensionGauche<255)
        analogWrite(PIN_MOTEUR_GAUCHE_PWM, tensionGauche);
      //else analogWrite(PIN_MOTEUR_GAUCHE_PWM, 255);
    }
    else
    {
      digitalWrite(PIN_MOTEUR_GAUCHE_SENS, LOW);   
      //if(tensionGauche > -255)
        analogWrite(PIN_MOTEUR_GAUCHE_PWM, 255+tensionGauche);
      //else analogWrite(PIN_MOTEUR_GAUCHE_PWM, 0);      
    }
}
