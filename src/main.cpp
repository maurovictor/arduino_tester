#include <Arduino.h>
#include <string>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>


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

int send_byte ;
int protec_state = 0;
int leading_bytes;


int refresh_device(int send_byte, int  registers[]){
        digitalWrite(latchPin, LOW);
        for (int i=15; i>=8; i--) {
                shiftOut(dataPin, clockPin, MSBFIRST, send_byte);
        }
        for (int i=7; i>=0; i--)
        {
                shiftOut(dataPin, clockPin, MSBFIRST, registers[i]);
        }
        digitalWrite(latchPin, HIGH);
        digitalWrite(latchPin, LOW);
}
void handleRoot()
{
        phase_state = 0;
        digitalWrite(latchPin, LOW);
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

void zerar(){
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
        if (phase_state == 0)
        {
                leading_bytes = 0b11111100;
                // Open contact relays when the button is pressed for too long
                int pressed_btn_counting = 0;
                while(digitalRead(btn1) == 0){
                        delayMicroseconds(100);
                        pressed_btn_counting++;
                        if (pressed_btn_counting >= 500)
                        {
                                if(protec_state == 0)
                                {
                                        send_byte = leading_bytes & 0b11111101 ;
                                        Serial.print(String(send_byte, BIN));
                                        Serial.print("    ::");
                                }
                                else{
                                        if(protec_state==1)
                                        {
                                                send_byte = leading_bytes | 0b00000010;
                                                Serial.print(String(send_byte, BIN));
                                                Serial.print("    ::");
                                        }
                                }
                                refresh_device(send_byte, registers);
                                phase_state = 1;
                        }
                }
        }
        else
        {
                leading_bytes = 0b00000000;
                int pressed_btn_counting = 0;
                while(digitalRead(btn1) == 0){
                        delayMicroseconds(100);
                        pressed_btn_counting++;
                        if (pressed_btn_counting >= 500)
                        {
                                if(protec_state == 0)
                                {
                                        send_byte = leading_bytes & 0b11111101 ;
                                }
                                else{
                                        if(protec_state==1)
                                        {
                                                send_byte = leading_bytes | 0b00000010;
                                        }
                                }
                                refresh_device(send_byte, registers);
                                phase_state = 0;
                        }
                }
        }
        Serial.println("Switch ok");
        Serial.println();
        server.send(200, "text/plain", "Phase relay switched");
}

void switch_protection(){
        if (protec_state == 0)
        {
                int pressed_btn_counting = 0;
                while(digitalRead(btn2) == 0){
                        delayMicroseconds(100);
                        pressed_btn_counting++;
                        if (pressed_btn_counting >= 500)
                        {
                                //Mudança de Estado
                                if(protec_state == 0)
                                {
                                        send_byte = leading_bytes | 0b00000010;
                                        Serial.println(String(send_byte));
                                }
                                else{
                                        if(protec_state==1)
                                        {
                                                send_byte = leading_bytes & 0b11111101;
                                                Serial.println(String(send_byte));
                                        }
                                }
                                refresh_device(send_byte, registers);
                                protec_state = 1;
                        }
                }
        }
        else
        {
                if(phase_state==0){
                        leading_bytes = 0b00000000;
                }
                else{
                        leading_bytes = 0b11111100;
                }
                int pressed_btn_counting = 0;
                while(digitalRead(btn2) == 0){
                        delayMicroseconds(100);
                        pressed_btn_counting++;
                        if (pressed_btn_counting >= 500)
                        {
                                //Mudança de Estado
                                if(protec_state == 0)
                                {
                                        send_byte = leading_bytes | 0b00000010;
                                        protec_state = 0;
                                        Serial.println(String(send_byte));
                                }
                                else{
                                        if(protec_state==1)
                                        {
                                                send_byte = leading_bytes & 0b11111101;
                                                protec_state = 0;
                                                Serial.println(String(send_byte));
                                        }
                                }
                                //Atualiza
                                refresh_device(send_byte, registers);
                                protec_state = 0;
                        }
                }
        }
        Serial.println("Switch ok");
        Serial.println();
        server.send(200, "text/plain", "Protection relay switched");
}

void web_switch_phase(){
        if (phase_state == 0)
        {
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
        server.on("/zerar", zerar);
        server.onNotFound(handleNotFound);
        server.begin();
        Serial.println("HTTP server started");
        attachInterrupt(digitalPinToInterrupt(btn2), switch_protection, FALLING);
        attachInterrupt(digitalPinToInterrupt(btn1), switch_phase, FALLING); //Since the button is pulled up it is better to trigger the interrupt with the FALLING transition
}

void loop(){
        server.handleClient();
}
