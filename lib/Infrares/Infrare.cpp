// 
// 
// 

#include "infrare.h"

_Infrare Infrare;

_Infrare::_Infrare(uint8_t _pin)
{
	pinMode(_pin, OUTPUT);
	pin = _pin;
	digitalWrite(_pin, TXD_OFF);
}
_Infrare::_Infrare()
{

}

_Infrare::~_Infrare()
{
}

/***************************************************************
** 功能：	红外发射端口初始化
** 参数：	无参数
** 返回值：	无
****************************************************************/
void _Infrare::Initialization(void)
{
	pinMode(RITX_PIN, OUTPUT);
	digitalWrite(RITX_PIN, TXD_OFF);
	//pin = _pin;

}

/***************************************************************
** 功能：	红外发射子程序
** 参数：	*s：指向要发送的数据
**          *n：数据长度
** 返回值：	无
****************************************************************/
void _Infrare::Transmition(uint8_t *s, int n)
{
	u8 i, j, temp;

	digitalWrite(RITX_PIN, TXD_ON);
	delayMicroseconds(9000);
	digitalWrite(RITX_PIN, TXD_OFF);
	delayMicroseconds(4560);
	

	for (i = 0; i < n; i++)
	{
		for (j = 0; j<8; j++)
		{
			temp = (s[ i ] >> j) & 0x01;
			if (temp == 0)						//发射0
			{
				digitalWrite(RITX_PIN, TXD_ON);
				delayMicroseconds(500);			//延时0.5ms
				digitalWrite(RITX_PIN,TXD_OFF);
				delayMicroseconds(500);			//延时0.5ms
			}
			if (temp == 1)						//发射1
			{
				digitalWrite(RITX_PIN, TXD_ON);
				delayMicroseconds(500);			//延时0.5ms
				digitalWrite(RITX_PIN, TXD_OFF);
				delayMicroseconds(1000);
				delayMicroseconds(800);			//延时1.69ms  690

			}
		}
	}
	digitalWrite(RITX_PIN, TXD_ON);				//结束
	delayMicroseconds(560);						//延时0.56ms
	digitalWrite(RITX_PIN, TXD_OFF);			//关闭红外发射
}



