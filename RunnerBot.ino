#include "include/Radio/Radio.h"
#include <Servo.h>
#include "include/DueTimer/DueTimer.h"
#include "include/PID/PID.h"
#include "include/JY901/JY901.h"
#include "include/Radio/Utils.h"
#include "include/DC_Motor/DC_Motor.h"
#include "defs.h"
#include "file.h"

volatile uint8_t bUpdateFlagsShared;

#if USE_TIMER_LOOP
DueTimer workLoop = Timer.getAvailable();
#endif //USE_TIMER_LOOP

Servo holderServo1,holderServo2,weightServo1,weightServo2;
PID pidGyro(0.5, 0.001, 0.0001, GYRO_Z_MAX, GYRO_Z_MIN, SERVO_MAX - SERVO_MID, SERVO_MIN - SERVO_MID);
DC_Motor motor(MOTOR_PIN1, MOTOR_PIN2);
int radioSignals[CHANNELS + 2]; // 飞控发来的信号(0-1000)
Point<float> acc, gyro;
double roll, pitch, yaw;

/*
控制目标量
CH1:偏航角速度
CH2:俯仰角
CH3:油门量
CH4:滚转（不用）
CH5:3段开关
CH6:旋钮
*/
int ctrlSignals[CHANNELS + 2];
uchr ledState = 0;

#define MPU JY901

void work();
void convertSignals();
#if USE_LOG_FILE
void printLog();
#endif //USE_LOG_FILE

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

#if USE_LOG_FILE
	initSD();
#endif //USE_LOG_FILE

	pinMode(LED, OUTPUT);
	pinMode(BEEPER, OUTPUT);

	MPU.attach(Serial1);
	MPU.setAccRange(4);
	MPU.setGyroRange(500);

	beep(500);
	delay(500);
	beep(500);
	delay(500);
}

void loop()
{
	if (bUpdateFlagsShared == ALL_UPD_FLAG)
	{
		updateRadio();
		getRadio(radioSignals);
#if !USE_TIMER_LOOP
		work();
#endif
#if USE_LOG_FILE
		printLog(ctrlSignals,acc,gyro,roll,yaw,pitch);
#endif
	}
}

// 主工作函数
void work()
{
	static auto lastStatic = SWITCH_LOW;
	digitalWrite(13, ledState ^= 1);
	convertSignals();
	if (ctrlSignals[switchCh] == SWITCH_MID)
	{
		if(lastStatic != SWITCH_MID)
		{
			beep(100);
			delay(100);
			beep(100);
			lastStatic = SWITCH_MID;
		}
		motor.run(FORWORD, ctrlSignals[throCh]);
		/*
		int gyroCtrl = pidGyro.update(gyro.z - ctrlSignals[yawCh], gyro.z);
		weightServo1.writeMicroseconds(SERVO_MID + gyroCtrl);
		weightServo2.writeMicroseconds(SERVO_MID + gyroCtrl);
		holderServo1.write(ctrlSignals[pitchCh]);
		holderServo2.write(ctrlSignals[pitchCh]);
		*/
		weightServo1.writeMicroseconds(radioSignals[yawCh]);
		weightServo2.writeMicroseconds(radioSignals[yawCh]);
		holderServo1.writeMicroseconds(radioSignals[pitchCh]);
		holderServo2.writeMicroseconds(radioSignals[pitchCh]);
	}
	else if(ctrlSignals[switchCh] == SWITCH_LOW)
	{
		motor.stop();
		if(lastStatic != SWITCH_LOW)
		{
			lastStatic = SWITCH_LOW;
			beep(500);
		}
	}
	else
	{
		if(lastStatic != SWITCH_HIGH)
		{
			beep(100);
			delay(100);
			beep(100);
			delay(100);
			beep(100);
			delay(100);
			lastStatic = SWITCH_HIGH;
		}
	}
	for (int i = 1; i <= CHANNELS; ++i)
	{
		Serial.print(ctrlSignals[i]);
		Serial.print("\t");
	}
	Serial.print("\n");
}

/*
将遥控器发来的0-1000的信号量变为各器件的控制量范围
*/
void convertSignals()
{
	// Todo:调整信号处理及控制
	ctrlSignals[yawCh]   = static_cast<int>(map_f(radioSignals[yawCh], MAP_RADIO_LOW, MAP_RADIO_HIGH, GYRO_Z_MIN, GYRO_Z_MAX) * 100 + 0.5);
	ctrlSignals[pitchCh] = static_cast<int>(map_f(radioSignals[pitchCh], MAP_RADIO_LOW, MAP_RADIO_HIGH, HOLDER_MIN_ANGLE, HOLDER_MAX_ANGLE) + 0.5);
	ctrlSignals[throCh]  = static_cast<int>(map_f(radioSignals[throCh], MAP_RADIO_LOW, MAP_RADIO_HIGH, PWM_MIN, PWM_MAX) + 0.5);
	ctrlSignals[rollCh]  = static_cast<int>(map_f(radioSignals[rollCh], MAP_RADIO_LOW, MAP_RADIO_HIGH, PWM_MIN, PWM_MAX) + 0.5);
	
	if(abs(radioSignals[switchCh] - 500) <= 50)
	{
		ctrlSignals[switchCh] = SWITCH_MID;
	}
	else if (radioSignals[switchCh] >= 950)
	{
		ctrlSignals[switchCh] = SWITCH_HIGH;
	}
	else
	{
		ctrlSignals[switchCh] = SWITCH_LOW;
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
	roll = MPU.getRoll();
	pitch = MPU.getPitch();
	yaw = MPU.getYaw();
}