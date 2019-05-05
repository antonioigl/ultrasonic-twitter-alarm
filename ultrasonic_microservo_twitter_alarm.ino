
#include <SPI.h>
#include <Ethernet.h> 
#include <Twitter.h>
#include <Servo.h>

// micro servo
Servo ms;
byte servoP = 2;
int pos = 0;

// ultrasonic
byte trigP = 7;
byte echoP = 6;
long distcm = 0;

// buzzer
byte buzzerP = 5;

// led
byte redP = 9; 
byte greenP = 8;

//Ethernet Shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 192, 168, 0, 250 };

// Twitter 
Twitter twitter("<<<  token  >>>");
boolean sendTweet = false;

void setup()
{
  Serial.begin(9600);
  Ethernet.begin(mac, ip);
    
  ms.attach(servoP);

  pinMode(trigP, OUTPUT);
  pinMode(echoP, INPUT);
  
  pinMode(buzzerP, OUTPUT);

  pinMode(redP, OUTPUT);
  pinMode(greenP, OUTPUT);
  
}

void loop()
{
  for (pos = 0; pos <= 180; pos += 1)
  {
    ms.write(pos);
    delay(50);
    checkDist();
  }
  
  sendTweet = false;
  
  for (pos = 180; pos >= 0; pos -= 1)
  {
    ms.write(pos);
    delay(50);
    checkDist();  
  }

  sendTweet = false;
}

void checkDist(void){

   long distRadar = radar();
   
   if(distRadar <= 30){
    tone(buzzerP, 440, 300);
    digitalWrite(redP, HIGH);
    digitalWrite(greenP, LOW);
    if(!sendTweet){
      sendTweet = tweet(distRadar);
    }
  }
  else {  
    noTone(buzzerP);
    digitalWrite(redP, LOW);
    digitalWrite(greenP, HIGH);
  }
  
  Serial.println(distRadar); 

}

long radar(void)
{
  digitalWrite(trigP, LOW);
  delayMicroseconds(5);
  digitalWrite(trigP, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigP, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  long dur = pulseIn(echoP, HIGH);

  // Convert the time into a distance (cm)
  distcm = (dur/2)/29.1;
  
  return distcm;
}

boolean tweet(long dist){

  boolean success = false;
  
  Serial.println("Establishing connection Twitter ...");
  
  char buffer[80];
  sprintf(buffer, "Intruder approaching: %d cm", dist);
  
  // Publish tweet. Return true or false.
  if (twitter.post(buffer)) { 
    int status = twitter.wait(&Serial);
    if (status == 200) { // Connection success
      success = true;
      Serial.println("OK.");
    } 
    else { // Connection error
      Serial.print("Error : code ");
      Serial.println(status);
    }
  } 
  else {
    Serial.println("Failed connection.");
  }
  return success;
}
