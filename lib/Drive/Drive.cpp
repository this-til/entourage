#include "Drive.h"
#include <ExtSRAMInterface.h>
#include <BH1750.h>
#include <Infrare.h>

_Drive Drive;

_Drive::_Drive()
{
	
}

_Drive::~_Drive()
{
	
}

uint8_t Zigbee[8];             // Zigbee发送数据缓存
uint8_t Infrared[6];           // 红外发送数据缓存
uint8_t YY_Init[5] = {0xFD, 0x00, 0x00, 0x01, 0x01};


/**********************************************************************
 * 函 数 名 ：  道闸系统标志物显示车牌
 * 参    数 ：  *Licence -> 车牌数据（ASICC）
 * 返 回 值 ：  无
 * 简    例 ：  Gate_Show_Zigbee("A123B4");
**********************************************************************/
void _Drive::_Drive::Gate_Show_Zigbee(char *Licence)
{
    Zigbee[0] = 0x55;
    Zigbee[1] = 0x03;
    Zigbee[2] = 0x10;
    Zigbee[3] = *(Licence + 0);     // 车牌数据【1】
    Zigbee[4] = *(Licence + 1);     // 车牌数据【2】
    Zigbee[5] = *(Licence + 2);     // 车牌数据【3】
    Zigbee[6] = (Zigbee[2] + Zigbee[3] + Zigbee[4] + Zigbee[5]) % 256;
    Zigbee[7] = 0xBB;
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, Zigbee, 8);     // 发送Zigbee数据
    delay(300);	
    Zigbee[2] = 0x11;
    Zigbee[3] = *(Licence + 3);     // 车牌数据【4】
    Zigbee[4] = *(Licence + 4);     // 车牌数据【5】
    Zigbee[5] = *(Licence + 5);     // 车牌数据【6】
    Zigbee[6] = (Zigbee[2] + Zigbee[3] + Zigbee[4] + Zigbee[5]) % 256;
    Zigbee[7] = 0xBB;
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, Zigbee, 8);     // 发送Zigbee数据
    delay(100);
}

/**********************************************************************
   函 数 名 ：  LED显示标志物显示测距信息
   参    数 ：  dis 距离值
   返 回 值 ：  无
   简    例 ：  LED_Dis_Zigbee(123);
**********************************************************************/
void _Drive::LED_Dis_Zigbee(uint16_t dis)
{
  Zigbee[0] = 0x55;
  Zigbee[1] = 0x04;
  Zigbee[2] = 0x04;
  Zigbee[3] = 0x00;
  Zigbee[4] = dis / 100 % 10;
  Zigbee[5] = ((dis / 10 % 10) * 16 + dis % 10);
  Zigbee[6] = (Zigbee[2] + Zigbee[3] + Zigbee[4] + Zigbee[5]) % 256;
  Zigbee[7] = 0xBB;
  ExtSRAMInterface.ExMem_Write_Bytes(0x6008, Zigbee, 8);  //发送Zigbee数据
  delay(100);
}

/**********************************************************************
 * 函 数 名 ：  LED显示标志物显示数据
 * 参    数 ：  One,Two,Three   数据（十六进制显示格式）
                rank    1 -> 第一行数码管显示数据
                        2 -> 第二行数码管显示数据（默认）
 * 返 回 值 ：  无
 * 简    例 ：  LED_Date_Zigbee(0x12,0x34,0x56,0x01);
**********************************************************************/
void _Drive::LED_Date_Zigbee(uint8_t One, uint8_t Two, uint8_t Three, uint8_t rank)
{
    Zigbee[0] = 0x55;
    Zigbee[1] = 0x04;
    if(rank == 1)  Zigbee[2] = 0x01;
    else  Zigbee[2] = 0x02;
    Zigbee[3] = One;
    Zigbee[4] = Two;
    Zigbee[5] = Three;
    Zigbee[6] = (Zigbee[2] + Zigbee[3] + Zigbee[4] + Zigbee[5]) % 256;
    Zigbee[7] = 0xBB;
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, Zigbee, 8);
    delay(100);
}

