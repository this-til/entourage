// CoreLED.h

#ifndef _CORELED_h
#define _CORELED_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#define LED1	1
#define LED2	2
#define LED3	3
#define LED4	4

class _CoreLED
{
public:
	_CoreLED();
	~_CoreLED();

	void Initialization(void);
	void TurnOn(uint8_t s);
	void TurnOff(uint8_t s);
	void TurnOnOff(uint8_t s);

private:

};

extern _CoreLED CoreLED;


#endif

