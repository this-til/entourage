// ExtSRAMInterface.h

#ifndef _EXTSRAMINTERFACE_h
#define _EXTSRAMINTERFACE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif


class _ExtSRAMInterface
{
public:
	_ExtSRAMInterface();
	~_ExtSRAMInterface();


	void Initialization(void);
	void ExMem_Write(uint16_t add, uint8_t dat);
	void ExMem_JudgeWrite(uint16_t add, uint8_t dat);
	void ExMem_Write_Bytes(uint8_t *Tbuf, uint8_t len);
	void ExMem_Write_Bytes(uint16_t BaseAdd, uint8_t *Tbuf, uint8_t len);


	uint8_t ExMem_Read(uint16_t add);
	void ExMem_Read_Bytes(uint8_t *Rbuf, uint8_t len);
	void ExMem_Read_Bytes(uint16_t BaseAdd, uint8_t *Rbuf, uint8_t len);
	

private:
	volatile uint8_t rw_temp;
	uint8_t *exm_p = (uint8_t *)((uint16_t)0);

	//void ExMem_Write_Bytes(uint16_t BaseAdd, uint8_t *Tbuf, uint8_t len);
	//void ExMem_Read_Bytes(uint16_t BaseAdd, uint8_t *Rbuf, uint8_t len);

};


extern _ExtSRAMInterface ExtSRAMInterface;

#endif