/**********************************************************************
 * 函 数 名 ：  立体显示标志物显示车牌数据
 * 参    数 ：  *src    车牌数据（ASICC）
                x,y     坐标信息
 * 返 回 值 ：  无
 * 简    例 ：  Rotate_show_Inf("A123B4",'C','5');
**********************************************************************/
void _Drive::Rotate_show_Inf(char* src, char x, char y)
{
    Infrared[0] = 0xFF;			// 起始位
    Infrared[1] = 0x20;			// 模式
    Infrared[2] = *(src + 0);	// 数据【1】
    Infrared[3] = *(src + 1);	// 数据【2】
    Infrared[4] = *(src + 2);	// 数据【3】
    Infrared[5] = *(src + 3);	// 数据【4】
    Infrare.Transmition(Infrared, 6);
    delay(500);
    Infrared[1] = 0x10;			// 模式
    Infrared[2] = *(src + 4);	// 数据【1】
    Infrared[3] = *(src + 5);	// 数据【2】
    Infrared[4] = x;			// 数据【3】
    Infrared[5] = y;			// 数据【4】
    Infrare.Transmition(Infrared, 6);
    delay(10);
}

/**********************************************************************
 * 函 数 名 ：  立体显示标志物显示距离信息（单位：ms）
 * 参    数 ：  dis  测距信息（四舍五入）
 * 返 回 值 ：  无
 * 简    例 ：  Rotate_Dis_Inf(123);
**********************************************************************/
void _Drive::Rotate_Dis_Inf(uint16_t dis)
{
    uint16_t csb = dis; //缓存超声波数据值

    csb += 5;   //四舍五入
    Infrared[0] = 0xFF;
    Infrared[1] = 0x11; //显示距离模式
    Infrared[2] = 0x30 + (uint8_t)(csb / 100 % 10); //距离十位--cm
    Infrared[3] = 0x30 + (uint8_t)(csb / 10 % 10); //距离个位--cm
    Infrared[4] = 0x00;
    Infrared[5] = 0x00;
    Infrare.Transmition(Infrared, 6);
    delay(100);
}

/**********************************************************************
 * 函 数 名 ：  控制语音播报标志物播报指定文本信息
 * 参    数 ：  *p  --> 需要发送的数据
 * 返 回 值 ：  无
 * 简    例 ：  YY_Play_Zigbee("北京欢迎您");
**********************************************************************/
void _Drive::YY_Play_Zigbee(char *p)
{
    uint16_t p_len = strlen(p);             // 文本长度

    YY_Init[1] = 0xff & ((p_len + 2) >> 8); // 数据区长度高八位
    YY_Init[2] = 0xff & (p_len + 2);        // 数据区长度低八位
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, YY_Init, 5);
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, (uint8_t *)p, p_len);
    delay(100);
}

/**********************************************************************
 * 函 数 名 ：  控制语音播报标志物播报语音控制命令
 * 参    数 ：  Primary   -> 主指令
                Secondary -> 副职令
                详见附录1
 * 返 回 值 ：  无
 * 简    例 ：  YY_Comm_Zigbee(0x20, 0x01);     // 语音播报随机语音命令

附录1：
-----------------------------------------------------------------------
| Primary | Secondary | 说明
|---------|-----------|------------------------------------------------
| 0x10    | 0x02      | 向右转弯
|         | 0x03      | 禁止右转
|         | 0x04      | 左侧行驶
|         | 0x05      | 左行被禁
|         | 0x06      | 原地掉头
|---------|-----------|------------------------------------------------
| 0x20    | 0x01      | 随机指令
|---------|-----------|------------------------------------------------
***********************************************************************/
void _Drive::YY_Comm_Zigbee(uint8_t Primary, uint8_t Secondary)
{
    Zigbee[0] = 0x55;
    Zigbee[1] = 0x06;
    Zigbee[2] = Primary;
    Zigbee[3] = Secondary;
    Zigbee[4] = 0x00;
    Zigbee[5] = 0x00;
    Zigbee[6] = (Zigbee[2] + Zigbee[3] + Zigbee[4] + Zigbee[5]) % 256;
    Zigbee[7] = 0xBB;
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, Zigbee, 8);
}

