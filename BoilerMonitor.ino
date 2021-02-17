#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>

//Blynk config
char ssid[] = "Your Network";
char pass[] = "Your Wifi Password";
char auth[] = "Your Blynk Key";

//inputs for DS18b20
const int oneWireOut = 2;     
OneWire out(oneWireOut);
DallasTemperature outsensor(&out);

const int oneWireSup = 4;     
OneWire sup(oneWireSup);
DallasTemperature supsensor(&sup);

const int oneWireGarage = 5;     
OneWire Garage(oneWireGarage);
DallasTemperature garagesensor(&Garage);

const int oneWireLounge = 14;     
OneWire lounge(oneWireLounge);
DallasTemperature loungesensor(&lounge);

const int oneWireArena = 12;     
OneWire arena(oneWireArena);
DallasTemperature arenasensor(&arena);

const int oneWireStable = 13;     
OneWire stable(oneWireStable);
DallasTemperature stablesensor(&stable);

const int fanPin = 3;
WidgetLED led(V10);

String highset = "90.0";  //default high temp alarm
String lowset = "60.0"; //default low temp alarm
String longburnsetmilli = "3600000";  //default long burn (1 hour)
int highalarm = 0;
int lowalarm = 0;
int longalarm = 0;     
int startBurn = 0;
int endBurn = 0;      
int burnTime = 0;        
int burnint = 0;

BLYNK_CONNECTED() {
  Blynk.syncAll();
}

BlynkTimer timer;

void hightemp()
{
   if (highalarm == 0){
    Blynk.notify("Boiler High Temp!");
    Blynk.email("your email", "Boiler High Temp!");
    highalarm = 1; 
   }
   else
   {
    
   }
}

void lowtemp()
{
   if (lowalarm == 0){
    Blynk.notify("Boiler Low Temp!");
    Blynk.email("your email", "Boiler Low Temp!");
    lowalarm = 1;
   }
   else
   {
    
   }
}

void longburn()
{
   if (longalarm == 0){
    Blynk.notify("Boiler Long Burn!");
    Blynk.email("your email", "Boiler Long Burn!"); 
    longalarm = 1;
    burnTime = 0;
   }
   else
   {
    
   }
}

BLYNK_WRITE(V8) 
{
int highValue = param.asInt(); 
  highset = highValue;
}  

BLYNK_WRITE(V9) 
{
int lowValue = param.asInt();
  lowset = lowValue;
}


BLYNK_WRITE(V11) 
{
int longBurn = param.asInt(); 
  longburnsetmilli = longBurn * 60000;
}

void setup()
{
  Blynk.begin(auth, wifi, ssid, pass);
  
  pinMode(fanPin, INPUT);

  outsensor.begin();
  supsensor.begin();
  garagesensor.begin();
  loungesensor.begin();
  arenasensor.begin();
  stablesensor.begin();

  timer.setInterval(1000L, updateblynk);

}

void updateblynk()
{
  burnint = longburnsetmilli.toInt();

  garagesensor.requestTemperatures(); 
  float garagetemperature = garagesensor.getTempCByIndex(0);
  Blynk.virtualWrite(V3, garagetemperature);

  loungesensor.requestTemperatures(); 
  float loungetemperature = loungesensor.getTempCByIndex(0);
  Blynk.virtualWrite(V4, loungetemperature);

  arenasensor.requestTemperatures(); 
  float arenatemperature = arenasensor.getTempCByIndex(0);
  Blynk.virtualWrite(V5, arenatemperature);

  stablesensor.requestTemperatures(); 
  float stabletemperature = stablesensor.getTempCByIndex(0);
  Blynk.virtualWrite(V6, stabletemperature);

  outsensor.requestTemperatures(); 
    float outtemperature = outsensor.getTempCByIndex(0);
    Blynk.virtualWrite(V1, outtemperature);

  supsensor.requestTemperatures(); 
  float suptemperature = supsensor.getTempCByIndex(0);
  Blynk.virtualWrite(V2, suptemperature);
    if (suptemperature > highset.toFloat()){
      hightemp();
    }
    else if (suptemperature < lowset.toFloat()){
      lowtemp();      
    }
    else if (suptemperature < highset.toFloat()){
      highalarm = 0;
    }
    else if (suptemperature > lowset.toFloat()){
      lowalarm = 0;
    }

  if (digitalRead(fanPin) == HIGH) {
    Blynk.virtualWrite(V7, 1);
    led.on();
    startBurn = millis();
  } 
  
  else {
    Blynk.virtualWrite(V7, 0); 
    led.off();
    longalarm = 0;
    endBurn = millis(); 
  }

  burnTime = startBurn - endBurn;

  if (burnTime > burnint) {
    longburn();
 }
}

void loop()
{
  Blynk.run();

  timer.run();
}
