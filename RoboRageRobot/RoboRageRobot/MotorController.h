/*
 * MotorController.h
 *
 * Created: 10/15/2016 12:07:51 PM
 *  Author: Andrew_2
 */ 


#ifndef MOTORCONTROLLER_H_
#define MOTORCONTROLLER_H_

#define WATCHDOG_TIMEOUT 3000

void setupMotorController();
void updateMotorController();
void motorOff(unsigned char motorId);
void writeMotor(unsigned char motorId, unsigned char dir, int value);
int readCurrentSense(unsigned char motorId);


#endif /* MOTORCONTROLLER_H_ */