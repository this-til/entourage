#include "2021_Arduino_Demo.h"

static uint8_t GarageB_To1[8] = {0x55, 0x05, 0x01, 0x01, 0x00, 0x00, 0x02, 0xBB};  // 立体车库B 到达第一层
static uint8_t GarageB_To2[8] = {0x55, 0x05, 0x01, 0x02, 0x00, 0x00, 0x03, 0xBB};  // 立体车库B 到达第二层
static uint8_t GarageB_To3[8] = {0x55, 0x05, 0x01, 0x03, 0x00, 0x00, 0x04, 0xBB};  // 立体车库B 到达第三层
static uint8_t GarageB_To4[8] = {0x55, 0x05, 0x01, 0x04, 0x00, 0x00, 0x05, 0xBB};  // 立体车库B 到达第四层

static uint8_t TrafficA_Open[8] = {0x55, 0x0E, 0x01, 0x00, 0x00, 0x00, 0x01, 0xBB};    // 智能交通灯A 进入识别模式 进入识别模式
static uint8_t TrafficA_Red[8] = {0x55, 0x0E, 0x02, 0x01, 0x00, 0x00, 0x03, 0xBB};     // 智能交通灯A 识别结果为红色请求确认
static uint8_t TrafficA_Green[8] = {0x55, 0x0E, 0x02, 0x02, 0x00, 0x00, 0x04, 0xBB};   // 智能交通灯A 识别结果为绿色请求确认
static uint8_t TrafficA_Yellow[8] = {0x55, 0x0E, 0x02, 0x03, 0x00, 0x00, 0x05, 0xBB};  // 智能交通灯A 识别结果为黄色请求确认

static uint8_t Gate_Open[8] = {0x55, 0x03, 0x01, 0x01, 0x00, 0x00, 0x02, 0xBB};  // 道闸 开启

static uint8_t Light_plus1[4] = {0x00, 0xFF, 0x0C, static_cast<uint8_t>(~(0x0C))};  // 智能路灯 光源挡位加1
static uint8_t Light_plus2[4] = {0x00, 0xFF, 0x18, static_cast<uint8_t>(~(0x18))};  // 智能路灯 光源挡位加2
static uint8_t Light_plus3[4] = {0x00, 0xFF, 0x5E, static_cast<uint8_t>(~(0x5E))};  // 智能路灯 光源挡位加3

static uint8_t SEG_TimOpen[8] = {0x55, 0x04, 0x03, 0x01, 0x00, 0x00, 0x04, 0xBB};   // LED显示 计时开启
static uint8_t SMG_TimClose[8] = {0x55, 0x04, 0x03, 0x00, 0x00, 0x00, 0x03, 0xBB};  // LED显示 计时关闭

static int zt = 1;

static String qrCodeRedData1[10];
static String qrCodeRedData2[10];
static String qrCodeRedData3[10];

static String qrCodeBlueData1[10];
static String qrCodeBlueData2[10];
static String qrCodeBlueData3[10];

static String qrCodeGreenData1[10];
static String qrCodeGreenData2[10];
static String qrCodeGreenData3[10];


static String qrCodeData1[10];
static String qrCodeData2[10];
static String qrCodeData3[10];

static char wuxiancd[3];
static uint8_t shujujieshou[6];

static uint8_t wuxiancd2023[5];

//qrCodeDataArray二维码数据
String qrCodeDataArray[10];
String chushiweizi = "D1";

//csdw==初始档位
//mbdw==目标档位
//cka==车库a层数
int csdw = 0, mbdw = 0, cka = 0, xzx = 0, juli = 0;;


void time_closs(void) {
    Command.Judgment(SMG_TimClose);  //计算校验和
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, SMG_TimClose, 8);
}

uint8_t ZigBee_back[16] = {0x55, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00};
// 存储ZigBee模块的返回数据，初始化为指定的16字节序列

#define TSendCycle 200
// 定义发送周期的时间间隔，值为200

#define ATM_Data_Length 48
// 定义ATM数据的长度，值为48

uint8_t ATM_Data[ATM_Data_Length];
// 存储ATM数据的数组，长度为48

uint8_t ZigBee_command[8];
// 存储ZigBee命令的数组，长度为8

uint8_t ZigBee_judge;
// 用于判断ZigBee模块的状态的单字节变量

uint8_t Rbuf_judge;

uint8_t infrare_com[6];
// 存储红外命令的数组，长度为6

uint8_t sendflag;
// 标记发送状态的单字节变量

unsigned long frisrtime;
// 记录起始时间的无符号长整型变量

