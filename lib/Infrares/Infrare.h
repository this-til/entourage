// infrare.h

#ifndef _INFRARE_h
#define _INFRARE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#define RITX_PIN	13
#define TXD_ON		HIGH	//低电平：表示发射打开
#define TXD_OFF		LOW		//高电平：表示发射关闭

class _Infrare
{
public:
	_Infrare(uint8_t _pin);
	_Infrare();
	~_Infrare();

	void Initialization(void);
	void Transmition(uint8_t */* s:发射的数据*/,int/* n数据长度*/);

private:
	uint8_t pin;

};

extern _Infrare Infrare;

#endif

