#define METAL_SENSOR 32
#define BUZZER 4

void setup()
{
Serial.begin(115200);

pinMode(METAL_SENSOR,INPUT);
pinMode(BUZZER,OUTPUT);
}

void loop()
{

int metalValue = analogRead(METAL_SENSOR);

if(metalValue > 2000)
{
Serial.println("Possible Landmine Detected");

digitalWrite(BUZZER,HIGH);
delay(500);
digitalWrite(BUZZER,LOW);
}

delay(300);
}