unsigned long Tcount;
// 计数的无符号长整型变量


uint8_t Data_Type;
uint8_t Data_Flag;
uint8_t Data_Length;
uint8_t Data_OTABuf[40];

/*void setup() {

}

void loop() {

}*/

void l_setup() {
    Serial.begin(115200);
    // 初始化串口通信，波特率115200
    CoreLED.Initialization();
    // 初始化CoreLED模块
    CoreKEY.Initialization();
    // 初始化CoreKEY模块
    CoreBeep.Initialization();
    // 初始化CoreBeep模块
    ExtSRAMInterface.Initialization();
    // 初始化ExtSRAMInterface模块
    carLight.Initialization();
    // 初始化LED模块
    BH1750.Initialization();
    // 初始化BH1750模块
    BEEP.Initialization();
    // 初始化BEEP模块
    Infrare.Initialization();
    // 初始化Infrare模块
    Ultrasonic.Initialization();
    // 初始化Ultrasonic模块
    DCMotor.Initialization(30000);
    // 初始化DCMotor模块，参数30000
    BKRC_Voice.Initialization();
    // 初始化BKRC_Voice模块

    while (!Serial);
    // 等待串口连接就绪
    sendflag = 0;
    // 初始化sendflag变量为0
    frisrtime = 0;
    // 初始化frisrtime变量为0
    Tcount = 0;
    // 初始化Tcount变量为0
}

int8_t a;

void l_loop() {
    uint8_t si = 0;
    frisrtime = millis();

    CoreKEY.Kwhile(KEY_Handler);  //按键检测

    if (ExtSRAMInterface.ExMem_Read(0x6100) != 0x00)  //从车接收ZigBee数据
    {
        ExtSRAMInterface.ExMem_Read_Bytes(ZigBee_command, 8);
        ZigBee_judge = ZigBee_command[6];  //获取校验和
        Command.Judgment(ZigBee_command);  //计算校验和
        // ZigBee_judge == ZigBee_command[6]) &&
        if ((ZigBee_command[0] == 0x55) && (ZigBee_command[7] == 0xBB)) {
            ZigBeeRx_Handler(ZigBee_command);  //ZigBee接收数据处理
        }
    }

    // if (ExtSRAMInterface.ExMem_Read(0x6038) != 0x00)  //检测OpenMV识别结果
    // {
    //   Data_Type = ExtSRAMInterface.ExMem_Read(0x603A);
    //   Data_Flag = ExtSRAMInterface.ExMem_Read(0x603B);
    //   Data_Length = ExtSRAMInterface.ExMem_Read(0x603C);
    //   Data_Length = Data_Length + 6;
    //   ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OTABuf, Data_Length);
    //   if ((Data_OTABuf[0] == 0x55) && (Data_OTABuf[1] == 0x02)) {
    //     ExtSRAMInterface.ExMem_Write_Bytes(0x6180, Data_OTABuf, Data_Length);  //使用自定义数据区上传OpenMV识别结果
    //     OpenMVRx_Handler(Data_OTABuf);                                         //接收OpenMV，数据处理函数
    //   }
    // }

    if (((millis() - frisrtime >= TSendCycle) || (Tcount >= TSendCycle)) && (sendflag == 1))  //获取、上传任务版数据
    {
        uint16_t tp = (uint16_t) (Ultrasonic.Ranging(CM) * 10.0);
        ZigBee_back[5] = (tp >> 8) & 0xff;
        ZigBee_back[4] = tp & 0xff;

        tp = BH1750.ReadLightLevel();
        ZigBee_back[7] = (tp >> 8) & 0xff;
        ZigBee_back[6] = tp & 0xff;

        ZigBee_back[9] = (uint8_t) ExtSRAMInterface.ExMem_Read(0x6003);
        if (ZigBee_back[9] >= 0x80) ZigBee_back[9] = 0xff - ZigBee_back[9];
        ZigBee_back[8] = (uint8_t) ExtSRAMInterface.ExMem_Read(0x6002);

        ExtSRAMInterface.ExMem_Write_Bytes(0x6080, ZigBee_back, 16);
        Tcount = 0x00;
    } else if (sendflag == 1) {
        Tcount += (millis() - frisrtime);
    }

    a++;
    a = a > 30 ? -80 : a;
    k230.setCameraSteeringGearAngle(a);

}


void ZigBeeRx_Handler(uint8_t* mar) {}

void OpenMVRx_Handler(uint8_t* mac) {}

void Analyze_Handle(uint8_t com) {}

void KEY_Handler(uint8_t k_value) {}

