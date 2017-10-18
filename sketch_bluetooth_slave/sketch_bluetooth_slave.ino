// This code based on I2C lybraries maded by Jeff Rowberg. Additional information is above.

// I2C device class (I2Cdev) demonstration Arduino sketch for MPU6050 class using DMP (MotionApps v2.0)
// 6/21/2012 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//      2013-05-08 - added seamless Fastwire support
//                 - added note about gyro calibration
//      2012-06-21 - added note about Arduino 1.0.1 + Leonardo compatibility error
//      2012-06-20 - improved FIFO overflow handling and simplified read process
//      2012-06-19 - completely rearranged DMP initialization code and simplification
//      2012-06-13 - pull gyro and accel data from FIFO packet instead of reading directly
//      2012-06-09 - fix broken FIFO read sequence and change interrupt detection to RISING
//      2012-06-05 - add gravity-compensated initial reference frame acceleration output
//                 - add 3D math helper file to DMP6 example sketch
//                 - add Euler output and Yaw/Pitch/Roll output formats
//      2012-06-04 - remove accel offset clearing for better results (thanks Sungon Lee)
//      2012-06-01 - fixed gyro sensitivity to be 2000 deg/sec instead of 250
//      2012-05-30 - basic DMP initialization working

/* ============================================
  I2Cdev device library code is placed under the MIT license
  Copyright (c) 2012 Jeff Rowberg

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
  ===============================================
*/





        // I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
        // for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
        //#include "MPU6050.h" // not necessary if using MotionApps include file
        // Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
        // is used in I2Cdev.h
#include <Wire.h> // library for comunicate with MPU from SCL SDA!!!!!
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#endif
        // class default I2C address is 0x68
        // specific I2C addresses may be passed as a parameter here
        // AD0 low = 0x68 (default for SparkFun breakout and InvenSense evaluation board)
        // AD0 high = 0x69
MPU6050 mpu; // make object "mpu" for MPU MPU6050 class
        //MPU6050 mpu(0x69); // <-- use for AD0 high

        // ------  !!!! for bluetooth !!!!
#include <SoftwareSerial.h>
//RX аnd TX for Bluetooth board
SoftwareSerial bluetooth(10, 11); //RX, TX
        // -------

//        // ----- TIME -----
//#include <Time.h>
//#define TIME_MSG_LEN 11 // time sync to PC is HEADER followed by Unix time_t as ten ASCII digits
//#define TIME_HEADER 'T' // Header tag for serial time sync message
//#define TIME_REQUEST 7 // ASCII bell character requests a time sync message
//        // ----------------

#define OUTPUT_READABLE_YAWPITCHROLL
#define INTERRUPT_PIN 2  // use pin 2 on Arduino Uno & most boards
#define LED_PIN 12 // (Arduino is 13, Teensy is 11, Teensy++ is 6)



bool blinkState = false;
        // MPU control/status vars
int schetchik = 0;
double x_gyro;
double y_gyro;
double z_gyro;
double x_acc;
double y_acc;
double z_acc;
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer
        // orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
        // packet structure for InvenSense teapot demo
uint8_t teapotPacket[14] = { '$', 0x02, 0, 0, 0, 0, 0, 0, 0, 0, 0x00, 0x00, '\r', '\n' };

// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high

void dmpDataReady() {
mpuInterrupt = true;
}


void(* resetFunc) (void) = 0;


// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================



void setup() {


        // join I2C bus (I2Cdev library doesn't do this automatically)
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
Wire.begin();
Wire.setClock(400000);
        // 400kHz I2C clock. Comment this line if having compilation difficulties

#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
    Fastwire::setup(400, true);
#endif

        // initialize serial communication
        // (115200 chosen because it is required for Teapot Demo output, but it's
        // really up to you depending on your project)


bluetooth.begin(38400);
Serial.begin(38400);
//Serial.begin(9600);
//while (!Serial); // wait for Leonardo enumeration, others continue immediately
//serial2.begin(19200); // ----- bluetooth



//        // ----- TIME -----
//Serial.print(hour());
//Serial.print(minute());
//Serial.print(second());
//
//
//        //-----------------





// initialize device
    Serial.println(F("Connecting I2C..."));
    mpu.initialize();
    pinMode(INTERRUPT_PIN, INPUT);
// verify connection
    Serial.println(F("Testing device connections..."));
    Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));
// wait for ready
    Serial.println(F("\nSend any character to begin DMP programming and demo: "));


    // -------- Uncomment if you need to manage start of device manualy --------
//    while (Serial.available() && Serial.read()); // empty buffer
//    while (!Serial.available()); // wait for data
//    while (Serial.available() && Serial.read()); // empty buffer again
    // -------------------------------------------------------------------------


// load and configure the DMP
    Serial.println(F("Initializing DMP..."));
    devStatus = mpu.dmpInitialize();
// supply your own gyro offsets here, scaled for min sensitivity
  mpu.setXGyroOffset(220);
    mpu.setYGyroOffset(76);
    mpu.setZGyroOffset(-85);
    mpu.setZAccelOffset(1788); // 1688 factory default for my test chip
