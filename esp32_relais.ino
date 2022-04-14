#include <esp_now.h>
#include <WiFi.h>

#define STUUR_OUTPUT  16

typedef struct relais_data{
  bool relais;
}relais_data;

relais_data ingelezen;

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&ingelezen, incomingData, sizeof(ingelezen));
  if(ingelezen.relais){
    digitalWrite(STUUR_OUTPUT, HIGH);
  }
  else{
    digitalWrite(STUUR_OUTPUT, LOW);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(STUUR_OUTPUT, OUTPUT);
  digitalWrite(STUUR_OUTPUT, LOW);
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
}