String DecIntToHexStr(long long num) {}

void OpenMVTrack_Disc_StartUp(void) {}

void OpenMVTrack_Disc_CloseUp(void) {}

void Servoangle_control(uint8_t data) {}

uint8_t qrdi1_buf[8] = {0x55, 0x02, 0x92, 0x00, 0x00, 0x00, 0x00, 0xBB};  // 给OpenMV发送识别二维码
//********************************************************************************
/*
功    能：向K210发送数据函数（优先在mixpy里给摄像头导入程序）
参    数： 01开启二维码识别
           02关闭二维码识别
           03开启交通灯识别
           04关闭交通灯识别 
           05切换至彩色摄像头
           06切换至灰度摄像头         
返 回 值：无
*/
void K210_Send(int8_t angle) {
    qrdi1_buf[3] = angle;
    Command.Judgment(qrdi1_buf);  //计算校验和
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, qrdi1_buf, 8);
}

void Beep_time3() {
    CoreBeep.Initialization();
    CoreBeep.TurnOn();
    delay(100);
    CoreBeep.TurnOff();
    delay(100);
    CoreBeep.TurnOn();
    delay(100);
    CoreBeep.TurnOff();
    delay(100);
    CoreBeep.TurnOn();
    delay(100);
    CoreBeep.TurnOff();
}


//磁悬浮无线充电标志物
//开启
uint8_t open_wuxiancongdian_buf[] = {0x55, 0x0A, 0x02, 0x00, 0x00, 0x00, 0x00, 0xBB};

void wuxiancongdian(uint8_t a, uint8_t b, uint8_t c) {
    open_wuxiancongdian_buf[3] = a;
    open_wuxiancongdian_buf[4] = b;
    open_wuxiancongdian_buf[5] = c;
    Command.Judgment(open_wuxiancongdian_buf);  //计算校验和
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, open_wuxiancongdian_buf, 8);
}

//修改
uint8_t xg_wuxiancongdian_buf[] = {0x55, 0x0A, 0x03, 0x00, 0x00, 0x00, 0x00, 0xBB};

void wuxiancongdianxg(uint8_t a, uint8_t b, uint8_t c) {
    xg_wuxiancongdian_buf[3] = a;
    xg_wuxiancongdian_buf[4] = b;
    xg_wuxiancongdian_buf[5] = c;
    Command.Judgment(xg_wuxiancongdian_buf);  //计算校验和
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, xg_wuxiancongdian_buf, 8);
}


uint8_t mr_wuxiancongdian_buf[] = {0x55, 0x0A, 0x01, 0x01, 0x00, 0x00, 0x00, 0xBB};

void wuxiancongdianmr(void) {
    Command.Judgment(mr_wuxiancongdian_buf);  //计算校验和
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, mr_wuxiancongdian_buf, 8);
}


/*
道闸测试
*/
uint8_t open_road_buf[] = {0x55, 0x03, 0x01, 0x01, 0x00, 0x00, 0x00, 0xBB};

void Road_Gate_Test(void) {
    Command.Judgment(open_road_buf);  //计算校验和
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, open_road_buf, 8);
}

uint8_t test_buf[] = {0xFD, 0x00, 0x06, 0x01, 0x01, 0xC4, 0xFA, 0xBA, 0xC3};

/*
  语音播报任意内容
*/
void Speech_Sounds_Ctr(void) {
    ExtSRAMInterface.ExMem_Write_Bytes(0x6180, test_buf, 13);
}


uint8_t repo_buf[] = {0x03, 0x05, 0x14, 0x45, 0xDE, 0x92};

/*
红外测试
*/
void Infrared_Test(void) {
    Infrare.Transmition(repo_buf, 6);
}

uint8_t trm_buf[] = {0xAF, 0x06, 0x00, 0x02, 0x00, 0x00, 0x01, 0xBB};

/*
语音识别
*/
void Speech_Disc(void) {
    uint8_t sph_id = 0;
    sph_id = BKRC_Voice.BKRC_Voice_Extern(0);  //0为控制语音播报标志物随机播报并开启识别，
    //2-6为指定词条识别，具体信息可查看通信协议
    delay(200);
    Serial.print("识别到的词条ID为：");
    Serial.println(sph_id, HEX);
    trm_buf[2] = sph_id;
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, trm_buf, 8);  //上传识别结果至评分终端
}


uint8_t carpai_buf[] = {0x55, 0x01, 0x44, wuxiancd2023[0], wuxiancd2023[1], wuxiancd2023[2], 0x00, 0xBB};

