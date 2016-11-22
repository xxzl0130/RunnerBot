#include "include/Radio/Radio.h"
#include <Servo.h>
#ifndef Servo_h
#include <Servo/src/Servo.h>
#endif
#include "include/DueTimer/DueTimer.h"
#include "include/PID/PID.h"

volatile uint8_t bUpdateFlagsShared;
DueTimer workLoop = DueTimer(8);

Servo myServo;
PID pid(5, 0.001, 0.0001, 1e4, 0, 2000, 0);

void setup()
{
	Serial.begin(9600);
	attachInterrupt(CH1_IN_PIN, calcCh1, CHANGE);
	attachInterrupt(CH2_IN_PIN, calcCh2, CHANGE);
	attachInterrupt(CH3_IN_PIN, calcCh3, CHANGE);
	attachInterrupt(CH4_IN_PIN, calcCh4, CHANGE);
	attachInterrupt(CH5_IN_PIN, calcCh5, CHANGE);
	attachInterrupt(CH6_IN_PIN, calcCh6, CHANGE);
	pinMode(23, OUTPUT);
	myServo.attach(23);
}

int values[10];

void loop()
{

	if (bUpdateFlagsShared == ALL_UPD_FLAG)
	{
		updateRadio();
		getRadio(values);
		myServo.writeMicroseconds(1000 + values[1]);
	}
	//delay(500);
}
