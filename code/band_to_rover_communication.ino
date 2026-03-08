#include <esp_now.h>
#include <WiFi.h>

typedef struct healthData
{
float heartRate;
float spo2;
float temperature;
} healthData;

healthData soldierData;

void setup()
{
Serial.begin(115200);

WiFi.mode(WIFI_STA);

if (esp_now_init() != ESP_OK)
{
Serial.println("ESP-NOW Init Failed");
return;
}

esp_now_register_send_cb(OnDataSent);
}

void loop()
{
soldierData.heartRate = random(70,100);
soldierData.spo2 = random(90,100);
soldierData.temperature = random(35,38);

esp_now_send(NULL,(uint8_t *)&soldierData,sizeof(soldierData));

Serial.println("Health Data Sent");

delay(2000);
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
Serial.print("Delivery Status: ");
Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}