// make sure it worked (returns 0 if so)
    if (devStatus == 0) {
// turn on the DMP, now that it's ready
        Serial.println(F("Enabling DMP..."));
        mpu.setDMPEnabled(true);
// enable Arduino interrupt detection
        Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
        attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();
// set our DMP Ready flag so the main loop() function knows it's okay to use it
        Serial.println(F("DMP ready! Waiting for first interrupt..."));
        dmpReady = true;
// get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
        Serial.print(F("DMP Initialization failed (code "));
        Serial.print(devStatus);
        Serial.println(F(")"));
    }
// configure LED for output
pinMode(LED_PIN, OUTPUT);





}








// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================
void loop() {

        // if programming failed, don't try to do anything
if (!dmpReady) return;
        // wait for MPU interrupt or extra packet(s) available
while (!mpuInterrupt && fifoCount < packetSize) {}
        // reset interrupt flag and get INT_STATUS byte
    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();
//    bluetooth.read();
        // get current FIFO count
fifoCount = mpu.getFIFOCount();
        //Serial.print("FIFO = ");
        //Serial.println(fifoCount);
        // check for overflow (this should never happen unless our code is too inefficient)


if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
        // reset so we can continue cleanly
mpu.resetFIFO();
Serial.println(F("FIFO overflow!"));
Serial.println(schetchik);
schetchik = 1;
        // otherwise, check for DMP data ready interrupt (this should happen frequently)


} else if (mpuIntStatus & 0x02) {
        // wait for correct available data length, should be a VERY short wait
while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
        // read a packet from FIFO
mpu.getFIFOBytes(fifoBuffer, packetSize);
        // track FIFO count here in case there is > 1 packet available
        // (this lets us immediately read more without waiting for an interrupt)
fifoCount -= packetSize;




#ifdef OUTPUT_READABLE_YAWPITCHROLL
        // display Euler angles in degrees
mpu.dmpGetQuaternion(&q, fifoBuffer);
mpu.dmpGetGravity(&gravity, &q);
mpu.dmpGetEuler(euler, &q);
mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

mpu.dmpGetAccel(&aa, fifoBuffer);
mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
mpu.dmpGetLinearAccelInWorld(&aaWorld, &aaReal, &q);
        //Serial.print("ypr\t");
//z_gyro = ypr[0] * 180 / M_PI;
//y_gyro = ypr[1] * 180 / M_PI;
//x_gyro = ypr[2] * 180 / M_PI;

z_gyro = euler[0] * 180 / M_PI;
y_gyro = euler[1] * 180 / M_PI;
x_gyro = euler[2] * 180 / M_PI;

x_acc = aaReal.x;
y_acc = aaReal.y;
z_acc = aaReal.z;

//x_acc = aaWorld.x;
//y_acc = aaWorld.y;
//z_acc = aaWorld.z;

//x_acc = q.x;
//y_acc = q.y;
//z_acc = q.z;

        //Serial.print(ypr[0] * 180 / M_PI);
        //Serial.print("\t");
//Serial.print(a);
//Serial.print(F(","));
//bluetooth.write(a);
//bluetooth.write(F(" "));
//bluetooth.write(F(","));
        //Serial.print(ypr[1] * 180 / M_PI);
        //Serial.print("\t");
//Serial.print(b);
//Serial.print(F(","));
//bluetooth.write(b);
//bluetooth.write(F(" "));
//bluetooth.write(F(","));
        //Serial.println(ypr[2] * 180 / M_PI);
//Serial.print(c);
//Serial.println(F(""));
//bluetooth.write(c);
//bluetooth.write(F(" \n"));
//bluetooth.write(F("\n"));

/*
Serial.print(ypr[0]);
Serial.print(ypr[1]);
Serial.println(ypr[2]);*/

Serial.println("" + String(z_gyro) + " " + String(y_gyro) + " " + String(x_gyro) + " " + String(x_acc) + " " + String(y_acc) + " " + String(z_acc) + "");


        //Serial.print("\t");
        //Serial.print("count");
        //Serial.print("\t");
        //Serial.println(schetchik++);

schetchik++;
#endif


// if (Serial.available() )
// {
// processSyncMessage();
// }
// if (timeStatus() == timeNotSet){
// Serial.println("waiting for sync message");}
// else{
// digitalClockDisplay();
// delay(1000);
//}

       // blink LED to indicate activity
blinkState = !blinkState;
digitalWrite(LED_PIN, blinkState);


// Send data from bluetooth to PC
  if(Serial.available()>0){
        int stop_mes = Serial.read();
        //Serial.println(stop_mes, DEC);
        if (stop_mes == 48){ // when we send "0" in Serial we have 48 in out, that's why we should use "48" for reset arduino
          Serial.println("You have stoped device!!! \n");
          delay(1000);
          resetFunc();
        } // Передаём данные из программной шины UART в аппаратную  (от модуля     через Arduino к компьютеру)
  }
}
}
