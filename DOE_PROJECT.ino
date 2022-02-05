
// Fill-in information from your Blynk Template here
#define BLYNK_TEMPLATE_ID "TMPLUVz-1bZJ"
#define BLYNK_DEVICE_NAME "IoT based Robot Arm"

#define BLYNK_FIRMWARE_VERSION        "0.1.0"

#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG

#define APP_DEBUG

// Uncomment your board, or configure a custom board in Settings.h
//#define USE_WROVER_BOARD
//#define USE_TTGO_T7
//#define USE_ESP32C3_DEV_MODULE
//#define USE_ESP32S2_DEV_KIT

#include "BlynkEdgent.h"

//#include "ThingSpeak.h"

#include <Adafruit_MLX90614.h>
#include <Wire.h>
#include <ESP32Servo.h>
//#include <WiFi.h>

#define SOUND_VELOCITY 0.034
#define CM_TO_INCH 0.393701

//BlynkTimer timer;

//WiFiClient client;

const int trigPin = 4;
const int echoPin = 2;

const int motor = 5;

long duration;
float distanceCm;

unsigned long myChannelNumber = 4;
const char * myWriteAPIKey = "EVTYQ4UGMQBRD5FD";

Servo temp1;
Servo temp2;
Servo servo1; 
Servo servo2;
Servo servo3; 
Servo servoT;

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

BLYNK_WRITE(V1)
{
  int val = param.asInt(); // assigning incoming value from pin V1 to a variable
  Serial.print("Servo 1: ");
  Serial.print(val);
  Serial.println("");
  servo1.write(val);
  sonicRead();
}

BLYNK_WRITE(V2)
{
  int val = param.asInt(); // assigning incoming value from pin V1 to a variable
  Serial.print("Servo 2: ");
  Serial.print(val);
  Serial.println("");
  servo2.write(val);
  sonicRead();
}

BLYNK_WRITE(V3)
{
  int val = param.asInt(); // assigning incoming value from pin V1 to a variable
  Serial.print("Servo 3: ");
  Serial.print(val);
  Serial.println("");
  servo3.write(val);
  sonicRead();
}

BLYNK_WRITE(V4)
{
  int val = param.asInt(); // assigning incoming value from pin V1 to a variable
  Serial.print("Servo 4: ");
  Serial.print(val);
  Serial.println("");
  servoT.write(val);
  sonicRead();
}

BLYNK_WRITE(V6)
{
  int val = param.asInt();
  Serial.print("Dispense: ");
  Serial.println(val);
  if(val == 1){
    setMotor(true);
    delay(3000);
    setMotor(false);
  }
}


BLYNK_WRITE(V7)
{
  int val = param.asInt();
  if(val == 1){
    
    Blynk.virtualWrite(V0,mlx.readObjectTempC());
    //int x = ThingSpeak.writeField(myChannelNumber, 1, mlx.readObjectTempC(), myWriteAPIKey);
  }
}

void setup()
{
  Serial.begin(115200);
  delay(100);
  temp1.attach(23);
  temp2.attach(19);
  servo1.attach(15); 
  servo2.attach(16);
  servo3.attach(17);
  servoT.attach(18);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(motor, OUTPUT);
  mlx.begin(); 
  //ThingSpeak.begin(client);  // Initialize ThingSpeak
  BlynkEdgent.begin();
  //timer.setInterval(500L,sonicRead);
}

void loop() {
  BlynkEdgent.run();
}

void sonicRead(){
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  distanceCm = duration * SOUND_VELOCITY/2;
  
  // Prints the distance on the Serial Monitor
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);

  Blynk.virtualWrite(V5,distanceCm);
}

void setMotor(bool x){
  if(x == true){
    digitalWrite(motor, HIGH);
  }
  else{
    digitalWrite(motor, LOW);
  }
}
