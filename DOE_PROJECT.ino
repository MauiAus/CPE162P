#define BLYNK_TEMPLATE_ID "TMPLnxQhKedE"
#define BLYNK_DEVICE_NAME "Design IoT"

#define BLYNK_FIRMWARE_VERSION        "0.1.0"

#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG

#define APP_DEBUG

// Uncomment your board, or configure a custom board in Settings.h
//#define USE_SPARKFUN_BLYNK_BOARD
#define USE_NODE_MCU_BOARD
//#define USE_WITTY_CLOUD_BOARD
//#define USE_WEMOS_D1_MINI

#include "BlynkEdgent.h"

#include "HX711.h"

#define DOUT 2
#define SCK 15

#define O3 D0
#define O2 D1
#define O1 D2
#define O0 D3

#define waterSensor A0

HX711 scale;

BlynkTimer timer;

float calibration_factor = 419640;//142600 419640

int valNum;

long last_sensor_reading = 0;

BLYNK_WRITE(V2)
{
  int timeVal = param.asInt(); // assigning incoming value from pin V1 to a variable
  Blynk.virtualWrite(V4,convertVal(timeVal,0));
  sendCommand(timeVal,0);
}

BLYNK_WRITE(V3)
{
  int weightVal = param.asInt(); // assigning incoming value from pin V1 to a variable
  Blynk.virtualWrite(V5,convertVal(weightVal,1));
  sendCommand(weightVal,1);
}

void setup()
{
  Serial.begin(115200);
  delay(100);
  
  scale.begin(DOUT, SCK);
  pinMode(O3, OUTPUT);
  pinMode(O2, OUTPUT);
  pinMode(O1, OUTPUT);
  pinMode(O0, OUTPUT);
  pinMode(waterSensor, INPUT);
  timer.setInterval(500L,getWeight);
  BlynkEdgent.begin();
}

void loop() {
  timer.run();
  BlynkEdgent.run();
}

void getWeight(){
  scale.tare(); //Reset the scale to 0
  scale.set_scale(calibration_factor);
  float val = scale.get_units(5);
  Blynk.virtualWrite(V0,val);
}

void sendCommand(int n,int select){
  int a[3] = {0,0,0}, i;
  for(i=0; n>0; i++){
    a[i]=n%2;
    n=n/2;
  }
  
  digitalWrite(O3,select);
  digitalWrite(O2,a[2]);
  digitalWrite(O1,a[1]);
  digitalWrite(O0,a[0]);

  Serial.println("");
  Serial.print(select);
  Serial.print(a[2]);
  Serial.print(a[1]);
  Serial.print(a[0]);
  Serial.println("");
}

int convertVal(int x, int select){
  if(select == 0){
    if(x == 0)
      return 0; 
    else
      return 24 / x;
  }
  else if(select == 1){
    return 400 * x;
  }
}
