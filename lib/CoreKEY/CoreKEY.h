// CoreKEY.h

#ifndef _COREKEY_h
#define _COREKEY_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

class _CoreKEY
{
public:
	_CoreKEY();
	~_CoreKEY();

	void Initialization(void);
	void Kwhile(void(*Key_Cmd_CB_)(uint8_t fk));
	

private:

	/*static uint32_t key_times;
	static uint8_t Key_cnt[4];
	static uint8_t Key_Rt;*/

	uint8_t Check(void);
	void Run(void);
	void Run(void(*Key_Cmd_CB_)(uint8_t fk));
	void Kwhile(void);
};

extern _CoreKEY CoreKEY;
extern void Key_Cmd_CB(uint8_t cmd);

#endif

