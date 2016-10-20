/*
 * MessageWriter.cpp
 *
 * Created: 7/31/2016 9:55:48 PM
 *  Author: Andrew_2
 */ 


#define MESSAGE_BUFFER_SIZE 254
#define WRITE_BUFFER_SIZE 256

#include "MessageWriter.h"
#include "RoboRageRobot.h"

unsigned char writeBuffer [WRITE_BUFFER_SIZE];

int request;

void stuffData(const unsigned char *ptr, unsigned char length, unsigned char *dst);

void setupWriter() {
}

void writeMessage(unsigned char outMessageBuffer[], int len) {
	if(len <= MESSAGE_BUFFER_SIZE) {
	
		stuffData(outMessageBuffer, len, writeBuffer);
	
		for(int i = 0; i < len + 2; i++) {
			Serial.write(writeBuffer[i]);
		}
	}
}


#define FinishBlock(X) (*code_ptr = (X), code_ptr = dst++, code = 0x01)

void stuffData(const unsigned char *ptr, unsigned char length, unsigned char *dst) {
	const unsigned char *end = ptr + length;
	unsigned char *code_ptr = dst++;
	unsigned char code = 0x01;

	while (ptr < end)
	{
		if (*ptr == 0) {
			FinishBlock(code);
		} else {
			*dst++ = *ptr;
			code++;
		}
		ptr++;
	}

	FinishBlock(code);
	dst--;
	*dst = 0;
}

