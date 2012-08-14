const int sensMoteurd=5; //  sens du moteur droit
const int vitesseMoteurd=2; //  vitesse du moteur droit
const int tension= A0;      // port analogique commande par fil
const int ultrason_av=10;
const int ultrason_ar=12;



const int sensMoteurg=9;    // sens du moteur gauche
const int vitesseMoteurg=7;  // vitesse du moteur gauche

const int AVANT=1; // gestion sens du moteur
const int ARRIERE=0; // gestion sens du  moteur


int vitesse=0;; // variable de vitesse
int valstart=0;      // Valeur analogique pour la commande par fil 
int distance =0;
signed int i=60;         // contole vitesse prendre entre 0 et 92
long duree =0;

void setup()    // debut de la fonction setup()
{ 
  pinMode (sensMoteurd,OUTPUT); // Broche sensMoteurd configurée en sortie
  pinMode (vitesseMoteurd,OUTPUT); // Broche vitesseMoteurd configurée en sortie
  
  pinMode (sensMoteurg,OUTPUT); // Broche sensMoteurg configurée en sortie
  pinMode (vitesseMoteurg,OUTPUT); // Broche vitesseMoteurg configurée en sortie

  Serial.begin(9600);
}

void loop() // debut de la fonction loop()
{
  
  
  valstart=analogRead(tension);
  if(valstart==0)
  {
    vitesse=125;  // vitesse de base
    vitesse =vitesse + i; // vitesse roue droite
    analogWrite(vitesseMoteurd, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurd,AVANT); // Marche avant

    vitesse = 127;    // vitesse de base
    vitesse=vitesse-i;  // vitesse roue gauche
    analogWrite(vitesseMoteurg, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurg,ARRIERE); // Marche avant 
    
    delay(1200);
    
    // -----------------------------------------------------------------------------------------------------
 //Rotation avant droite pour avancer
//---------------------------------------------------------------- 
     vitesse=127;  // vitesse de base
    vitesse =vitesse + i; // vitesse roue droite
    analogWrite(vitesseMoteurd, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurd,AVANT); // Marche avant

    //vitesse = 127;    // vitesse de base
    //vitesse=vitesse-i;  // vitesse roue gauche
    analogWrite(vitesseMoteurg, 0); // génère une impulsion PWM sur la broche de vitesse du moteur
    //digitalWrite(sensMoteurg,ARRIERE); // Marche avant 
    
    delay(2800);
    
       
  // ----------------------------------------------------------------------------------------------------
//   Marche avant  pour continuer
// ----------------------------------------------------------------------------------------------------
  
    
    vitesse=127;  // vitesse de base
    vitesse =vitesse + i; // vitesse roue droite
    analogWrite(vitesseMoteurd, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurd,AVANT); // Marche avant

    vitesse = 128;    // vitesse de base
    vitesse=vitesse-i;  // vitesse roue gauche
    analogWrite(vitesseMoteurg, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurg,ARRIERE); // Marche avant 
    
    delay(1500);   

   // -----------------------------------------------------------------------------------------------------
 //Rotation droite pour pousser le lingot
//---------------------------------------------------------------- 
     vitesse=127;  // vitesse de base
    vitesse =vitesse + i; // vitesse roue droite
    analogWrite(vitesseMoteurd, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurd,AVANT); // Marche avant

    //vitesse = 127;    // vitesse de base
    //vitesse=vitesse-i;  // vitesse roue gauche
    analogWrite(vitesseMoteurg, 0); // génère une impulsion PWM sur la broche de vitesse du moteur
    //digitalWrite(sensMoteurg,ARRIERE); // Marche avant 
    
    delay(2800);
   
   // ----------------------------------------------------------------------------------------------------
//   Marche avant pour pousser le lingot
// ----------------------------------------------------------------------------------------------------
 
   
    vitesse=127;  // vitesse de base
    vitesse =vitesse + i; // vitesse roue droite
    analogWrite(vitesseMoteurd, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurd,AVANT); // Marche avant

    vitesse = 127;    // vitesse de base
    vitesse=vitesse-i;  // vitesse roue gauche
    analogWrite(vitesseMoteurg, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurg,ARRIERE); // Marche avant 
    
  delay(1500);
  
  //--------------------------------------------------------------
  //Marche arrière
  //----------------------------------------------------------
  vitesse=127;  // vitesse de base
    vitesse =vitesse + i; // vitesse roue droite
    analogWrite(vitesseMoteurg, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurg,AVANT); // Marche avant

    vitesse = 127;    // vitesse de base
    vitesse=vitesse-i;  // vitesse roue gauche
    analogWrite(vitesseMoteurd, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurd,ARRIERE); // Marche avant 
    
  delay(1200);
  
  //------------------------------------------------------------
 // rotation  arrière quart de tour
 //-----------------------------------------------------------
    
    //vitesse=127;  // vitesse de base
    //vitesse =vitesse + i; // vitesse roue droite
    analogWrite(vitesseMoteurg, 255); // génère une impulsion PWM sur la broche de vitesse du moteur
    //digitalWrite(sensMoteurg,AVANT); // Marche avant

    vitesse = 127;    // vitesse de base
     vitesse=vitesse-i;  // vitesse roue gauche
    analogWrite(vitesseMoteurd, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurd,ARRIERE); // Marche avant 
    
    delay(2200);
    
 //-------------------------------------------------------------
 //Marche avant pour aller vers la 1er bouteille
 //-------------------------------------------------------------
     
     vitesse=125;  // vitesse de base
    vitesse =vitesse + i; // vitesse roue droite
    analogWrite(vitesseMoteurd, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurd,AVANT); // Marche avant

    vitesse = 127;    // vitesse de base
    vitesse=vitesse-i;  // vitesse roue gauche
    analogWrite(vitesseMoteurg, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurg,ARRIERE); // Marche avant 
    delay(15000);
    
    /*
    
     //---------------------------------------------------------------
//marche arrière 
//------------------------------------------------------------
  
    
    vitesse=127;  // vitesse de base
    vitesse =vitesse + i; // vitesse roue droite
    analogWrite(vitesseMoteurg, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurg,AVANT); // Marche avant

    vitesse = 127;    // vitesse de base
    vitesse=vitesse-i;  // vitesse roue gauche
    analogWrite(vitesseMoteurd, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurd,ARRIERE); // Marche avant 
    
    
    delay(2000);
    
    //------------------------------------------------------------
 // rotation  arrière quart de tour
 //-----------------------------------------------------------
    
    //vitesse=127;  // vitesse de base
    //vitesse =vitesse + i; // vitesse roue droite
    analogWrite(vitesseMoteurg, 255); // génère une impulsion PWM sur la broche de vitesse du moteur
    //digitalWrite(sensMoteurg,AVANT); // Marche avant

    vitesse = 127;    // vitesse de base
     vitesse=vitesse-i;  // vitesse roue gauche
    analogWrite(vitesseMoteurd, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurd,ARRIERE); // Marche avant 
    
    delay(2200);

  // ----------------------------------------------------------------------------------------------------
//   Marche avant pour aller vers la 2eme bouteille
// ----------------------------------------------------------------------------------------------------

    
    
  
   
 long  k=0;
  while(k<=1200)
{ 
     
    vitesse=127;  // vitesse de base
    vitesse =vitesse + i; // vitesse roue droite
    analogWrite(vitesseMoteurd, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurd,AVANT); // Marche avant

    vitesse = 128;    // vitesse de base
    vitesse=vitesse-i;  // vitesse roue gauche
    analogWrite(vitesseMoteurg, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurg,ARRIERE); // Marche avant 
    
  
      pinMode(ultrason_av, OUTPUT);
  digitalWrite(ultrason_av, LOW);
  delayMicroseconds(2);
  digitalWrite(ultrason_av, HIGH);
  delayMicroseconds(5);
  digitalWrite(ultrason_av, LOW);
  pinMode(ultrason_av, INPUT);
  duree = pulseIn(ultrason_av, HIGH);

  distance = duree/29/2;
  Serial.print(distance);
      
      if(distance<10)
   {
   vitesse =0; 
   analogWrite(vitesseMoteurg, vitesse);
   vitesse = 255;
   analogWrite(vitesseMoteurd, vitesse);
   delay(1000);
   }
   k++;
} 

delay(1000);
//------------------------------------------------------------
 // rotation  avant quart de tour
 //-----------------------------------------------------------
    
    //vitesse=127;  // vitesse de base
    //vitesse =vitesse + i; // vitesse roue droite
    analogWrite(vitesseMoteurd, 255); // génère une impulsion PWM sur la broche de vitesse du moteur
    //digitalWrite(sensMoteurd,AVANT); // Marche avant

    vitesse = 127;    // vitesse de base
    vitesse=vitesse-i;  // vitesse roue gauche
    analogWrite(vitesseMoteurg, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurg,ARRIERE); // Marche avant 
    
    delay(2200);

  // ----------------------------------------------------------------------------------------------------
//   Marche arrière  pour pousser la 2eme bouteille
// ----------------------------------------------------------------------------------------------------
     
    vitesse=127;  // vitesse de base
    vitesse =vitesse + i; // vitesse roue droite
    analogWrite(vitesseMoteurg, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurg,AVANT); // Marche avant

    vitesse = 128;    // vitesse de base
    vitesse=vitesse-i;  // vitesse roue gauche
    analogWrite(vitesseMoteurd, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurd,ARRIERE); // Marche avant 
    
    delay(3000);
    
    //-------------------------------------------------
  //Marche avant après la 2ème btlle
  //--------------------------------------------

    k=0;
  while(k<=400)
{ 
     
    vitesse=127;  // vitesse de base
    vitesse =vitesse + i; // vitesse roue droite
    analogWrite(vitesseMoteurd, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurd,AVANT); // Marche avant

    vitesse = 128;    // vitesse de base
    vitesse=vitesse-i;  // vitesse roue gauche
    analogWrite(vitesseMoteurg, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurg,ARRIERE); // Marche avant 
    
  
      pinMode(ultrason_av, OUTPUT);
  digitalWrite(ultrason_av, LOW);
  delayMicroseconds(2);
  digitalWrite(ultrason_av, HIGH);
  delayMicroseconds(5);
  digitalWrite(ultrason_av, LOW);
  pinMode(ultrason_av, INPUT);
  duree = pulseIn(ultrason_av, HIGH);

  distance = duree/29/2;
  Serial.print(distance);
      
      if(distance<10)
   {
   vitesse =0; 
   analogWrite(vitesseMoteurg, vitesse);
   vitesse = 255;
   analogWrite(vitesseMoteurd, vitesse);
   delay(1000);
   }
  
}    

  // -----------------------------------------------------------------------------------------------------
 //Rotation quart de tour avant gauche
//---------------------------------------------------------------- 
     //vitesse=127;  // vitesse de base
    //vitesse =vitesse + i; // vitesse roue droite
    analogWrite(vitesseMoteurd, 255); // génère une impulsion PWM sur la broche de vitesse du moteur
    //digitalWrite(sensMoteurd,AVANT); // Marche avant

    vitesse = 127;    // vitesse de base
    vitesse=vitesse-i;  // vitesse roue gauche
    analogWrite(vitesseMoteurg, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurg,ARRIERE); // Marche avant 
    
    delay(2200);
    
    
        
     // ----------------------------------------------------------------------------------------------------
//   Marche avant 
// ----------------------------------------------------------------------------------------------------
  
   k=0;
  while(k<=1500)
{ 
     
    vitesse=127;  // vitesse de base
    vitesse =vitesse + i; // vitesse roue droite
    analogWrite(vitesseMoteurd, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurd,AVANT); // Marche avant

    vitesse = 127;    // vitesse de base
    vitesse=vitesse-i;  // vitesse roue gauche
    analogWrite(vitesseMoteurg, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurg,ARRIERE); // Marche avant 
    
  
      pinMode(ultrason_av, OUTPUT);
  digitalWrite(ultrason_av, LOW);
  delayMicroseconds(2);
  digitalWrite(ultrason_av, HIGH);
  delayMicroseconds(5);
  digitalWrite(ultrason_av, LOW);
  pinMode(ultrason_av, INPUT);
  duree = pulseIn(ultrason_av, HIGH);

  distance = duree/29/2;
  Serial.print(distance);
      
      if(distance<10)
   {
   vitesse =0; 
   analogWrite(vitesseMoteurg, vitesse);
   vitesse = 255;
   analogWrite(vitesseMoteurd, vitesse);
   delay(1000);
   }
   k++;
}
 
//------------------------------------------------------------
 // rotation  à droite  pour continuer
 //-----------------------------------------------------------
    
    
    
    vitesse=127;  // vitesse de base
    vitesse =vitesse + i; // vitesse roue droite
    analogWrite(vitesseMoteurd, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurd,AVANT); // Marche avant

    //vitesse = 127;    // vitesse de base
    //vitesse=vitesse-i;  // vitesse roue gauche
    analogWrite(vitesseMoteurg, 0); // génère une impulsion PWM sur la broche de vitesse du moteur
    //digitalWrite(sensMoteurg,ARRIERE); // Marche avant 
    
    delay(3200); 
    
     // ----------------------------------------------------------------------------------------------------
//   Marche avant 
// ----------------------------------------------------------------------------------------------------
  
   k=0;
  while(k<=500)
{ 
     
    vitesse=127;  // vitesse de base
    vitesse =vitesse + i; // vitesse roue droite
    analogWrite(vitesseMoteurd, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurd,AVANT); // Marche avant

    vitesse = 127;    // vitesse de base
    vitesse=vitesse-i;  // vitesse roue gauche
    analogWrite(vitesseMoteurg, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurg,ARRIERE); // Marche avant 
    
  
      pinMode(ultrason_av, OUTPUT);
  digitalWrite(ultrason_av, LOW);
  delayMicroseconds(2);
  digitalWrite(ultrason_av, HIGH);
  delayMicroseconds(5);
  digitalWrite(ultrason_av, LOW);
  pinMode(ultrason_av, INPUT);
  duree = pulseIn(ultrason_av, HIGH);

  distance = duree/29/2;
  Serial.print(distance);
      
      if(distance<10)
   {
   vitesse =0; 
   analogWrite(vitesseMoteurg, vitesse);
   vitesse = 255;
   analogWrite(vitesseMoteurd, vitesse);
   delay(1000);
   }
   k++;
}
    
    //------------------------------------------------------------
 // rotation  à gauche  pour terminer
 //-----------------------------------------------------------
    
    //vitesse=127;  // vitesse de base
    //vitesse =vitesse + i; // vitesse roue droite
    analogWrite(vitesseMoteurd, 255); // génère une impulsion PWM sur la broche de vitesse du moteur
    //digitalWrite(sensMoteurd,AVANT); // Marche avant

    vitesse = 127;    // vitesse de base
     vitesse=vitesse-i;  // vitesse roue gauche
    analogWrite(vitesseMoteurg, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurg,ARRIERE); // Marche avant 
    
    delay(3200); 
    
     // ----------------------------------------------------------------------------------------------------
//   Marche avant  pour terminer
// ----------------------------------------------------------------------------------------------------
  
   k=0;
  while(k<=500)
{ 
     
    vitesse=127;  // vitesse de base
    vitesse =vitesse + i; // vitesse roue droite
    analogWrite(vitesseMoteurd, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurd,AVANT); // Marche avant

    vitesse = 127;    // vitesse de base
    vitesse=vitesse-i;  // vitesse roue gauche
    analogWrite(vitesseMoteurg, vitesse); // génère une impulsion PWM sur la broche de vitesse du moteur
    digitalWrite(sensMoteurg,ARRIERE); // Marche avant 
    
  
      pinMode(ultrason_av, OUTPUT);
  digitalWrite(ultrason_av, LOW);
  delayMicroseconds(2);
  digitalWrite(ultrason_av, HIGH);
  delayMicroseconds(5);
  digitalWrite(ultrason_av, LOW);
  pinMode(ultrason_av, INPUT);
  duree = pulseIn(ultrason_av, HIGH);

  distance = duree/29/2;
  Serial.print(distance);
      
      if(distance<10)
   {
   vitesse =0; 
   analogWrite(vitesseMoteurg, vitesse);
   vitesse = 255;
   analogWrite(vitesseMoteurd, vitesse);
   delay(1000);
   }
   k++;
}
*/
   vitesse =0; 
   analogWrite(vitesseMoteurg, vitesse);
   vitesse = 255;
   analogWrite(vitesseMoteurd, vitesse);
   delay(100000);
 }
}

    
  
  
  
  
  
  
    
