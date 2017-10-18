#include <SoftwareSerial.h>     // Library for "speaking" with Arduino on UART
SoftwareSerial bluetooth(10,11); // Make softSerial object: connect 10 to RX, 11 to TX of bluetooth module
                                  // On Arduino: TX - digital 2, RX - digital 3

void setup(){
pinMode(9,OUTPUT);
digitalWrite(9,HIGH);

Serial.begin(9600);     // Inicialyze data transfer on speed 9600 (between Arduino and PC)
bluetooth.begin(38400);    // Inicialyze data transfer on speed 38400 (between bluetooth and Arduino)

Serial.println("Enter AT commands:");
}
// Make retransmission: data from bluetooth send to PC and data from PC send to bluetooth
void loop(){
    if(softSerial.available()){
      Serial.write(softSerial.read());
    } // Sending data from bluetooth to PC

    if(    Serial.available()){
      bluetooth.write(Serial.read());
    } // Sending data from PC to bluetooth
}
