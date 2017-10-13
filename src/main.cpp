#include <Arduino.h>
#include <string>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <stdio.h>


const char* ssid = "jsplacas";
const char* password = "3132333435";

ESP8266WebServer server(80);
//Pin connected to ST_CP of 74HC595
int latchPin = 4;
//Pin connected to SH_CP of 74HC595
int clockPin = 14;
////Pin connected to DS of 74HC595
int dataPin = 12;

int btn1 = 2;
int btn2 = 0;
//Handle the request with the commants for each line of relays(columns)
int registers[16];
int phase_state = 0;
unsigned long int lastTime=0;

//int send_byte ;
int protec_state = 0;
int leading_bytes = 0;

void handleRoot()
{
        phase_state = 0;
        Serial.print('s');
        Serial.print('t');
        Serial.print('a');
        Serial.print('r');
        for (int i=15; i>=8; i--)
        {
                registers[i] = 0;
        }
        for (int i=7; i>=0; i--)
        {
                String argument = "byte_" + String(i);
                //Serial.println(argument);
                String command_str = server.arg(argument);
                //Serial.println(command_str);
                registers[i] = command_str.toInt();
        }
        for (int i=15; i>=0; i--)
        {
                Serial.write(registers[i]);
        }
        server.send(200, "text/plain", "Command received and executed");
}

void zerar(){
        digitalWrite(latchPin, LOW);
        for(int i=0; i<=7; i++)
        {
                Serial.println(0b00000000);
        }
        digitalWrite(latchPin, HIGH);
        digitalWrite(latchPin, LOW);
        server.send(200, "text/plain", "Todos os reles abertos");
}

void handleNotFound(){
        String message = "File Not Found\n\n";
        message += "URI: ";
        message += server.uri();
        message += "\nMethod: ";
        message += (server.method() == HTTP_GET)?"GET":"POST";
        message += "\nArguments: ";
        message += server.args();
        message += "\n";
        for (uint8_t i=0; i<server.args(); i++){
                message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
        }
        server.send(404, "text/plain", message);
}

void web_switch_phase(){
        Serial.print('s');
        Serial.print('t');
        Serial.print('a');
        Serial.print('r');

        if (phase_state == 0)
        {
                if(protec_state == 0)
                {
                        for (int i=15; i>=8; i--)
                        {
                                Serial.write(0b11111100);
                        }
                        for (int i=7; i>=0; i--){
                                Serial.write(registers[i]);
                        }
                }
                else if(protec_state == 1)
                {
                        for (int i=15; i>=8; i--)
                        {
                                Serial.write(0b11111110);
                        }
                        for (int i=7; i>=0; i--){
                                Serial.write(registers[i]);
                        }
                }
                phase_state = 1;
        }
        else
        {
                if(protec_state == 0)
                {
                        for (int i=15; i>=8; i--)
                        {
                                Serial.write(0b00000000);
                        }
                        for (int i=7; i>=0; i--)
                        {
                                Serial.write(registers[i]);
                        }
                }
                else if(protec_state == 1)
                {
                        for (int i=15; i>=8; i--)
                        {
                                Serial.write(0b00000010);
                        }
                        for (int i=7; i>=0; i--)
                        {
                                Serial.write(registers[i]);
                        }
                }
                phase_state = 0;
        }
        server.send(200, "text/plain", "Phase relay switched from web");
}

void web_switch_protec(){
        Serial.print('s');
        Serial.print('t');
        Serial.print('a');
        Serial.print('r');

        if (protec_state == 0)
        {
                if(phase_state == 0)
                {
                        for (int i=15; i>=8; i--){
                                Serial.write(0b00000010);
                        }
                        for (int i=7; i>=0; i--)
                        {
                                Serial.write(registers[i]);
                        }
                }
                else if(phase_state == 1)
                {
                        for (int i=15; i>=8; i--){
                                Serial.write(0b11111110);
                        }
                        for (int i=7; i>=0; i--)
                        {
                                Serial.write(registers[i]);
                        }
                }
                protec_state = 1;
        }
        else
        {
                if(phase_state == 0)
                {
                        for (int i=15; i>=8; i--){
                                Serial.write(0b00000000);
                        }
                        for (int i=7; i>=0; i--)
                        {
                                Serial.write(registers[i]);
                        }
                }
                else if(phase_state == 1)
                {
                        for (int i=15; i>=8; i--){
                                Serial.write(0b11111100);
                        }
                        for (int i=7; i>=0; i--)
                        {
                                Serial.write(registers[i]);
                        }
                }
                protec_state = 0;
        }
        server.send(200, "text/plain", "Phase relay switched from web");
}

void setup(){
        //set pins to output so you can control the shift register
        pinMode(btn1, INPUT);
        pinMode(btn2, INPUT);
        pinMode(5, OUTPUT);
        pinMode(latchPin, OUTPUT);
        pinMode(clockPin, OUTPUT);
        pinMode(dataPin, OUTPUT);
        zerar();
        // Safety procedure, wait a while before turn the relays on
        // The relays will working after 10 cycles on pin 5
        for (int i=0;i<10; i++)
        {
                digitalWrite(5,HIGH);
                delay(300);
                digitalWrite(5, LOW);
                delay(300);
        }
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
        server.on("/activate_phase", web_switch_phase);
        server.on("/activate_protec", web_switch_protec);
        server.on("/zerar", zerar);
        server.onNotFound(handleNotFound);
        server.begin();
        //Serial.println("HTTP server started");
}

void loop(){
        server.handleClient();
}
