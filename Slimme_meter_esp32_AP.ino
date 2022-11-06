/*
* MIT License
*
* Copyright (c) 2022 thieu-b55
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#include <WiFi.h>
#include <Preferences.h>
#include <esp_now.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

esp_now_peer_info_t   peerInfo;
esp_err_t             result;

AsyncWebServer server(80);

Preferences pref;

#define RXD2          16
#define TXD2          17
#define BLINKIE       4

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
meter_data    ingelezen;


typedef struct relais_data{
  bool relais;
}relais_data;
relais_data   uitsturen;


typedef struct pwm_data{
  int procent;
}
pwm_data;
pwm_data    pwm_sturing;


String buffer_data =    "                                                         ";
String kwh_dag = "          ";
String kwh_nacht = "          ";
String injectie_dag = "          ";
String injectie_nacht = "          ";
String kw_nu = "          ";
String injectie_nu = "          ";
String gas = "         ";

int module_teller = 0;
int relais_module_teller = 0;

unsigned long nu;

float kwh_dag_float;
float kwh_nacht_float;
float kwh_totaal_float;
float injectie_dag_float;
float injectie_nacht_float;
float injectie_totaal_float;
float kw_nu_float;
float injectie_nu_float;
float verbruik_nu_float;
float gas_totaal_float;
float verbruik_pwm_float;

char kwh_totaal_float_char[12];
char injectie_totaal_float_char[12];
char kw_nu_float_char[12];
char injectie_nu_float_char[12];
char gas_totaal_float_char[12];

char broadcastAddressX_0_char[8];
char broadcastAddressX_1_char[8];
char broadcastAddressX_2_char[8];
char broadcastAddressX_3_char[8];
char broadcastAddressX_4_char[8];
char broadcastAddressX_5_char[8];

String broadcastAddress1_string = "                       ";
String broadcastAddress2_string = "                       ";
String broadcastAddress3_string = "                       ";
String broadcastAddress4_string = "                       ";
String broadcastAddress5_string = "                       ";

char module_char[20];

const char* INPUT_MACX_0 = "input_macx_0";
const char* INPUT_MACX_1 = "input_macx_1";
const char* INPUT_MACX_2 = "input_macx_2";
const char* INPUT_MACX_3 = "input_macx_3";
const char* INPUT_MACX_4 = "input_macx_4";
const char* INPUT_MACX_5 = "input_macx_5";

const char* MODULE_MIN = "module_min";
const char* MODULE_PLUS = "module_plus";
const char* MODULE_BEVESTIG = "module_bevestig";

uint8_t broadcastAddress1[6];
uint8_t broadcastAddress2[6];
uint8_t broadcastAddress3[6];
uint8_t broadcastAddress4[6];
uint8_t broadcastAddress5[6];

uint8_t input_macx_0;
uint8_t input_macx_1;
uint8_t input_macx_2;
uint8_t input_macx_3;
uint8_t input_macx_4;
uint8_t input_macx_5;

const char* INPUT_KW_ON = "input_kw_on";
const char* INPUT_OVERRIDE = "input_override";
const char* INPUT_DELAY = "input_delay";
const char* INPUT_SCHAKEL_TIJD = "input_schakel_tijd";
const char* RELAIS_MODULE_MIN = "relais_module_min";
const char* RELAIS_MODULE_PLUS = "relais_module_plus";
const char* RELAIS_MODULE_BEVESTIG = "relais_module_bevestig";

const char* INPUT_PWM_KW = "input_pwm_kw";
const char* INPUT_PWM_TIJD_ON = "input_pwm_tijd_on";
const char* INPUT_PWM_TIJD_OFF = "input_pwm_tijd_off";
const char* INPUT_PWM_OVERRIDE = "input_pwm_override";
const char* BEVESTIG_PWM = "bevestig_pwm";

bool relais1_uit;
bool relais2_uit;
bool relais3_uit;
bool vijf_seconden = false;
int relais1_delay;
int relais2_delay;
int relais3_delay;
int uren_on1_int;
int uren_on2_int;
int uren_on3_int;
int uren_on4_int;
int uren_off4_int;
int minuten_on1_int;
int minuten_on2_int;
int minuten_on3_int;
int minuten_on4_int;
int minuten_off4_int;
float relais1_on;
float relais2_on;
float relais3_on;
float pwm_kw_float;
String relais1_override;
String relais2_override;
String relais3_override;
char relais_module_char[20];
char kw_on_char[12];
char override_char[8];
char schakel_delay_char[12];
char pwm_tijd_on_char[8];
char pwm_tijd_off_char[8];
char relais1_sturing_char[12];
char relais2_sturing_char[12];
char relais3_sturing_char[12];
unsigned long relais1_vertraging_long;
unsigned long relais2_vertraging_long;
unsigned long relais3_vertraging_long;

int uren;
int minuten;
String tijd_string = "         ";
char tijd_char[12];

bool pwm_tijd_gezet = false;
bool pwm_tijd_gezet_vorig;
int uitsturing_pwm_int = 0;
float uitsturing_pwm_float = 0.0;
String pwm_override;
char uitsturing_pwm_char[6];
char schakel_tijd_char[12];
char pwm_override_char[8];

const char* APSSID = "ESP32Energie";
const char* APPSWD = "ESP32pswd";

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  Serial.print("Packet to: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(macStr);
  Serial.print(" send status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

const char energie_html[] = R"rawliteral(
<!DOCTYPE HTML>
<html>
  <head>
    <iframe style="display:none" name="hidden-form"></iframe>
    <title>Energie Beheer</title>
    <style>
        div.kader {
          position: relative;
          width: 400px;
          height: 12x;
        }
          div.links{
          position: absolute;
          left : 0px;
          width; 100px;
          height: 12px;
        }
        div.links_midden{
          position:absolute;
          left:  120px;
          width: 100px
          height: 12px; 
        }
        div.midden{
          position:absolute;
          left: 150px;
          width: 100px
          height: 12px; 
        }
        div.titel{
          height: 25px;
          width: auto;
        }
        div.bottom{
          position: fixed;
          bottom: 0px;
        }
    </style>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
  </head>
  <body>
    <h3><center> ESP32 Slimme Meter Interface </center></h3>
    <small>
    <div class="titel"><center><b>Verbruik gegevens</b></center></div>
    <div class="kader">
      <div class="links">Totaal electriciteit : </div>
      <div class="midden">%electriciteit_totaal% &nbsp; KWh</div>
    </div>
    <br>
    <div class="kader">
      <div class="links">Totaal injectie : </div>
      <div class="midden">%injectie_totaal% &nbsp; KWh</div>
    </div>
    <br>
    <div class="kader">
      <div class="links">Verbruik nu : </div>
      <div class="midden">%kw_nu% &nbsp; KW</div>
    </div>
    <br>
    <div class="kader">
      <div class="links">Injectie nu : </div>
      <div class="midden">%injectie_nu% &nbsp; KW</div>
    </div>
    <br>
    <div class="kader">
      <div class="links">Totaal gas : </div>
      <div class="midden">%gas_totaal% &nbsp; m3</div>
    </div>
    <br><br>
    <div class="titel"><b><center>Tijd</center></b></div>
    <center><input type="text" style="text-align:center;" value="%tijd%" size=2></center>
    <br>
    <form action="/get" target="hidden-form">
    <br>
    <div class="titel"><b><center>Relais schakelwaarden</center></b></div>
    <center><input type= "text" style="text-align:center;" value="%relais_module%" size = 20></center>
    <br>
    <center>
    <b>KW:</b>&nbsp;<input type="text" style="text-align:center;" value="%kw_on%" name="input_kw_on" size=1>
    &nbsp; <b>V:</b>&nbsp;<input type="text" style="text-align:center;" value="%delay%" name="input_delay" size=1>
    &nbsp; <b>Tijd:</b>&nbsp;<input type="text" style="text-align:center;" value="%schakel_tijd%" name="input_schakel_tijd" size=1>
    &nbsp; <b>A/M:</b>&nbsp;<input type="text" style="text-align:center;" value="%override%" name="input_override" size=1>
    </center>
    <br>
    <center>
      <input type="submit" name="relais_module_min" value="   -   " onclick="ok()">
      &nbsp;&nbsp;&nbsp;
      <input type="submit" name="relais_module_plus" value="   +   " onclick="ok()">
      &nbsp;&nbsp;&nbsp;
      <input type="submit" name="relais_module_bevestig" value="OK" onclick="ok()">
    </center>
    </form>
    <br><br>
    <form action="/get" target="hidden-form">
    <div class="titel"><center><b>PWM sturing instellen</b></center></div>
    <center>
    <b>KW:</b>&nbsp;<input type="text" style="text-align:center;" value="%pwm_kw%" name="input_pwm_kw" size=1>
    &nbsp;<b>1:</b>&nbsp;<input type="text" style="text-align:center;" value="%pwm_tijd_on%" name="input_pwm_tijd_on" size=1>
    &nbsp;<b>0:</b>&nbsp;<input type="text" style="text-align:center;" value="%pwm_tijd_off%" name="input_pwm_tijd_off" size=1>
    &nbsp;<b>A/M:</b>&nbsp;<input type="text" style="text-align:center;" value="%pwm_override%" name="input_pwm_override" size=1>
    </center>
    <br>
    <center><input type="submit" name="bevestig_pwm" value="OK" onclick="ok()"></center>
    </form>
    <br><br>
    <div class="titel"><center><b>Huidige relais sturing</b></center></div>
    <div class="kader">
      <div class="links">Relais 1 : </div>
      <div class="links_midden">%relais1_sturing%</div>
    </div>
    <br>
    <div class="kader">
      <div class="links">Relais 2 : </div>
      <div class="links_midden">%relais2_sturing%</div>
    </div>
    <br>
    <div class="kader">
      <div class="links">Relais 3 : </div>
      <div class="links_midden">%relais3_sturing%</div>
    </div>
    <br>
    <div class="kader">
      <div class="links">PWM sturing : </div>
      <div class="links_midden">%procent% &#37 </div>
    </div>
    <br><br>
    <form action="/get" target="hidden-form">
    <div class="titel"><center><b>Ingeven MAC address</b></center></div>
    <center>
    <input type= "text" style="text-align:center;" value="%module%" size = 20>
    </center>
    <br>
    <center>
    <input type="text" style="text-align:center;" value="%display_macx_0%" name="input_macx_0" size=1>
    &nbsp;
    <input type="text" style="text-align:center;" value="%display_macx_1%" name="input_macx_1" size=1>
    &nbsp;
    <input type="text" style="text-align:center;" value="%display_macx_2%" name="input_macx_2" size=1>
    &nbsp;
    <input type="text" style="text-align:center;" value="%display_macx_3%" name="input_macx_3" size=1>
    &nbsp;
    <input type="text" style="text-align:center;" value="%display_macx_4%" name="input_macx_4" size=1>
    &nbsp;
    <input type="text" style="text-align:center;" value="%display_macx_5%" name="input_macx_5" size=1>
    </center>
    <br>
    <center>
      <input type="submit" name="module_min" value="   -   " onclick="ok()">
      &nbsp;&nbsp;&nbsp;
      <input type="submit" name="module_plus" value="   +   " onclick="ok()">
      &nbsp;&nbsp;&nbsp;
      <input type="submit" name="module_bevestig" value="OK" onclick="ok()">
    </center>
    </form>
    </small>
    <br>
    <br>
    <br>
    <h6><b>thieu-b55 april 2022</b></h6>
    <script>
      function ok(){
        setTimeout(function(){document.location.reload();},250);
      }
    </script>
  </body>  
</html>
)rawliteral";

String processor(const String& var){
  String temp = "                              ";
  String module = "                            ";
  int macx_0;
  int macx_1;
  int macx_2;
  int macx_3;
  int macx_4;
  int macx_5;
  if(var == "electriciteit_totaal"){
    temp = String(kwh_totaal_float, 3);
    temp.toCharArray(kwh_totaal_float_char, (temp.length() + 1));
    return(kwh_totaal_float_char);
  }
  if(var == "injectie_totaal"){
    temp = String(injectie_totaal_float, 3);
    temp.toCharArray(injectie_totaal_float_char, (temp.length() + 1));
    return(injectie_totaal_float_char);
  }
  if(var == "kw_nu"){
    temp = String(kw_nu_float, 3);
    temp.toCharArray(kw_nu_float_char, (temp.length() + 1));
    return(kw_nu_float_char);
  }
  if(var == "injectie_nu"){
    temp = String(injectie_nu_float, 3);
    temp.toCharArray(injectie_nu_float_char, (temp.length() + 1));
    return(injectie_nu_float_char);
  }
  if(var == "gas_totaal"){
    temp = String(gas_totaal_float, 3);
    temp.toCharArray(gas_totaal_float_char, (temp.length() + 1));
    return(gas_totaal_float_char);
  }
  if(var == "tijd"){
    sprintf(tijd_char, "%02d:%02d", uren, minuten);
    return(tijd_char);
  }
  if(var == "relais_module"){
    switch(relais_module_teller){
      case 0:
        temp = "Relais 1";
        break;
      case 1:
        temp = "Relais 2";
        break;
      case 2:
        temp = "Relais 3";
        break;
    }
    temp.toCharArray(relais_module_char, (temp.length() + 1));
    return(relais_module_char);
  }
  if(var == "kw_on"){
    switch(relais_module_teller){
      case 0:
        return(String(relais1_on));
        break;
      case 1:
        return(String(relais2_on));
        break;
      case 2:
        return(String(relais3_on));
        break;
    }
  }
  if(var == "override"){
    switch(relais_module_teller){
      case 0:
        temp = relais1_override;
        break;
      case 1:
        temp = relais2_override;
        break;
      case 2:
        temp = relais3_override;
        break;
    }
    temp.toCharArray(override_char, (temp.length() + 1));
    return(override_char);
  }
  if(var == "delay"){
    switch(relais_module_teller){
      case 0:
        return(String(relais1_delay));
        break;
      case 1:
        return(String(relais2_delay));
        break;
      case 2:
        return(String(relais3_delay));
        break;
    }
  }
  if(var == "schakel_tijd"){
    switch(relais_module_teller){
      case 0:
        sprintf(schakel_tijd_char, "%02d:%02d", uren_on1_int, minuten_on1_int);
        return(schakel_tijd_char);
        break;
      case 1:
        sprintf(schakel_tijd_char, "%02d:%02d", uren_on2_int, minuten_on2_int);
        return(schakel_tijd_char);
        break;
      case 2:
        sprintf(schakel_tijd_char, "%02d:%02d", uren_on3_int, minuten_on3_int);
        return(schakel_tijd_char);
    }
  }
  if(var == "pwm_kw"){
    return(String(pwm_kw_float));
  }
  if(var == "pwm_tijd_on"){
    sprintf(pwm_tijd_on_char, "%02d:%02d", uren_on4_int, minuten_on4_int);
    return(pwm_tijd_on_char);
  }
  if(var == "pwm_tijd_off"){
    sprintf(pwm_tijd_off_char, "%02d:%02d", uren_off4_int, minuten_off4_int);
    return(pwm_tijd_off_char);
  }
  if(var == "pwm_override"){
    pwm_override.toCharArray(pwm_override_char, (pwm_override.length() + 1));
    return(pwm_override_char);
  }
  
  if(var == "relais1_sturing"){
    if(relais1_uit == true){
      temp = "1";
    }
    if(relais1_uit == false){
      temp = "0";
    }
    if(relais1_override == "0"){
      temp = "0";
    }
    if(relais1_override == "1"){
      temp = "1";
    }
    temp.toCharArray(relais1_sturing_char, (temp.length() + 1));
    return(relais1_sturing_char);
  }
  if(var == "relais2_sturing"){
    if(relais2_uit == true){
      temp = "1";
    }
    if(relais2_uit == false){
      temp = "0";
    }
    if(relais2_override == "0"){
      temp = "0";
    }
    if(relais2_override == "1"){
      temp = "1";
    }
    temp.toCharArray(relais2_sturing_char, (temp.length() + 1));
    return(relais2_sturing_char);
  }
  if(var == "relais3_sturing"){
    if(relais3_uit == true){
      temp = "1";
    }
    if(relais3_uit == false){
      temp = "0";
    }
    if(relais3_override == "0"){
      temp = "0";
    }
    if(relais3_override == "1"){
      temp = "1";
    }
    temp.toCharArray(relais3_sturing_char, (temp.length() + 1));
    return(relais3_sturing_char);
  }
  if(var == "procent"){
    temp = String(uitsturing_pwm_int);
    temp.toCharArray(uitsturing_pwm_char, (temp.length() + 1));
    return(uitsturing_pwm_char);
  }
  switch(module_teller){
    case 0:
      module = "MAC address Display";
      macx_0 = broadcastAddress1[0];
      macx_1 = broadcastAddress1[1];
      macx_2 = broadcastAddress1[2];
      macx_3 = broadcastAddress1[3];
      macx_4 = broadcastAddress1[4];
      macx_5 = broadcastAddress1[5];
      break;
    case 1:
      module = "MAC address Relais 1";
      macx_0 = broadcastAddress2[0];
      macx_1 = broadcastAddress2[1];
      macx_2 = broadcastAddress2[2];
      macx_3 = broadcastAddress2[3];
      macx_4 = broadcastAddress2[4];
      macx_5 = broadcastAddress2[5];
      break;
    case 2:
      module = "MAC address Relais 2";
      macx_0 = broadcastAddress3[0];
      macx_1 = broadcastAddress3[1];
      macx_2 = broadcastAddress3[2];
      macx_3 = broadcastAddress3[3];
      macx_4 = broadcastAddress3[4];
      macx_5 = broadcastAddress3[5];
      break;
    case 3:
      module = "MAC address Relais 3";
      macx_0 = broadcastAddress4[0];
      macx_1 = broadcastAddress4[1];
      macx_2 = broadcastAddress4[2];
      macx_3 = broadcastAddress4[3];
      macx_4 = broadcastAddress4[4];
      macx_5 = broadcastAddress4[5];
      break;
    case 4:
      module = "MAC address PWM Sturing";
      macx_0 = broadcastAddress5[0];
      macx_1 = broadcastAddress5[1];
      macx_2 = broadcastAddress5[2];
      macx_3 = broadcastAddress5[3];
      macx_4 = broadcastAddress5[4];
      macx_5 = broadcastAddress5[5];
  }
  if(var == "module"){
    module.toCharArray(module_char, (module.length() + 1));
    return(module_char);
  }
  if(var == "display_macx_0"){
    sprintf(broadcastAddressX_0_char, "%02x%", macx_0);
    return(broadcastAddressX_0_char);
  }
  if(var == "display_macx_1"){
    sprintf(broadcastAddressX_1_char, "%02x%", macx_1);
    return(broadcastAddressX_1_char);
  }
  if(var == "display_macx_2"){
    sprintf(broadcastAddressX_2_char, "%02x%", macx_2);
    return(broadcastAddressX_2_char);;
  }
  if(var == "display_macx_3"){
    sprintf(broadcastAddressX_3_char, "%02x%", macx_3);
    return(broadcastAddressX_3_char);
  }
  if(var == "display_macx_4"){
    sprintf(broadcastAddressX_4_char, "%02x%", macx_4);
    return(broadcastAddressX_4_char);
  }
  if(var == "display_macx_5"){
    sprintf(broadcastAddressX_5_char, "%02x%", macx_5);
    return(broadcastAddressX_5_char);
  }
}

void html_input(){
  server.begin();
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
  request->send_P(200, "text/html", energie_html, processor);
  });
  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request){
    char terminator = char(0x0a);
    String temp = "                        ";
    char temp_char[30];
    float kw_on;
    float kw_off;
    String override = "           ";
    int schakel_delay;
    char char_temp[10];
    bool fout;
    String uren_string = "        ";
    String minuten_string = "        ";
    int uren_int;
    int minuten_int;    
    
    if(request->hasParam(INPUT_KW_ON)){
      temp = ((request->getParam(INPUT_KW_ON)->value()) + String(terminator));
      temp.replace(',', '.');
      kw_on = temp.toFloat();
    }
    if(request->hasParam(INPUT_DELAY)){
      schakel_delay = ((request->getParam(INPUT_DELAY)->value()) + String(terminator)).toInt();
      if(schakel_delay < 10){
        schakel_delay = 10;
      }
    }
    if(request->hasParam(INPUT_SCHAKEL_TIJD)){
      temp = ((request->getParam(INPUT_SCHAKEL_TIJD)->value()) + String(terminator));
      if(temp.length() == 6){
        uren_string = temp.substring(0, 2);
        minuten_string = temp.substring(3,5);
        uren_int = uren_string.toInt();
        minuten_int = minuten_string.toInt();
        if((uren_int >= 0) && (uren_int <= 24)){
          if((minuten_int >= 0) && (minuten_int <= 59)){
            switch(relais_module_teller){
              case 0:
                uren_on1_int = uren_int;
                minuten_on1_int = minuten_int;
                pref.putInt("uren_on1", uren_int);
                pref.putInt("minuten_on1", minuten_int);
                break;
              case 1:
                uren_on2_int = uren_int;
                minuten_on2_int = minuten_int;
                pref.putInt("uren_on1", uren_int);
                pref.putInt("minuten_on1", minuten_int);
                break;
              case 2:
                uren_on3_int = uren_int;
                minuten_on3_int = minuten_int;
                pref.putInt("uren_on1", uren_int);
                pref.putInt("minuten_on1", minuten_int);
                break;
            }
          }
        }
      }
    }
    if(request->hasParam(INPUT_OVERRIDE)){
      override = (request->getParam(INPUT_OVERRIDE)->value());
      override.toCharArray(char_temp, (override.length() + 1));
      switch(int(char_temp[0])){
        case 48:          //0
          break;
        case 49:          //1
          break;
        case 97:          //a
          override = "A";
          break;
        case 65:          //A
          break;
        default:
          override = "0";
      }
    }
    if(request->hasParam(RELAIS_MODULE_MIN)){
      relais_module_teller --;
      if(relais_module_teller < 0){
        relais_module_teller = 2;
      }
    }
    if(request->hasParam(RELAIS_MODULE_PLUS)){
      relais_module_teller ++;
      if(relais_module_teller > 2){
        relais_module_teller = 0;
      }
    }
    if(request->hasParam(RELAIS_MODULE_BEVESTIG)){
      switch(relais_module_teller){
        case 0:
          relais1_vertraging_long = millis();
          pref.putFloat("relais1_on", kw_on);
          pref.putString("relais1_ov", override);
          pref.putInt("relais1_del", schakel_delay);
          relais1_on = pref.getFloat("relais1_on");
          relais1_override = pref.getString("relais1_ov");
          relais1_delay = pref.getInt("relais1_del");
          if(relais1_override == "1"){
            relais1_uit = true;
            uitsturen.relais = true;
            result = esp_now_send(broadcastAddress2, (uint8_t *) &uitsturen, sizeof(uitsturen));
            if (result == ESP_OK) {
              Serial.println("Met succes verzonden relais 1");
            }
            else {
              Serial.println("fout bij verzenden naar relais 1");
            }
          }
          else{
            relais1_uit = false;
            uitsturen.relais = false;
            result = esp_now_send(broadcastAddress2, (uint8_t *) &uitsturen, sizeof(uitsturen));
            if (result == ESP_OK) {
              Serial.println("Met succes verzonden relais 1");
            }
            else {
              Serial.println("fout bij verzenden naar relais 1");
            }
          }
          break;
        case 1:
          relais2_vertraging_long = millis();
          pref.putFloat("relais2_on", kw_on);
          pref.putString("relais2_ov", override);
          pref.putInt("relais2_del", schakel_delay);
          relais2_on = pref.getFloat("relais2_on");
          relais2_override = pref.getString("relais2_ov");
          relais2_delay = pref.getInt("relais2_del");
          if(relais2_override == "1"){
            relais2_uit = true;
            uitsturen.relais = true;
            result = esp_now_send(broadcastAddress3, (uint8_t *) &uitsturen, sizeof(uitsturen));
            if (result == ESP_OK) {
              Serial.println("Met succes verzonden relais 2");
            }
            else {
              Serial.println("fout bij verzenden naar relais 2");
            }
          }
          else{
            relais2_uit = false;
            uitsturen.relais = false;
            result = esp_now_send(broadcastAddress3, (uint8_t *) &uitsturen, sizeof(uitsturen));
            if (result == ESP_OK) {
              Serial.println("Met succes verzonden relais 2");
            }
            else {
              Serial.println("fout bij verzenden naar relais 2");
            }
          }
          break;
        case 2:
          relais3_vertraging_long = millis();
          pref.putFloat("relais3_on", kw_on);
          pref.putString("relais3_ov", override);
          pref.putInt("relais3_del",schakel_delay);
          relais3_on = pref.getFloat("relais3_on");
          relais3_override = pref.getString("relais3_ov");
          relais3_delay = pref.getInt("relais3_del");
          if(relais3_override == "1"){
            relais3_uit = true;
            uitsturen.relais = true;
            result = esp_now_send(broadcastAddress4, (uint8_t *) &uitsturen, sizeof(uitsturen));
            if (result == ESP_OK) {
              Serial.println("Met succes verzonden relais 3");
            }
            else {
              Serial.println("fout bij verzenden naar relais 3");
            }
          }
          else{
            relais3_uit = false;
            uitsturen.relais = false;
            result = esp_now_send(broadcastAddress4, (uint8_t *) &uitsturen, sizeof(uitsturen));
            if (result == ESP_OK) {
              Serial.println("Met succes verzonden relais 3");
            }
            else {
              Serial.println("fout bij verzenden naar relais 3");
            }
          }
          break;
      }
    }
    if(request->hasParam(INPUT_PWM_KW)){
      temp = ((request->getParam(INPUT_PWM_KW)->value()) + String(terminator));
      temp.replace(',', '.');
      pwm_kw_float = temp.toFloat();
    }
    if(request->hasParam(INPUT_PWM_TIJD_ON)){
      temp = ((request->getParam(INPUT_PWM_TIJD_ON)->value()) + String(terminator));
      if(temp.length() == 6){
        uren_string = temp.substring(0, 2);
        minuten_string = temp.substring(3,5);
        uren_int = uren_string.toInt();
        minuten_int = minuten_string.toInt();
        if((uren_int >= 0) && (uren_int <= 24)){
          if((minuten_int >= 0) && (minuten_int <= 59)){
            uren_on4_int = uren_int;
            minuten_on4_int = minuten_int;
          }
        }
      }
    }
    if(request->hasParam(INPUT_PWM_TIJD_OFF)){
     temp = ((request->getParam(INPUT_PWM_TIJD_OFF)->value()) + String(terminator));
     if(temp.length() == 6){
       uren_string = temp.substring(0, 2);
       minuten_string = temp.substring(3,5);
       uren_int = uren_string.toInt();
       minuten_int = minuten_string.toInt();
       if((uren_int >= 0) && (uren_int <= 23)){
         if((minuten_int >= 0) && (minuten_int <= 59)){
           uren_off4_int = uren_int;
           minuten_off4_int = minuten_int;
         }
       }
     }
    }
    if(request->hasParam(INPUT_PWM_OVERRIDE)){
      pwm_override = (request->getParam(INPUT_PWM_OVERRIDE)->value());
      pwm_override.toCharArray(char_temp, (override.length() + 1));
      switch(int(char_temp[0])){
        case 48:          //0
          break;
        case 49:          //1
          break;
        case 97:          //a
          pwm_override = "A";
          break;
        case 65:          //A
          break;
        default:
          pwm_override = "0";
      }
    }
    if(request->hasParam(BEVESTIG_PWM)){
      pref.putFloat("pwm_kw", pwm_kw_float);
      pref.putInt("uren_on4", uren_on4_int);
      pref.putInt("minuten_on4", minuten_on4_int);
      pref.putInt("uren_off4", uren_off4_int);
      pref.putInt("minuten_off4", minuten_off4_int);
      pref.putString("pwm_override", pwm_override);
      pwm_kw_float = pref.getFloat("pwm_kw");
      uren_on4_int = pref.getInt("uren_on4");
      minuten_on4_int = pref.getInt("minuten_on4");
      uren_off4_int = pref.getInt("uren_off4");
      minuten_off4_int = pref.getInt("minuten_off4");
      pwm_override = pref.getString("pwm_override");
      if(pwm_override == "A"){
        uitsturing_pwm_float = 0.0;
        uitsturing_pwm_int = 0;
      }
      /*
       * tijdsturing uitchakelen bij Manueel 0 tijdens tijdsturing
       */
      if(pwm_override == "0"){
        pwm_tijd_gezet = false;
      }
    }
    if(request->hasParam(INPUT_MACX_0)){
      temp = ((request->getParam(INPUT_MACX_0)->value()) + String(terminator));
      temp.toCharArray(temp_char, (temp.length() +1));
      input_macx_0 = strtol(temp_char, 0, 16);
    }
    if(request->hasParam(INPUT_MACX_1)){
      temp = ((request->getParam(INPUT_MACX_1)->value()) + String(terminator));
      temp.toCharArray(temp_char, (temp.length() +1));
      input_macx_1 = strtol(temp_char, 0, 16);
    }
    if(request->hasParam(INPUT_MACX_2)){
      temp = ((request->getParam(INPUT_MACX_2)->value()) + String(terminator));
      temp.toCharArray(temp_char, (temp.length() +1));
      input_macx_2 = strtol(temp_char, 0, 16);
    }
    if(request->hasParam(INPUT_MACX_3)){
      temp = ((request->getParam(INPUT_MACX_3)->value()) + String(terminator));
      temp.toCharArray(temp_char, (temp.length() +1));
      input_macx_3 = strtol(temp_char, 0, 16);
    }
    if(request->hasParam(INPUT_MACX_4)){
      temp = ((request->getParam(INPUT_MACX_4)->value()) + String(terminator));
      temp.toCharArray(temp_char, (temp.length() +1));
      input_macx_4 = strtol(temp_char, 0, 16);
    }
    if(request->hasParam(INPUT_MACX_5)){
      temp = ((request->getParam(INPUT_MACX_5)->value()) + String(terminator));
      temp.toCharArray(temp_char, (temp.length() +1));
      input_macx_5 = strtol(temp_char, 0, 16);
    }
    if(request->hasParam(MODULE_MIN)){
      module_teller --;
      if(module_teller < 0){
        module_teller = 4;
      }
    }
    if(request->hasParam(MODULE_PLUS)){
      module_teller ++;
      if(module_teller > 4){
        module_teller = 0;
      }
    }
    if(request->hasParam(MODULE_BEVESTIG)){
      temp = "";
      fout = false;
      temp = temp + String(input_macx_0)+ String(input_macx_1)+ String(input_macx_2)
                  + String(input_macx_3)+ String(input_macx_4)+ String(input_macx_5);
      if(broadcastAddress1_string == temp){
        fout = true;
      }
      if(broadcastAddress2_string == temp){
        fout = true;
      }
      if(broadcastAddress3_string == temp){
        fout = true;
      }
      if(broadcastAddress4_string == temp){
        fout = true;
      }
      if(broadcastAddress5_string == temp){
        fout = true;
      }
      if(fout == false){
        switch(module_teller){
          case 0:
            pref.putInt("mac1_0", input_macx_0);
            pref.putInt("mac1_1", input_macx_1);
            pref.putInt("mac1_2", input_macx_2);
            pref.putInt("mac1_3", input_macx_3);
            pref.putInt("mac1_4", input_macx_4);
            pref.putInt("mac1_5", input_macx_5);
            broadcastAddress1[0] = pref.getInt("mac1_0");
            broadcastAddress1[1] = pref.getInt("mac1_1");
            broadcastAddress1[2] = pref.getInt("mac1_2");
            broadcastAddress1[3] = pref.getInt("mac1_3");
            broadcastAddress1[4] = pref.getInt("mac1_4");
            broadcastAddress1[5] = pref.getInt("mac1_5");
            break;
          case 1:
            pref.putInt("mac2_0", input_macx_0);
            pref.putInt("mac2_1", input_macx_1);
            pref.putInt("mac2_2", input_macx_2);
            pref.putInt("mac2_3", input_macx_3);
            pref.putInt("mac2_4", input_macx_4);
            pref.putInt("mac2_5", input_macx_5);
            broadcastAddress2[0] = pref.getInt("mac2_0");
            broadcastAddress2[1] = pref.getInt("mac2_1");
            broadcastAddress2[2] = pref.getInt("mac2_2");
            broadcastAddress2[3] = pref.getInt("mac2_3");
            broadcastAddress2[4] = pref.getInt("mac2_4");
            broadcastAddress2[5] = pref.getInt("mac2_5");
            break;
          case 2:
            pref.putInt("mac3_0", input_macx_0);
            pref.putInt("mac3_1", input_macx_1);
            pref.putInt("mac3_2", input_macx_2);
            pref.putInt("mac3_3", input_macx_3);
            pref.putInt("mac3_4", input_macx_4);
            pref.putInt("mac3_5", input_macx_5);
            broadcastAddress3[0] = pref.getInt("mac3_0");
            broadcastAddress3[1] = pref.getInt("mac3_1");
            broadcastAddress3[2] = pref.getInt("mac3_2");
            broadcastAddress1[3] = pref.getInt("mac3_3");
            broadcastAddress3[4] = pref.getInt("mac3_4");
            broadcastAddress3[5] = pref.getInt("mac3_5");
            break;
          case 3:
            pref.putInt("mac4_0", input_macx_0);
            pref.putInt("mac4_1", input_macx_1);
            pref.putInt("mac4_2", input_macx_2);
            pref.putInt("mac4_3", input_macx_3);
            pref.putInt("mac4_4", input_macx_4);
            pref.putInt("mac4_5", input_macx_5);
            broadcastAddress4[0] = pref.getInt("mac4_0");
            broadcastAddress4[1] = pref.getInt("mac4_1");
            broadcastAddress4[2] = pref.getInt("mac4_2");
            broadcastAddress4[3] = pref.getInt("mac4_3");
            broadcastAddress4[4] = pref.getInt("mac4_4");
            broadcastAddress4[5] = pref.getInt("mac4_5");
            break;
          case 4:
            pref.putInt("mac5_0", input_macx_0);
            pref.putInt("mac5_1", input_macx_1);
            pref.putInt("mac5_2", input_macx_2);
            pref.putInt("mac5_3", input_macx_3);
            pref.putInt("mac5_4", input_macx_4);
            pref.putInt("mac5_5", input_macx_5);
            broadcastAddress5[0] = pref.getInt("mac5_0");
            broadcastAddress5[1] = pref.getInt("mac5_1");
            broadcastAddress5[2] = pref.getInt("mac5_2");
            broadcastAddress5[3] = pref.getInt("mac5_3");
            broadcastAddress5[4] = pref.getInt("mac5_4");
            broadcastAddress5[5] = pref.getInt("mac5_5");
            break;
        }
        delay(2000);
        ESP.restart();
      }
    }
  });
}

