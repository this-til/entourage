// 
// 
// 

#include "CoreKEY.h"
#include <Metro.h>

Metro KeyMetro(10);
_CoreKEY CoreKEY;

static uint32_t key_times;
static uint8_t Key_cnt[4];
static uint8_t Key_Rt;

_CoreKEY::_CoreKEY()
{
}

_CoreKEY::~_CoreKEY()
{
}

/************************************************************************************************************
【函 数 名】：  Initialization		小车核心板按键初始化函数
【参数说明】：	无
【返 回 值】：	无
【简    例】：	Initialization();	小车核心板按键初始化
************************************************************************************************************/
void _CoreKEY::Initialization(void)
{
	uint8_t i;
	/*核心板按键端口 K0/1/2/3*/
	PORTK &= 0xf0;
	DDRK  &= 0xf0;

	key_times = 0;
	for (i = 0; i<4; i++)
		Key_cnt[i] = 0;
	Key_Rt = 0;
}

/************************************************************************************************************
【函 数 名】：  Check		小车核心板按键检测函数
【参数说明】：	无
【返 回 值】：	Rt			返回相应的键值
【简    例】：	Check();	小车核心板按键检测
************************************************************************************************************/
uint8_t _CoreKEY::Check(void)
{
	uint8_t i;
	uint8_t Rt = 0;

	for (i = 0; i<4; i++)
	{
		if (Key_cnt[i] > 8)
		{
			Rt = i + 1;
			break;
		}
	}
	if (Rt == 0)
		Key_Rt = 0;
	else if (Key_Rt == 0)
		Key_Rt = Rt;
	else
		Rt = 0;
	return Rt;
}

void _CoreKEY::Run(void)
{
	uint8_t fk;
	fk = Check();

	if (fk != 0)
	{
		Key_Cmd_CB(fk);
	}
}
void _CoreKEY::Run(void(*Key_Cmd_CB_)(uint8_t fk))
{
	uint8_t fk;
	fk = Check();

	if (fk != 0)
	{
		(*Key_Cmd_CB_)(fk);
	}
}


void _CoreKEY::Kwhile(void)
{
	if (KeyMetro.check() == 1)
	{
		uint8_t i, rt;
		rt = PINK & 0x0f;
		for (i = 0; i<4; i++)
		{
			if (rt & (0x01 << i))
			{
				if (Key_cnt[i]>8)
					Key_cnt[i]--;
				else
					Key_cnt[i] = 0;
			}
			else
			{
				if (Key_cnt[i] < 12)
					Key_cnt[i]++;
			}
		}
		Run();
	}
}

/************************************************************************************************************
【函 数 名】：  Kwhile					小车核心板按键执行函数
【参数说明】：	function(uint8_t fk)	入口参数是函数，按键响应函数
【返 回 值】：	无
【简    例】：	Kwhile();				小车核心板按键执行
************************************************************************************************************/
void _CoreKEY::Kwhile(void(*function)(uint8_t fk))
{
	if (KeyMetro.check() == 1)
	{
		uint8_t i, rt;
		rt = PINK & 0x0f;
		for (i = 0; i<4; i++)
		{
			if (rt & (0x01 << i))
			{
				if (Key_cnt[i]>8)
					Key_cnt[i]--;
				else
					Key_cnt[i] = 0;
			}
			else
			{
				if (Key_cnt[i] < 12)
					Key_cnt[i]++;
			}
		}
		Run((*function));
	}
}


//// 
//// 
//// 
//
//#include "CoreKEY.h"
//#include <Metro.h>
//
//Metro KeyMetro(10);
//_CoreKEY CoreKEY;
//
//_CoreKEY::_CoreKEY()
//{
//}
//
//_CoreKEY::~_CoreKEY()
//{
//}
//
///************************************************************************************************************
//【函 数 名】：  Initialization		小车核心板按键初始化函数
//【参数说明】：	无
//【返 回 值】：	无
//【简    例】：	Initialization();	小车核心板按键初始化
//************************************************************************************************************/
//void _CoreKEY::Initialization(void)
//{
//	/*核心板按键端口 K0/1/2/3*/
//	PORTK &= 0xf0;
//	DDRK  &= 0xf0;
//}
//
///************************************************************************************************************
//【函 数 名】：  Scan		小车核心板按键扫描函数
//【参数说明】：	mode		true:表示支持连按，false;表示不支持连按
//【返 回 值】：	无
//【简    例】：	Scan();		小车核心板按键初始化
//************************************************************************************************************/
//uint8_t _CoreKEY::Scan(boolean mode)
//{
//	uint8_t key = 0;
//	if ((PINK & 0x0f) != 0x0f)
//	{
//		//delay(1);
//		//if ((PINK & 0x0f) != 0x0f)
//		//{
//			key = PINK & 0x0f;
//			switch (key)
//			{
//			case 0x0e:
//				key = 1;
//				break;
//			case 0x0d:
//				key = 2;
//				break;
//			case 0x0b:
//				key = 3;
//				break;
//			case 0x07:
//				key = 4;
//				break;
//			default:
//				key = 0;
//				break;
//			}
//			while ((!mode) && ((PINK & 0x0f) != 0x0f));
//		//}
//	}
//	return key;
//}
//
