#include "BKRC_Voice.h"
#include "ExtSRAMInterface.h"


_BKRC_Voice BKRC_Voice;


_BKRC_Voice::_BKRC_Voice()
{
}

_BKRC_Voice::~_BKRC_Voice()
{
}

//��ʼ��
/************************************************************************************************************
���� �� ������	Initialization		��ʼ������
������˵������	��
���� �� ֵ����	��
����    ������	Initialization();	��ʼ����ؽӿڼ�����
************************************************************************************************************/
void _BKRC_Voice::Initialization(void)
{
	Serial2.begin(115200);
  while(Serial2.read() >=0);
Serial.begin(115200);
while(Serial.read() >=0);
}

/**********************************************************************
   �� �� �� ��  ��������������־�ﲥ��������������
   ��    �� ��  Primary   -> ��ָ��
                Secondary -> ��ְ��
                �����¼1
   �� �� ֵ ��  ��
   ��    �� ��  YY_Comm_Zigbee(0x20, 0x01);     // �������������������

  ��¼1��
  -----------------------------------------------------------------------
  | Primary | Secondary | ˵��
  |---------|-----------|------------------------------------------------
  |  0x10   |  0x02     | ��������
  |         |  0x03     | ����ɽ��
  |         |  0x04     | ׷������
  |         |  0x05     | �﷫����
  |         |  0x06     | ��ͷ����
  |---------|-----------|------------------------------------------------
  |  0x20   |  0x01     | ���ָ��
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
  ��  �ܣ�����ʶ����
  ��  ����  ��
  ����ֵ��  ��������ID    ��������

    0x01      ��������

    0x02      ����ɽ��

    0x03      ׷������

    0x04      �﷫����

    0x05      ��ͷ����

    0x00      δʶ�𵽴���/ʶ��ʱ
**************************************************/
uint8_t _BKRC_Voice::BKRC_Voice_Extern(uint8_t yy_mode)   // ����ʶ��
{
  uint8_t SYN7318_Flag = 0;           // SYN7318����ʶ������ID���
  uint16_t timers = 0;               // ����ֵ2
  Serial2.write(start_voice_dis, 5); //���Ϳ�������ʶ��ָ��
  delay(500);
  SYN7318_Flag = Voice_Drive();
  while (Serial.read() >= 0); //��մ�������
  delay(1000);
  if(yy_mode==0)
  {
  YY_Comm_Zigbee(0x20, 0x01);     //�������������������

  }else
  {
YY_Comm_Zigbee(0x10, yy_mode);     //�������������������
  }
  

  SYN7318_Flag = 0;
  while (1)
  {
    delay(1);
    timers++;
    SYN7318_Flag = Voice_Drive();
    if (SYN7318_Flag != 0x00 || timers > 6000) //�жϳ�ʱ�˳�
    {
      timers = 0;
      return SYN7318_Flag;
    }
  }
  return 0;
}


/**************************************************
  ��  �ܣ�����ʶ��ش������������
  ��  ���� ��
  ����ֵ��  ��������ID /С������ʶ��ģ��״̬
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


