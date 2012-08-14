

#include <Servo.h> 

Servo myservo;        // servomoteur avant

const int sensMoteurd=4; //  sens du moteur droit
const int vitesseMoteurd=5; //  vitesse du moteur droit
const int tension= A0;      // port analogique commande par fil
const int ultrason=8;



const int sensMoteurg=2;    // sens du moteur gauche
const int vitesseMoteurg=3;  // vitesse du moteur gauche

const int AVANT=1; // gestion sens du moteur
const int ARRIERE=0; // gestion sens du  moteur


int pos;
int vitesse=0;; // variable de vitesse
int valstart=0;      // Valeur analogique pour la commande par fil 
int distance =0;
int a=2;
int x=0;
int y=0;
signed int i=75;         // contole vitesse prendre entre 0 et 92

void setup()    // debut de la fonction setup()
{ 
  
  
  pinMode (sensMoteurd,OUTPUT); // Broche sensMoteurd configurée en sortie
  pinMode (vitesseMoteurd,OUTPUT); // Broche vitesseMoteurd configurée en sortie
  
  pinMode (sensMoteurg,OUTPUT); // Broche sensMoteurg configurée en sortie
  pinMode (vitesseMoteurg,OUTPUT); // Broche vitesseMoteurg configurée en sortie
  myservo.attach(10);   // Broche servomoteur en sortie
  Serial.begin(9600);
}

