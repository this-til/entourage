
#ifndef Ultrasonic_h
#define Ultrasonic_h

#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif


#define TRIG_PIN		4
#define ECHO_PIN		PIN_A15

#define CM	1
#define INC 0

class _Ultrasonic
{
  public:
	_Ultrasonic();
	void Initialization(void);
	void Initialization(uint32_t TO);
    long Timing();
	double Ranging(int sys);

  private:
    int Trig_pin;
    int Echo_pin;
	long Time_out;
    long duration, distance_cm, distance_inc;

};

extern _Ultrasonic Ultrasonic;

#endif
