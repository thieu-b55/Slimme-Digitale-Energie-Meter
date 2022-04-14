#include <esp_now.h>
#include <WiFi.h>
#include <TFT_eSPI.h>
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();

typedef struct meter_data{
  float kwh_totaal;
  float injectie_totaal;
  float injectie_nu;
  float verbruik_nu;
  float gas_totaal;
  bool  relais1;
  bool  relais2;
  bool  relais3;
  int   pwm_sturing;
}meter_data;
meter_data ingelezen;

unsigned long begin_millis;
int positie = 0;
int x = 1;

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&ingelezen, incomingData, sizeof(ingelezen));
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.setCursor(0, positie);
  tft.print("Totaal electriciteit");
  tft.setCursor(200, positie);
  tft.print("Totaal injectie");
  tft.setCursor(400, positie);
  tft.print("Totaal gas");
  tft.setCursor(0, positie + 15);
  tft.print(ingelezen.kwh_totaal, 3);
  tft.print(" KWh");
  tft.setCursor(200, positie + 15);
  tft.print(ingelezen.injectie_totaal, 3);
  tft.print(" KWh");
  tft.setCursor(400, positie + 15);
  tft.print(ingelezen.gas_totaal, 3);
  tft.print(" m3");
  tft.setTextSize(2);
  if(ingelezen.relais1){
    tft.setCursor(20, positie + 40);
    tft.setTextColor(TFT_YELLOW);
    tft.print("R1");
  }
  if(ingelezen.relais2){
    tft.setCursor(90, positie + 40);
    tft.setTextColor(TFT_YELLOW);
    tft.print("R2");
  }
  if(ingelezen.relais3){
    tft.setCursor(370, positie + 40);
    tft.setTextColor(TFT_YELLOW);
    tft.print("R3");
  }
  if(ingelezen.pwm_sturing > 0){
    tft.setCursor(420, positie + 40);
    tft.setTextColor(TFT_YELLOW);
    tft.print(ingelezen.pwm_sturing);
    tft.print("%");
  }
   
  tft.setCursor(200, positie + 40);
  if(ingelezen.injectie_nu >= ingelezen.verbruik_nu){
    tft.setTextColor(TFT_GREEN);
    tft.print(ingelezen.injectie_nu, 3);
    tft.print(" KW");
  }
  else{
    tft.setTextColor(TFT_RED);
    tft.setTextSize(2);
    tft.print(ingelezen.verbruik_nu, 3);
    tft.print(" KW");
  }
  positie = positie + x;
  if((positie == 250)|| (positie == 0)){
    x = x * -1;
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);
  tft.begin();
  tft.setRotation(1);
  tft.setTextSize(1);
  tft.fillScreen(TFT_BLACK);
}

void loop() {
  
}
