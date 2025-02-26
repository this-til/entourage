#ifndef _DRIVE_H
#define _DRIVE_H

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif /* defined(ARDUINO) && ARDUINO >= 100 */

class _Drive
{
public:
	_Drive();
	~_Drive();
	
	uint8_t Light_plus1[4] = {0x00, 0xFF, 0x0C, ~(0x0C)};   // 智能路灯 光源挡位加1
	uint8_t Light_plus2[4] = {0x00, 0xFF, 0x18, ~(0x18)};   // 智能路灯 光源挡位加2
	uint8_t Light_plus3[4] = {0x00, 0xFF, 0x5E, ~(0x5E)};   // 智能路灯 光源挡位加3

	uint8_t Alarm_Open[6] = {0x03, 0x05, 0x14, 0x45, 0xDE, 0x92};  // 烽火台 开启
	uint8_t Alarm_Close[6] = {0x67, 0x34, 0x78, 0xA2, 0xFD, 0x27}; // 烽火台 关闭

	uint8_t Rotate_1[6] = {0xFF, 0x14, 0x01, 0x00, 0x00, 0x00};    // 立体显示 隧道有事故，请绕行
	uint8_t Rotate_2[6] = {0xFF, 0x14, 0x02, 0x00, 0x00, 0x00};    // 立体显示 前方施工，请绕行

	uint8_t Gate_Open[8] = {0x55, 0x03, 0x01, 0x01, 0x00, 0x00, 0x02, 0xBB};      // 道闸 开启
	uint8_t Gate_Close[8] = {0x55, 0x03, 0x01, 0x02, 0x00, 0x00, 0x03, 0xBB};     // 道闸 关闭
	uint8_t Gate_GetStatus[8] = {0x55, 0x03, 0x20, 0x02, 0x00, 0x00, 0x03, 0xBB}; // 道闸 状态查询

	uint8_t SEG_TimOpen[8] = {0x55, 0x04, 0x03, 0x01, 0x00, 0x00, 0x04, 0xBB};   // LED显示 计时开启
	uint8_t SMG_TimClose[8] = {0x55, 0x04, 0x03, 0x00, 0x00, 0x00, 0x03, 0xBB};  // LED显示 计时关闭
	uint8_t SMG_TimClear[8] = {0x55, 0x04, 0x03, 0x02, 0x00, 0x00, 0x05, 0xBB};  // LED显示 计时清零

	uint8_t TrafficA_Open[8] = {0x55, 0x0E, 0x01, 0x00, 0x00, 0x00, 0x01, 0xBB};   // 智能交通灯A 进入识别模式 进入识别模式
	uint8_t TrafficA_Red[8] = {0x55, 0x0E, 0x02, 0x01, 0x00, 0x00, 0x03, 0xBB};    // 智能交通灯A 识别结果为红色请求确认
	uint8_t TrafficA_Green[8] = {0x55, 0x0E, 0x02, 0x02, 0x00, 0x00, 0x04, 0xBB};  // 智能交通灯A 识别结果为绿色请求确认
	uint8_t TrafficA_Yellow[8] = {0x55, 0x0E, 0x02, 0x03, 0x00, 0x00, 0x05, 0xBB}; // 智能交通灯A 识别结果为黄色请求确认

	uint8_t TrafficB_Open[8] = {0x55, 0x0F, 0x01, 0x00, 0x00, 0x00, 0x01, 0xBB};   // 智能交通灯B 进入识别模式 进入识别模式
	uint8_t TrafficB_Red[8] = {0x55, 0x0F, 0x02, 0x01, 0x00, 0x00, 0x03, 0xBB};    // 智能交通灯B 识别结果为红色请求确认
	uint8_t TrafficB_Green[8] = {0x55, 0x0F, 0x02, 0x02, 0x00, 0x00, 0x04, 0xBB};  // 智能交通灯B 识别结果为绿色请求确认
	uint8_t TrafficB_Yellow[8] = {0x55, 0x0F, 0x02, 0x03, 0x00, 0x00, 0x05, 0xBB}; // 智能交通灯B 识别结果为黄色请求确认

