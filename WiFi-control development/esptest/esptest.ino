/**************************************************************************************************************************************
  EEERover Starter Example
  
  Based on AdvancedWebServer.ino - Simple Arduino web server sample for SAMD21 running WiFiNINA shield
  For any WiFi shields, such as WiFiNINA W101, W102, W13x, or custom, such as ESP8266/ESP32-AT, Ethernet, etc
  
  WiFiWebServer is a library for the ESP32-based WiFi shields to run WebServer
  Forked and modified from ESP8266 https://github.com/esp8266/Arduino/releases
  Forked and modified from Arduino WiFiNINA library https://www.arduino.cc/en/Reference/WiFiNINA
  Built by Khoi Hoang https://github.com/khoih-prog/WiFiWebServer
  Licensed under MIT license
  
  Copyright (c) 2015, Majenko Technologies
  All rights reserved.
  
  Redistribution and use in source and binary forms, with or without modification,
  are permitted provided that the following conditions are met:
  
  Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.
  
  Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.
  
  Neither the name of Majenko Technologies nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.
  
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
  ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ***************************************************************************************************************************************/
#define USE_WIFI_NINA         false
#define USE_WIFI101           true
#include <WebServer.h>
#include <WiFi.h>
#include <Arduino.h>

#define LED_BUILTIN 2

const int pinA1 = 0;  // Replaces digital pin 8
const int pinA2 = 1;  // Replaces analogWrite on pin 9
const int pinB1 = 2;  // Replaces digital pin 11
const int pinB2 = 3;

const int chA2 = 0;  // Channel for GPIO 1
const int chB2 = 1; 

const char ssid[] = "EEERover";
const char pass[] = "exhibition";
const int groupNumber = 12; // Set your group number to make the IP address constant - only do this on the EEERover network

//Webpage to return when root is requested
const char webpage[] = \
"<html><head><style>\
.btn {background-color: inherit;padding: 14px 28px;font-size: 16px;}\
.btn:hover {background: #eee;}\
</style></head>\
<body>\
<button class=\"btn\" onclick=\"ledOn()\">LED On</button>\
<button class=\"btn\" onclick=\"ledOff()\">LED Off</button>\
<button class=\"btn\" onclick=\"fast()\">FAST FAST FAST</button>\
<button class=\"btn\" onclick=\"slow()\">SLOWWWWWWWW</button>\
<br>LED STATE: <span id=\"state\">OFF</span>\
</body>\
<script>\
var xhttp = new XMLHttpRequest();\
xhttp.onreadystatechange = function() {\
  if (this.readyState == 4 && this.status == 200) {\
    document.getElementById(\"state\").innerHTML = this.responseText;\
  }\
};\
function ledOn() {\
  xhttp.open(\"GET\", \"/on\");\
  xhttp.send();\
}\
function ledOff() {\
  xhttp.open(\"GET\", \"/off\");\
  xhttp.send();\
}\
function fast() {\
  xhttp.open(\"GET\", \"/fast\");\
  xhttp.send();\
}\
function slow() {\
  xhttp.open(\"GET\", \"/slow\");\
  xhttp.send();\
}\
document.addEventListener(\"keydown\", function(event) {\
  var key = event.key.toLowerCase();\
  if(key == \"w\"){\
    xhttp.open(\"GET\", \"/f\", true);\
    xhttp.send();\
  }\
  if(key == \"a\"){\
    xhttp.open(\"GET\", \"/l\", true);\
    xhttp.send();\
  }\
  if(key == \"d\"){\
    xhttp.open(\"GET\", \"/r\", true);\
    xhttp.send();\
  }\
  if(key == \"s\"){\
    xhttp.open(\"GET\", \"/b\", true);\
    xhttp.send();\
  }\
  if(key == \" \"){\
    xhttp.open(\"GET\", \"/s\", true);\
    xhttp.send();\
  }\
});\
</script></body></html>";

WebServer server(80);

int speed = 255;

void top_speed(){
  speed = 255;
  server.send(200, F("text/plain"), F("fast"));
  }

void slow_speed(){
  speed = 100;
  server.send(200, F("text/plain"), F("slow"));
}


//Return the web page
void handleRoot()
{
  server.send(200, F("text/html"), webpage);
}