void carpai_Test(void) {
    Command.Judgment(carpai_buf);  //计算校验和
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, carpai_buf, 8);
}

uint8_t carpai1_buf[] = {0x55, 0x01, 0x45, wuxiancd2023[3], wuxiancd2023[4], wuxiancd2023[5], 0x00, 0xBB};

void carpai1_Test(void) {
    Command.Judgment(carpai1_buf);  //计算校验和
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, carpai1_buf, 8);
}

void zdytoA(int data) {
    uint8_t road_bufa[] = {0x55, 0x01, 0x46, 0x00, 0x00, 0x00, 0x00, 0xBB};
    road_bufa[3] = (uint8_t) (data >> 8);  // 将data的高八位存储到road_bufa[3]
    road_bufa[4] = (uint8_t) data;         // 将data的低八位存储到road_bufa[4]
    Command.Judgment(road_bufa);          // 计算校验和
    delay(500);
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, road_bufa, 8);
}


//车牌发送到主车
void carpaitoA(const char* data) {
    uint8_t road_bufa[] = {0x55, 0x01, 0x44, 0x00, 0x00, 0x00, 0x00, 0xBB};
    uint8_t road_bufb[] = {0x55, 0x01, 0x45, 0x00, 0x00, 0x00, 0x00, 0xBB};
    road_bufa[3] = data[0];
    road_bufa[4] = data[1];
    road_bufa[5] = data[2];
    road_bufb[3] = data[3];
    road_bufb[4] = data[4];
    road_bufb[5] = data[5];
    Command.Judgment(road_bufa);  //计算校验和
    Command.Judgment(road_bufb);  //计算校验和

    delay(300);
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, road_bufa, 8);
    delay(300);
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, road_bufb, 8);
    delay(300);
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, road_bufa, 8);
    delay(300);
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, road_bufb, 8);
    delay(300);
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, road_bufa, 8);
    delay(300);
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, road_bufb, 8);
}


void shuju_Test(void) {
    while (1) {
        if (ExtSRAMInterface.ExMem_Read(0x6100) != 0x00)  //从车接收ZigBee数据
        {
            ExtSRAMInterface.ExMem_Read_Bytes(ZigBee_command, 8);
            ZigBee_judge = ZigBee_command[6];  //获取校验和
            Command.Judgment(ZigBee_command);  //计算校验和
            if ((ZigBee_command[0] == 0x55) && (ZigBee_command[7] == 0xBB) && (ZigBee_command[1] == 0x02) &&
                (ZigBee_command[2] == 0x34)) {
                break;
            }
        }
    }
}



// void KEY_Handler(uint8_t k_value) {
//   switch (k_value) {
//     case 1:
//     //循迹前进固定距离
//       BEEP.TurnOn();
//       delay(50);
//       BEEP.TurnOff();
//       Servo_Control(-55);      //循迹
//       delay(1000);
//       OpenMV_Track(50);
//       delay(1000);
//       DCMotor.Go(20, 300);
//       break;
//     case 2:
//     //有卡循迹前进固定距离
//       BEEP.TurnOn();
//       delay(50);
//       BEEP.TurnOff();
//       Servo_Control(-55);      //循迹
//       delay(1000);
//       OpenMV_Track(50);
//       delay(1000);
//       DCMotor.Go(20, 350);
//       break;
//     case 3:
//     //左转
//       BEEP.TurnOn();
//       delay(50);
//       BEEP.TurnOff();
//       DCMotor.TurnLeft_Code(60, 680);
//       TEXT_display3(0x02);
//       break;
//     case 4:
//     //右转
//       BEEP.TurnOn();
//       delay(50);
//       BEEP.TurnOff();
//       DCMotor.TurnRight_Code(60, 800);
//       // Road_Gate_TestA("ABD012");
//         //  Road_Traffic_ATest();
//       break;
//   }
// }


