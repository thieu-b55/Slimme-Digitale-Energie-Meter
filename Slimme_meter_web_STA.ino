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
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);

Preferences pref;

#define RXD2          16
#define TXD2          17
#define BLINKIE       4

bool netwerk;

String buffer_data =    "                                                         ";
String kwh_dag = "          ";
String kwh_nacht = "          ";
String injectie_dag = "          ";
String injectie_nacht = "          ";
String kw_nu = "          ";
String injectie_nu = "          ";
String gas = "         ";
String ssid_string = "                    ";
String pswd_string = "                    ";

unsigned long nu;
unsigned long wacht_op_netwerk;

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

char kwh_totaal_float_char[12];
char injectie_totaal_float_char[12];
char kw_nu_float_char[12];
char injectie_nu_float_char[12];
char gas_totaal_float_char[12];
char ssid[40];
char pswd[40];

const char* APSSID = "ESP32Energie";
const char* APPSWD = "ESP32pswd";
const char* STA_SSID = "ssid";
const char* STA_PSWD = "pswd";

const char energie_html[] = R"rawliteral(
<!DOCTYPE HTML>
<html>
  <head>
    <iframe style="display:none" name="hidden-form"></iframe>
    <title>Energie Beheer</title>
    <meta http-equiv="refresh" content="15">
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
          bottom: 25px;
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
      <div class="midden">%electriciteit_totaal% &nbsp; kWh</div>
    </div>
    <br>
    <div class="kader">
      <div class="links">Totaal injectie : </div>
      <div class="midden">%injectie_totaal% &nbsp; kWh</div>
    </div>
    <br>
    <div class="kader">
      <div class="links">Verbruik nu : </div>
      <div class="midden">%kw_nu% &nbsp; kWh</div>
    </div>
    <br>
    <div class="kader">
      <div class="links">Injectie nu : </div>
      <div class="midden">%injectie_nu% &nbsp; kWh</div>
    </div>
    <br>
    <div class="kader">
      <div class="links">Totaal gas : </div>
      <div class="midden">%gas_totaal% &nbsp; m3</div>
    </div>
    <br>
    </small>
    <div class="bottom"><h6>thieu september 2022</h6></div>
  </body>  
</html>
)rawliteral";

const char netwerk_html[] = R"rawliteral(
<!DOCTYPE HTML>
<html>
  <head>
    <iframe style="display:none" name="hidden-form"></iframe>
    <title>Energie Beheer</title>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
  </head>
  <body>
  <h5><center><strong>ESP32 Netwerk instellingen</strong></center></h5>
    <form action="/get">
    <table style="width:100%;">
      <tr>
        <td style="text-align:left;"><p><small><b><labelfor="dummy">ssid :</label></b></small></p></td>
        <td style="text-align:center;"><input type= "text" name="ssid"></td>
      </tr>
      <tr>
        <td style="text-align:left;"><p><small><b><labelfor="dummy">pswd : </label></b></small></p></td>
        <td style="text-align:center;"><input type= "text" name="pswd"></td>
      </tr>
    </table>
    <center><input type="submit" value="Bevestig" onclick="ok()"></center>
    </form>
    <br>
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
}

void html_input(){
  server.begin();
  if(netwerk){
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", energie_html, processor);
    });
  }
  if(!netwerk){
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", netwerk_html);
    });
    server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request){
      bool ssid_ingevuld = false;
      bool pswd_ingevuld = false;
      String netwerk = "                         ";
      String paswoord = "                          ";
      char terminator = char(0x0a);
      if(request->hasParam(STA_SSID)){
        netwerk = (request->getParam(STA_SSID)->value());
        netwerk = netwerk + String(terminator);
        pref.putString("ssid", netwerk);
        ssid_ingevuld = true;
      }
      if(request->hasParam(STA_PSWD)){
        paswoord = (request->getParam(STA_PSWD)->value());
        paswoord = paswoord + String(terminator);
        pref.putString("pswd", paswoord);
        pswd_ingevuld = true;
      }
      if((ssid_ingevuld) && (pswd_ingevuld)){
        delay(5000);
        ESP.restart();
      }
    });
  }
}

void setup() {
  delay(5000);
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  pinMode(BLINKIE, OUTPUT);
  digitalWrite(BLINKIE, true);
  pref.begin("WiFi_data", false);
  if(pref.getString("controle") != "ingesteld"){
    pref.putString("ssid", "dummy");
    pref.putString("pswd", "dummy");
    pref.putString("controle", "ingesteld");
  }
  ssid_string = pref.getString("ssid");
  pswd_string = pref.getString("pswd");
  ssid_string.toCharArray(ssid, ssid_string.length());
  pswd_string.toCharArray(pswd, pswd_string.length());
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pswd);
  netwerk = true;
  wacht_op_netwerk = millis();
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    if(millis() - wacht_op_netwerk > 15000){
      netwerk = false;
      break;
    }
  }
  if(netwerk == true){  
    IPAddress subnet(WiFi.subnetMask());
    IPAddress gateway(WiFi.gatewayIP());
    IPAddress dns(WiFi.dnsIP(0));
    IPAddress static_ip(192,168,1,222);
    WiFi.disconnect();
    if (WiFi.config(static_ip, gateway, subnet, dns, dns) == false) {
      Serial.println("Configuration failed.");
    }
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pswd);
    wacht_op_netwerk = millis();
    while(WiFi.status() != WL_CONNECTED){
      delay(500);
      if(millis() - wacht_op_netwerk > 15000){
        netwerk = false;
        break;
      }
    }
  }
  else{
    WiFi.disconnect();
    WiFi.mode(WIFI_AP);
    WiFi.softAP(APSSID, APPSWD);
  }
  delay(1000);
  html_input();
  nu = millis();
}

void loop() {
  while(Serial2.available()){
    char lees_byte = Serial2.read();
    if(lees_byte == 0x2f){
      digitalWrite(BLINKIE, (digitalRead(BLINKIE) ^ 1));
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
      buffer_data = "";
    }
    buffer_data += lees_byte;
    if(lees_byte == 0x0a){
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
      buffer_data = "";
    }
  }
}
