//----------------------------------------------------
// CONFIGURATION I2C
//----------------------------------------------------

#include <Wire.h>

#define ADRESSE_CARTE_MOTEUR 0x50
int position_pointer = 0x0D;

int isConnection;


// ---------------------------------------------------
void setup() {
  // initialisation liaison série
  Serial.begin(9600);
  // initialisation i2c
  Wire.begin();
  // initialise la carte moteur
  /*Wire.beginTransmission(ADRESSE_CARTE_MOTEUR); // connection a la carte moteur 
  Wire.send(position_pointer); // Adresse de départ
  Wire.send('a');
  Wire.endTransmission(); */
  Serial.println("i2c : begin");
  establishContact();
}

void establishContact()
{
  char contact = 0x00;
  while(contact != 0x7E)
  {
    while (Serial.available() <= 0) {   
      delay(300);
    }
    
    contact = Serial.read();
  }
}

// ------------------------------------------------------------------
void loop() {
  delay(500);

    if(Serial.available() > 0)
    {
      unsigned int receivedByte = Serial.read();
      
      if(receivedByte == 0x7F)
      {
        establishContact();
      }
      
      
      // ----------------------------------------------------------------
      // ----------------------------------------------------------------
      
      
      else if(receivedByte == 0x80)
      {
        int n=0;
        char buff[32];
        while(n<24)
        {
          if(Serial.available() > 0)
          {
            
            buff[n] = Serial.read();
            n++;
          }
        }
        
        Wire.beginTransmission(ADRESSE_CARTE_MOTEUR);
        Wire.send(0x14);
        Wire.send((uint8_t*)buff,24);
        Wire.endTransmission();
        
        Wire.beginTransmission(ADRESSE_CARTE_MOTEUR);
        Wire.send(0x00);
        Wire.send(0x90);
        Wire.endTransmission();
        
        char retour[9];
        for(int i=0; i<8; i++)
        {
          Wire.beginTransmission(ADRESSE_CARTE_MOTEUR);
          Wire.send(0x14+i);
          Wire.endTransmission();
          Wire.requestFrom(ADRESSE_CARTE_MOTEUR, 1);
          
          while(Wire.available())
          { 
            retour[i+1] = Wire.receive();
          }
        }
    
        
        //char retour[] = "aParamRob";
        retour[0] = 0x11;
        Serial.write((uint8_t*)retour, 9);
      }
      
      // ----------------------------------------------------------------
      // ----------------------------------------------------------------
      
      
      else if(receivedByte == 0x81)
      {
        int n=0;
        char buff[64];
        while(n<48)
        {
          if(Serial.available() > 0)
          {
            buff[n] = Serial.read();
            n++;
          }
        }
        Wire.beginTransmission(ADRESSE_CARTE_MOTEUR);
        Wire.send(0x14);
        Wire.send((uint8_t*)buff,24);
        Wire.endTransmission();
        
        Wire.beginTransmission(ADRESSE_CARTE_MOTEUR);
        Wire.send(0x2C);
        Wire.send((uint8_t*)&buff[24],24);
        Wire.endTransmission();
        
        Wire.beginTransmission(ADRESSE_CARTE_MOTEUR);
        Wire.send(0x00);
        Wire.send(0x91);
        Wire.endTransmission();
        
        //char retour[9];
        char retour[] = "aPidRobot";
        retour[0] = 0x09;
        Serial.write((uint8_t*)retour, 9);
        
        delay(1000);
        
        Wire.beginTransmission(ADRESSE_CARTE_MOTEUR);
        Wire.send(0x00);
        Wire.send(0x92);
        Wire.endTransmission();
        
        delay(100);

        for(int i=0; i<8; i++)
        {
          Wire.beginTransmission(ADRESSE_CARTE_MOTEUR);
          Wire.send(0x14+i);
          Wire.endTransmission();
          Wire.requestFrom(ADRESSE_CARTE_MOTEUR, 1);
          
          while(Wire.available())
          { 
            retour[i+1] = Wire.receive();
          }
        }
        retour[0] = 0x11;
        Serial.write((uint8_t*)retour, 9);
        
        for(int i=0; i<8; i++)
        {
          Wire.beginTransmission(ADRESSE_CARTE_MOTEUR);
          Wire.send(0x1C+i);
          Wire.endTransmission();
          Wire.requestFrom(ADRESSE_CARTE_MOTEUR, 1);
          
          while(Wire.available())
          { 
            retour[i+1] = Wire.receive();
          }
        }
        retour[0] = 0x11;
        Serial.write((uint8_t*)retour, 9);
        
        for(int i=0; i<8; i++)
        {
          Wire.beginTransmission(ADRESSE_CARTE_MOTEUR);
          Wire.send(0x24+i);
          Wire.endTransmission();
          Wire.requestFrom(ADRESSE_CARTE_MOTEUR, 1);
          
          while(Wire.available())
          { 
            retour[i+1] = Wire.receive();
          }
        }
        retour[0] = 0x11;
        Serial.write((uint8_t*)retour, 9);
        
        
        
        
        
        
        
        for(int i=0; i<8; i++)
        {
          Wire.beginTransmission(ADRESSE_CARTE_MOTEUR);
          Wire.send(0x2C+i);
          Wire.endTransmission();
          Wire.requestFrom(ADRESSE_CARTE_MOTEUR, 1);
          
          while(Wire.available())
          { 
            retour[i+1] = Wire.receive();
          }
        }
        retour[0] = 0x11;
        Serial.write((uint8_t*)retour, 9);
        for(int i=0; i<8; i++)
        {
          Wire.beginTransmission(ADRESSE_CARTE_MOTEUR);
          Wire.send(0x34+i);
          Wire.endTransmission();
          Wire.requestFrom(ADRESSE_CARTE_MOTEUR, 1);
          
          while(Wire.available())
          { 
            retour[i+1] = Wire.receive();
          }
        }
        retour[0] = 0x11;
        Serial.write((uint8_t*)retour, 9);
        for(int i=0; i<8; i++)
        {
          Wire.beginTransmission(ADRESSE_CARTE_MOTEUR);
          Wire.send(0x3C+i);
          Wire.endTransmission();
          Wire.requestFrom(ADRESSE_CARTE_MOTEUR, 1);
          
          while(Wire.available())
          { 
            retour[i+1] = Wire.receive();
          }
        }
        retour[0] = 0x11;
        Serial.write((uint8_t*)retour, 9);
      }
      
      // ----------------------------------------------------------------
      // ----------------------------------------------------------------
      
      
      else if(receivedByte == 0x01)
      {
        int n=0;
        char buff[8];
        while(n<4)
        {
          if(Serial.available() > 0)
          {
            buff[n] = Serial.read();
            n++;
          }
        }
        
        Wire.beginTransmission(ADRESSE_CARTE_MOTEUR);
        Wire.send(0x14);
        Wire.send((uint8_t*)buff,4);
        Wire.endTransmission();
        
        Wire.beginTransmission(ADRESSE_CARTE_MOTEUR);
        Wire.send(0x00);
        Wire.send(0x81);
        Wire.endTransmission();
        
        delay(100);
        char retour[9];
        for(int i=0; i<8; i++)
        {
          Wire.beginTransmission(ADRESSE_CARTE_MOTEUR);
          Wire.send(0x14+i);
          Wire.endTransmission();
          Wire.requestFrom(ADRESSE_CARTE_MOTEUR, 1);
          
          while(Wire.available())
          { 
            retour[i+1] = Wire.receive();
          }
        }
        //char retour[] = "aAvancerR";
        retour[0] = 0x11;
        Serial.write((uint8_t*)retour, 9);
      }
      
       
      // ----------------------------------------------------------------
      // ----------------------------------------------------------------
      
      
      else if(receivedByte == 0x02)
      {
        int n=0;
        char buff[8];
        while(n<4)
        {
          if(Serial.available() > 0)
          {
            buff[n] = Serial.read();
            n++;
          }
        }
        
        Wire.beginTransmission(ADRESSE_CARTE_MOTEUR);
        Wire.send(0x14);
        Wire.send((uint8_t*)buff,4);
        Wire.endTransmission();
        
        Wire.beginTransmission(ADRESSE_CARTE_MOTEUR);
        Wire.send(0x00);
        Wire.send(0x82);
        Wire.endTransmission();
 
        char retour[] = "aTournerR";
        retour[0] = 0x09;
        Serial.write((uint8_t*)retour, 9);
      }
      
       
      // ----------------------------------------------------------------
      // ----------------------------------------------------------------
      
      
      else if(receivedByte == 0x03)
      {
        int n=0;
        char buff[16];
        while(n<8)
        {
          if(Serial.available() > 0)
          {
            buff[n] = Serial.read();
            n++;
          }
        }
        
        Wire.beginTransmission(ADRESSE_CARTE_MOTEUR);
        Wire.send(0x14);
        Wire.send((uint8_t*)buff,8);
        Wire.endTransmission();
        
        Wire.beginTransmission(ADRESSE_CARTE_MOTEUR);
        Wire.send(0x00);
        Wire.send(0x83);
        Wire.endTransmission();
        
        char retour[] = "aAllerXYR";
        retour[0] = 0x09;
        Serial.write((uint8_t*)retour, 9);
      }
      
       
      // ----------------------------------------------------------------
      // ----------------------------------------------------------------
      
      
      else
      {
        while(Serial.available() > 0)
        {
          Serial.read();
          delay(5);
        }
        char retour[] = "aaErreur!";
        retour[0] = 0xFF;
        retour[1] = receivedByte;
        Serial.write((uint8_t*)retour, 9);
      }
    }
    
    
    {
      char buff[9];
   
      for(int i=0; i<4; i++)
      {
        Wire.beginTransmission(ADRESSE_CARTE_MOTEUR);
        Wire.send(0x08+i);
        Wire.endTransmission();
        Wire.requestFrom(ADRESSE_CARTE_MOTEUR, 1);
        
        while(Wire.available())
        { 
          buff[i+1] = Wire.receive();
        }
      }
      
      for(int i=0; i<4; i++)
      {
        Wire.beginTransmission(ADRESSE_CARTE_MOTEUR);
        Wire.send(0x0C+i);
        Wire.endTransmission();
        Wire.requestFrom(ADRESSE_CARTE_MOTEUR, 1);
        
        while(Wire.available())
        { 
          buff[i+5] = Wire.receive();
        }
      }
      buff[0] = 0x10;
      Serial.write((uint8_t*)buff, 9);
    }
}