	uint8_t GarageA_To1[8] = {0x55, 0x0D, 0x01, 0x01, 0x00, 0x00, 0x02, 0xBB};   // 立体车库A 到达第一层
	uint8_t GarageA_To2[8] = {0x55, 0x0D, 0x01, 0x02, 0x00, 0x00, 0x03, 0xBB};   // 立体车库A 到达第二层
	uint8_t GarageA_To3[8] = {0x55, 0x0D, 0x01, 0x03, 0x00, 0x00, 0x04, 0xBB};   // 立体车库A 到达第三层
	uint8_t GarageA_To4[8] = {0x55, 0x0D, 0x01, 0x04, 0x00, 0x00, 0x05, 0xBB};   // 立体车库A 到达第四层
	uint8_t GarageA_Get_Floor[8] = {0x55, 0x0D, 0x02, 0x01, 0x00, 0x00, 0x03, 0xBB}; // 立体车库A 请求返回车库位于第几层
	uint8_t GarageA_Get_Infr[8] = {0x55, 0x0D, 0x02, 0x02, 0x00, 0x00, 0x04, 0xBB};  // 立体车库A 请求返回前后侧红外状态

	uint8_t GarageB_To1[8] = {0x55, 0x05, 0x01, 0x01, 0x00, 0x00, 0x02, 0xBB};   // 立体车库B 到达第一层
	uint8_t GarageB_To2[8] = {0x55, 0x05, 0x01, 0x02, 0x00, 0x00, 0x03, 0xBB};   // 立体车库B 到达第二层
	uint8_t GarageB_To3[8] = {0x55, 0x05, 0x01, 0x03, 0x00, 0x00, 0x04, 0xBB};   // 立体车库B 到达第三层
	uint8_t GarageB_To4[8] = {0x55, 0x05, 0x01, 0x04, 0x00, 0x00, 0x05, 0xBB};   // 立体车库B 到达第四层
	uint8_t GarageB_Get_Floor[8] = {0x55, 0x05, 0x02, 0x01, 0x00, 0x00, 0x03, 0xBB}; // 立体车库B 请求返回车库位于第几层
	uint8_t GarageB_Get_Infr[8] = {0x55, 0x05, 0x02, 0x02, 0x00, 0x00, 0x04, 0xBB};  // 立体车库B 请求返回前后侧红外状态

	uint8_t TFT_PageUp[8] = {0x55, 0x0B, 0x10, 0x01, 0x00, 0x00, 0x11, 0xBB};    // TFT显示器 上翻页
	uint8_t TFT_PageDown[8] = {0x55, 0x0B, 0x10, 0x02, 0x00, 0x00, 0x12, 0xBB};  // TFT显示器 下翻页

	uint8_t Charge_Open[8] = {0x55, 0x0A, 0x01, 0x01, 0x00, 0x00, 0x02, 0xBB};   // 无线充电 开启


	void Gate_Show_Zigbee(char *Licence);	// 道闸系统标志物显示车牌
	void LED_Date_Zigbee(uint8_t One, uint8_t Two, uint8_t Three, uint8_t rank);	// LED显示标志物显示数据
	void LED_Dis_Zigbee(uint16_t dis);		// LED显示标志物显示测距信息
	void Rotate_show_Inf(char* src, char x, char y);	// 立体显示标志物显示车牌数据
	void Rotate_Dis_Inf(uint16_t dis);      // 立体显示标志物显示距离信息
	void YY_Play_Zigbee(char *p);			// 语音播报标志物播报指定文本信息
	void YY_Comm_Zigbee(uint8_t Primary, uint8_t Secondary); // 控制语音播报标志物播报语音控制命令
	void TFT_Test_Zigbee(char Device,uint8_t Pri,uint8_t Sec1,uint8_t Sec2,uint8_t Sec3);	// TFT显示标志物控制指令
	void TFT_Dis_Zigbee(char Device,uint16_t dis);		// 智能TFT显示器显示距离信息
	void TFT_Show_Zigbee(char Device,char *Licence);	// TFT显示器显示车牌
	void Garage_Test_Zigbee(char Device,uint8_t Pri,uint8_t Sec1);		// 立体车库标志物控制函数
	void Light_Inf(uint8_t gear);		// 自动调节光照强度函数
	void Test_Zigbee(uint8_t Head,uint8_t Pri,uint8_t Sec1,uint8_t Sec2,uint8_t Sec3);

private:
	
};


extern _Drive Drive;

#endif /* _DRIVE_H */

