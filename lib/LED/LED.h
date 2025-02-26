// LED.h

#ifndef _LED_h
#define _LED_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#define LED_ON			HIGH
#define LED_OFF			LOW
#define LED_PIN_R		12
#define LED_PIN_L		11

class _LED
{
	public:
		_LED();
		~_LED();
		void Initialization(void);
		void RightTurnOn(void);
		void RightTurnOff(void);
		void LeftTurnOn(void);
		void LeftTurnOff(void);

	private:
		uint8_t L_pin;
		uint8_t R_pin;
};

extern _LED LED;

#endif

