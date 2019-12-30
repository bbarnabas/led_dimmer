#define ledPin 2
#define pwmPin 15
#define irPin A0
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

// Replace with your network credentials
const char* ssid = "ssid";
const char* password = "password";
int value = 0;
int valueOld = 0;
int raw = 0;
int ledValue = 1023;
bool flagdim = false;
bool flagon = false;
bool up = true;
bool turn_on_full = false;
uint16_t cnt = 0;

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(pwmPin, OUTPUT);
  pinMode(A0, INPUT);
  analogWrite(ledPin, 1023);
  analogWrite(pwmPin, 0);
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  ArduinoOTA.handle();
  cnt = 0;
  while(analogRead(A0)>512)
  {
      delay(4);
      cnt++;
      if(cnt == 50){
          turn_on_full = true;
      }
      else if (cnt > 200){
          turn_on_full = false;
          if(up){
            if(++ledValue>1023){
              up = false;
              ledValue=1023;
            }
          }else{
            if(--ledValue<=1){
              up = true;
              ledValue=0;
            }
          }
          analogWrite(ledPin, ledValue);
          analogWrite(pwmPin, 1023-ledValue);
      }
  }
  delay(4);
  if(turn_on_full)
  {
    if(ledValue == 1023){
      up = false;
      ledValue = 0;
    }else{
      up = true;
      ledValue = 1023;
    }
    analogWrite(ledPin, ledValue);
    analogWrite(pwmPin, 1023-ledValue);
    turn_on_full = false;
  }
}

