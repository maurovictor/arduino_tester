#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "Net Virtua Jonatan";
const char* password = "16807864";

//Pin connected to ST_CP of 74HC595
int latchPin = 4;
//Pin connected to SH_CP of 74HC595
int clockPin = 14;
////Pin connected to DS of 74HC595
int dataPin = 5;

ESP8266WebServer server(80);

void handleRoot() {
  server.send(200, "text/plain", "hello from esp8266!");
}



void VR1() {
  server.send(200, "text/plain", "VR1 funcionando");
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, 0b11001101);
  shiftOut(dataPin, clockPin, MSBFIRST, 0b11111111);
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin, LOW);
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message  += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);

}

void setup(){
  //set pins to output so you can control the shift register
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);


  Serial.begin(9600);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(9600);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/2byte0", []{

    digitalWrite(latchPin, LOW);

    shiftOut(dataPin, clockPin, MSBFIRST, 0b10000000);//16
    shiftOut(dataPin, clockPin, MSBFIRST, 0b01000000);//15
    shiftOut(dataPin, clockPin, MSBFIRST, 0b00100000);//14
    shiftOut(dataPin, clockPin, MSBFIRST, 0b00010000);//13
    shiftOut(dataPin, clockPin, MSBFIRST, 0b00001000);//12
    shiftOut(dataPin, clockPin, MSBFIRST, 0b00000100);//11
    shiftOut(dataPin, clockPin, MSBFIRST, 0b00000010);//10
    shiftOut(dataPin, clockPin, MSBFIRST, 0b00000001);//9
    shiftOut(dataPin, clockPin, MSBFIRST, 0b00000000);//8
    shiftOut(dataPin, clockPin, MSBFIRST, 0b00000000);//7
    shiftOut(dataPin, clockPin, MSBFIRST, 0b00000000);//6
    shiftOut(dataPin, clockPin, MSBFIRST, 0b00000000);//5
    shiftOut(dataPin, clockPin, MSBFIRST, 0b00000000);//4
    shiftOut(dataPin, clockPin, MSBFIRST, 0b00000000);//3
    shiftOut(dataPin, clockPin, MSBFIRST, 0b00000000);//2
    shiftOut(dataPin, clockPin, MSBFIRST, 0b00000000);//1
    digitalWrite(latchPin, HIGH);
    digitalWrite(latchPin, LOW);
    server.send(200, "text/plain", "apaga tudo");
  });
  server.on("/2byte1",[]{

    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, 0b11111111);
    shiftOut(dataPin, clockPin, MSBFIRST, 0b11111111);
    shiftOut(dataPin, clockPin, MSBFIRST, 0b11111111);
    shiftOut(dataPin, clockPin, MSBFIRST, 0b11111111);
    shiftOut(dataPin, clockPin, MSBFIRST, 0b11111111);
    shiftOut(dataPin, clockPin, MSBFIRST, 0b11111111);
    shiftOut(dataPin, clockPin, MSBFIRST, 0b11111111);
    shiftOut(dataPin, clockPin, MSBFIRST, 0b11111111);
    shiftOut(dataPin, clockPin, MSBFIRST, 0b11111111);
    shiftOut(dataPin, clockPin, MSBFIRST, 0b11111111);
    shiftOut(dataPin, clockPin, MSBFIRST, 0b11111111);
    shiftOut(dataPin, clockPin, MSBFIRST, 0b11111111);
    shiftOut(dataPin, clockPin, MSBFIRST, 0b11111111);
    shiftOut(dataPin, clockPin, MSBFIRST, 0b11111111);
    shiftOut(dataPin, clockPin, MSBFIRST, 0b11111111);
    shiftOut(dataPin, clockPin, MSBFIRST, 0b11111111);
    digitalWrite(latchPin, HIGH);
    digitalWrite(latchPin, LOW);
    server.send(200, "text/plain", "liga tudo");

  });

  server.on("/l_d",[]{

    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, 0b11111111);
    shiftOut(dataPin, clockPin, MSBFIRST, 0b00000000);
    shiftOut(dataPin, clockPin, MSBFIRST, 0b11111111);
    shiftOut(dataPin, clockPin, MSBFIRST, 0b00000000);
    shiftOut(dataPin, clockPin, MSBFIRST, 0b11111111);
    shiftOut(dataPin, clockPin, MSBFIRST, 0b00000000);
    shiftOut(dataPin, clockPin, MSBFIRST, 0b11111111);
    shiftOut(dataPin, clockPin, MSBFIRST, 0b00000000);
    shiftOut(dataPin, clockPin, MSBFIRST, 0b11111111);
    shiftOut(dataPin, clockPin, MSBFIRST, 0b00000000);
    shiftOut(dataPin, clockPin, MSBFIRST, 0b11111111);
    shiftOut(dataPin, clockPin, MSBFIRST, 0b00000000);
    shiftOut(dataPin, clockPin, MSBFIRST, 0b11111111);
    shiftOut(dataPin, clockPin, MSBFIRST, 0b00000000);
    shiftOut(dataPin, clockPin, MSBFIRST, 0b11111111);
    shiftOut(dataPin, clockPin, MSBFIRST, 0b00000000);

    digitalWrite(latchPin, HIGH);
    digitalWrite(latchPin, LOW);
    server.send(200, "text/plain", "liga tudo");


    });
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
}
