#include <WiFi.h>
#include <HCSR04.h>
#include "FirebaseESP32.h"

#define FIREBASE_HOST "https://smarthomeiot-5a5e3.firebaseio.com/"
#define FIREBASE_AUTH "eybGUOvJ8QswkyoFORmjMfUJhuXIMcTRgL9eD2w4"
#define WIFI_SSID "G149" 
#define WIFI_PASSWORD "149ModalDong"

UltraSonicDistanceSensor distanceSensor(22, 23 );

#define led1 2
#define led2 4
#define ANALOG_PIN_0 36

FirebaseData firebaseData;

String path = "/Node1";

int oldDistance;
int newDistance;

int oldPotValue;
int newPotValue;

int freq = 5000;
int ledChannel = 0;
int resolution = 8;
int dutyCycle = 0;

void setup() {
  
  Serial.begin(115200);

  pinMode(led2, OUTPUT);

  initWifi();
  
  oldDistance = distanceSensor.measureDistanceCm();
  oldPotValue = analogRead(ANALOG_PIN_0);

  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(led1, ledChannel);
  ledcWrite(ledChannel, dutyCycle);
}

void loop() {

  delay(500);

  newDistance = distanceSensor.measureDistanceCm();
  Serial.println(newDistance);
  if(newDistance != oldDistance){
    Firebase.setInt(firebaseData, path + "/distance", newDistance);
    oldDistance = newDistance; 
  }

  newPotValue = analogRead(ANALOG_PIN_0);
  Serial.println(newPotValue);
  if(newPotValue != oldPotValue){
    Firebase.setInt(firebaseData, path + "/potential", newPotValue);
    oldPotValue = newPotValue;
  }

  //get value /Node1/led1
  if(Firebase.getInt(firebaseData, path + "/led1")){
    dutyCycle = map(firebaseData.intData(), 0, 4095, 0, 255);
    ledcWrite(ledChannel, dutyCycle);
  }

  //get value /Node1/led2
  if(Firebase.getInt(firebaseData, path + "/led2")){
    if(firebaseData.intData() == 0)
      digitalWrite(led2,0);
    else
      digitalWrite(led2,1);
  }
}

void initWifi(){
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  Firebase.setReadTimeout(firebaseData, 1000* 60);
  Firebase.setwriteSizeLimit(firebaseData, "tiny");
}
