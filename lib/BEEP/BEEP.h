// BEEP.h

#ifndef _BEEP_h
#define _BEEP_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif


#define BEEP_ON			HIGH
#define BEEP_OFF		LOW
#define BEEP_PIN		9


class _BEEP
{
public:
	_BEEP();
	~_BEEP();
	void Initialization(void);
	void TurnOn(void);
	void TurnOff(void);

private:
	uint8_t pin;
};

extern _BEEP	BEEP;

#endif

