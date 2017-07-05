
#include <Arduino.h>
#include <string>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>


const char* ssid = "Net Virtua Jonatan";
const char* password = "16807864";

//Pin connected to ST_CP of 74HC595
int latchPin = 12;
//Pin connected to SH_CP of 74HC595
int clockPin = 14;
////Pin connected to DS of 74HC595
int dataPin = 5;

ESP8266WebServer server(80);

void handleRoot() {
        digitalWrite(latchPin, LOW);
        for (int i=8; i>=1; i--)
        {
                String argument = "byte_" + String(i);
                Serial.println(argument);
                String command_str = server.arg(argument);
                Serial.println(command_str);
                //int correction = 10 - command_str.length();
                int command = command_str.toInt();

                shiftOut(dataPin, clockPin, MSBFIRST, byte(command));
        }
        digitalWrite(latchPin, HIGH);
        digitalWrite(latchPin, LOW);
}

void zerar()
{
        digitalWrite(latchPin, LOW);
        for(int i=1; i<=16; i++)
        {
                shiftOut(dataPin, clockPin, MSBFIRST, 0b00000000);
        }
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
        pinMode(2, OUTPUT);
        pinMode(latchPin, OUTPUT);
        pinMode(clockPin, OUTPUT);
        pinMode(dataPin, OUTPUT);

        //Safty procedure
        digitalWrite(2, LOW);
        zerar();
        digitalWrite(2, HIGH);


        //Serial
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
        server.onNotFound(handleNotFound);
        server.begin();
        Serial.println("HTTP server started");
}

void loop(){
        server.handleClient();
}
