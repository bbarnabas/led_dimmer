#define ledPin 2
#define pwmPin 15
#define irPin A0

#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>

//SSID of your network
char ssid[] = "ssid"; //SSID of your Wi-Fi router
char pass[] = "password"; //Password of your Wi-Fi router
WiFiServer server(80);
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
  delay(10);

  // Connect to Wi-Fi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to...");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  Serial.println("");
  Serial.println("Wi-Fi connected successfully");

  // Start the server
  server.begin();
  Serial.println("Server started");
 
  // Print the IP address
  Serial.print("Use this URL : ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

   //ArduinoOTA.setHostname("WemosD1");
   ArduinoOTA.setPassword((const char *)"");
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
  /*
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
  */
  if((value == 1023 || value == 0) && (value!= valueOld)){
    if(value == 1023){
      while(value!=valueOld){
        analogWrite(pwmPin, valueOld);
        valueOld++;
        delay(1);
      }
    }else{
      if(value == 0){
        while(value!=valueOld){
          analogWrite(pwmPin, valueOld);
          valueOld--;
          delay(1);
        }
      }
    }
  }else{
    analogWrite(pwmPin, value);
  }

  WiFiClient client = server.available();
  if (!client) {
    return;
  }
 
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
    Serial.print(".");
  }
  Serial.println("client available");
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
 
  // Match the request
  if(request.indexOf("/LED=") != -1){
    valueOld = value;
    value = request.substring(request.indexOf("/LED=")+5,request.indexOf("HTTP")-1).toInt();
    if(value > 1023){
      value = 1023;
    } else if (value < 0){
      value = 0;
    }
  }
  
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
 
  client.print("Led pin is now: ");
  client.print(value);
  
  delay(1);
  Serial.println("Client disconnected");
  Serial.println("");

}
