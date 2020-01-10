#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHT.h"

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "1234567890abcdef";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "ssid";
char pass[] = "password";
#define ONE_WIRE_BUS 2
#define DHTTYPE DHT11
#define DHTPIN 0

// For system led
bool systemStatus = false;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
DHT dht(DHTPIN, DHTTYPE);
WidgetLED ledSystem(V10);
WidgetLED led18B20(V11);
WidgetLED ledDHT(V12);
BlynkTimer timer1;
BlynkTimer timer2;

// **** Temperature ****
// reading inteval
#define timer1Interval 2000L
// adjustment of temperature
float tempBias = -1.6;

// **** Humidity ****
// reading inteval
#define timer2Interval 10000L
// weight of the new mesure
float weight = 1.0/6.0;

void setup()
{
    Blynk.begin(auth, ssid, pass);
    
    DS18B20.begin();
    dht.begin();
    
    timer1.setInterval(timer1Interval, timer1Event);
    timer1.setInterval(timer2Interval, timer2Event);

    //ESP.deepSleep(1000000*sleepTime);
}

void loop()
{
  Blynk.run();  
  timer1.run();  
  timer2.run();  
}

void timer1Event()
{
  systemStatus = !systemStatus;
  if(systemStatus)
    ledSystem.on();
  else
    ledSystem.off();
  
  DS18B20.requestTemperatures();
  float temp = DS18B20.getTempCByIndex(0);
  if(temp != DEVICE_DISCONNECTED_C) {
    temp += tempBias;
    Blynk.virtualWrite(V2, temp);
    led18B20.on();
  } else {
     led18B20.off();
  }
}

float lastH = 0;
void timer2Event()
{ 
  float h = dht.readHumidity();
  if (!isnan(h)){
    if(lastH == 0)
      lastH = h;
    else
      // limit the influence of the new value
      lastH = lastH * (1 - weight) + h * weight;
      
    Blynk.virtualWrite(V1, lastH);
    ledDHT.on();
  } else {
    ledDHT.off();
  }
}
