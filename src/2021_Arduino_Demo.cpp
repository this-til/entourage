#include "2021_Arduino_Demo.h"





/*void setup() {
    l_setup();
}

void loop() {
    l_loop();
}*/

defineTask(TaskMain);
defineTask(TaskCar);


void l_setup() {
    Serial.begin(115200);
    Serial3.begin(9600);
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
    while (!Serial3);

    dataSetUp();
    deviceSetup();

    mySCoop.start();

    Serial.println("init end");

}

/*
void serialEvent() {
    char data = Serial.read();
    Serial3.write(data);
}
*/

void l_loop() {
    yield();
}

void TaskMain::setup() {
}

/***
 * 循环内总线发送0x6008 接收0x6100、0x6038
 */
void TaskMain::loop() {
    CoreKEY.Kwhile(keyHandler);
}

void TaskCar::setup() {
}

uint8_t zigbeeBuf[8] = {};

void acceptZigbee() {
    // zigbee
    if (ExtSRAMInterface.ExMem_Read(0x6100) == 0x00) {
        return;
    }

    ExtSRAMInterface.ExMem_Read_Bytes(0x6100, zigbeeBuf, 8);
#if DE_BUG
    Serial.print("[DEBUG] read from zigbee:");
    logHex(zigbeeBuf, 8);
#endif

    uint32_t com = 0;
    for (uint8_t i = 2; i < 8 - 2; i++) {
        com += zigbeeBuf[i];
    }
    auto checksum = (uint8_t) (com % 256);
    if (zigbeeBuf[8 - 2] != checksum) {
#if DE_BUG
        Serial.print("  inaccuracy checksum:");
        Serial.print("predict:");
        logHex(checksum);
        Serial.print(",source:");
        logHex(zigbeeBuf[8 - 2]);
        Serial.println("    check failure...");
#endif
        return;
    }

#if DE_BUG
    Serial.println("    check successful...");
#endif


    uint8_t id = zigbeeBuf[1];

    DeviceBase* deviceBase = allDeviceBase[id];

    if (deviceBase != nullptr) {
        deviceBase->onReceiveZigbeeMessage(zigbeeBuf);
    }

}

