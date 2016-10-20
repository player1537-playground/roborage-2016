/*
 * Robot.cpp
 *
 * Created: 10/15/2016 11:47:17 AM
 *  Author: Andrew_2
 */ 


#include "RoboRageRobot.h"
#include "Robot.h"
#include "MessageReader.h"
#include "MessageWriter.h"
#include "MotorController.h"



#define MESSAGE_BUFFER_SIZE 254
unsigned char outMessageBuffer [MESSAGE_BUFFER_SIZE];

void parseMessage(unsigned char *message, int len);
void writeData();



void setupRobot() {
	setupReader();
	setParser(&parseMessage);
	setupWriter();
	
	writeMotor(0, 0, 0);
	writeMotor(1, 0, 0);
	
}


void runRobot() {
	readData();
	
	//writeMotor(0, 1, 100);
	//writeMotor(1, 1, 128);
	
	updateMotorController();
	
	//writeData();
	
}

void parseMessage(unsigned char *message, int len) {
	unsigned char readLength = message[0];
	if(len >= 8) {
		unsigned char type = message[1];
		
		unsigned char dir0 = message[2];
		int val0 = (message[3] << 8) + message[4];
		unsigned char dir1 = message[5];
		int val1 = (message[6] << 8) + message[7];
		
		writeMotor(0, dir0, val0);
		writeMotor(1, dir1, val1);
	}
	
}

#define SHORT_TO_BYTES(shortVal, byteArray, index) {byteArray[index] = (shortVal >> 8)  & 0xFF; byteArray[index + 1] = shortVal & 0xFF;}

void writeData() {
	
	for(int i = 0; i < 16; i++) {
		outMessageBuffer[i] = 0;
	}
	
	unsigned char len = 0;
	
		len = 16;
		
		outMessageBuffer[0] = len;
		outMessageBuffer[1] = 1;
		
		SHORT_TO_BYTES(readCurrentSense(0), outMessageBuffer, 2);
		SHORT_TO_BYTES(readCurrentSense(1), outMessageBuffer, 4);
		
		
		writeMessage(outMessageBuffer, len);


}