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


#include <Adafruit_MLX90614.h>
#include <Wire.h>
#include <ESP32Servo.h>

#include <WiFi.h>
#include "ThingSpeak.h"

#include "ESP_Mail_Client.h"

#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT esp_mail_smtp_port_587

#define AUTHOR_EMAIL "mauibot07@gmail.com"
#define AUTHOR_PASSWORD "M@uiBOTp@ss"

SMTPSession smtp;
void smtpCallback(SMTP_Status status);

const char rootCACert[] PROGMEM = "-----BEGIN CERTIFICATE-----\n"
                                  "-----END CERTIFICATE-----\n";

const char* ssid = "HG8145V5_62884";             // SSID
const char* password = "q4tk9DZx";

#define SOUND_VELOCITY 0.034
#define CM_TO_INCH 0.393701

//BlynkTimer timer;

WiFiClient client;

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
    float temp = mlx.readObjectTempC();
    Blynk.virtualWrite(V0,temp);
    int x = ThingSpeak.writeField(myChannelNumber, 1, temp, myWriteAPIKey);
    if(temp > 37)
    {
      sendEmail();
    }
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
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
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

void sendEmail(){
  smtp.debug(1);

  /* Set the callback function to get the sending results */
  smtp.callback(smtpCallback);

  /* Declare the session config data */
  ESP_Mail_Session session;

  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;
  session.login.user_domain = "mydomain.net";

  /* Set the NTP config time */
  session.time.ntp_server = "pool.ntp.org,time.nist.gov";
  session.time.gmt_offset = 3;
  session.time.day_light_offset = 0;

  /* Declare the message class */
  SMTP_Message message;

  /* Set the message headers */
  message.sender.name = "Baymax";
  message.sender.email = AUTHOR_EMAIL;
  message.subject = "HIGH TEMPERATURE RECORDED";
  message.addRecipient("Someone", "markaustin017@gmail.com");

  String textMsg = "Baymax has detected risk temperature!";
  message.text.content = textMsg.c_str();
  
  message.text.charSet = "us-ascii";

  message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

  message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_low;

  message.addHeader("Message-ID: <abcde.fghij@gmail.com>");

  if (!smtp.connect(&session))
    return;

  /* Start sending Email and close the session */
  if (!MailClient.sendMail(&smtp, &message))
    Serial.println("Error sending Email, " + smtp.errorReason());

  //to clear sending result log
  //smtp.sendingResult.clear();

  ESP_MAIL_PRINTF("Free Heap: %d\n", MailClient.getFreeHeap());
}

void smtpCallback(SMTP_Status status)
{
  /* Print the current status */
  Serial.println(status.info());

  /* Print the sending result */
  if (status.success())
  {
    Serial.println("----------------");
    ESP_MAIL_PRINTF("Message sent success: %d\n", status.completedCount());
    ESP_MAIL_PRINTF("Message sent failled: %d\n", status.failedCount());
    Serial.println("----------------\n");
    struct tm dt;

    for (size_t i = 0; i < smtp.sendingResult.size(); i++)
    {
      /* Get the result item */
      SMTP_Result result = smtp.sendingResult.getItem(i);
      time_t ts = (time_t)result.timestamp;
      localtime_r(&ts, &dt);

      ESP_MAIL_PRINTF("Message No: %d\n", i + 1);
      ESP_MAIL_PRINTF("Status: %s\n", result.completed ? "success" : "failed");
      ESP_MAIL_PRINTF("Date/Time: %d/%d/%d %d:%d:%d\n", dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday, dt.tm_hour, dt.tm_min, dt.tm_sec);
      ESP_MAIL_PRINTF("Recipient: %s\n", result.recipients);
      ESP_MAIL_PRINTF("Subject: %s\n", result.subject);
    }
    Serial.println("----------------\n");

    //You need to clear sending result as the memory usage will grow up as it keeps the status, timstamp and
    //pointer to const char of recipients and subject that user assigned to the SMTP_Message object.

    //Because of pointer to const char that stores instead of dynamic string, the subject and recipients value can be
    //a garbage string (pointer points to undefind location) as SMTP_Message was declared as local variable or the value changed.

    //smtp.sendingResult.clear();
  }
}
