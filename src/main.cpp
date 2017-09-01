#include <Arduino.h>
#include <string>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>


const char* ssid = "jsplacas";
const char* password = "3132333435";
//const char* password = "16807864";

//Pin connected to ST_CP of 74HC595
int latchPin = 4;
//Pin connected to SH_CP of 74HC595
int clockPin = 14;
////Pin connected to DS of 74HC595
int dataPin = 12;

ESP8266WebServer server(80);

//Handle the request with the commants for each line of relays(columns)
int registers[16];
int phase_state = 0;
int buttonState;
int lastButtonState = LOW;

void handleRoot() {
        phase_state = 0;
        digitalWrite(latchPin, LOW);
        //Set the 9th byte as 0b01000000
        for (int i=15; i>=8; i--)
        {
                registers[i] = 0;
                shiftOut(dataPin, clockPin, MSBFIRST, 0b00000000);
        }
        for (int i=7; i>=0; i--)
        {
                String argument = "byte_" + String(i);
                //Serial.println(argument);
                String command_str = server.arg(argument);
                //Serial.println(command_str);
                registers[i] = command_str.toInt();
                int command = command_str.toInt();
                shiftOut(dataPin, clockPin, MSBFIRST, command);
        }
        for (int i=0; i<=15; i++)
        {
                Serial.println(registers[i]);
        }
        digitalWrite(latchPin, HIGH);
        digitalWrite(latchPin, LOW);
        server.send(200, "text/plain", "Command received and executed");
}

void zerar()
{
        digitalWrite(latchPin, LOW);
        for(int i=0; i<=7; i++)
        {
                shiftOut(dataPin, clockPin, MSBFIRST, 0b00000000);
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



void switch_phase(){
        delayMicroseconds(158000);
        int reading = digitalRead(2);

        if (phase_state == 0)
        {
                // Unarm relays when the button is pressed for too long
                unsigned long int pressed_btn_counting = 0;
                while(digitalRead(2) == 0){
                        delayMicroseconds(100);
                        pressed_btn_counting++;
                        if (pressed_btn_counting >= 20000)
                        {
                                for(int i=15; i>=0; i --)
                                        shiftOut(dataPin, clockPin, MSBFIRST, 0b00000000);
                        }
                }
                registers[15] =  255;
                for (int i=15; i>=8; i--) {
                        shiftOut(dataPin, clockPin, MSBFIRST, 0b11111100);
                }
                for (int i=7; i>=0; i--)
                {
                        shiftOut(dataPin, clockPin, MSBFIRST, registers[i]);
                }
                phase_state = 1;
        }
        else
        {
                registers[15] = 0;
                for (int i=15; i>=8; i--)
                {
                        shiftOut(dataPin, clockPin, MSBFIRST, 0b00000000);
                }
                for (int i=7; i>=0; i--)
                {
                        shiftOut(dataPin, clockPin, MSBFIRST, registers[i]);
                }
                phase_state = 0;
        }
        digitalWrite(latchPin, HIGH);
        digitalWrite(latchPin, LOW);
        Serial.println("Switch ok");
        Serial.println();
        for (int i=0; i<=15; i++)
        {
                Serial.print(registers[i]);
                Serial.print(" | ");
        }
        server.send(200, "text/plain", "Phase relay switched");
}

void setup(){
        //set pins to output so you can control the shift register
        pinMode(5, OUTPUT);
        pinMode(2, INPUT);
        pinMode(0, OUTPUT);
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
        server.on("/activate_phase", switch_phase);
        server.on("/zerar", zerar);
        server.onNotFound(handleNotFound);
        server.begin();
        Serial.println("HTTP server started");
        attachInterrupt(digitalPinToInterrupt(2), switch_phase, FALLING);
}

void loop(){
        server.handleClient();
}