/********************************************************************************
 * 函 数 名 ：  TFT显示标志物控制指令
 * 参    数 ：  Device -> 选择设备
				Pri	   -> 主指令
                Sec1   -> 副职令【1】
				Sec2   -> 副职令【2】
				Sec3   -> 副职令【3】
                详见附录1
 * 返 回 值 ：  无
 * 简    例 ：  TFT_Test_Zigbee('A',0x40,0xA1,0xB2,0xC3);    // TFT显示器显示图形信息

附录1：
--------------------------------------------------------------------------------
|主指令	| 副指令[1]	| 副指令[2]	| 副指令[3]	|			说明
|-------|-----------|-----------|-----------|-----------------------------------
| 0x10	|	0x00	| 0x01~0x20	|	0x00	| 由第二副指令指定显示那张图片
|		|	0x01    |	0x00	|	0x00	| 图片向上翻页
|		|	0x02    |	0x00	|	0x00	| 图片向下翻页
|		|	0x03    |	0x00	|	0x00	| 图片自动向下翻页显示，间隔时间 10S
|-------|-----------|-----------|-----------|-----------------------------------
| 0x20	|	0xXX	|	0xXX	|	0xXX	| 车牌前三位数据（ASCII）
|-------|-----------|-----------|-----------|-----------------------------------
| 0x21	|	0xXX	|	0xXX	|	0xXX	| 车牌后三位数据（ASCII）
|-------|-----------|-----------|-----------|-----------------------------------
| 0x30	|	0x00	|	0x00	|	0x00	| 计时模式关闭
|		|	0x01	|	0x00	|	0x00	| 计时模式打开
|		|	0x02	|	0x00	|	0x00	| 计时模式清零
|-------|-----------|-----------|-----------|-----------------------------------
| 0x40	|	0xXX	|	0xXX	|	0xXX	| 六位显示数据（HEX格式）
|-------|-----------|-----------|-----------|-----------------------------------
| 0x50	|	0x00	|	0x0X	|	0xXX	| 距离显示模式（十进制）
--------------------------------------------------------------------------------
********************************************************************************/
void _Drive::TFT_Test_Zigbee(char Device,uint8_t Pri,uint8_t Sec1,uint8_t Sec2,uint8_t Sec3)
{
	Zigbee[0] = 0x55;
	if (Device == 'B') { Zigbee[1] = 0x08; }
	else { Zigbee[1] = 0x0B; }
	Zigbee[2] = Pri;
	Zigbee[3] = Sec1;
	Zigbee[4] = Sec2;
	Zigbee[5] = Sec3;
	Zigbee[6] = (Zigbee[2]+Zigbee[3]+Zigbee[4]+Zigbee[5])%256;
	Zigbee[7] = 0xBB;
	ExtSRAMInterface.ExMem_Write_Bytes(0x6008, Zigbee, 8);
	delay(100);
}

/**********************************************************************
 * 函 数 名 ：  智能TFT显示器显示距离信息
 * 参    数 ：  dis	距离值
 * 返 回 值 ：  无
 * 简    例 ：  TFT_Dis_Zigbee('A',123);
**********************************************************************/
void _Drive::TFT_Dis_Zigbee(char Device,uint16_t dis)
{
	Zigbee[0] = 0x55;
	if (Device == 'B') { Zigbee[1] = 0x08; }
	else { Zigbee[1] = 0x0B; }
	Zigbee[2] = 0x50;
	Zigbee[3] = 0x00;
	Zigbee[4] = dis/100%10;
	Zigbee[5] = ((dis/10%10)*16+dis%10);
	Zigbee[6] = (Zigbee[2]+Zigbee[3]+Zigbee[4]+Zigbee[5])%256;
	Zigbee[7] = 0xBB;
	ExtSRAMInterface.ExMem_Write_Bytes(0x6008, Zigbee, 8);  //发送Zigbee数据
	delay(100);
}


/**********************************************************************
 * 函 数 名 ：  TFT显示器显示车牌
 * 参    数 ：  *Licence -> 车牌数据（ASICC）
 * 返 回 值 ：  无
 * 简    例 ：  TFT_Show_Zigbee('A',"A123B4");
**********************************************************************/
void _Drive::TFT_Show_Zigbee(char Device,char *Licence)
{
    Zigbee[0] = 0x55;
    if (Device == 'B') { Zigbee[1] = 0x08; }
	else { Zigbee[1] = 0x0B; }
    Zigbee[2] = 0x20;
    Zigbee[3] = *(Licence + 0);     // 车牌数据【1】
    Zigbee[4] = *(Licence + 1);     // 车牌数据【2】
    Zigbee[5] = *(Licence + 2);     // 车牌数据【3】
    Zigbee[6] = (Zigbee[2] + Zigbee[3] + Zigbee[4] + Zigbee[5]) % 256;
    Zigbee[7] = 0xBB;
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, Zigbee, 8);     // 发送Zigbee数据
    delay(500);
    Zigbee[2] = 0x21;
    Zigbee[3] = *(Licence + 3);     // 车牌数据【4】
    Zigbee[4] = *(Licence + 4);     // 车牌数据【5】
    Zigbee[5] = *(Licence + 5);     // 车牌数据【6】
    Zigbee[6] = (Zigbee[2] + Zigbee[3] + Zigbee[4] + Zigbee[5]) % 256;
    Zigbee[7] = 0xBB;
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, Zigbee, 8);     // 发送Zigbee数据
    delay(100);
}