void stop() {
  ledcWrite(chA2, 0);
  ledcWrite(chB2, 0);
  server.send(200, F("text/plain"), F("STOP"));
}

// Move forwards
void forward() {
  digitalWrite(pinA1, HIGH);
  digitalWrite(pinB1, HIGH);
  ledcWrite(chA2, speed);
  ledcWrite(chB2, speed);
  delay(40);
  ledcWrite(chA2, 0);
  ledcWrite(chB2, 0);
  server.send(200, F("text/plain"), F("F"));
}

// Move backwards
void backwards() {
  digitalWrite(pinA1, LOW);
  ledcWrite(chA2, speed);
  digitalWrite(pinB1, LOW);
  ledcWrite(chB2, speed);
  delay(40);
  ledcWrite(chA2, 0);
  ledcWrite(chB2, 0);
  server.send(200, F("text/plain"), F("B"));
}

// Turn right
void right() {
  digitalWrite(pinA1, LOW);
  ledcWrite(chA2, speed);
  digitalWrite(pinB1, HIGH);
  ledcWrite(chB2, speed);
  delay(40);
  ledcWrite(chA2, 0);
  ledcWrite(chB2, 0);
  server.send(200, F("text/plain"), F("R"));
}

// Turn left
void left() {
  digitalWrite(pinA1, HIGH);
  ledcWrite(chA2, speed);
  digitalWrite(pinB1, LOW);
  ledcWrite(chB2, speed);
  delay(40);
  ledcWrite(chA2, 0);
  ledcWrite(chB2, 0);
  server.send(200, F("text/plain"), F("L"));
}


//Switch LED on and acknowledge
void ledON()
{
  digitalWrite(LED_BUILTIN,1);
  server.send(200, F("text/plain"), F("ON"));
}

//Switch LED on and acknowledge
void ledOFF()
{
  digitalWrite(LED_BUILTIN,0);
  server.send(200, F("text/plain"), F("OFF"));
}



//Generate a 404 response with details of the failed request
void handleNotFound()
{
  String message = F("File Not Found\n\n"); 
  message += F("URI: ");
  message += server.uri();
  message += F("\nMethod: ");
  message += (server.method() == HTTP_GET) ? F("GET") : F("POST");
  message += F("\nArguments: ");
  message += server.args();
  message += F("\n");
  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, F("text/plain"), message);
}

  const int freq = 5000;       // 5 kHz
  const int resolution = 8; 

void setup() {

  pinMode(pinA1, OUTPUT);
  pinMode(pinB1, OUTPUT);

  int speed = 255;

  ledcSetup(chA2, 5000, 8);  // 5 kHz, 8-bit resolution
  ledcAttachPin(pinA2, chA2);

  ledcSetup(chB2, 5000, 8);
  ledcAttachPin(pinB2, chB2);


  stop();

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 0);

  Serial.begin(9600);

  //Wait 10s for the serial connection before proceeding
  //This ensures you can see messages from startup() on the monitor
  //Remove this for faster startup when the USB host isn't attached
  while (!Serial && millis() < 10000);  

  Serial.println(F("\nStarting Web Server"));

  //Check WiFi shield is present
  if (WiFi.status() == WL_NO_SHIELD)
  {
    Serial.println(F("WiFi shield not present"));
    while (true);
  }

  //Configure the static IP address if group number is set
  if (groupNumber)
    WiFi.config(IPAddress(192,168,0,groupNumber+1));

  // attempt to connect to WiFi network
  Serial.print(F("Connecting to WPA SSID: "));
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED)
  {
    delay(500);
    Serial.print('.');
  }

  //Register the callbacks to respond to HTTP requests
  server.on("/", handleRoot);
  server.on("/on", ledON);
  server.on("/off", ledOFF);
  server.on("/f", forward);
  server.on("/b", backwards);
  server.on("/l", left);
  server.on("/r", right);
  server.on("/s", stop);
  server.on("/fast", top_speed);
  server.on("/slow", slow_speed);

  server.onNotFound(handleNotFound);
  
  server.begin();
  
  
  Serial.print(F("HTTP server started @ "));
  Serial.println(static_cast<IPAddress>(WiFi.localIP()));
}

//Call the server polling function in the main loop
void loop() {
  server.handleClient();



}