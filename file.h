#pragma once

#include <Arduino.h>
#include "defs.h"
#include <SPI.h>
#include <SD.h>

/*
ICSP
1_MISO	oo	2_+Vcc
3_SCK	oo	4_MOSI
5_REST	oo	6_GND
*/
#define MOSI	ICSP_4
#define MISO	ICSP_1
#define CLK		ICSP_3
#define SD_CS	52

extern File logFile;
extern File configFile;

bool initSD();
void initLogFile();
void readConfig(int* buf,uchr size);
void writeConfig(int* buf, uchr size);
void printLog(const int* ctrlSignals, const Point<float>& acc, const Point<float>& gyro,
	const double& roll, const double& yaw, const double& pitch);