/*
void KEY_Handler(uint8_t k_value) {
    switch (k_value) {
        case 1:
            //抬头红绿灯
            BEEP.TurnOn();
            delay(50);
            BEEP.TurnOff();
            wuxiancongdianmr();
            break;
        case 2:
            BEEP.TurnOn();
            delay(50);
            BEEP.TurnOff();
            // wuxiancongdian(0xA1, 0x23, 0xB4);
            delay(500);
            Servo_Control(20);
            delay(500);
            K210_Send(0x05);

            break;
        case 3:
            //拍照
            BEEP.TurnOn();
            delay(50);
            BEEP.TurnOff();
            // wuxiancongdianxg(0x11,0x22,0x33);
            // OpenMV_TrackCAR(15);
            // DCMotor.Go(20, 150);
            // OpenMV_TrackLZW(50);
            // OpenMV_TrackRZW(50);
            // OpenMV_TrackCAR(30);
            K210_Send(0x08);
            // TEXT_display3(0x02);
            break;
        case 4:
            //右转

            BEEP.TurnOn();
            delay(50);
            BEEP.TurnOff();
            // wuxiancongdian(0x11,0x22,0x33);
            delay(500);

            switch (0x01) {
                //D7
                case 0x01:
                    chushiweizi = "D7";
                    OpenMV_Track(15);
                    delay(500);
                    DCMotor.Go(20, 150);
                    delay(500);
                    OpenMV_TrackRZW(60);
                    OpenMV_Track(15);
                    delay(500);
                    DCMotor.Go(20, 150);
                    delay(500);
                    OpenMV_TrackLZW(60);
                    break;
                    //F7
                case 0x02:
                    chushiweizi = "F7";
                    OpenMV_Track(15);
                    break;
                    //G6
                case 0x03:
                    chushiweizi = "G6";
                    OpenMV_Track(15);
                    delay(500);
                    DCMotor.Go(20, 150);
                    delay(500);
                    OpenMV_TrackRZW(60);
                    break;
            }
            DCMotor.Go(20, 200);
            Road_Traffic_BTest();

            OpenMV_TrackCAR(15);
            OpenMV_Track(15);
            DCMotor.Go(20, 150);
            OpenMV_TrackRZW(60);
            //超声波测距
            juli = csbcm();
            Serial.println(juli);
            maintainDistance(20, 15);
            //QR
            Road_QR_Test();
            maintainDistance(20, juli);
            OpenMV_TrackLZW(60);
            OpenMV_TrackLZW(60);
            DCMotor.TurnLeft_Code(40, 300);
            delay(500);
            TEXT_display4(qrCodeDataArray[0].c_str(), chushiweizi.c_str());
            DCMotor.TurnRight_Code(40, 300);
            OpenMV_Track(15);
            Road_Gate_TestA(qrCodeDataArray[0].c_str());
            OpenMV_Track(15);
            DCMotor.Go(20, 150);
            OpenMV_TrackLZW(60);
            DCMotor.Go(20, 150);
            A1_Cat_ku_wait();
            DCMotor.Back(40, 800);
            carparkA3_Test();
            xzx = tihuan(qrCodeDataArray[1].c_str(), csdw, mbdw, cka);
            carpaitoA(qrCodeDataArray[0].c_str());
            delay(1000);
            zdytoA(xzx);
            delay(500);
            while (1);
            break;
    }
}
*/



// /**
// 功   能: 按键处理函数
// 参   数: K_value 按键值
// 返回值:无
// */
// void KEY_Handler(uint8_t k_value) {
//   switch (k_value) {
//     case 1:
//       A1_Cat_ku_wait();
//       Servo_Control(10);
//       delay(1000);
//       delay(1000);
//       DCMotor.CarTrack(40);
//       delay(1000);
//       DCMotor.Go(45, 300);
//       delay(1000);
//       DCMotor.TurnLeft_Code(80, 680);
//       delay(1000);
//       DCMotor.Go(40, 100);
//       delay(1000);
//       Road_QR_Test();  //二維碼
//       delay(1000);
//       DCMotor.Back(40, 100);
//       delay(1000);
//       K210_Send(0x05);
//       delay(2000);
//       DCMotor.TurnRight_Code(80, 680);  //右转
//       delay(1000);
//       DCMotor.TurnRight_Code(80, 680);  //右转
//       delay(1000);
//       DCMotor.CarTrack_RFID(30);
//       delay(1000);
//       DCMotor.Go(45, 300);
//       delay(1000);
//       DCMotor.Go(40, 1200);
//       delay(1000);
//       DCMotor.CarTrack(40);
//       delay(1000);
//       DCMotor.Go(45, 300);
//       delay(1000);
//       DCMotor.TurnLeft_Code(80, 600);
//       delay(1000);
//       DCMotor.Back(45, 100);
//       delay(1000);
//       Servo_Control(20);
//       delay(1000);

//       Road_Traffic_ATest();  //紅綠燈A
//       delay(1000);
//       DCMotor.Go(45, 200);
//       delay(1000);
//       DCMotor.CarTrack(40);
//       delay(1000);
//       Road_Traffic_BTest();  //紅綠燈B
//       DCMotor.Go(45, 300);
//       delay(1000);
//       delay(1000);
//       DCMotor.CarTrack(40);
//       delay(1000);
//       DCMotor.Go(45, 600);
//       delay(1000);
//       DCMotor.Back(45, 300);
//       delay(1000);
//       DCMotor.TurnRight_Code(80, 680);  //右转
//       delay(1000);
//       Servo_Control(10);
//       delay(1000);


