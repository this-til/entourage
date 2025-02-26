// CoreBeep.h

#ifndef _COREBEEP_h
#define _COREBEEP_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#define COREBEEP_PIN		10
#define TURNON				HIGH
#define TURNOFF				LOW

#define COREBEEP_PORT		PORTB
#define COREBEEP_BIT		PB4

class _CoreBeep
{
public:
	_CoreBeep();
	~_CoreBeep();

	void Initialization(void);
	void TurnOn(void);
	void TurnOff(void);
	

private:
	uint8_t pin;

	void CBeepWhile(uint16_t time);
};


extern _CoreBeep CoreBeep;


#endif

