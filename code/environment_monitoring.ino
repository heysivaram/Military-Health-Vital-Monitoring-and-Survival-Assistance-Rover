#define GAS_SENSOR 34
#define FLAME_SENSOR 35
#define SOUND_SENSOR 33

void setup()
{
Serial.begin(115200);

pinMode(GAS_SENSOR,INPUT);
pinMode(FLAME_SENSOR,INPUT);
pinMode(SOUND_SENSOR,INPUT);
}

void loop()
{
int gasValue = analogRead(GAS_SENSOR);

if(gasValue > 3000)
{
Serial.println("Gas Hazard Detected");
}

if(digitalRead(FLAME_SENSOR) == LOW)
{
Serial.println("Flame Detected");
}

int soundLevel = analogRead(SOUND_SENSOR);

if(soundLevel > 2000)
{
Serial.println("High Sound Detected");
}

delay(500);
}