//       carpai_Test();
//       delay(2000);
//       carpai_Test();
//       delay(2000);
//       carpai1_Test();
//       delay(2000);
//       carpai1_Test();
//       delay(3000);
//       carpai_Test();
//       delay(3000);
//       carpai_Test();
//       delay(3000);
//       carpai1_Test();
//       delay(3000);
//       carpai1_Test();
//       delay(1000);



//       //二維碼識別A
//       DCMotor.TurnLeft_Code(80, 600);
//       delay(1000);
//       //開報警台
//       DCMotor.TurnLeft_Code(80, 650);
//       delay(1000);
//       //開道閘
//       DCMotor.CarTrack(70);
//       delay(1000);
//       DCMotor.Go(45, 300);
//       delay(1000);
//       DCMotor.TurnLeft_Code(80, 600);
//       delay(1000);
//       DCMotor.Back(30, 900);
//       switch (zt) {
//         case 1:
//           delay(1000);
//           carparkA1_Test();
//           delay(1000);
//           carparkA1_Test();
//           delay(1000);
//           break;
//         case 2:
//           delay(1000);
//           carparkA2_Test();
//           delay(1000);
//           carparkA2_Test();
//           delay(1000);
//           break;
//         case 3:
//           delay(1000);
//           carparkA3_Test();
//           delay(1000);
//           carparkA3_Test();
//           delay(1000);
//           break;
//       }
//       delay(1000);
//       time_closs();
//       delay(1000);
//       time_closs();
//       delay(1000);
//       time_closs();

//       break;
//     case 2:
//       BEEP.TurnOn();
//       delay(50);
//       BEEP.TurnOff();
//       // K210_Send(0x05);
//       // delay(7000);
//       // DCMotor.Back(45, 100);
//       // delay(1000);
//       Servo_Control(20);
//       delay(1000);
//       Road_Traffic_ATest();  //紅綠燈A
//       delay(1000);
//       BEEP.TurnOn();
//       delay(50);
//       BEEP.TurnOff();
//       // DCMotor.Go(45, 200);
//       // delay(1000);
//       // DCMotor.CarTrack(40);
//       // delay(1000);
//       // Road_Traffic_BTest();  //紅綠燈B
//       // DCMotor.Go(45, 300);
//       // delay(1000);
//       // delay(1000);
//       // DCMotor.CarTrack(40);
//       // delay(1000);




//       break;
//     case 3:

//       BEEP.TurnOn();
//       delay(50);
//       BEEP.TurnOff();
//       OpenMV_Track(50);
//       delay(1000);
//       DCMotor.Go(20, 500);
//       delay(1000);
//       DCMotor.TurnRight_Code(60, 680);
//       delay(1000);
//       OpenMV_Track(50);
//       // K210_Send(0x08);




//       tihuan("((n*y+z)^4)/100", 2, 3, 4);

//       // QR72022("014684","AE1D6Y8FDS");
//       // wuxiancongdian(0xA1,0x23,0xB4);
//       // Road_QR_Test2();
//       // Road_QR_Test1();
//       // QR72023();
//       // delay(1000);
//       // time_closs();
//       // delay(1000);
//       // time_closs();
//       // delay(1000);
//       // time_closs();
//       // Road_QR_Test2();
//       // delay(2000);
//       // carpai_Test();
//       // delay(1000);
//       // carpai_Test();
//       // delay(2000);
//       // carpai1_Test();
//       // delay(1000);
//       // carpai1_Test();
//       break;
//     case 4:
//       BEEP.TurnOn();
//       delay(50);
//       BEEP.TurnOff();
//       Servo_Control(-55);



//       // wuxiancongdia(nxg(0xA1,0x23,0xB4);
//       // BJT_OPEN(0x03,0x05,0x14,0x45,0xDE,0x92);
//       // while(1){
//       // DCMotor.CarTrack(40);
//       // DCMotor.Go(45, 300);
//       // delay(2000);
//       // DCMotor.TurnLeft_Code(60, 680);
//       // delay(2000);
//       // }




