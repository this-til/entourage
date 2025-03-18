// 
// 
// 

#include "BEEP.h"

_BEEP	BEEP;

_BEEP::_BEEP()
{
}

_BEEP::~_BEEP()
{
}

/************************************************************************************************************
【函 数 名】：	Initialization		初始化函数
【参数说明】：	无
【返 回 值】：	无
【简    例】：	Initialization();	初始化任务板蜂鸣器引脚
************************************************************************************************************/
void _BEEP::Initialization(void)
{
	//pin = _pin;
	pinMode(BEEP_PIN, OUTPUT);
	TurnOff();
}

/************************************************************************************************************
【函 数 名】：	TurnOn			打开任务板蜂鸣器函数
【参数说明】：	无
【返 回 值】：	无
【简    例】：	TurnOn();		打开任务板蜂鸣器
************************************************************************************************************/
void _BEEP::TurnOn(void)
{
	digitalWrite(BEEP_PIN, BEEP_ON);
}

/************************************************************************************************************
【函 数 名】：	TurnOff			关闭任务板蜂鸣器函数
【参数说明】：	无
【返 回 值】：	无
【简    例】：	TurnOff();		关闭任务板蜂鸣器
************************************************************************************************************/
void _BEEP::TurnOff(void)
{
	digitalWrite(BEEP_PIN, BEEP_OFF);
}

