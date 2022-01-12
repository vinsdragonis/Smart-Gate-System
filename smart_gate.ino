/*
Ultrasound, Ardiuno, RGB LED, RFID, Servo Motor x2, 
*/
#include<SoftwareSerial.h>  
#include<Servo.h>
#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN 3
#define RST_PIN 2
Servo dispense, door;
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.


const int trigPin = 6, echoPin = 7;
int redLed = 8, greenLed = 9, blueLed = 5;

String stored ="C3 A4 CD 0A";
int no = 3;    
boolean flag = true;     
void setup()
{
  Serial.begin(9600);  
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();
  pinMode(trigPin,OUTPUT);
  pinMode(echoPin,INPUT); 
  pinMode(redLed,OUTPUT); 
  pinMode(greenLed,OUTPUT); 
  pinMode(blueLed,OUTPUT);
  digitalWrite(redLed,LOW);
  digitalWrite(greenLed,LOW);
  digitalWrite(blueLed,LOW); 
  dispense.attach(4);
  door.attach(5); 
}
void loop()
{
  String id;
  boolean access = false;
  //reset the leds
   digitalWrite(redLed,LOW);
   digitalWrite(greenLed,LOW);
   digitalWrite(blueLed,LOW);
  //To check if the tag owner belongs to institution
 
  if (flag){
    // Look for new cards
    if ( ! mfrc522.PICC_IsNewCardPresent()) {
       return;
     }
     // Select one of the cards
     if ( ! mfrc522.PICC_ReadCardSerial()){
        return;
     }
      //Show UID on serial monitor
      Serial.print("UID tag :");
      String content= "";
      byte letter;
      for (byte i = 0; i < mfrc522.uid.size; i++){
         Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
         Serial.print(mfrc522.uid.uidByte[i], HEX);
         content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
         content.concat(String(mfrc522.uid.uidByte[i], HEX));
      }
      Serial.println();
      Serial.print("Message : ");
      content.toUpperCase();
      if (content.substring(1) == stored){
        //The UID of the card/cards that you want to give access
        Serial.println("Authorized access");
        Serial.println();
        flag = false;
        access = true;
        delay(300);
      }
     
     else   {
        Serial.println("Denied Entry: contact the institute if discrepency");
        flag = true;
        access = false;
        digitalWrite(redLed,HIGH);
        delay(1000);
        digitalWrite(redLed,LOW);
        delay(300);
      }
    }
  while (access && !flag){ 
    //To dispence the sanitizer when the user puts hands below the dispenser(Ultrasound sersor is bound there)
    long duration=10, inches=10;
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    inches = (duration)/74/2;
    if (inches < 3){
        access =false;
        flag  = true;
        digitalWrite(blueLed,HIGH);
      //dispense the sanitizer if hand near
      for(int i=0;i<=180;i++){
        delay(50);
        dispense.write(i);
      }
      delay(1000);
      for(int i=180;i>=0;i--){
        delay(50);
        dispense.write(i);
      }
      digitalWrite(blueLed,LOW);
      digitalWrite(greenLed,HIGH);
      
      Serial.println("You can enter the premises now!");
      digitalWrite(redLed,HIGH);
      digitalWrite(greenLed,LOW);
      digitalWrite(blueLed,LOW);
      openDoor();
    }
  }
}

void openDoor(){
  //To open the door once sanitized
 for(int i=0;i<=180;i++){
    door.write(i);
   }
 delay(3000);
 digitalWrite(greenLed,LOW);
 flickColor();
 for(int i=180;i<=0;i++){
     door.write(i);
   }
}

void flickColor(){
  //To show the user to go quickly before the door closes
  for(int i=0;i<3;i++){
    digitalWrite(blueLed,HIGH);
    delay(100);
    digitalWrite(redLed,LOW);
    delay(200);
    digitalWrite(blueLed,LOW);
    delay(100);
    digitalWrite(redLed,HIGH);
    delay(200);
  }
}
