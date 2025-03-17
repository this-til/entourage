#include "BKRC_Voice.h"
#include "ExtSRAMInterface.h"


_BKRC_Voice BKRC_Voice;


_BKRC_Voice::_BKRC_Voice()
{
}

_BKRC_Voice::~_BKRC_Voice()
{
}

//初始化
/************************************************************************************************************
【函 数 名】：	Initialization		初始化函数
【参数说明】：	无
【返 回 值】：	无
【简    例】：	Initialization();	初始化相关接口及变量
************************************************************************************************************/
void _BKRC_Voice::Initialization(void)
{
	Serial2.begin(115200);
  while(Serial2.read() >=0);
Serial.begin(115200);
while(Serial.read() >=0);
}

/**********************************************************************
   函 数 名 ：  控制语音播报标志物播报语音控制命令
   参    数 ：  Primary   -> 主指令
                Secondary -> 副职令
                详见附录1
   返 回 值 ：  无
   简    例 ：  YY_Comm_Zigbee(0x20, 0x01);     // 语音播报随机语音命令

  附录1：
  -----------------------------------------------------------------------
  | Primary | Secondary | 说明
  |---------|-----------|------------------------------------------------
  |  0x10   |  0x02     | 美好生活
  |         |  0x03     | 秀丽山河
  |         |  0x04     | 追逐梦想
  |         |  0x05     | 扬帆启航
  |         |  0x06     | 齐头并进
  |---------|-----------|------------------------------------------------
  |  0x20   |  0x01     | 随机指令
  |---------|-----------|------------------------------------------------
***********************************************************************/
void _BKRC_Voice::YY_Comm_Zigbee(uint8_t Primary, uint8_t Secondary)
{
  uint8_t Zigbee[8] = {0};
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


/**************************************************
  功  能：语音识别函数
  参  数：  无
  返回值：  语音词条ID    词条内容

    0x01      美好生活

    0x02      秀丽山河

    0x03      追逐梦想

    0x04      扬帆启航

    0x05      齐头并进

    0x00      未识别到词条/识别超时
**************************************************/
uint8_t _BKRC_Voice::BKRC_Voice_Extern(uint8_t yy_mode)   // 语音识别
{
  uint8_t SYN7318_Flag = 0;           // SYN7318语音识别命令ID编号
  uint16_t timers = 0;               // 计数值2
  Serial2.write(start_voice_dis, 5); //发送开启语音识别指令
  delay(500);
  SYN7318_Flag = Voice_Drive();
  while (Serial.read() >= 0); //清空串口数据
  delay(1000);
  if(yy_mode==0)
  {
  YY_Comm_Zigbee(0x20, 0x01);     //语音播报随机语音命令

  }else
  {
YY_Comm_Zigbee(0x10, yy_mode);     //语音播报随机语音命令
  }
  

  SYN7318_Flag = 0;
  while (1)
  {
    delay(1);
    timers++;
    SYN7318_Flag = Voice_Drive();
    if (SYN7318_Flag != 0x00 || timers > 6000) //判断超时退出
    {
      timers = 0;
      return SYN7318_Flag;
    }
  }
  return 0;
}


/**************************************************
  功  能：语音识别回传命令解析函数
  参  数： 无
  返回值：  语音词条ID /小创语音识别模块状态
**************************************************/
uint8_t _BKRC_Voice::Voice_Drive(void)
{
  uint8_t  status = 0;
  if (Serial2.available() > 0) {
    numdata = Serial2.readBytes(buffer, 4);
    if (buffer[0] ==0x55 && buffer[1] == 0x02 && buffer[3] == 0x00)
    {
      status = buffer[2];
    } else
    {
      status = 0;
    }
  }
  return status;
}


