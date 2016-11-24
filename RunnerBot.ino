#include "include/Radio/Radio.h"
#include <Servo.h>
#ifndef Servo_h
#include <Servo/src/Servo.h>
#endif
#include "include/DueTimer/DueTimer.h"
#include "include/PID/PID.h"
#include "include/JY901/JY901.h"
#include "include/Radio/Utils.h"
#include "include/DC_Motor/DC_Motor.h"
#include "defs.h"

volatile uint8_t bUpdateFlagsShared;
DueTimer workLoop = Timer.getAvailable();

Servo holderServo1,holderServo2,weightServo1,weightServo2;
PID pid(5, 0.001, 0.0001, 10000, 0, 2000, 0);
DC_Motor motor(MOTOR_PIN1, MOTOR_PIN2);
int radioSignals[CHANNELS + 2]; // �ɿط������ź�(0-1000)
int ctrlSignals[CHANNELS + 2]; // ת��������Ŀ�����
uchr ledState = 0;

#define MPU JY901

void work();
void convertSignals();
void setup()
{
	Serial.begin(9600);
	Serial1.begin(9600); // MPU9250

	attachInterrupt(CH1_IN_PIN, calcCh1, CHANGE);
	attachInterrupt(CH2_IN_PIN, calcCh2, CHANGE);
	attachInterrupt(CH3_IN_PIN, calcCh3, CHANGE);
	attachInterrupt(CH4_IN_PIN, calcCh4, CHANGE);
	attachInterrupt(CH5_IN_PIN, calcCh5, CHANGE);
	attachInterrupt(CH6_IN_PIN, calcCh6, CHANGE);
	
	holderServo1.attach(HOLDER_SERVO_1_PIN);
	holderServo2.attach(HOLDER_SERVO_2_PIN);
	weightServo1.attach(WEIGHT_SERVO_1_PIN);
	weightServo2.attach(WEIGHT_SERVO_2_PIN);

	workLoop.attachInterrupt(work);
	workLoop.setFrequency(CONTROL_FREQUENCE);
	workLoop.start();

	pinMode(LED, OUTPUT);

	MPU.attach(Serial1);
}

void loop()
{
	if (bUpdateFlagsShared == ALL_UPD_FLAG)
	{
		updateRadio();
		getRadio(radioSignals);
		//holderServo1.writeMicroseconds(1000 + radioSignals[1]);
		
	}
	//delay(500);
}

void work()
{
	digitalWrite(13, ledState ^= 1);
	convertSignals();
	if (ctrlSignals[5] == SWITCH_HIGH)
	{
		motor.run(FORWORD, ctrlSignals[3]);
		weightServo1.write(ctrlSignals[1]);
		weightServo2.write(ctrlSignals[1]);
		holderServo1.write(ctrlSignals[2]);
		holderServo2.write(ctrlSignals[2]);
		for (int i = 1; i <= CHANNELS; ++i)
		{
			Serial.print(ctrlSignals[i]);
			Serial.print("\t");
		}
		Serial.print("\n");
	}
}

/*
��ң����������0-1000���ź�����Ϊ�������Ŀ�������Χ
CH1��ƫ����
CH2��������
CH3������
CH4����ת����
CH5��3�ο���
CH6����ť
TODO:���map�����Ը����ı任
*/
void convertSignals()
{
	ctrlSignals[1] = static_cast<uint>(map_f(radioSignals[1], MAP_RADIO_LOW, MAP_RADIO_HIGH, WEIGHT_MIN_ANGLE, WEIGHT_MAX_ANGLE) + 0.5);
	ctrlSignals[2] = static_cast<uint>(map_f(radioSignals[2], MAP_RADIO_LOW, MAP_RADIO_HIGH, HOLDER_MIN_ANGLE, HOLDER_MAX_ANGLE) + 0.5);
	ctrlSignals[3] = static_cast<uint>(map_f(radioSignals[3], MAP_RADIO_LOW, MAP_RADIO_HIGH, PWM_MIN, PWM_MAX) + 0.5);
	ctrlSignals[4] = static_cast<uint>(map_f(radioSignals[4], MAP_RADIO_LOW, MAP_RADIO_HIGH, PWM_MIN, PWM_MAX) + 0.5);
	
	if(abs(radioSignals[5] - 500) <= 20)
	{
		ctrlSignals[5] = SWITCH_MID;
	}
	else if (radioSignals[5] >= 980)
	{
		ctrlSignals[5] = SWITCH_HIGH;
	}
	else
	{
		ctrlSignals[5] = SWITCH_LOW;
	}
}

void serialEvent1()
{
	MPU.receiveSerialData();
}
