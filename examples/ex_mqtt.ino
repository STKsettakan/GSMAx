#include <SoftwareSerial.h>
#include "GSMAx.h"
#include "GSMAx_mqtt.h"
#include "DHT.h"
SoftwareSerial myserial(10,11);//rx,tx
#define PWRPIN 8
#define DHTPIN 2 
#define DHTTYPE DHT22 
GSMAxMQTT mqtt;
DHT dht(DHTPIN, DHTTYPE);
unsigned long previousMillis = 0; 
const long interval = 2000;

#define MQTT_SERVER   ""
#define MQTT_PORT     ""
#define MQTT_THING    ""
#define MQTT_USER     ""
#define MQTT_PASSWORD ""
void setup() 
{
   Serial.begin(9600);
   gsm.begin(&myserial,9600,PWRPIN);
   Serial.println(F("Power On"));
   gsm.power();
   
  while(!gsm.waitReady())
  {
    Serial.print(".");
  }
  Serial.println();
  Serial.println(F("GSM Ready"));
  Serial.print(F("Close Echo --> "));
  Serial.println(gsm.closeEcho()?"OK":"Error");
  Serial.print(F("Signal --> "));
  Serial.println(String(gsm.getSignal()));
  Serial.print(F("Connect Internet --> "));
  while(!gsm.connectNet("True"))
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("OK");
  
  connectServer();
}
unsigned char flag=0;
void loop() 
{
  unsigned long currentMillis = millis();
   if (currentMillis - previousMillis >= interval) 
   {
       float h = dht.readHumidity();
       float t = dht.readTemperature();
       mqtt.publish("temp",t);
       mqtt.publish("humi",h);
        previousMillis = millis();
   }
   if(!mqtt.connectState())
   {
    connectServer();
   }
   mqtt.loop();

}

void connectServer()
{
  Serial.print(F("Connect Server --> "));
  mqtt.disconnectMQTTServer();
  bool ret = mqtt.connectMQTTServer(MQTT_SERVER,MQTT_PORT);
  if(ret)
  {
      Serial.println("OK");
      ret = mqtt.login(MQTT_THING,MQTT_USER,MQTT_PASSWORD);
      if(ret)
      {
        mqtt.subscribe("led1");
        mqtt.subscribe("led2");
      }
  }
  else
      Serial.println("Fail");
}
