#include "Radio.h"
#include "Utils.h"

// Shared variables are updated by the ISR and read by loop.
// In loop we immediatly take local copies so that the ISR can keep ownership of the
// shared ones. To access these in loop
// we first turn interrupts off with noInterrupts
// we take a copy to use in loop and the turn interrupts back on
// as quickly as possible, this ensures that we are always able to receive new signals

volatile uint16_t unCh1InShared;
volatile uint16_t unCh2InShared;
volatile uint16_t unCh3InShared;
volatile uint16_t unCh4InShared;
volatile uint16_t unCh5InShared;
volatile uint16_t unCh6InShared;

extern volatile uint8_t bUpdateFlagsShared;

// These are used to record the rising edge of a pulse in the calcInput functions
uint32_t ulCh1Start;
uint32_t ulCh2Start;
uint32_t ulCh3Start;
uint32_t ulCh4Start;
uint32_t ulCh5Start;
uint32_t ulCh6Start;

uint16_t unCh1In;
uint16_t unCh2In;
uint16_t unCh3In;
uint16_t unCh4In;
uint16_t unCh5In;
uint16_t unCh6In;

bool getRadio(int pChannels[])
{

	pChannels[0] = 0;
	pChannels[1] = unCh1In;
	pChannels[2] = unCh2In;
	pChannels[3] = unCh3In;
	pChannels[4] = unCh4In;
	pChannels[5] = unCh5In;
	pChannels[6] = unCh6In;

	for(int i = 1;i <= 6;++i)
	{
		pChannels[i] = map_f(pChannels[i], minValue[i], maxValue[i], MAP_RADIO_LOW, MAP_RADIO_HIGH);
	}

	if (DEADZONE_ENABLE)
	{
		int i;
		for (i = 2; i <= 4; ++i)
		{
			if (abs( pChannels[i] - MAP_RADIO_MID) < DEADZONE)
			{
				pChannels[i] = MAP_RADIO_MID;
			}
			else
			{
				pChannels[i] = pChannels[i] - sgn(pChannels[i] - MAP_RADIO_MID) * DEADZONE;
			}
		}

		//Special case for the throttle
		if (pChannels[1] < DEADZONE)
		{
			pChannels[1] = MAP_RADIO_LOW;
		}
		else
		{
			pChannels[1] = pChannels[1] - DEADZONE;
		}
	}

	if (PRINT_ALL_CHANNELS)
	{
		int i;
		for (i = 1; i <= 6; ++i)
		{
			Serial.print(pChannels[i]);
			Serial.print("\t");
		}
		Serial.print("\n");
	}
	return true;
}

void updateRadio()
{
	unCh1In = unCh1InShared;
	unCh2In = unCh2InShared;
	unCh3In = unCh3InShared;
	unCh4In = unCh4InShared;
	unCh5In = unCh5InShared;
	unCh6In = unCh6InShared;

	// clear shared copy of updated flags as we have already taken the updates
	bUpdateFlagsShared = 0;
}

// simple interrupt service routine
void calcCh1()
{
	if (digitalRead(CH1_IN_PIN) == HIGH)
	{
		ulCh1Start = micros();
	}
	else
	{
		unCh1InShared = (uint16_t)(micros() - ulCh1Start);
		bUpdateFlagsShared |= CH1_FLAG;
	}
}

void calcCh2()
{
	if (digitalRead(CH2_IN_PIN) == HIGH)
	{
		ulCh2Start = micros();
	}
	else
	{
		unCh2InShared = (uint16_t)(micros() - ulCh2Start);
		bUpdateFlagsShared |= CH2_FLAG;
	}
}

void calcCh3()
{
	if (digitalRead(CH3_IN_PIN) == HIGH)
	{
		ulCh3Start = micros();
	}
	else
	{
		unCh3InShared = (uint16_t)(micros() - ulCh3Start);
		bUpdateFlagsShared |= CH3_FLAG;
	}
}

void calcCh4()
{
	if (digitalRead(CH4_IN_PIN) == HIGH)
	{
		ulCh4Start = micros();
	}
	else
	{
		unCh4InShared = (uint16_t)(micros() - ulCh4Start);
		bUpdateFlagsShared |= CH4_FLAG;
	}
}


void calcCh5()
{
	if (digitalRead(CH5_IN_PIN) == HIGH)
	{
		ulCh5Start = micros();
	}
	else
	{
		unCh5InShared = (uint16_t)(micros() - ulCh5Start);
		bUpdateFlagsShared |= CH5_FLAG;
	}
}

void calcCh6()
{
	if (digitalRead(CH6_IN_PIN) == HIGH)
	{
		ulCh6Start = micros();
	}
	else
	{
		unCh6InShared = (uint16_t)(micros() - ulCh6Start);
		bUpdateFlagsShared |= CH6_FLAG;
	}
}
