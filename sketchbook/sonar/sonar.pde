// Sweep
// by BARRAGAN <http://barraganstudio.com> 
// This example code is in the public domain.


#include <Servo.h> 
 
Servo myservo;  // create servo object to control a servo 
                // a maximum of eight servo objects can be created 
 
int pos = 0;    // variable to store the servo position 
const int pingPin = 7; //Pin sur laquelle est branchée le télémetre US
 
void setup() 
{ 
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object 
  pinMode(13, OUTPUT);
   Serial.begin(9600); 
} 

long detecter_distance(); 
 
void loop() 
{ 
  long cm, duration; //distance mesuree
  int angle=60; //angle de balayage
  int pas=1;
  int angle_min=90-angle/2;
  int angle_max=90+angle/2;
  int pos;
  
  for(pos = angle_min; pos < angle_max; pos += pas)  
  {          

    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
    cm=detecter_distance();
    SendData(pos,cm);

  } 
  digitalWrite(13, HIGH);    
  for(pos = angle_max; pos > angle_min; pos -= pas)     // goes from 180 degrees to 0 degrees 
  {                     

    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
        cm=detecter_distance();
	SendData(pos,cm);

  } 
} 

void SendData(int pos, long cm)
{
	Serial.print(pos);
        Serial.print(" ");
        Serial.print(cm);
        Serial.print("\n");
}

long detecter_distance()
{
  long duration;
   pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(pingPin, LOW);
  
    pinMode(pingPin, INPUT);
  duration = pulseIn(pingPin, HIGH);
  return duration/58;
}
