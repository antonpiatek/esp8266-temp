/*
 * ESP8266+DS18B20 temperature sensor for MQTT
 * 
 * Copy sample_config.h to new file config.h and edit
 * 
 * Copyright 2018 Anton Piatek
 * 
 * Released under MIT license - https://anton.mit-license.org/
 * 
 */

#include "config.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h> // https://github.com/adafruit/MAX31850_DallasTemp/tree/master/examples/Single

#ifndef WIFI_SSID
  #error "WIFI_SSID must be defined in config"
#endif

#ifndef WIFI_PASS
  #error "WIFI_PASS must be defined in config"
#endif

#ifndef BROKER
  #error "BROKER must be defined in config"
#endif

#ifndef BROKER_PASS
  #error "BROKER_PASS must be defined in config"
#endif

#ifndef TOPIC_ROOT
  #error "TOPIC_ROOT must be defined in config"
#endif

#ifndef ONE_WIRE_BUS
  #error "ONE_WIRE_BUS must be defined in config"
#endif

#ifndef DELAY
  #define DELAY 10000
#endif


// Main program
OneWire  oneWire(ONE_WIRE_BUS); 
DallasTemperature sensors(&oneWire);
DeviceAddress themometer;
WiFiClient espClient;
PubSubClient client(espClient);
String clientId;
String topic;
String willTopic;

void wait10(){
  for(int i=0; i<10; i++){
    delay(1000);
    Serial.print("_");
  }
  Serial.println();
}

void setup(void)
{
  Serial.begin(9600);

  //set wifi
  Serial.println();
  Serial.print("WIFI configured to ");
  Serial.println(WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  topic = String(TOPIC_ROOT)+"/"+WiFi.macAddress()+"/temp";
  willTopic = String(TOPIC_ROOT)+"/"+WiFi.macAddress()+"/status";
  Serial.println("TOPIC: "+topic);

  //set mqtt
  clientId = WiFi.macAddress();
  client.setServer(BROKER, 1883);

  checkComms();

  //setup temp sensors
  initSensors();
}

void initSensors(){
    while(true){
        sensors.begin();
        Serial.print("Found ");
        Serial.print(sensors.getDeviceCount(), DEC);
        Serial.println(" devices.");
        if (!sensors.getAddress(themometer, 0)){
            Serial.println("Unable to find address for Device 0"); 
            continue;
        }
        sensors.setResolution(themometer, 10);
        break;
    }
}

void checkComms(){
    if(WiFi.status() != WL_CONNECTED){
        while (WiFi.status() != WL_CONNECTED) {
            Serial.print("waiting for wifi, rc= ");
            Serial.println(WiFi.status());
            delay(1000);
            //TODO while connecting flash led
            Serial.print(".");
        }
        Serial.println("");
        Serial.println("WiFi connected");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
    }
    if(!client.connected()){
        Serial.print("MQTT connecting...");
        while (!client.connected()) {
        Serial.print(".");
            if (client.connect(clientId.c_str(), "use-token-auth", BROKER_PASS, willTopic.c_str(), 0, 1, "0")) {
                Serial.println("connected");
                client.publish (willTopic.c_str(), "1", true);
            }else {
                Serial.print("failed, rc=");
                Serial.println(client.state());
            }
        }
    }
}


void loop(void)
{
  checkComms();

  sensors.requestTemperatures();
  float tempC = sensors.getTempC(themometer);
  Serial.print("Temp C: ");
  Serial.print(tempC);
  Serial.println("C");
  
  //TODO something with boolean RC
  client.publish (topic.c_str(), String(tempC,2).c_str(), true);


//TODO #def - also how much does this affect an accurate reading
  delay(DELAY);
}
