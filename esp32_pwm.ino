#include <esp_now.h>
#include <WiFi.h>

const int ssd = 16;
const int freq = 10;
const int ssdChannel = 0;
const int resolution = 8;

typedef struct pwm_data{
  int procent;
}
pwm_data;
pwm_data pwm_sturing;

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&pwm_sturing, incomingData, sizeof(pwm_sturing));
  int uit = (pwm_sturing.procent * 255) / 100;
  Serial.println(uit);
  ledcWrite(ssdChannel, uit);
}

void setup() {
  Serial.begin(115200);
  ledcSetup(ssdChannel, freq, resolution);
  ledcAttachPin(ssd, ssdChannel);
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);
}

void loop(){
}
