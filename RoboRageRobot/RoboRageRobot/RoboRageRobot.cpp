/*
 * RoboRageRobot.cpp
 *
 * Created: 7/31/2016 8:42:16 PM
 *  Author: Andrew_2
 */ 

#include "RoboRageRobot.h"
#include <avr/io.h>
#include "Robot.h"

void setup();
void loop();

SoftwareSerial serial(0,1);

int main(void) {
	init();
	
	#ifdef USBCON
	USBDevice.attach();
	#endif
	
	setup();
    while(1)
    {
        loop();
		if( serialEventRun ) serialEventRun();
    }
}

void setup() {
	Serial.begin(USB_BAUD);
	setupRobot();
}

void loop() {
	
	runRobot();
	delay(20);
}
