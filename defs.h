#pragma once

#include "Arduino.h"
#include <math.h>

#ifndef uchr
#define uchr unsigned char
#endif

#ifndef uint
#define uint unsigned int
#endif

#ifndef ulong
#define ulong unsigned long
#endif

#define Deg2Rad(x) ((x) / 180.0 * M_PI)
#define Rad2Deg(x) ((x) / M_PI * 180.0)

#define HOLDER_SERVO_1_PIN 22
#define HOLDER_SERVO_2_PIN 23
#define WEIGHT_SERVO_1_PIN 24
#define WEIGHT_SERVO_2_PIN 25

#define CONTROL_FREQUENCE  20

#define PWM_MIN            0
#define PWM_MAX            255

#define SWITCH_HIGH		   0x12
#define SWITCH_MID         0x11
#define SWITCH_LOW	       0x10

#define MOTOR_PIN1         2
#define MOTOR_PIN2         3
#define MOTOR_EN		   6

#define GYRO_Z_MAX         10
#define GYRO_Z_MIN         -10

#define HOLDER_MAX_ANGLE   120
#define HOLDER_MIN_ANGLE   0

#define LED 13
#define BEEPER			   38

#define SERVO_MAX          2500
#define SERVO_MID	       1500
#define SERVO_MIN          500

/*
�Ƿ�ʹ�ö�ʱ�����ÿ���ѭ����
1 ʹ�ö�ʱ��
0 ��ÿ��Radio���º���������
*/
#define USE_TIMER_LOOP		0
#define USE_LOG_FILE		0

#define LOG_FILE_NAME		"log.txt"
#define LOG_TITLE			"time,ch1,ch2,ch3,ch4,ch5,ch6,ax,ay,az,gx,gy,gz,roll,pitch,yaw"

/*
����Ŀ����
CH1:ƫ�����ٶ�
CH2:������
CH3:������
CH4:��ת�����ã�
CH5:3�ο���
CH6:��ť
*/
#define yawCh		1
#define pitchCh		2
#define throCh		3
#define rollCh		4
#define switchCh	5
#define knobCh		6

#define beep(t)	digitalWrite(BEEPER,HIGH);delay(t);digitalWrite(BEEPER,LOW);

template<typename T>
class Point
{
public:
	T x, y, z;
	Point(){}
	Point(T x_,T y_,T z_):x(x_),y(y_),z(z_){}
};