void setup() {
  delay(5000);
  pinMode(BLINKIE, OUTPUT);
  digitalWrite(BLINKIE, 0);
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  pref.begin("data", false);
  if(pref.getString("controle") != "dummy geladen"){
    pref.putInt("mac1_0", 0x30);
    pref.putInt("mac1_1", 0xae);
    pref.putInt("mac1_2", 0xa4);
    pref.putInt("mac1_3", 0x0d);
    pref.putInt("mac1_4", 0x69);
    pref.putInt("mac1_5", 0xb8);
    pref.putInt("mac2_0", 7);
    pref.putInt("mac2_1", 1);
    pref.putInt("mac2_2", 2);
    pref.putInt("mac2_3", 3);
    pref.putInt("mac2_4", 4);
    pref.putInt("mac2_5", 7);
    pref.putInt("mac3_0", 2);
    pref.putInt("mac3_1", 1);
    pref.putInt("mac3_2", 2);
    pref.putInt("mac3_3", 3);
    pref.putInt("mac3_4", 4);
    pref.putInt("mac3_5", 5);
    pref.putInt("mac4_0", 6);
    pref.putInt("mac4_1", 7);
    pref.putInt("mac4_2", 8);
    pref.putInt("mac4_3", 9);
    pref.putInt("mac4_4", 0);
    pref.putInt("mac4_5", 1);
    pref.putInt("mac5_0", 0x7c);
    pref.putInt("mac5_1", 0x9e);
    pref.putInt("mac5_2", 0xbd);
    pref.putInt("mac5_3", 0x06);
    pref.putInt("mac5_4", 0xb4);
    pref.putInt("mac5_5", 0xdc);
    pref.putFloat("relais1_on", 2.0);
    pref.putString("relais1_ov", "0");
    pref.putInt("relais1_del", 10);
    pref.putFloat("relais2_on", 2.0);
    pref.putString("relais2_ov", "0");
    pref.putInt("relais2_del", 10);
    pref.putFloat("relais3_on", 2.0);
    pref.putString("relais3_ov", "0");
    pref.putInt("relais3_del", 10);
    pref.putInt("uren_on1", 24);
    pref.putInt("minuten_on1", 0);
    pref.putInt("uren_on2", 24);
    pref.putInt("minuten_on2", 0);
    pref.putInt("uren_on3", 24);
    pref.putInt("minuten_on3", 0);
    pref.putFloat("pwm_kw", 0.0);
    pref.putInt("uren_on4", 24);
    pref.putInt("minuten_on4", 0);
    pref.putInt("uren_off4", 0);
    pref.putInt("minuten_off4", 0);
    pref.putString("pwm_override", "0");
    pref.putString("controle", "dummy geladen");
  }
  broadcastAddress1[0] = pref.getInt("mac1_0");
  broadcastAddress1[1] = pref.getInt("mac1_1");
  broadcastAddress1[2] = pref.getInt("mac1_2");
  broadcastAddress1[3] = pref.getInt("mac1_3");
  broadcastAddress1[4] = pref.getInt("mac1_4");
  broadcastAddress1[5] = pref.getInt("mac1_5");
  broadcastAddress2[0] = pref.getInt("mac2_0");
  broadcastAddress2[1] = pref.getInt("mac2_1");
  broadcastAddress2[2] = pref.getInt("mac2_2");
  broadcastAddress2[3] = pref.getInt("mac2_3");
  broadcastAddress2[4] = pref.getInt("mac2_4");
  broadcastAddress2[5] = pref.getInt("mac2_5");
  broadcastAddress3[0] = pref.getInt("mac3_0");
  broadcastAddress3[1] = pref.getInt("mac3_1");
  broadcastAddress3[2] = pref.getInt("mac3_2");
  broadcastAddress3[3] = pref.getInt("mac3_3");
  broadcastAddress3[4] = pref.getInt("mac3_4");
  broadcastAddress3[5] = pref.getInt("mac3_5");
  broadcastAddress4[0] = pref.getInt("mac4_0");
  broadcastAddress4[1] = pref.getInt("mac4_1");
  broadcastAddress4[2] = pref.getInt("mac4_2");
  broadcastAddress4[3] = pref.getInt("mac4_3");
  broadcastAddress4[4] = pref.getInt("mac4_4");
  broadcastAddress4[5] = pref.getInt("mac4_5");
  broadcastAddress5[0] = pref.getInt("mac5_0");
  broadcastAddress5[1] = pref.getInt("mac5_1");
  broadcastAddress5[2] = pref.getInt("mac5_2");
  broadcastAddress5[3] = pref.getInt("mac5_3");
  broadcastAddress5[4] = pref.getInt("mac5_4");
  broadcastAddress5[5] = pref.getInt("mac5_5");
  relais1_on = pref.getFloat("relais1_on");
  relais1_override = pref.getString("relais1_ov");
  relais1_delay = pref.getInt("relais1_del");
  relais2_on = pref.getFloat("relais2_on");
  relais2_override = pref.getString("relais2_ov");
  relais2_delay = pref.getInt("relais2_del");
  relais3_on = pref.getFloat("relais3_on");
  relais3_override = pref.getString("relais3_ov");
  relais3_delay = pref.getInt("relais3_del");
  uren_on1_int = pref.getInt("uren_on1");
  minuten_on1_int = pref.getInt("minuten_on1");
  uren_on2_int = pref.getInt("uren_on2");
  minuten_on2_int = pref.getInt("minuten_on2");
  uren_on3_int = pref.getInt("uren_on3");
  minuten_on3_int = pref.getInt("minuten_on3");
  pwm_kw_float = pref.getFloat("pwm_kw");
  uren_on4_int = pref.getInt("uren_on4");
  minuten_on4_int = pref.getInt("minuten_on4");
  uren_off4_int = pref.getInt("uren_off4");
  minuten_off4_int = pref.getInt("minuten_off4");
  pwm_override = pref.getString("pwm_override");
  broadcastAddress1_string = "";
  broadcastAddress1_string = broadcastAddress1_string + String(broadcastAddress1[0])+ String(broadcastAddress1[1])
                                                      + String(broadcastAddress1[2])+ String(broadcastAddress1[3])
                                                      + String(broadcastAddress1[4])+ String(broadcastAddress1[5]);
  broadcastAddress2_string = "";
  broadcastAddress2_string = broadcastAddress2_string + String(broadcastAddress2[0])+ String(broadcastAddress2[1])
                                                      + String(broadcastAddress2[2])+ String(broadcastAddress2[3])
                                                      + String(broadcastAddress2[4])+ String(broadcastAddress2[5]);
  broadcastAddress3_string = "";
  broadcastAddress3_string = broadcastAddress3_string + String(broadcastAddress3[0])+ String(broadcastAddress3[1])
                                                      + String(broadcastAddress3[2])+ String(broadcastAddress3[3])
                                                      + String(broadcastAddress3[4])+ String(broadcastAddress3[5]);                            
  broadcastAddress4_string = "";
  broadcastAddress4_string = broadcastAddress4_string + String(broadcastAddress4[0])+ String(broadcastAddress4[1])
                                                      + String(broadcastAddress4[2])+ String(broadcastAddress4[3])
                                                      + String(broadcastAddress4[4])+ String(broadcastAddress4[5]);
  broadcastAddress5_string = "";
  broadcastAddress5_string = broadcastAddress5_string + String(broadcastAddress5[0])+ String(broadcastAddress5[1])
                                                      + String(broadcastAddress5[2])+ String(broadcastAddress5[3])
                                                      + String(broadcastAddress5[4])+ String(broadcastAddress5[5]); 
  WiFi.disconnect();                                                      
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(APSSID, APPSWD);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  esp_now_register_send_cb(OnDataSent);
   
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
   
  memcpy(peerInfo.peer_addr, broadcastAddress1, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer 1");
    return;
  }
  memcpy(peerInfo.peer_addr, broadcastAddress2, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer 2");
    return;
  }
  memcpy(peerInfo.peer_addr, broadcastAddress3, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer 3");
    return;
  }
  memcpy(peerInfo.peer_addr, broadcastAddress4, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer 4");
    return;
  }
  memcpy(peerInfo.peer_addr, broadcastAddress5, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer 5");
    return;
  }
  delay(1000);
  Serial.println(relais1_override);
  if(relais1_override == "0"){
    uitsturen.relais = false;
    relais1_uit = false;
  }
  if(relais1_override == "1"){
    uitsturen.relais = true;
    relais1_uit = true;
  }
  result = esp_now_send(broadcastAddress2, (uint8_t *) &uitsturen, sizeof(uitsturen));
  if (result == ESP_OK) {
    Serial.println("Met succes verzonden relais 1");
  }
  else {
    Serial.println("fout bij verzenden naar relais 1");
  }
  delay(1000);
  if(relais2_override == "0"){
    uitsturen.relais = false;
    relais2_uit = false;
  }
  if(relais2_override == "1"){
    uitsturen.relais = true;
    relais2_uit = true;
  }
  result = esp_now_send(broadcastAddress3, (uint8_t *) &uitsturen, sizeof(uitsturen));
  if (result == ESP_OK) {
    Serial.println("Met succes verzonden relais 2");
  }
  else {
    Serial.println("fout bij verzenden naar relais 2");
  }
  delay(1000);
  if(relais3_override == "0"){
    uitsturen.relais = false;
    relais3_uit = false;
  }
  if(relais3_override == "1"){
    uitsturen.relais = true;
    relais3_uit = true;
  }
  result = esp_now_send(broadcastAddress4, (uint8_t *) &uitsturen, sizeof(uitsturen));
  if (result == ESP_OK) {
    Serial.println("Met succes verzonden relais 3");
  }
  else {
    Serial.println("fout bij verzenden naar relais 3");
  }
  delay(1000);
  html_input();
  nu = millis();
}