/********************************************************************************
 * 函 数 名 ：  立体车库标志物控制函数
 * 参    数 ：  Device -> 选择设备
				Pri	   -> 主指令
                Sec1   -> 副职令【1】
                详见附录1
 * 返 回 值 ：  无
 * 简    例 ：  Garage_Test_Zigbee('A',0x01,0x01);    // 立体车库A 到达第一层

附录1：
--------------------------------------------------------------------------------
|主指令	| 副指令[1]	| 副指令[2]	| 副指令[3]	|			说明
|-------|-----------|-----------|-----------|-----------------------------------
| 0x01  |   0x01    |   0x00    |   0x00    | 到达第一层
|       |   0x02    |   0x00    |   0x00    | 到达第二层
|       |   0x03    |   0x00    |   0x00    | 到达第三层
|       |   0x04    |   0x00    |   0x00    | 到达第四层
|-------|-----------|-----------|-----------|-----------------------------------
| 0x02  |   0x01    |   0x00    |   0x00    | 请求返回车库位于第几层
|       |   0x02    |   0x00    |   0x00    | 请求返回前后侧红外状态
|-------|-----------|-----------|-----------|-----------------------------------
| 0x03  |   0x01    |   0x01    |   0x00    | 返回车库位于第一层
|       |           |   0x02    |   0x00    | 返回车库位于第二层
|       |           |   0x03    |   0x00    | 返回车库位于第三层
|       |           |   0x04    |   0x00    | 返回车库位于第四层
|       |-----------|-----------|-----------|-----------------------------------
|       |   0x02    |0x01 触发  |0x01 触发  | 返回车库前后侧红外对管状态
|       |           |0x02 未触发|0x02 未触发| 
|       |           | - 前侧 -  | - 后侧 -  | 
--------------------------------------------------------------------------------
********************************************************************************/
void _Drive::Garage_Test_Zigbee(char Device,uint8_t Pri,uint8_t Sec1)
{
	Zigbee[0] = 0x55;
	if (Device == 'B') { Zigbee[1] = 0x08; }
	else { Zigbee[1] = 0x0D; }
	Zigbee[2] = Pri;
	Zigbee[3] = Sec1;
	Zigbee[4] = 0x00;
	Zigbee[5] = 0x00;
	Zigbee[6] = (Zigbee[2]+Zigbee[3]+Zigbee[4]+Zigbee[5])%256;
	Zigbee[7] = 0xBB;
	ExtSRAMInterface.ExMem_Write_Bytes(0x6008, Zigbee, 8);
	delay(100);
}

void _Drive::Test_Zigbee(uint8_t Head,uint8_t Pri,uint8_t Sec1,uint8_t Sec2,uint8_t Sec3)
{
	Zigbee[0] = 0x55;
	Zigbee[1] = Head;
	Zigbee[2] = Pri;
	Zigbee[3] = Sec1;
	Zigbee[4] = Sec2;
	Zigbee[5] = Sec3;
	Zigbee[6] = (Zigbee[2]+Zigbee[3]+Zigbee[4]+Zigbee[5])%256;
	Zigbee[7] = 0xBB;
	ExtSRAMInterface.ExMem_Write_Bytes(0x6008, Zigbee, 8);
	delay(100);
}

/********************************************************
 * 函 数 名 ：	自动调节光照强度函数
 * 参    数 ：	gear	目标挡位值
 * 返 回 值 ：	无
********************************************************/
void _Drive::Light_Inf(uint8_t gear)		// 自动调节光照强度函数
{
	uint8_t i;
	uint16_t array[2];	//缓存自学习的光档位数组
	
	if((gear > 0) && (gear < 5))
	{
		delay(100);
		array[0] = BH1750.ReadLightLevel();		// 获取光照强度值
		for(i=0; i<4; i++)
		{
			Infrare.Transmition(Light_plus1,4);
			delay(1500);
			array[1] = BH1750.ReadLightLevel();		// 获取光照强度值
			if (array[0] < array[1]) 
			{
				array[0] = array[1];
				array[1] = 0;
			}
			else
			{
				break;
			}
		}
		if(gear==2)
		{
			Infrare.Transmition(Light_plus1,4);	//光源档位加1
		}else if(gear==3)
		{
			Infrare.Transmition(Light_plus2,4);	//光源档位加2
		}else if(gear==4)
		{
			Infrare.Transmition(Light_plus3,4);	//光源档位加3
		}
	}
}

