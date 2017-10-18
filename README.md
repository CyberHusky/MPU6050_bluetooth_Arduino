# MPU6050_bluetooth
Code for transferrig data from MPU6050 to PC via bluetooth. Plus visualisation in Processing.

## Getting Started
You need:
- 2x Arduino borad (one for PC, one for MPU6050);
- 2x bluetooth module (for example HC-05, that could work in master and slave mode);
- 1x diode (optionaly);
- 1x battery (optinaly).
- Install next library:
* [I2C](https://github.com/jrowberg/i2cdevlib) - For communication with device
* [MPU6050](https://github.com/jarzebski/Arduino-MPU6050) - For MPU6050

All connection you can find in fritzing file.

## Bluetooth setup
Before starting communication between MPU6050 and PC via bluetooth you need to setup your bluetooth module. We need to make bluetooth master/slave pair with same speed. Algorithm:
1. Install code from file to Arduino board, that will implement bluetooth setup:
'''
sketch_bluetooth_setup.ino
'''
2. Connect first bluetooth module to Arduino:
- Arduino 11 pin to HC-05 Rx pin;
- Arduino 10 pin to HC-05 Tx pin;
- Arduino 9 pin to HC-05 EN pin (or to KEY pin).
3. Make first powering HC-05 and keep the EN button clamped (if you have lucky you wouldn't need to keep button clamped). In right case the diode should begin to flash with a long period.
4. Go to Arduino port monitor switch sending mode to "NL & CR" for future AT comand. Without if you should need tape "\r\n" command in the end of each AT command.
5. Send AT comand to HC-05 for programming slave bluetooth:
AT - to check the connection with Arduino
AT+DISC - try to disconnect you bluetooth if he was connected
AT+ORGL - make default setup
AT+RMAAD - clear saved before pair on bluetooth
AT+NAME=iArduino - new name of module
AT+PSWD=7777 - new PIN security code to module
AT+ROLE=0 - make a slave role mode to module
AT+ADDR? - try to know address name of device (in my case it was: +ADDR:98d3:36:80efc0). Save this address!
AT+UART=38400,0,0 - make 38400 speed
6. Connect second bluetooth module as before. Send AT comand to HC-05 for programming master bluetooth:
AT+DISC - try to disconnect you bluetooth if he was connected
AT+ORGL - make default setup
AT+RMAAD - clear saved before pair on bluetooth
AT+BIND=98d3,36,80efc0 - make fix address for connecting to slave bluetooth automaticly
AT+CMODE=0 - setup module for connec to fix address device
AT+ROLE=1 - make a master role mode to module
AT+PSWD=7777 - fixed security PIN for connecting
AT+UART=38400,0,0 - make 38400 speed
AT+PAIR=98d3,36,80efc0,5 - initialize pair connection to slave module with 5sec delay
AT+RESET - reset module

The bluetooth modules should automaticly connected to each other!

## Installing code to slave bluetooth
Connect MPU6050, slave bluetooth and battery to Arduino and install slave bluetooth
'''
sketch_bluetooth_slave.ino
'''

## Installing code to master bluetooth
Connect master bluetooth to Arduino and connected Arduino to PC. Install master bluetooth code from file:
'''
sketch_bluetooth_master.ino
'''

## Processing
For visualisation and saving transfer data You need to install Processing version 3.3. Start next code in file after bluetooth have already connected to each other:
'''
Processing_save.pde
'''
Press space button for start saving to "data.txt" file in your home directory. Press space button to stop saving.

## Authors
Gordieiev Artem
For donation or questions you could write me on email: gordieie.artem@gmail.com

Code development with support of [Biocybernetic and aerocosmic medicine department](http://bikam.kiev.ua/) (National aviation university), Ukraine, Kiev.
And don't forget about authors that made needed libraries:
* [jarzebski](https://github.com/jrowberg/i2cdevlib)
* [jrowberg](https://github.com/jarzebski/Arduino-MPU6050)
