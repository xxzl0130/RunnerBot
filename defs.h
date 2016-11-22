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

#define HOLDER_SERVO_1_PIN 50
#define HOLDER_SERVO_2_PIN 51
#define WEIGHT_SERVO_1_PIN 52
#define WEIGHT_SERVO_2_PIN 53

#define CONTROL_FREQUENCE  20

#define PWM_MIN            0
#define PWM_MAX            255

#define SWITCH_MID         0
#define SWITCH_HIGH		   1
#define SWITCH_LOW	       -1

#define MOTOR_PIN1         2
#define MOTOR_PIN2         3

#define WEIGHT_MAX_ANGLE   30
#define WEIGHT_MIN_ANGLE   -30

#define HOLDER_MAX_ANGLE  120
#define HOLDER_MIN_ANGLE  -120