#ifndef Radio_h
#define Radio_h

#include "Arduino.h"

#define  PRINT_ALL_CHANNELS 0

#define CHANNELS   6

#define CH1_IN_PIN 22
#define CH2_IN_PIN 24
#define CH3_IN_PIN 26
#define CH4_IN_PIN 28
#define CH5_IN_PIN 30
#define CH6_IN_PIN 32

#define CH1_FLAG 1
#define CH2_FLAG 2
#define CH3_FLAG 4
#define CH4_FLAG 8
#define CH5_FLAG 16
#define CH6_FLAG 32
#define ALL_UPD_FLAG 0x3f

#define MAP_RADIO_HIGH 1000
#define MAP_RADIO_LOW 0
#define MAP_RADIO_MID (MAP_RADIO_HIGH + MAP_RADIO_LOW)/2

#define MAP_RADIO_HIGH 1000

#define DEADZONE_ENABLE 0
#define DEADZONE_PERCENT 0.05

#define DEADZONE (MAP_RADIO_HIGH - MAP_RADIO_LOW) * DEADZONE_PERCENT

#define MID(x,y) ((x + y) / 2)

const int maxValue[] = {0,1981,1977,1967 ,1973 ,2001 ,2000};
const int minValue[] = {0,982,986,1001 ,996 ,996 ,1000};
const int midValue[] = {0,MID(maxValue[1],minValue[1]),MID(maxValue[2],minValue[2]),
	MID(maxValue[3],minValue[3]),MID(maxValue[4],minValue[4]),
	MID(maxValue[5],minValue[5]),MID(maxValue[6],minValue[6])};

void updateRadio();
bool getRadio(int pChannels[]);
void initRadio();
void calcCh1();
void calcCh2();
void calcCh3();
void calcCh4();
void calcCh5();
void calcCh6();

#endif

