#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <OneWire.h>
#include <DallasTemperature.h>

#define TEMP_PIN 5

PulseOximeter pox;

OneWire oneWire(TEMP_PIN);
DallasTemperature sensors(&oneWire);

void setup()
{
Serial.begin(115200);

Wire.begin();

pox.begin();
sensors.begin();

Serial.println("Health Monitoring Band Started");
}

void loop()
{
pox.update();

sensors.requestTemperatures();

float bodyTemp = sensors.getTempCByIndex(0);

Serial.print("Heart Rate: ");
Serial.println(pox.getHeartRate());

Serial.print("SpO2: ");
Serial.println(pox.getSpO2());

Serial.print("Body Temperature: ");
Serial.println(bodyTemp);

delay(1000);
}
