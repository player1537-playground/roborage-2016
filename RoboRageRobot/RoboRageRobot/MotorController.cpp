/*
 * MotorController.cpp
 *
 * Created: 10/15/2016 12:06:47 PM
 *  Author: Andrew_2
 */ 

#include "MotorController.h"
#include "RoboRageRobot.h"

#define BRAKEVCC 0
#define CW   1
#define CCW  2
#define BRAKEGND 3
#define CS_THRESHOLD 100

unsigned char inApin[2] = {7, 4};  // INA: Clockwise input
unsigned char inBpin[2] = {8, 9}; // INB: Counter-clockwise input
unsigned char pwmpin[2] = {5, 6}; // PWM input
unsigned char cspin[2] = {2, 3}; // CS: Current sense ANALOG input
unsigned char enpin[2] = {0, 1}; // EN: Status of switches output (Analog pin)
	
unsigned long lastWrite[2];

void setupMotorController() {

	// Initialize digital pins as outputs
	for (int i=0; i<2; i++) {
		pinMode(inApin[i], OUTPUT);
		pinMode(inBpin[i], OUTPUT);
		pinMode(pwmpin[i], OUTPUT);
	}
	// Initialize braked
	for (int i=0; i<2; i++)	{
		digitalWrite(inApin[i], LOW);
		digitalWrite(inBpin[i], LOW);
	}
}

void updateMotorController() {
	for (int i=0; i<2; i++)	{
		if(millis() - lastWrite[i] > WATCHDOG_TIMEOUT) {
			motorOff(i);
		}
	}
}

void motorOff(unsigned char motorId) {
	writeMotor(motorId, 0, 0);
}

/* 
 
 dir: Should be between 0 and 3, with the following result
 0: Brake to VCC
 1: Clockwise
 2: CounterClockwise
 3: Brake to GND
 
 value: pwm should be a value between 0 and 255
*/
void writeMotor(unsigned char motorId, unsigned char dir, int value) {
	// Set inA[motor]
	if (dir == 0 || dir == 1) {
		digitalWrite(inApin[motorId], HIGH);
	} else if(dir == 2 || dir == 3) {
		digitalWrite(inApin[motorId], LOW);
	}

	// Set inB[motor]
	if (dir == 0 || dir == 2) {
		digitalWrite(inBpin[motorId], HIGH);
	} else if (dir == 1 || dir == 3) {
		digitalWrite(inBpin[motorId], LOW);
	}

	analogWrite(pwmpin[motorId], value);
	lastWrite[motorId] = millis();
}

//returns a value between 0 and 1023
int readCurrentSense(unsigned char motorId) {
	return analogRead(cspin[motorId]);
}


