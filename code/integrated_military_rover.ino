#include <Wire.h>
#include <Servo.h>
#include <WiFi.h>
#include <esp_now.h>

#include <Adafruit_MLX90614.h>
#include "MAX30100_PulseOximeter.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TinyGPS++.h>

/* PIN DEFINITIONS */

#define TRIG 5
#define ECHO 18

#define SERVO_PIN 25

#define IR1 13
#define IR2 15
#define IR3 16
#define IR4 17

#define GAS_SENSOR 34
#define FLAME_SENSOR 35
#define SOUND_SENSOR 33
#define LDR_SENSOR 32

#define METAL_SENSOR 39

#define BUZZER 4
#define ALERT_LED 2
#define HEADLIGHT 23

#define IN1 12
#define IN2 14
#define IN3 27
#define IN4 26

#define TEMP_PIN 5

Servo scanner;

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
PulseOximeter pox;

OneWire oneWire(TEMP_PIN);
DallasTemperature bodyTempSensor(&oneWire);

TinyGPSPlus gps;

typedef struct healthData
{
float heartRate;
float spo2;
float temperature;
} healthData;

healthData bandData;

long duration;
int distance;

/* MOTOR FUNCTIONS */

void moveForward()
{
digitalWrite(IN1,HIGH);
digitalWrite(IN2,LOW);
digitalWrite(IN3,HIGH);
digitalWrite(IN4,LOW);
}

void stopMotors()
{
digitalWrite(IN1,LOW);
digitalWrite(IN2,LOW);
digitalWrite(IN3,LOW);
digitalWrite(IN4,LOW);
}

void turnLeft()
{
digitalWrite(IN1,LOW);
digitalWrite(IN2,HIGH);
digitalWrite(IN3,HIGH);
digitalWrite(IN4,LOW);
}

void turnRight()
{
digitalWrite(IN1,HIGH);
digitalWrite(IN2,LOW);
digitalWrite(IN3,LOW);
digitalWrite(IN4,HIGH);
}

/* OBSTACLE DETECTION */

int measureDistance()
{
digitalWrite(TRIG,LOW);
delayMicroseconds(2);

digitalWrite(TRIG,HIGH);
delayMicroseconds(10);

digitalWrite(TRIG,LOW);

duration = pulseIn(ECHO,HIGH);

return duration * 0.034 / 2;
}

void checkObstacle()
{
distance = measureDistance();

Serial.print("Distance: ");
Serial.println(distance);

if(distance < 25)
{
stopMotors();

scanner.write(40);
delay(400);
int leftDist = measureDistance();

scanner.write(140);
delay(400);
int rightDist = measureDistance();

scanner.write(90);

if(leftDist > rightDist)
{
turnLeft();
delay(600);
}
else
{
turnRight();
delay(600);
}
}
}

/* PIT DETECTION */

void checkPit()
{
if(digitalRead(IR1)==LOW ||
digitalRead(IR2)==LOW ||
digitalRead(IR3)==LOW ||
digitalRead(IR4)==LOW)
{
Serial.println("Pit detected");

stopMotors();
digitalWrite(BUZZER,HIGH);
delay(500);
digitalWrite(BUZZER,LOW);
}
}

/* ENVIRONMENT MONITOR */

void checkGas()
{
int gasVal = analogRead(GAS_SENSOR);

if(gasVal > 3000)
{
Serial.println("Gas hazard!");
digitalWrite(BUZZER,HIGH);
}
}

void checkFlame()
{
if(digitalRead(FLAME_SENSOR)==LOW)
{
Serial.println("Flame detected");
digitalWrite(BUZZER,HIGH);
}
}

void checkSound()
{
int soundVal = analogRead(SOUND_SENSOR);

if(soundVal > 2000)
{
Serial.println("High sound detected");
}
}

/* LANDMINE DETECTION */

void detectLandmine()
{
int metal = analogRead(METAL_SENSOR);

if(metal > 2000)
{
Serial.println("Possible landmine detected");
digitalWrite(BUZZER,HIGH);
delay(400);
digitalWrite(BUZZER,LOW);
}
}

/* LIGHT CONTROL */

void autoHeadlight()
{
int light = analogRead(LDR_SENSOR);

if(light < 1500)
digitalWrite(HEADLIGHT,HIGH);
else
digitalWrite(HEADLIGHT,LOW);
}

/* ESP-NOW RECEIVE */

void OnDataRecv(const uint8_t * mac,
const uint8_t *incomingData,int len)
{
memcpy(&bandData,incomingData,sizeof(bandData));

Serial.println("Band Data Received");

Serial.print("Heart Rate: ");
Serial.println(bandData.heartRate);

Serial.print("SpO2: ");
Serial.println(bandData.spo2);

Serial.print("Temperature: ");
Serial.println(bandData.temperature);
}

/* HEALTH SENSORS */

void readHealthSensors()
{
pox.update();

bodyTempSensor.requestTemperatures();
float temp = bodyTempSensor.getTempCByIndex(0);

Serial.print("Local Body Temp: ");
Serial.println(temp);

double irTemp = mlx.readObjectTempC();

Serial.print("Infrared Temp: ");
Serial.println(irTemp);

Serial.print("Heart Rate: ");
Serial.println(pox.getHeartRate());

Serial.print("SpO2: ");
Serial.println(pox.getSpO2());
}

/* GPS */

void readGPS()
{
while(Serial.available()>0)
gps.encode(Serial.read());

if(gps.location.isUpdated())
{
Serial.print("Latitude: ");
Serial.println(gps.location.lat());

Serial.print("Longitude: ");
Serial.println(gps.location.lng());
}
}

void setup()
{
Serial.begin(115200);

Wire.begin();

mlx.begin();
pox.begin();

bodyTempSensor.begin();

pinMode(TRIG,OUTPUT);
pinMode(ECHO,INPUT);

pinMode(IR1,INPUT);
pinMode(IR2,INPUT);
pinMode(IR3,INPUT);
pinMode(IR4,INPUT);

pinMode(GAS_SENSOR,INPUT);
pinMode(FLAME_SENSOR,INPUT);
pinMode(SOUND_SENSOR,INPUT);

pinMode(LDR_SENSOR,INPUT);
pinMode(METAL_SENSOR,INPUT);

pinMode(BUZZER,OUTPUT);
pinMode(ALERT_LED,OUTPUT);
pinMode(HEADLIGHT,OUTPUT);

pinMode(IN1,OUTPUT);
pinMode(IN2,OUTPUT);
pinMode(IN3,OUTPUT);
pinMode(IN4,OUTPUT);

scanner.attach(SERVO_PIN);
scanner.write(90);

WiFi.mode(WIFI_STA);

if(esp_now_init()!=ESP_OK)
{
Serial.println("ESP-NOW init failed");
return;
}

esp_now_register_recv_cb(OnDataRecv);

Serial.println("Military Rover Ready");
}

void loop()
{
checkObstacle();
checkPit();
checkGas();
checkFlame();
checkSound();
detectLandmine();
autoHeadlight();
readHealthSensors();
readGPS();
moveForward();
delay(200);
}
