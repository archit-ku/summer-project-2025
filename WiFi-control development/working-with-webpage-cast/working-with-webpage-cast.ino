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
#include <WiFiWebServer.h>

const char ssid[] = "EEERover";
const char pass[] = "exhibition";
const int groupNumber = 12; // Set your group number to make the IP address constant - only do this on the EEERover network

//Webpage to return when root is requested



WiFiWebServer server(80);



//Return the web page

void stop(){
  digitalWrite(9,LOW);
  digitalWrite(12,LOW);
  server.send(200, F("text/plain"), F("STOP"));
}

//move forwards

void root() {
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.sendHeader("Content-Type", "text/html");
  server.send(200);

  // HTML START
  server.sendContent("<!DOCTYPE html><html><head>");
  server.sendContent("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no, maximum-scale=1.0, minimum-scale=1.0\">");
  server.sendContent("<style>");
  server.sendContent(".btn {background-color: inherit; padding: 20px 28px; font-size: 16px; transform: rotate(90deg); margin: 80px 0px 15px -10px;}");
  server.sendContent(".btn:hover {background: #eee;}");
  server.sendContent(".slider-container {margin-top:30px; width: 65%; overflow: visible; display: block;}");
  server.sendContent(".slider {width: 100%; max-width: 600px;}");
  server.sendContent(".rotated-text-container {display: flex; flex-direction: column; align-items: flex-start; margin: 15px 0px 80px 285px; transform: rotate(90deg); transform-origin: left top;}");
  server.sendContent(".rotated-row {display: flex; gap: 10px; margin-bottom: 10px;}");
  server.sendContent(".rotated-label, .rotated-value {white-space: nowrap; font-size: 16px;}");
  server.sendContent("</style></head><body>");

  server.sendContent("<div class=\"slider-container\"><p>Left Motor: <span id=\"sliderVal\">0</span></p>");
  server.sendContent("<input type=\"range\" id=\"controlSlider\" class=\"slider\" min=\"-255\" max=\"255\" value=\"0\"></div>");

  server.sendContent("<button class=\"btn\" onclick=\"getName()\"> Get name </button>");
  server.sendContent("<button class=\"btn\" onclick=\"getSpecies()\">Get species</button>");

  server.sendContent("<div class=\"rotated-text-container\">");
  server.sendContent("<div class=\"rotated-row\"><span class=\"rotated-label\">NAME:</span><span id=\"name\" class=\"rotated-value\">unknown</span></div>");
  server.sendContent("<div class=\"rotated-row\"><span class=\"rotated-label\">SPECIES:</span><span id=\"species\" class=\"rotated-value\">unknown</span></div></div>");

  server.sendContent("<button class=\"btn\" onclick=\"ledOn()\">LED On</button>");
  server.sendContent("<button class=\"btn\" onclick=\"ledOff()\">LED Off</button>");

  server.sendContent("<div class=\"slider-container\"><p>Right Motor: <span id=\"sliderVal2\">0</span></p>");
  server.sendContent("<input type=\"range\" id=\"controlSlider2\" class=\"slider\" min=\"-255\" max=\"255\" value=\"0\"></div>");

  server.sendContent("<script>");
  server.sendContent("var xhttp = new XMLHttpRequest();");
  server.sendContent("xhttp.onreadystatechange = function() {if (this.readyState == 4 && this.status == 200) {document.getElementById(\"state\").innerHTML = this.responseText;}};");

  server.sendContent("var xhttpNAME = new XMLHttpRequest();");
  server.sendContent("xhttpNAME.onreadystatechange = function() {if (this.readyState == 4 && this.status == 200) {document.getElementById(\"name\").innerHTML = this.responseText;}};");
  server.sendContent("function getName() {xhttpNAME.open(\"GET\", \"/getName\", true); xhttpNAME.send();}");

  server.sendContent("var xhttpSPECIES = new XMLHttpRequest();");
  server.sendContent("xhttpSPECIES.onreadystatechange = function() {if (this.readyState == 4 && this.status == 200) {document.getElementById(\"species\").innerHTML = this.responseText;}};");
  server.sendContent("function getSpecies() {xhttpSPECIES.open(\"GET\", \"/getSpecies\", true); xhttpSPECIES.send();}");

  server.sendContent("var xhttpled = new XMLHttpRequest();");
  server.sendContent("function ledOn() {xhttpled.open(\"GET\", \"/on\"); xhttpled.send();}");
  server.sendContent("function ledOff() {xhttpled.open(\"GET\", \"/off\"); xhttpled.send();}");

  server.sendContent("const slider = document.getElementById(\"controlSlider\");");
  server.sendContent("const sliderVal = document.getElementById(\"sliderVal\");");
  server.sendContent("const resetVal = 0;");
  server.sendContent("slider.addEventListener(\"touchmove\", () => {sliderVal.textContent = slider.value; sendSliderValue(slider.value);});");
  server.sendContent("slider.addEventListener(\"touchend\", () => {resetSlider();});");

  server.sendContent("var xhttpslidone = new XMLHttpRequest();");
  server.sendContent("function sendSliderValue(value) {xhttpslidone.open(\"GET\", \`/slider?val=${value}\`, true); xhttpslidone.send();}");
  server.sendContent("function resetSlider() {slider.value = resetVal; sliderVal.textContent = resetVal; sendSliderValue(0);}");

  server.sendContent("const slider2 = document.getElementById(\"controlSlider2\");");
  server.sendContent("const sliderVal2 = document.getElementById(\"sliderVal2\");");
  server.sendContent("const resetVal2 = 0;");
  server.sendContent("slider2.addEventListener(\"touchmove\", () => {sliderVal2.textContent = slider2.value; sendSlider2Value(slider2.value);});");
  server.sendContent("slider2.addEventListener(\"touchend\", () => {resetSlider2();});");

  server.sendContent("var xhttpslidtwo = new XMLHttpRequest();");
  server.sendContent("function sendSlider2Value(value) {xhttpslidtwo.open(\"GET\", \`/slider2?val=${value}\`, true); xhttpslidtwo.send();}");
  server.sendContent("function resetSlider2() {slider2.value = resetVal2; sliderVal2.textContent = resetVal2; sendSlider2Value(0);}");

  server.sendContent("</script></body></html>");
}



