
#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "Ultrasonic.h"

_Ultrasonic Ultrasonic;

_Ultrasonic::_Ultrasonic()
{
	
}

/************************************************************************************************************
【函 数 名】：	Initialization		初始化函数
【参数说明】：	TO					设置最大的测量时间，默认为3000us
【返 回 值】：	无
【简    例】：	Initialization();	初始化超声波模块引脚
************************************************************************************************************/
void _Ultrasonic::Initialization(void)
{
	pinMode(TRIG_PIN, OUTPUT);
	pinMode(ECHO_PIN, INPUT);
	//Trig_pin = TP;
	//Echo_pin = EP;
	Time_out = 3000;  // 3000 µs = 50cm // 30000 µs = 5 m 
}
void _Ultrasonic::Initialization(uint32_t TO)
{
	pinMode(TRIG_PIN, OUTPUT);
	pinMode(ECHO_PIN, INPUT);
	//Trig_pin = TP;
	//Echo_pin = EP;
	Time_out = TO;
}

/************************************************************************************************************
【函 数 名】：	Timing		测量函数
【参数说明】：	无
【返 回 值】：	duration
【简    例】：	Timing();	返回测量的时间，超声波一个来回的时间
************************************************************************************************************/
long _Ultrasonic::Timing()
{
	digitalWrite(TRIG_PIN, LOW);
	delayMicroseconds(2);
	digitalWrite(TRIG_PIN, HIGH);
	delayMicroseconds(10);
	digitalWrite(TRIG_PIN, LOW);
	duration = pulseIn(ECHO_PIN,HIGH,Time_out);
	if ( duration == 0 )
	{
		duration = Time_out; 
	}
	return duration;
}

/************************************************************************************************************
【函 数 名】：	Ranging							数据处理函数
【参数说明】：	sys								设置测量距离的单位，1表示厘米，0表示英寸
【返 回 值】：	(double(distance_cm) / 10.0)	返回测量的
【简    例】：	Ranging();						返回测量距离
************************************************************************************************************/
double _Ultrasonic::Ranging(int sys)
{
	Timing();
	
	if (sys) 
	{
		distance_cm = duration * 10 / 29 / 2;
		return (double(distance_cm) / 10.0);
	}
	else 
	{
		distance_inc = duration * 10 / 74 / 2;
		return (double(distance_inc) / 10.0);
	}
}