uint8_t uploadTaskBoardZigBeeBack[16] = {0x55, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

#define uploadTaskBoardCycleMs 200
bool isUploadTaskBoard = 0;
unsigned long uploadTaskBoardRecordingTime = 0;

void uploadTaskBoard() {
    if (!isUploadTaskBoard) {
        return;
    }
    if (millis() - uploadTaskBoardRecordingTime < uploadTaskBoardCycleMs) {
        return;
    }
    uploadTaskBoardRecordingTime = millis();

    uint16_t tp = (uint16_t) (Ultrasonic.Ranging(CM) * 10.0);
    uploadTaskBoardZigBeeBack[5] = (tp >> 8) & 0xff;
    uploadTaskBoardZigBeeBack[4] = tp & 0xff;

    tp = BH1750.ReadLightLevel();
    uploadTaskBoardZigBeeBack[7] = (tp >> 8) & 0xff;
    uploadTaskBoardZigBeeBack[6] = tp & 0xff;

    uploadTaskBoardZigBeeBack[9] = (uint8_t) ExtSRAMInterface.ExMem_Read(0x6003);
    if (uploadTaskBoardZigBeeBack[9] >= 0x80) uploadTaskBoardZigBeeBack[9] = 0xff - uploadTaskBoardZigBeeBack[9];
    uploadTaskBoardZigBeeBack[8] = (uint8_t) ExtSRAMInterface.ExMem_Read(0x6002);

    ExtSRAMInterface.ExMem_Write_Bytes(0x6080, uploadTaskBoardZigBeeBack, 16);

}

uint8_t posVersion = 0;

uint16_t pathDebug[32] = {};
uint16_t forecastPathDebug[32] = {};
char atrDebug[65] = {};

void whenVarUpdated() {
#if DE_BUG
    if (posVersion != netSynchronization.globalVariableVersion[180]) {
        posVersion = netSynchronization.globalVariableVersion[180];

        uint8_t len = 0;
        netSynchronization.getPathInformation(pathDebug, 32, &len);

        /*Serial.print("obj?:");
        Serial.print((uint32_t) &barrierGateA);
        Serial.print(":");
        Serial.print((uint32_t) allDeviceBase[barrierGateA.id]);
        Serial.println();*/

        Serial.print("[DEBUG] a new path data:");
        Serial.print("  len:");
        Serial.print(len);
        Serial.print("  data(HEX)");
        logHex(pathDebug, len);
        Serial.print("  data(str):");
        analyze(pathDebug, len, atrDebug, 65);
        Serial.println(atrDebug);

        uint16_t* _path = pathDebug + 2;
        uint8_t _len = len - 3;


        assembly("D4", forecastPathDebug, _len);
        if (equals(_path, forecastPathDebug, _len)) {
            Serial.print("D4");
        }

        assembly("B6", forecastPathDebug, _len);
        if (equals(_path, forecastPathDebug, _len)) {
            Serial.print("B6");
        }

    }
#endif


}

void TaskCar::loop() {
    acceptZigbee();
    uploadTaskBoard();
    whenVarUpdated();
}

uint16_t path[32] = {};
uint16_t forecastPath[32] = {};
uint8_t qrBuf[48] = {};
uint8_t licensePlateBuf[7] = "A12345";

void questions5() {
    uint8_t len = 0;
    while (!netSynchronization.getPathInformation(path, 32, &len)) {
        yield();
    }

    uint16_t initiation = path[0];

    k230.setCameraSteeringGearAngle(-55);
    k230.setTrackModel(TRACK_MIDDLE);

    car.trackAdvanceToNextJunction(); //F1->F2
    car.trackTurnRight();
    car.trimCar();

    k230.setTrackModel(0);
    k230.setCameraSteeringGearAngle(0);
    trafficLightA.requestToEnterRecognitionMode();

    K210Color trafficLightColor;
    k230.trafficLightRecognize_rigorous(&trafficLightColor);
    trafficLightA.requestConfirmationOfIdentificationResults(k230ColorToTrafficLightModel(trafficLightColor)); // 任务8

    k230.setCameraSteeringGearAngle(-55);
    k230.setTrackModel(TRACK_MIDDLE);

    car.trackAdvanceToNextJunction(); //F2->D2

    car.trackAdvanceToNextJunction(); //D2->B2

    k230.setTrackModel(0);
    k230.setCameraSteeringGearAngle(0);

    //二维码
    uint8_t qrCount;
    k230.qrRecognize(&qrCount, qrMessageArray, 4);


    uint8_t qr1SpecialNumber = 0;
    uint8_t qr2SpecialNumber = 0;

    excludeSpecialCharacter(qrMessageArray[0].message, qrMessageArray[0].messageLen, qrBuf, 48, &qr1SpecialNumber);
    excludeSpecialCharacter(qrMessageArray[1].message, qrMessageArray[1].messageLen, qrBuf, 48, &qr1SpecialNumber);

    QrMessage* qr1Message;
    QrMessage* qr2Message;

    if (qr1SpecialNumber < qr2SpecialNumber) {
        qr1Message = qrMessageArray;
        qr2Message = qrMessageArray + 1;
    } else {
        qr1Message = qrMessageArray + 1;
        qr2Message = qrMessageArray;
    }

    excludeSpecialCharacter(qr1Message->message, qr1Message->messageLen, licensePlateBuf, 6);

    netSynchronization.synchronousLicensePlateNumber(licensePlateBuf);


    k230.setCameraSteeringGearAngle(-55);
    k230.setTrackModel(TRACK_MIDDLE);

    car.trackTurnLeft();

    unsigned long startTime = millis();
    while (millis() - startTime < 10000) {
        barrierGateA.setLicensePlateData(licensePlateBuf);
        CoreBeep.TurnOn();
        sleep(1000);
        CoreBeep.TurnOff();
        if (barrierGateA.getGateControl()) {
            break;
        }
    }

    car.trackAdvanceToNextJunction(); //B2->B4

    car.turnLeft(60);
    sleep(500);
    stereoscopicDisplayA.showLicensePlate(licensePlateBuf, 0, 0);
    sleep(500);
    car.turnRight(60);
    car.trimCar();

    uint16_t* _path = path + 2;
    uint8_t _len = len - 3;


    assembly("D4", forecastPath, _len);
    if (equals(_path, forecastPath, _len)) {
        car.trackTurnLeft();
        car.trackAdvanceToNextJunction(); //B4->D4

        netSynchronization.synchronousGlobalVariable('q', 0);
        CoreBeep.TurnOn();
        sleep(1000);
        CoreBeep.TurnOff();

        car.trackTurnRight();

        car.trackAdvanceToNextJunction(); //D4->D6

        car.trackTurnRight();
        car.trackTurnRight();
    }

    assembly("B6", forecastPath, _len);
    if (equals(_path, forecastPath, _len)) {
        car.trackAdvanceToNextJunction(); //B4->B6
        netSynchronization.synchronousGlobalVariable('q', 0);
        CoreBeep.TurnOn();
        sleep(1000);
        CoreBeep.TurnOff();

        car.turnLeft();
        car.recoil(500);
        car.trimCar();
        car.advance(50);
        car.overspecificRelief();
        car.trackAdvanceToNextJunction(); //B6->D6
        car.turnLeft();
    }

    /* Direction endDirection;
     for (int i = 1; i < len - 1; ++i) {
         sandTable.moveTo(pack(path[i]), D_UNDER, pack(path[i + 1]), &endDirection);
         if (path[i + 1] == assembly("B6")) {
             CoreBeep.TurnOn();
             sleep(1000);
             CoreBeep.TurnOff();
             netSynchronization.synchronousGlobalVariable('q', 0);
         }
     }

     sandTable.adjustDirection(pack(assembly("D6")), endDirection, D_UP);*/

    bool haveGarage = false;
    if (haveGarage) {

        uint8_t level;
        carportA.getLevel(&level);
        netSynchronization.synchronousGlobalVariable('h', level);


    } else {
        netSynchronization.synchronousGlobalVariable('h', 2);
        car.advanceCorrection(50, 6);
        car.recoilToNextJunction();
        //car.recoil(600);
    }
}

bool renderToScreen = false;
uint8_t cameraState = CAMERA_RGB565;

void keyHandler(uint8_t k_value) {
#if DE_BUG
    Serial.print("KEY_Handler:");
    logHex(k_value);
    Serial.println();
#endif

    CoreBeep.TurnOn();
    sleep(100);
    CoreBeep.TurnOff();

    uint8_t level;

    bool ventral;
    bool rearSide;


    switch (k_value) {
        case 0x01:
            //questions5();
            carTest.overspecificReliefTest();
            break;
        case 0x02:
            //carTest.advanceCorrectionTest();

            netSynchronization.synchronousGlobalVariable('q', 0);
            //carTest.overspecificReliefTest();

            break;
        case 0x03:

            cameraState++;
            if (cameraState > CAMERA_GRAYSCALE_LOW_GAIN) {
                cameraState = 1;
            }

            k230.setCameraState((CameraState) cameraState);

            /*     barrierGateA.setLicensePlateData(licensePlateBuf);

                 netSynchronization.synchronousLicensePlateNumber((uint8_t*) "A12345");*/

/*
            k230.setTrackModel(TRACK_MIDDLE);
            carportA.moveToLevel(1);

            car.advanceCorrection(50, 6);

            k230.setTrackModel(0);

            while (true) {
                if (carportA.getLevel(&level)) {
                    if (level == 1) {
                        break;
                    }
                }
            }

            DCMotor.SpeedCtr(-20, -20);


            while (true) {
                if (carportA.getInfraredState(&ventral, &rearSide)) {
                    if (ventral) {
                        break;
                    }
                }
            }

            while (true) {
                if (carportA.getInfraredState(&ventral, &rearSide)) {
                    if (!ventral || rearSide) {
                        break;
                    }
                }
            }

            DCMotor.Stop();

            carportA.moveToLevel(3);*/

            break;
        case 0x04:

            renderToScreen = !renderToScreen;
            k230.setRenderToScreen(renderToScreen);

            break;
    }
}

/*
语音识别
*/
/*
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
*/




/*
//从车任务处理函数
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