void getName()
{
  server.sendHeader("Access-Control-Allow-Origin", "*");
  String name = "a";
  server.send(200, F("text/plain"), name);
}

void getSpecies()
{
  server.sendHeader("Access-Control-Allow-Origin", "*");
  String species = "pig";
  server.send(200, F("text/plain"), species);
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

void setup() {
  pinMode(8, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(9, OUTPUT);



  digitalWrite(8,HIGH);
  digitalWrite(11,HIGH);

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
  server.on("/", root);
  server.on("/on", ledON);
  server.on("/off", ledOFF);

  server.on("/s", stop);

  server.on(F("/getName"), getName);
  server.on(F("/getSpecies"), getSpecies);
  server.on("/slider", HTTP_GET, [](){
    if (server.hasArg("val")) {
      String val = server.arg("val");
      int intval = val.toInt();
      if(intval<0){
        digitalWrite(11,LOW);
        analogWrite(12,abs(intval));
      }
      else{
       digitalWrite(11,HIGH);
       analogWrite(12, intval);
      }

      
    }
  });

  server.on("/slider2", HTTP_GET, [](){
    if (server.hasArg("val")) {
      String val = server.arg("val");
      int intval = val.toInt();
      if(intval<0){
        digitalWrite(8,LOW);
        analogWrite(9,abs(intval));
      }
      else{
       digitalWrite(8,HIGH);
       analogWrite(9, intval);
      }

    }
  });

  server.onNotFound(handleNotFound);
  
  server.begin();
  
  
  Serial.print(F("HTTP server started @ "));
  Serial.println(static_cast<IPAddress>(WiFi.localIP()));
}

//Call the server polling function in the main loop
void loop() {
  server.handleClient();



}