//       // wuxiancongdianmr();
//       // K210_Send(0x05);
//       // delay(7000);
//       // DCMotor.Back(45, 100);
//       // delay(1000);
//       // Servo_Control(20);
//       // delay(1000);
//       // Road_Traffic_ATest();  //紅綠燈A
//       // delay(1000);
//       // DCMotor.Go(45, 200);
//       // delay(1000);
//       // DCMotor.CarTrack(40);
//       // delay(1000);
//       // Road_Traffic_BTest();  //紅綠燈B
//       // DCMotor.Go(45, 300);
//       // delay(1000);
//       // delay(1000);
//       // DCMotor.CarTrack(40);
//       // delay(1000);



//       // chaoShengBo();
//       // Road_QR_Test();
//       // Servo_Control(-60);
//       // delay(1000);
//       // OpenMV_Track(10);
//       // delay(1000);
//       // DCMotor.Go(20, 350);
//       // delay(1000);
//       // OpenMV_TrackRZW(30);

//       // OpenMV_Track(20);


//       // K210_Send(0x08);
//       // Road_QR_color_Test();
//       //G6
//       // OpenMV_Track(50);
//       // DCMotor.Go(20, 350);
//       // delay(1000);
//       // OpenMV_TrackRZW(50);
//       // delay(1000);
//       break;
//   }
// }









