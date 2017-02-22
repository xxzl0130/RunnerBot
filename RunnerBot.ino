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
PID pidGyro(0.5, 0.001, 0.0001, GYRO_Z_MAX, GYRO_Z_MIN, SERVO_MAX - SERVO_MID, SERVO_MIN - SERVO_MID);
DC_Motor motor(MOTOR_PIN1, MOTOR_PIN2);
int radioSignals[CHANNELS + 2]; // �ɿط������ź�(0-1000)
Point<float> acc, gyro;
double rool, pitch, yaw;

/*
����Ŀ����
CH1:ƫ�����ٶ�
CH2:������
CH3:������
CH4:��ת�����ã�
CH5:3�ο���
CH6:��ť
*/
int ctrlSignals[CHANNELS + 2];
uchr ledState = 0;

#define MPU JY901

void work();
void convertSignals();
void setup()
{
	Serial.begin(9600);
	Serial1.begin(9600); // MPU9250

	initRadio();
	
	holderServo1.attach(HOLDER_SERVO_1_PIN);
	holderServo2.attach(HOLDER_SERVO_2_PIN);
	weightServo1.attach(WEIGHT_SERVO_1_PIN);
	weightServo2.attach(WEIGHT_SERVO_2_PIN);
#if USE_TIMER_LOOP
	workLoop.attachInterrupt(work);
	workLoop.setFrequency(CONTROL_FREQUENCE);
	workLoop.start();
#endif //USE_TIMER_LOOP
	pinMode(LED, OUTPUT);

	MPU.attach(Serial1);
	MPU.setAccRange(4);
	MPU.setGyroRange(500);
}

void loop()
{
	/*
	if (bUpdateFlagsShared == ALL_UPD_FLAG)
	{
		updateRadio();
		getRadio(radioSignals);
#if !USE_TIMER_LOOP
		work();
#endif
	}*/
	Serial.print(MPU.getRoll());
	Serial.print(" ");
	Serial.print(MPU.getPitch());
	Serial.print(" ");
	Serial.println(MPU.getYaw());
	delay(300);

}

void work()
{
	digitalWrite(13, ledState ^= 1);
	convertSignals();
	if (ctrlSignals[5] == SWITCH_HIGH)
	{
		motor.run(FORWORD, ctrlSignals[3]);
		
		int gyroCtrl = pidGyro.update(gyro.z - ctrlSignals[1], gyro.z);
		weightServo1.writeMicroseconds(SERVO_MID + gyroCtrl);
		weightServo2.writeMicroseconds(SERVO_MID + gyroCtrl);
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
*/
void convertSignals()
{
	ctrlSignals[1] = static_cast<int>(map_f(radioSignals[1], MAP_RADIO_LOW, MAP_RADIO_HIGH, GYRO_Z_MIN, GYRO_Z_MAX) * 100 + 0.5);
	ctrlSignals[2] = static_cast<int>(map_f(radioSignals[2], MAP_RADIO_LOW, MAP_RADIO_HIGH, HOLDER_MIN_ANGLE, HOLDER_MAX_ANGLE) + 0.5);
	ctrlSignals[3] = static_cast<int>(map_f(radioSignals[3], MAP_RADIO_LOW, MAP_RADIO_HIGH, PWM_MIN, PWM_MAX) + 0.5);
	ctrlSignals[4] = static_cast<int>(map_f(radioSignals[4], MAP_RADIO_LOW, MAP_RADIO_HIGH, PWM_MIN, PWM_MAX) + 0.5);
	
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
	acc.x = MPU.getAccX();
	acc.y = MPU.getAccY();
	acc.z = MPU.getAccZ();
	gyro.x = MPU.getGyroX();
	gyro.y = MPU.getGyroY();
	gyro.z = MPU.getGyroZ();
	rool = MPU.getRoll();
	pitch = MPU.getPitch();
	yaw = MPU.getYaw();
}