void loop() {
  while(Serial2.available()){
    char lees_byte = Serial2.read();
    if(lees_byte == 0x2f){
      kwh_dag_float = kwh_dag.toFloat();
      kwh_nacht_float = kwh_nacht.toFloat();
      kwh_totaal_float = kwh_nacht_float + kwh_dag_float;
      injectie_dag_float = injectie_dag.toFloat();
      injectie_nacht_float = injectie_nacht.toFloat();
      injectie_totaal_float = injectie_nacht_float + injectie_dag_float;
      kw_nu_float = kw_nu.toFloat();
      injectie_nu_float = injectie_nu.toFloat();
      verbruik_nu_float = injectie_nu_float - kw_nu_float;
      gas_totaal_float = gas.toFloat();
      ingelezen.kwh_totaal = kwh_totaal_float;
      ingelezen.injectie_totaal = injectie_totaal_float;
      ingelezen.verbruik_nu = kw_nu_float;
      ingelezen.injectie_nu = injectie_nu_float;
      ingelezen.gas_totaal = gas_totaal_float;
      ingelezen.relais1 = relais1_uit;
      ingelezen.relais2 = relais2_uit;
      ingelezen.relais3 = relais3_uit;
      ingelezen.pwm_sturing = uitsturing_pwm_int;
      digitalWrite(BLINKIE, (digitalRead(BLINKIE) ^ 1));
      if(((millis() - nu) > 5000) && (!vijf_seconden)){
        vijf_seconden = true;
        pwm_tijd_gezet_vorig = pwm_tijd_gezet;
        if((uren_on4_int == uren) && (minuten_on4_int == minuten)){
          pwm_tijd_gezet = true;
        }
        if((uren_off4_int == uren) && (minuten_off4_int == minuten)){
          pwm_tijd_gezet = false;
        }
        if((pwm_tijd_gezet_vorig) && (!pwm_tijd_gezet)){
          uitsturing_pwm_int = 0;
        }
        if(pwm_tijd_gezet == false){
          uitsturing_pwm_float = uitsturing_pwm_float + ((verbruik_nu_float / pwm_kw_float) / 2);
          if(uitsturing_pwm_float > 1.0){
            uitsturing_pwm_float = 1.0;
          }
          if(uitsturing_pwm_float < 0.0){
            uitsturing_pwm_float = 0.0;
          }
          uitsturing_pwm_int = uitsturing_pwm_float * 100;
        }
        else{
          uitsturing_pwm_int = 100;
        }
        if(pwm_override == "0"){
          uitsturing_pwm_int = 0;
          uitsturing_pwm_float = 0.0;
        }
        if(pwm_override == "1"){
          uitsturing_pwm_int = 100;
        }
        pwm_sturing.procent = uitsturing_pwm_int;
        result = esp_now_send(broadcastAddress5, (uint8_t *) &pwm_sturing, sizeof(pwm_sturing));
        if (result == ESP_OK) {
          Serial.println("Sent with success");
        }
        else {
          Serial.println("Error sending the data");
        }
      }
      if((millis() - nu) > 10000){
        nu = millis();
        vijf_seconden = false;
        /*
        * Pulse sturing
        */
        pwm_tijd_gezet_vorig = pwm_tijd_gezet;
        if((uren_on4_int == uren) && (minuten_on4_int == minuten)){
          pwm_tijd_gezet = true;
        }
        if((uren_off4_int == uren) && (minuten_off4_int == minuten)){
          pwm_tijd_gezet = false;
        }
        if((pwm_tijd_gezet_vorig) && (!pwm_tijd_gezet)){
          uitsturing_pwm_int = 0;
        }
        if(pwm_tijd_gezet == false){
          uitsturing_pwm_float = uitsturing_pwm_float + ((verbruik_nu_float / pwm_kw_float) / 2);
          if(uitsturing_pwm_float > 1.0){
            uitsturing_pwm_float = 1.0;
          }
          if(uitsturing_pwm_float < 0.0){
            uitsturing_pwm_float = 0.0;
          }
          uitsturing_pwm_int = uitsturing_pwm_float * 100;
        }
        else{
          uitsturing_pwm_int = 100;
        }
        if(pwm_override == "0"){
          uitsturing_pwm_int = 0;
          uitsturing_pwm_float = 0.0;
        }
        if(pwm_override == "1"){
          uitsturing_pwm_int = 100;
        }
        pwm_sturing.procent = uitsturing_pwm_int;
        result = esp_now_send(broadcastAddress1, (uint8_t *) &ingelezen, sizeof(ingelezen));
        if (result == ESP_OK) {
          Serial.println("Sent with success");
        }
        else {
          Serial.println("Error sending the data");
        }
        result = esp_now_send(broadcastAddress5, (uint8_t *) &pwm_sturing, sizeof(pwm_sturing));
        if (result == ESP_OK) {
          Serial.println("Sent with success");
        }
        else {
          Serial.println("Error sending the data");
        }
      }
      buffer_data = "";
    }
    buffer_data += lees_byte;
    if(lees_byte == 0x0a){
      if((buffer_data.substring(4,9)) == "1.0.0"){
        uren = (buffer_data.substring(16,18)).toInt();;
        minuten = (buffer_data.substring(18,20)).toInt();;
      }
      if((buffer_data.substring(4,9)) == "1.8.1"){
        kwh_dag = buffer_data.substring(10,20);
      }
      if((buffer_data.substring(4,9)) == "1.8.2"){
        kwh_nacht = buffer_data.substring(10,20);
      }
      if((buffer_data.substring(4,9)) == "2.8.1"){
        injectie_dag = buffer_data.substring(10,20);
      }
      if((buffer_data.substring(4,9)) == "2.8.2"){
        injectie_nacht = buffer_data.substring(10,20);
      }
      if((buffer_data.substring(4,9)) == "1.7.0"){
        kw_nu = buffer_data.substring(10,16);
      }
      if((buffer_data.substring(4,9)) == "2.7.0"){
        injectie_nu = buffer_data.substring(10,16);
      }
      if((buffer_data.substring(4,10)) == "24.2.3"){
        gas = buffer_data.substring(26,35);
      }
      //Serial.print(buffer_data);
      buffer_data = "";
    }
  }
  if(pwm_override == "1"){
    verbruik_pwm_float = verbruik_nu_float; 
  }
  else{
    verbruik_pwm_float = verbruik_nu_float+ (uitsturing_pwm_float * pwm_kw_float);
  }
  
  /*
   * Relais 1
   */
  if(relais1_override == "0"){
    relais1_vertraging_long = millis();
  }
  if(relais1_override == "1"){
    relais1_vertraging_long = millis();
  }
  if((relais1_uit == false) && (relais1_override != "0")){
    if(relais1_on > verbruik_pwm_float){
      relais1_vertraging_long = millis();
    }
    if((millis() - relais1_vertraging_long) > (relais1_delay * 60000)){        //1 minuut = 60000 mS
      relais1_uit = true;
      uitsturen.relais = true;
      result = esp_now_send(broadcastAddress2, (uint8_t *) &uitsturen, sizeof(uitsturen));
      if (result == ESP_OK) {
        Serial.println("Met succes verzonden relais 1");
      }
      else {
        Serial.println("fout bij verzenden naar relais 1");
      }
    }
    relais2_vertraging_long = millis();
    relais3_vertraging_long = millis();
    if((uren == uren_on1_int) && (minuten == minuten_on1_int)){
      relais1_uit = true;
      uitsturen.relais = true;
      result = esp_now_send(broadcastAddress2, (uint8_t *) &uitsturen, sizeof(uitsturen));
      if (result == ESP_OK) {
        Serial.println("Met succes verzonden relais 1");
      }
      else {
        Serial.println("fout bij verzenden naar relais 1");
      }
    }
  }
   /*
   * Relais 2
   */
  if(relais2_override == "0"){
    relais2_vertraging_long = millis();
  }
  if(relais2_override == "1"){
    relais2_vertraging_long = millis();
  }
  if((relais2_uit == false) && (relais2_override != "0")){
    if(relais2_on > verbruik_pwm_float){
      relais2_vertraging_long = millis();
    }
    if((millis() - relais2_vertraging_long) > (relais2_delay * 60000)){        //1 minuut = 60000 mS
      relais2_vertraging_long = millis();
      relais2_uit = true;
      uitsturen.relais = true;
      result = esp_now_send(broadcastAddress3, (uint8_t *) &uitsturen, sizeof(uitsturen));
      if (result == ESP_OK) {
        Serial.println("Met succes verzonden relais 2");
      }
      else {
        Serial.println("fout bij verzenden naar relais 2");
      }
    }
    relais3_vertraging_long = millis();
    if((uren == uren_on2_int) && (minuten == minuten_on2_int)){
      relais2_uit = true;
      uitsturen.relais = true;
      result = esp_now_send(broadcastAddress3, (uint8_t *) &uitsturen, sizeof(uitsturen));
      if (result == ESP_OK) {
        Serial.println("Met succes verzonden relais 2");
      }
      else {
        Serial.println("fout bij verzenden naar relais 2");
      }
    }
  }
  /*
   * Relais 3
   */
  if(relais3_override == "0"){
    relais3_vertraging_long = millis();
  }
  if(relais3_override == "1"){
    relais3_vertraging_long = millis();
  }
  if((relais3_uit == false) && (relais3_override != "0")){
    if(relais3_on > verbruik_pwm_float){
      relais3_vertraging_long = millis();
    }
    if((millis() - relais3_vertraging_long) > (relais3_delay * 60000)){        //1 minuut = 60000 mS
      relais3_vertraging_long = millis();
      relais3_uit = true;
      uitsturen.relais = true;
      result = esp_now_send(broadcastAddress4, (uint8_t *) &uitsturen, sizeof(uitsturen));
      if (result == ESP_OK) {
        Serial.println("Met succes verzonden relais 3");
      }
      else {
        Serial.println("fout bij verzenden naar relais 3");
      }
    }
    if((uren == uren_on3_int) && (minuten == minuten_on3_int)){
      relais3_uit = true;
      uitsturen.relais = true;
      result = esp_now_send(broadcastAddress4, (uint8_t *) &uitsturen, sizeof(uitsturen));
      if (result == ESP_OK) {
        Serial.println("Met succes verzonden relais 3");
      }
      else {
        Serial.println("fout bij verzenden naar relais 3");
      }
    }
  }
}
