/*
 * ControllerReader.h
 *
 * Created: 8/1/2016 1:49:01 PM
 *  Author: Andrew_2
 */ 


#ifndef MESSAGEREADER_H_
#define MESSAGEREADER_H_

void setupReader();
void setParser(void (*parser) (unsigned char*, int));
void readData();


#endif /* MESSAGEREADER_H_ */