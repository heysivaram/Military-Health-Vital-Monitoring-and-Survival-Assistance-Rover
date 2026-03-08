#include <esp_now.h>
#include <WiFi.h>

typedef struct healthData
{
float heartRate;
float spo2;
float temperature;
} healthData;

healthData receivedData;

void OnDataRecv(const uint8_t * mac,const uint8_t *incomingData,int len)
{
memcpy(&receivedData,incomingData,sizeof(receivedData));

Serial.print("Heart Rate: ");
Serial.println(receivedData.heartRate);

Serial.print("SpO2: ");
Serial.println(receivedData.spo2);

Serial.print("Temperature: ");
Serial.println(receivedData.temperature);
}

void setup()
{
Serial.begin(115200);

WiFi.mode(WIFI_STA);

esp_now_init();

esp_now_register_recv_cb(OnDataRecv);
}

void loop()
{
}
