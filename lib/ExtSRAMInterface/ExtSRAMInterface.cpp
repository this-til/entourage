// 
// 
// 

#include "ExtSRAMInterface.h"
#include <Command.h>

_ExtSRAMInterface ExtSRAMInterface;

_ExtSRAMInterface::_ExtSRAMInterface()
{
}

_ExtSRAMInterface::~_ExtSRAMInterface()
{
}

/************************************************************************************************************
【函 数 名】：  Initialization		外部总线接口初始化函数
【参数说明】：	无
【返 回 值】：	无
【简    例】：	Initialization();	初始化外部总线接口
************************************************************************************************************/
void _ExtSRAMInterface::Initialization(void)
{
	XMCRA = 0x00;
	XMCRB = 0x00;
	XMCRA |= _BV(SRW10);
	XMCRA |= _BV(SRE);	/*这一位使能外部SRAM*/
}

/************************************************************************************************************
【函 数 名】：  ExMem_Write					向外部总线写一个字节数据函数
【参数说明】：	add	：						外部总线地址
				dat	：						向外部总线写的数据
【返 回 值】：	无
【简    例】：	ExMem_Write(0x6000,0x55);	向外部总线接口地址为0x6000写0x55数据
************************************************************************************************************/
void _ExtSRAMInterface::ExMem_Write(uint16_t add, uint8_t dat)
{
	rw_temp = add & 0x0ff;
	exm_p[ 0x3000 + rw_temp ] = rw_temp;
	exm_p[ add ] = dat;
}

void _ExtSRAMInterface::ExMem_JudgeWrite(uint16_t add, uint8_t dat)
{
	unsigned long t = millis();
	do
	{
		ExtSRAMInterface.ExMem_Write(add, dat);
	} while ((ExtSRAMInterface.ExMem_Read(add) != dat) && ((millis() - t) < 1000));
}
/************************************************************************************************************
【函 数 名】：  ExMem_Write_Btyes				连续向外部总线写数据函数
【参数说明】：	BaseAdd	：						向外部总线写数据的第一个地址
				Tbuf	：						向外部总线写数据的数组
				len		：						数组的长度
【返 回 值】：	无
【简    例】：	uint8_t Tbuf[8] = {0x00};
				ExMem_Write_Btyes(0x6000,Tbuf,8);	向外部总线写长度为8的数组数据
************************************************************************************************************/
void _ExtSRAMInterface::ExMem_Write_Bytes(uint8_t *Tbuf, uint8_t len)
{
	unsigned long t = millis();
	while ((ExMem_Read(WRITEADDRESS) != 0x00) && ((millis() - t) < 1000));
	for (int8_t i = len - 1; i >= 0; i--)
	{
		ExMem_JudgeWrite(WRITEADDRESS + i, Tbuf[ i ]);
	}
}

void _ExtSRAMInterface::ExMem_Write_Bytes(uint16_t BaseAdd, uint8_t *Tbuf, uint8_t len)
{
	unsigned long t = millis();
	while ((ExMem_Read(BaseAdd) != 0x00) && ((millis() - t) < 1000));
	for (int8_t i = len - 1; i >= 0; i--)
	{
		ExMem_JudgeWrite(BaseAdd + i, Tbuf[i]);
	}

	//for (size_t i = 0; i < len; i++)
	//{
	//	ExMem_JudgeWrite(BaseAdd + i, Tbuf[ i ]);
	//}
}

/************************************************************************************************************
【函 数 名】：  ExMem_Read					向外部总线读一个字节数据函数
【参数说明】：	add	：						外部总线读地址
【返 回 值】：	exm_p[ add ]				返回外部总线地址为add的数据
【简    例】：	tp = ExMem_Read(0x6000);	读取外部总线接口地址为0x6000的数据
************************************************************************************************************/
uint8_t _ExtSRAMInterface::ExMem_Read(uint16_t add)
{
	rw_temp = add & 0x0ff;
	//rw_temp = exm_p[ 0x3000 + rw_temp ];
	exm_p[ 0x3000 + rw_temp ] = rw_temp;
	return exm_p[ add ];
}

/************************************************************************************************************
【函 数 名】：  ExMem_Read_Bytes				连续向外部总线读取数据函数
【参数说明】：	BaseAdd	：						向外部总线读数据的第一个地址
				Tbuf	：						读取外部总线写数据存放的数组
				len		：						数组的长度
【返 回 值】：	无
【简    例】：	uint8_t Rbuf[8] = {0x00};
				ExMem_Read_Bytes(0x6000,Rbuf,8);	向外部总线起始地址为0x6000开始读取8个数据，存放于Rbuf中
************************************************************************************************************/
void _ExtSRAMInterface::ExMem_Read_Bytes(uint8_t *Rbuf, uint8_t len)
{
	unsigned long t = millis();
	while ((ExMem_Read(READADDRESS) == 0x00) && ((millis() - t) < 5000));
	//delay(10);
	for (size_t i = 0; i < len; i++)
	{
		Rbuf[i] = ExMem_Read(READADDRESS + i);
	}
	ExMem_JudgeWrite(READADDRESS, 0x00);
}

void _ExtSRAMInterface::ExMem_Read_Bytes(uint16_t BaseAdd, uint8_t *Rbuf, uint8_t len)
{
	unsigned long t = millis();
	while ((ExMem_Read(BaseAdd) == 0x00) && ((millis() - t) < 5000));
	for (size_t i = 0; i < len; i++)
	{
		Rbuf[ i ] = ExMem_Read(BaseAdd + i);
	}
	ExMem_JudgeWrite(BaseAdd, 0x00);
}