/*
*//*
功    能：从车ZigBee接收，处理函数
参    数：*mar 接收数据指针
返 回 值：无
*//*
void ZigBeeRx_Handler(uint8_t* mar) {
    switch (mar[1]) {
        case 0x02:  //主车
            Serial.println(mar[2], HEX);
            Analyze_Handler(mar);  //主车命令相应函数
            break;
        case 0x03:  //道闸标志物
            break;
        case 0x04:  //LED显示标志物（暂无返回）
            break;
        case 0x05:  //立体车库标志物B
            break;
        case 0x06:  //语音播报标志物
            break;
        case 0x07:  //红外报警标志物
            break;
        case 0x08:  //TFT显示标志物B
            break;
        case 0x09:  //调光标志物
            break;
        case 0x0A:  //磁悬浮无线充电标志物
            break;
        case 0x0B:  //TFT显示标志物A
            break;
        case 0x0C:  //ETC系统标志物
            break;
        case 0x0D:  //立体车库标志物A
            break;
        case 0x0E:  //交通灯标注物A
            break;
        case 0x0F:  //交通灯标志物B
            break;
        default:
            break;
    }
}

*//*
功    能：从车接收 机器视觉Camera返回数据
参    数：*mac 接收数据指针
返 回 值：无
*//*
void OpenMVRx_Handler(uint8_t* mac) {
    switch (mac[2]) {
        case 0x91:  //保留

            break;
        case 0x92:  //二维码识别

            break;
        default:
            break;
    }
}

*//**
功    能：从车任务处理函数
参    数：com 主指令
返 回 值：无
*//*
void Analyze_Handler(uint8_t* com) {
    Serial.println(com[2], HEX);
    switch (com[2]) {
        case 0x01:  //停止
            DCMotor.Stop();
            break;
        case 0x02:  //前进
            DCMotor.Go(com[3], (com[4] + (com[5] << 8)));
            ZigBee_back[2] = 0x03;
            break;
        case 0x03:  //后退
            DCMotor.Back(com[3], (com[4] + (com[5] << 8)));
            ZigBee_back[2] = 0x03;
            break;
        case 0x04:  //左转
            DCMotor.TurnLeft(com[3]);
            ZigBee_back[2] = 0x02;
            break;
        case 0x05:  //右转
            DCMotor.TurnRight(com[3]);
            ZigBee_back[2] = 0x02;
            break;
        case 0x06:  //循迹
            DCMotor.CarTrack(com[3]);
            ZigBee_back[2] = 0x00;
            break;
        case 0x07:                                                   //码盘清零
            Command.Judgment(Command.command01);                       //计算校验和
            ExtSRAMInterface.ExMem_Write_Bytes(Command.command01, 8);  //码盘清零
            break;
        case 0x10:  //保存红外数据
            infrare_com[0] = com[3];
            infrare_com[1] = com[4];
            infrare_com[2] = com[5];
            break;
        case 0x11:  //保存红外数据
            infrare_com[3] = com[3];
            infrare_com[4] = com[4];
            infrare_com[5] = com[5];
            break;
        case 0x12:  //发送红外数据
            Infrare.Transmition(infrare_com, 6);
            break;
        case 0x20:  //左右转向灯
            if (com[3] == 0x01) carLight.LeftTurnOn();
            else carLight.LeftTurnOff();
            if (com[4] == 0x01) carLight.RightTurnOn();
            else carLight.RightTurnOff();
            break;
        case 0x30:  //蜂鸣器
            if (com[3] == 0x01) BEEP.TurnOn();
            else BEEP.TurnOff();
            break;
        case 0x31:  //蜂鸣器
            shujujieshou[0] = com[3];
            shujujieshou[1] = com[4];
            shujujieshou[2] = com[5];
            Serial.println(shujujieshou[0], HEX);
            Serial.println(shujujieshou[1], HEX);
            Serial.println(shujujieshou[2], HEX);
            break;
        case 0x32:  //蜂鸣器
            shujujieshou[3] = com[3];
            shujujieshou[4] = com[4];
            shujujieshou[5] = com[5];
            Serial.println(shujujieshou[3], HEX);
            Serial.println(shujujieshou[4], HEX);
            Serial.println(shujujieshou[5], HEX);
            break;
        case 0x33:
            BEEP.TurnOn();
            delay(50);
            BEEP.TurnOff();
            // wuxiancongdian(0x11,0x22,0x33);
            delay(500);

            switch (com[3]) {
                //D7
                case 0x01:
                    chushiweizi = "D7";
                    OpenMV_Track(15);
                    delay(500);
                    DCMotor.Go(20, 150);
                    delay(500);
                    OpenMV_TrackRZW(60);
                    OpenMV_Track(15);
                    delay(500);
                    DCMotor.Go(20, 150);
                    delay(500);
                    OpenMV_TrackLZW(60);
                    break;
                    //F7
                case 0x02:
                    chushiweizi = "F7";
                    OpenMV_Track(15);
                    break;
                    //G6
                case 0x03:
                    chushiweizi = "G6";
                    OpenMV_Track(15);
                    delay(500);
                    DCMotor.Go(20, 150);
                    delay(500);
                    OpenMV_TrackRZW(60);
                    break;
            }
            DCMotor.Go(20, 200);
            Road_Traffic_BTest();

            OpenMV_TrackCAR(15);
            OpenMV_Track(15);
            DCMotor.Go(20, 150);
            OpenMV_TrackRZW(60);
            //超声波测距
            juli = csbcm();
            Serial.println(juli);
            maintainDistance(20, 15);
            //QR
            Road_QR_Test();
            maintainDistance(20, juli);
            OpenMV_TrackLZW(60);
            OpenMV_TrackLZW(60);
            DCMotor.TurnLeft_Code(40, 300);
            delay(500);
            TEXT_display4(qrCodeDataArray[0].c_str(), chushiweizi.c_str());
            DCMotor.TurnRight_Code(40, 300);
            OpenMV_Track(15);
            Road_Gate_TestA(qrCodeDataArray[0].c_str());
            OpenMV_Track(15);
            DCMotor.Go(20, 150);
            OpenMV_TrackLZW(60);
            DCMotor.Go(20, 150);
            A1_Cat_ku_wait();
            DCMotor.Back(40, 1000);
            carparkA3_Test();
            xzx = tihuan(qrCodeDataArray[1].c_str(), csdw, mbdw, cka);
            carpaitoA(qrCodeDataArray[0].c_str());
            delay(1000);
            zdytoA(xzx);
            delay(500);
            break;
        case 0x40:  //保留
            csdw = (int) com[3];
            Serial.print("初始档位：");
            Serial.println(csdw);
            mbdw = (int) com[4];
            Serial.print("目标档位：");
            Serial.println(mbdw);
            ckcscx();
            Serial.print("车库档位：");
            Serial.println(cka);
            break;
        case 0x50:  //LCD图片上翻页
            Command.Judgment(Command.command13);
            ExtSRAMInterface.ExMem_Write_Bytes(Command.command13, 8);
            break;
        case 0x51:  //LCD图片下翻页
            Command.Judgment(Command.command14);
            ExtSRAMInterface.ExMem_Write_Bytes(Command.command14, 8);
            break;
        case 0x61:  //光源加一档
            Infrare.Transmition(Command.HW_Dimming1, 4);
            break;
        case 0x62:  //光源加二挡
            Infrare.Transmition(Command.HW_Dimming2, 4);
            break;
        case 0x63:  //光源加三挡
            Infrare.Transmition(Command.HW_Dimming3, 4);
            break;
        case 0x80:  //从车返回数据切换
            if (com[3] == 0x01) {
                sendflag = 0x01;  //上传从车数据
            } else {
                sendflag = 0x00;  //关闭上从车数据
            }
            break;
        case 0x91:
            if (com[3] == 0x03)  //舵机控制
            {
                Servoangle_control(com[4]);
            }
            break;
        default:
            break;
    }
}*/
