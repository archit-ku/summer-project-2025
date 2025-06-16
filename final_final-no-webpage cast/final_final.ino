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

#include <Arduino.h>


const char ssid[] = "EEERover";
const char pass[] = "exhibition";
const int groupNumber = 12; // Set your group number to make the IP address constant - only do this on the EEERover network

String species;
String name;

// Pins
const byte radio_ir_Pin = 2;      
const byte infrared_ir_Pin = 3;
//magnetic sensor is connected to A5    

// Radio signal variables
volatile unsigned long lastRiseTime_radio = 0;
volatile unsigned long periodMicros_radio = 0;
volatile bool newPeriod_radio = false;

// Infrared signal variables
volatile unsigned long lastRiseTime_infrared = 0;
volatile unsigned long periodMicros_infrared = 0;
volatile bool newPeriod_infrared = false;

unsigned long lastReconnectAttempt = 0;

// Frequency results
int frequency_radio = 0;
int frequency_infrared = 0;

// Magnetic sensor variables
String magnet;
int magnetic_threshold = 402;

void risingEdgeDetected_radio() {
  unsigned long now = micros();
  static bool firstRise_radio = true;

  if (!firstRise_radio) {
    periodMicros_radio = now - lastRiseTime_radio;
    newPeriod_radio = true;
  }

  lastRiseTime_radio = now;
  firstRise_radio = false;
}

void risingEdgeDetected_infrared() {
  unsigned long now = micros();
  static bool firstRise_infrared = true;

  if (!firstRise_infrared) {
    periodMicros_infrared = now - lastRiseTime_infrared;
    newPeriod_infrared = true;
  }

  lastRiseTime_infrared = now;
  firstRise_infrared = false;
}

// --- Frequency calculation functions ---

void calculateRadioFrequency() {
  if (newPeriod_radio) {
    noInterrupts();
    unsigned long period = periodMicros_radio;
    newPeriod_radio = false;
    interrupts();

    float freq = 1e6 / period;

    if (abs(freq - 100) < 10) {
      frequency_radio = 100;
    } 
    else if (abs(freq - 150) < 10) {
      frequency_radio = 150;
    } 
    else {
      frequency_radio = 0;
    }
  }
}

void calculateInfraredFrequency() {
  if (newPeriod_infrared) {
    noInterrupts();
    unsigned long period = periodMicros_infrared;
    newPeriod_infrared = false;
    interrupts();

    float freq = 1e6 / period;

    if (abs(freq - 293) < 10) {
      frequency_infrared = 293;
    } 
    else if (abs(freq - 457) < 10) {
      frequency_infrared = 457;
    } 
    else {
      frequency_infrared = 0;
    }
  }
}

// --- Magnetic sensor reading ---

void readMagneticSensor() {
  int sum = 0; 
  float average;
  for(int i = 0; i < 10; i++){
    sum += analogRead(A5);
    delay(10);
  }
  average = sum / 10.0;

  if (average > (magnetic_threshold + 3)) {
    magnet = "up";
  }
  else if (average < (magnetic_threshold - 3)) {
    magnet = "down";
  }
  else {
    magnet = "unknown";
  }
}

// --- Determine species based on results from sensors ---

void determine_species() {
  readMagneticSensor();
  calculateRadioFrequency();
  calculateInfraredFrequency();
  String error = magnet + frequency_radio + frequency_infrared;

  if (magnet == "down") {
    if (frequency_infrared == 457) {
      species = "Wibbo";
    } 
    else if (frequency_radio == 100) {
      species = "Gribbit";
    } 
    else {
      species = error;
    }
  } 
  else if (magnet == "up") {
    if (frequency_infrared == 293) {
      species = "Snorkle";
    } 
    else if (frequency_radio == 150) {
      species = "Zapple";
    } 
    else {
      species = error;
    }
  } 
  else {
    species = error;
  }
}

String readsingleName(){
  String name;
  char VA0;
  //in loop, wait for #
  if (Serial1.available()){
    Serial.println("source detected");
    while(true){
      VA0 = Serial1.read();

      if(VA0 == '#'){
        break;
      }

      //read char, add to string name
      VA0 = Serial1.read();
      while(VA0 != '#'){
        name += VA0;
      }

    }
    //if char #, return name
    return name;
  }
  return "not found";
}

void findName(){
  String names[75];
  for(int i = 0; i < 75; i++){
    names[i] = readsingleName();
  }

  String modeName = "";
  int maxCount = 0;

  for (int i = 0; i < 75; i++) {
    int count = 1;
    for (int j = i + 1; j < 75; j++) {
      if (names[i] == names[j]) {
        count++;
      }
    }

    if (count > maxCount) {
      maxCount = count;
      modeName = names[i];
    }
  }
  name = modeName;
}


WiFiWebServer server(80);



//Return the web page

void stop(){
  digitalWrite(9,LOW);
  digitalWrite(12,LOW);
  server.send(200, F("text/plain"), F("STOP"));
}

//move forwards



void getName()
{
  server.sendHeader("Access-Control-Allow-Origin", "*");
  findName();
  server.send(200, F("text/plain"), name);
}

void getSpecies()
{
  server.sendHeader("Access-Control-Allow-Origin", "*");
  determine_species();
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
  pinMode(radio_ir_Pin, INPUT); 
  pinMode(infrared_ir_Pin, INPUT);




  digitalWrite(8,HIGH);
  digitalWrite(11,HIGH);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 0);

  Serial.begin(9600);

  attachInterrupt(digitalPinToInterrupt(radio_ir_Pin), risingEdgeDetected_radio, RISING); 
  attachInterrupt(digitalPinToInterrupt(infrared_ir_Pin), risingEdgeDetected_infrared, RISING);

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


