// BKRC_Voice.h

#ifndef _BKRC_VOICE_h
#define _BKRC_VOICE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif



class _BKRC_Voice
{
public:
	_BKRC_Voice();
	~_BKRC_Voice();

	void Initialization(void);							/*С������ģ���ʼ��*/
	uint8_t BKRC_Voice_Extern(uint8_t yy_mode);
	void YY_Comm_Zigbee(uint8_t Primary, uint8_t Secondary);			

	char buffer[18]={0};
	int numdata = 0;
private:

	uint8_t start_voice_dis[5] = {0xFA, 0xFA, 0xFA, 0xFA, 0xA1};
	uint8_t Voice_Drive(void);
};

extern _BKRC_Voice BKRC_Voice;



#endif