void loop() // debut de la fonction loop()
{
  
  
  valstart=analogRead(tension);
  if(valstart==0)
  {
    
    long duree;
  pinMode(ultrason, OUTPUT);
  digitalWrite(ultrason, LOW);
  delayMicroseconds(2);
  digitalWrite(ultrason, HIGH);
  delayMicroseconds(5);
  digitalWrite(ultrason, LOW);
  pinMode(ultrason, INPUT);
  duree = pulseIn(ultrason, HIGH);

  distance = duree/29/2;
  Serial.print(distance);

  
// ----------------------------------------------------------------------------------------------------
//   Marche avant 
// ----------------------------------------------------------------------------------------------------
    
    vitesse=127;  // vitesse de base
    vitesse =vitesse + i; // vitesse roue droite
    analogWrite(vitesseMoteurd, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurd,AVANT); // Marche avant

    vitesse = 127;    // vitesse de base
    vitesse=vitesse-i;  // vitesse roue gauche
    analogWrite(vitesseMoteurg, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurg,ARRIERE); // Marche avant 
    
    delay(2000);
// -----------------------------------------------------------------------------------------------------
    
    
    if(distance<0) 
    //if(a==2)
     goto x;
 //------------------------------------------------------------------------------------------------------
  //  Rotation à gauche
 //-------------------------------------------------------------------------------------------------------
    y:
    analogWrite(vitesseMoteurd, 255); // moteur droit fixe

    vitesse = 127;    // vitesse de base
    vitesse=vitesse-i;  // vitesse roue gauche
    analogWrite(vitesseMoteurg, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurg,ARRIERE); // Marche avant
    delay(3200);
//-----------------------------------------------------------------------------------------------------------
//Marche arrière
//----------------------------------------------------------------------------------------------------------
    vitesse=127;  // vitesse de base
    vitesse =vitesse + i; // vitesse roue droite
    analogWrite(vitesseMoteurg, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurg,AVANT); // Marche avant

    vitesse = 128;    // vitesse de base
    vitesse=vitesse-i;  // vitesse roue gauche
    analogWrite(vitesseMoteurd, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurd,ARRIERE); // Marche avant 
  
    delay(15000);
//------------------------------------------------------------------------------------------------------------
//Marche avant
//------------------------------------------------------------------------------------------------------------
    vitesse=127;  // vitesse de base
    vitesse =vitesse + i; // vitesse roue droite
    analogWrite(vitesseMoteurd, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurd,AVANT); // Marche avant

    vitesse = 127;    // vitesse de base
    vitesse=vitesse-i;  // vitesse roue gauche
    analogWrite(vitesseMoteurg, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurg,ARRIERE); // Marche avant 
    
    delay(6000);
    
//--------------------------------------------------------------------------------------------------------------
//Rotation à droite 
//--------------------------------------------------------------------------------------------------------------
    //vitesse=128;  // vitesse de base
    //vitesse =vitesse + i; // vitesse roue droite
    analogWrite(vitesseMoteurd, 255); // génère une impulsion PWM sur la broche de vitesse du moteur
    //digitalWrite(sensMoteurd,AVANT); // Marche avant

    vitesse = 127;    // vitesse de base
    vitesse=vitesse-i;  // vitesse roue gauche
    analogWrite(vitesseMoteurg, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurg,ARRIERE); // Marche avant 
    
    delay(3200);
    
//__________________________________________________________
// Marche avant pour pousser le lingot
//--------------------------------------------------------------
    vitesse=127;  // vitesse de base
    vitesse =vitesse + i; // vitesse roue droite
    analogWrite(vitesseMoteurd, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurd,AVANT); // Marche avant

    vitesse = 127;    // vitesse de base
    vitesse=vitesse-i;  // vitesse roue gauche
    analogWrite(vitesseMoteurg, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurg,ARRIERE); // Marche avant 
    
    delay(1000);
    
//-------------------------------------------------------------------
//Marche  arrière pour continuer
//-----------------------------------------------------------------
    vitesse=127;  // vitesse de base
    vitesse =vitesse + i; // vitesse roue droite
    analogWrite(vitesseMoteurg, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurg,AVANT); // Marche avant

    vitesse = 127;    // vitesse de base
    vitesse=vitesse-i;  // vitesse roue gauche
    analogWrite(vitesseMoteurd, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurd,ARRIERE); // Marche avant 
  
    delay(1500);
    
//-----------------------------------------------------------
//Rotation pour continuer
//-----------------------------------------------------------
    vitesse=127;  // vitesse de base
    vitesse =vitesse + i; // vitesse roue droite
    analogWrite(vitesseMoteurg, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurg,AVANT); // Marche avant

    //vitesse = 127;    // vitesse de base
    //vitesse=vitesse-i;  // vitesse roue gauche
    analogWrite(vitesseMoteurd, 20); // génère une impulsion PWM sur la broche de vitesse du moteur
    //digitalWrite(sensMoteurd,ARRIERE); // Marche avant 
  
    delay(10000);
    
 //------------------------------------------------------------
 //Marche avant
 //------------------------------------------------------------
   vitesse=128;  // vitesse de base
    vitesse =vitesse + i; // vitesse roue droite
    analogWrite(vitesseMoteurd, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurd,AVANT); // Marche avant

    vitesse = 127;    // vitesse de base
    vitesse=vitesse-i;  // vitesse roue gauche
    analogWrite(vitesseMoteurg, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurg,ARRIERE); // Marche avant 
    
    delay(4000);
    
    
    
   vitesse =255; 
   analogWrite(vitesseMoteurd, vitesse);
   vitesse = 0;
   analogWrite(vitesseMoteurg, vitesse);
   delay(1000000);
   

   x:
   if(a==2)
   {
   vitesse =0; 
   analogWrite(vitesseMoteurg, vitesse);
   vitesse = 255;
   analogWrite(vitesseMoteurd, vitesse);
   delay(5000);
    
   pinMode(ultrason, OUTPUT);
  digitalWrite(ultrason, LOW);
  delayMicroseconds(2);
  digitalWrite(ultrason, HIGH);
  delayMicroseconds(5);
  digitalWrite(ultrason, LOW);
  pinMode(ultrason, INPUT);
  duree = pulseIn(ultrason, HIGH);

  distance = duree/29/2;
  Serial.print(distance);
  if(distance >5)
  goto y;
  
   else
  goto x; 
   }
  }
    
   
} // fin de la fonction loop() - le programme recommence au début de la fonction loop sans fin




