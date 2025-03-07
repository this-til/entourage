import gc

import lcd
import sensor
from Maix import GPIO
from fpioa_manager import fm
from machine import PWM, UART, Timer

isDebug = True

# region 配置
DATA_FRAME_HEADER = 0x55
DATA_FRAME_TAIL = 0xBB

NATIVE_IDENTIFIER = 0x02

NONE = 0x00
WHITE = 0x01
BLACK = 0x02
RED = 0x03
GREEN = 0x04
YELLOW = 0x05

COLOR_TO_STRING_MAP = {
    NONE: "NONE",
    WHITE: "WHITE",
    BLACK: "BLACK",
    RED: "RED",
    GREEN: "GREEN",
    YELLOW: "YELLOW"
}
COLOR_TO_RBG_MAP = {
    NONE: (0, 0, 0),
    WHITE: (255, 255, 255),
    BLACK: (0, 0, 0),
    RED: (255, 0, 0),
    GREEN: (0, 255, 0),
    YELLOW: (255, 255, 0),
}


# endregion

# region Util


def colorToString(color):
    return COLOR_TO_STRING_MAP[color]
    pass


def colorToRbg(color):
    return COLOR_TO_RBG_MAP[color]


def max(array):
    _max = float('-inf')
    for i in array:
        _max = _max if _max < i else i
    return _max


def min(array):
    _min = float('inf')
    for i in array:
        _min = _min if _min > i else i
    return _min


# endregion


# region INIT
def init():
    initGpio()
    initUart()
    initTime()
    initPwm()
    initCameraHighRes()
    # initColorExtraction()
    initTrack()
    pass


# endregion


# region GPIO
LED_B = None


def initGpio():
    fm.register(12, fm.fpioa.GPIO0)
    global LED_B
    LED_B = GPIO(GPIO.GPIO0, GPIO.OUT)
    pass


# endregion


# region TIME
uartTime = None
sendTime = None
pwmTime = None


# trackTime = None


def initTime():
    global uartTime
    global pwmTime
    global trackTime
    # uartTime = Timer(Timer.TIMER1, Timer.CHANNEL1, mode=Timer.MODE_PERIODIC, period=50, callback=uartReadBack)
    # uartTime.start()
    pwmTime = Timer(Timer.TIMER0, Timer.CHANNEL0, mode=Timer.MODE_PWM)
    # trackTime = Timer(Timer.TIMER2, Timer.CHANNEL2, mode=Timer.MODE_PERIODIC, period=100, callback=sendTrack)
    # trackTime.start()
    # sendTime = Timer(Timer.TIMER3, Timer.CHANNEL3, mode=Timer.MODE_PERIODIC, period=200, callback=planSend)
    pass


# endregion


# region PWM
steeringEngine = None


def initPwm():
    global steeringEngine
    steeringEngine = PWM(pwmTime, freq=50, duty=0, pin=17)
    pass


# endregion


# region UART
uart = None

_uart_buffer = b''


def initUart():
    fm.register(6, fm.fpioa.UART1_RX, force=True)
    fm.register(7, fm.fpioa.UART1_TX, force=True)
    global uart
    uart = UART(UART.UART1, 115200, read_buf_len=4096)
    pass


def uartReadBack(e):
    buf = uart.read(64)

    if not buf:
        return

    global _uart_buffer
    _uart_buffer += buf

    processed_idx = 0

    # 循环处理所有可能帧
    while len(_uart_buffer) >= 8:
        # 查找帧头
        head_idx = -1  # _uart_buffer.find(b'\x55', processed_idx)
        for i in range(processed_idx, len(_uart_buffer)):
            if _uart_buffer[i] == DATA_FRAME_HEADER:
                head_idx = i

        if head_idx == -1 or (len(_uart_buffer) - head_idx < 8):
            break

        # 验证帧尾和长度
        if _uart_buffer[head_idx + 7] == DATA_FRAME_TAIL:
            frame_data = _uart_buffer[head_idx: head_idx + 8]
            if isDebug:
                log("a full frame is received：" + str(frame_data))
            handleInstruction(frame_data)
            processed_idx = head_idx + 8
        else:
            processed_idx = head_idx + 1  # 继续向后查找

    # 保留未处理数据
    residualLength = len(_uart_buffer) - processed_idx
    save_idx = processed_idx if residualLength > 4096 else len(_uart_buffer) - 4096
    _uart_buffer = _uart_buffer[save_idx:]
    pass


def handleInstruction(buf):
    if buf[1] != NATIVE_IDENTIFIER:
        return
    if buf[len(buf) - 2] != judgment(buf):
        if isDebug:
            log("judgment fail, s:" + str(buf[len(buf) - 2]) + " t:" + str(judgment(buf)))
        return
    if buf[2] == 0x91:
        if buf[3] == 0x01:
            if buf[4] == 0x01:
                setTrack(True)
            if buf[4] == 0x02:
                setTrack(False)
        if buf[3] == 0x02:
            byte = buf[4]
            angle = (byte ^ 0x80) - 0x80
            angle = angle if angle < -80 else angle if angle > 20 else angle
            setCameraSteeringGearAngle(angle)
    if buf[2] == 0x92:
        if buf[3] == 0x01:
            qrRecognize(primitiveImg, None)
        if buf[3] == 0x03:
            trafficLightRecognize(primitiveImg, None)
        if buf[3] == 0xFE:
            pinged()
        pass


def send(buf):
    buf[len(buf) - 2] = judgment(buf)
    byteBuf = bytes(buf)
    if isDebug:
        log("send:" + str(byteBuf))
    uart.write(byteBuf)
    pass


def judgment(buf):
    f = 0
    for i in range(2, len(buf) - 2):
        f += buf[i]

    return f % 256


# endregion


# region CAMERA
def initCameraHighRes():
    '''
    初始化感光芯片为高清分辨率
    '''

    # 定义全局变量
    lcd.init(freq=15000000)  # 初始化LCD0
    sensor.reset()  # 复位和初始化摄像头
    sensor.set_vflip(1)  # 将摄像头设置成后置方式（所见即所得）
    sensor.set_pixformat(sensor.RGB565)  # 设置像素格式为彩色 RGB565
    sensor.set_framesize(sensor.QVGA)  # 设置帧大小为 VGA (640x480)
    sensor.skip_frames(time=200)  # 等待设置生效
    print("Camera initialized")


# 灰度摄像机
def grayscaleCamera():
    lcd.init(freq=15000000)
    sensor.reset()
    sensor.set_vflip(1)
    sensor.set_pixformat(sensor.GRAYSCALE)
    sensor.set_framesize(sensor.QVGA)
    # sensor.set_auto_gain(False)
    # sensor.set_auto_whitebal(False, gain_db=16)
    sensor.skip_frames(time=200)
    print("Camera initialized")


def initRgb():
    '''
    初始化感光芯片
    '''
    lcd.init(freq=15000000)  # 初始化LCD
    sensor.reset()  # 复位和初始化摄像头
    sensor.set_vflip(1)  # 将摄像头设置成后置方式（所见即所得）
    sensor.set_pixformat(sensor.RGB565)  # 设置像素格式为彩色 RGB565
    sensor.set_framesize(sensor.QVGA)  # 设置帧大小为 QVGA (320x240)
    sensor.set_auto_gain(False)
    sensor.set_auto_whitebal(False)
    # sensor.set_auto_gain(0,0)
    sensor.skip_frames(time=200)  # 等待设置生效
    print("Camera initialized")


# endregion


# region 舵机控制
def setCameraSteeringGearAngle(angle):
    '''
    设置摄像头舵机角度
    @param angle 角度 舵机角度，范围-80至+40，0度垂直于车身
    '''
    if isDebug:
        log("setCameraSteeringGearAngle:" + str(angle))
    steeringEngine.duty((angle + 90) / 180 * 10 + 2.5)
    pass


# endregion


# region 寻迹
openTrack = False

## ROI区域权重值
# ROIS_WEIGHT = [1, 1, 1, 1]
#
# DISTORTION_FACTOR = 1  # 设定畸变系数
# IMG_WIDTH = 240
# IMG_HEIGHT = 320

trackFlagBit = 0

# trackFlagBitHigh = 0
# trackFlagBitLow = 0

# 高位区域数组，位于图像上侧
# HIGH_RECTANGLES = [(260, 5), (260, 35), (260, 65), (260, 95), (260, 125), (260, 155), (260, 185), (260, 215)]
# 低位区域数组，位于图像下侧
# LOW_RECTANGLES_ROTATIONAL_TIME = [(160, 5), (160, 35), (160, 65), (160, 95), (160, 125), (160, 155), (160, 185), (160, 215)]
# LOW_RECTANGLES = [(60, 5), (60, 35), (60, 65), (60, 95), (60, 125), (60, 155), (60, 185), (60, 215)]

RECTANGLES = []

# RECTANGLES = HIGH_RECTANGLES + LOW_RECTANGLES
# RECTANGLES_ROTATIONAL_TIME = HIGH_RECTANGLES + LOW_RECTANGLES_ROTATIONAL_TIME

LINE_COLOR_THRESHOLD = [(0, 45)]


def initTrack():
    # 224
    # 16

    for i in range(0, 16):
        y = 8 + i * 14
        RECTANGLES.append((160, y))
        pass

    pass


def track(img, outImg=None, sendRecognize=False):
    # global trackFlagBitHigh, trackFlagBitLow
    # trackFlagBitHigh = 0
    # trackFlagBitLow = 0

    global trackFlagBit
    trackFlagBit = 0

    for i, (x, y) in enumerate(RECTANGLES):
        w, h = 20, 20
        blobs = img.find_blobs(LINE_COLOR_THRESHOLD, roi=(x, y, w, h))
        for blob in blobs:
            if blob.area() > 100:
                if outImg is not None:
                    outImg.draw_rectangle(x, y, 20, 20, color=(0, 255, 0), thickness=2, fill=True)
                    outImg.draw_string(x, y, str(i))

                trackFlagBit |= 1 << (16 - i)

                break
        pass

    # 高位判断
    # for i, (x, y) in enumerate(HIGH_RECTANGLES):
    #    w, h = 20, 20
    #    blobs = img.find_blobs(LINE_COLOR_THRESHOLD, roi=(x, y, w, h))
    #    # 检查是否有任意一个色块的阈值大于 100。高位判断
    #    for blob in blobs:
    #        if blob.area() > 100:
    #            if outImg is not None:
    #                outImg.draw_rectangle(x, y, 20, 20, color=(0, 255, 0), thickness=2, fill=True)
    #                outImg.draw_string(x, y, str(i))
    #            _rackFlagBit_high |= 1 << (7 - i)
    #            break
    #            # why 为啥不是  _rackFlagBit |= 1 << i
    #            # no why
    #            # 请渡学长的遗物
    #            # bools = [False] * 8
    #            # .....
    #            # bools[i] = True
    #            # .....
    #            # binary_str = ''.join(['1' if b else '0' for b in bools])
    #            # hex_value = int(binary_str, 2)
    ## 低位判断
    # for i, (x, y) in enumerate(LOW_RECTANGLES):
    #    w, h = 20, 20
    #    # 低位检测使用不同的ROI，向下偏移一定距离
    #    roi_low = (x, y, w, h)
    #    blobs = img.find_blobs(LINE_COLOR_THRESHOLD, roi=roi_low)
    #    for blob in blobs:
    #        if blob.area() > 100:
    #            if outImg is not None:
    #                # 为了与绘制的矩形保持一致，这里也可以选择绘制在 roi_low 位置
    #                outImg.draw_rectangle(x, y, 20, 20, color=(0, 255, 0), thickness=2, fill=True)
    #                outImg.draw_string(x - 200, y, str(i))
    #            _rackFlagBit_low |= 1 << (7 - i)
    #            break

    if sendRecognize:
        sendTrack(None)

    pass


def sendTrack(e):
    send([
        DATA_FRAME_HEADER,
        NATIVE_IDENTIFIER,
        0x91,
        0x01,
        (trackFlagBit >> 8) & 0xFF,
        trackFlagBit & 0xFF,
        0x00,
        DATA_FRAME_TAIL
    ])


def setTrack(open):
    global openTrack
    if isDebug:
        log("setTrack:" + str(open))
    openTrack = open
    pass


# endregion


# region 取色
# class ColorBlock:
#    roi = (0, 0, 0, 0)
#
#    cx = 0
#    cy = 0
#    w = 0
#    blob_flag = False
#
#    def __new__(cls, rest):
#        cls.roi = rest
#        return super().__new__(cls)
#
#
# left = ColorBlock((0, 0, 180, 50))  # 纵向取样-左侧
# right = ColorBlock((0, 190, 180, 50))  # 纵向取样-右侧
# up = ColorBlock((240, 0, 80, 240))  # 横向取样-上方
# middle_up = ColorBlock((160, 0, 80, 240))  # 横向取样-中上
# middle_down = ColorBlock((80, 0, 80, 240))  # 横向取样-中下
# down = ColorBlock((0, 0, 80, 240))  # 横向取样-下方
# car = ColorBlock((0, 50, 120, 140))
# middle = ColorBlock((80, 80, 80, 80))
#
# colorBlockList = {
#    left,
#    right,
#    up,
#    middle_up,
#    middle_down,
#    down,
#    car,
#    middle
# }
#
# LINE_COLOR_THRESHOLD = [(0, 31, -18, 23, -23, 27)]
#
#
# def initColorExtraction():
#    pass
#
#
# def colorExtractionLoop(img, outImg=None):
#    '''
#    说明：在ROIS中寻找色块，获取ROI中色块的中心区域与是否有色块的信息
#    '''
#
#    for colorBlock in colorBlockList:
#        blobs = img.find_blobs(LINE_COLOR_THRESHOLD, roi=colorBlock.roi, merge=True)
#        if len(blobs) == 0:
#            continue
#        largest_blob = max(blobs, key=lambda b: b.pixels())
#        if largest_blob.area() < 1000:
#            continue
#        colorBlock.cx = largest_blob.cy()  # 传（学长）说，此处获取到的值的反的，所以反正赋值也是反的~反正它能跑
#        colorBlock.cy = largest_blob.cx()
#        colorBlock.w = largest_blob.h()
#        colorBlock.blob_flag = True
#        if outImg is not None:
#            x, y, width, height = largest_blob[:4]
#            outImg.draw_rectangle((x, y, width, height), color=(128))
#
#    pass


# endregion


# region 二维码
def qrRecognize(img, outImg=None, sendRecognize=True):
    global openTrack
    openTrack = False

    if isDebug:
        log("qrRecognize...")
        pass

    qrcodes = img.find_qrcodes()
    qrcodesLen = len(qrcodes)

    if isDebug:
        log("count:" + str(qrcodesLen))
        pass

    if sendRecognize:
        buf = [
            DATA_FRAME_HEADER,
            NATIVE_IDENTIFIER,
            0x92,
            0x01,
            0x01,  # 同步数量
            qrcodesLen,
            0x00,
            DATA_FRAME_TAIL
        ]
        # noinspection PyTypeChecker
        send(buf)

    for i in range(qrcodesLen):
        qr = qrcodes[i]

        qrImg = img.copy(qr.rect())
        colorId = identifyQrCodeColor(qrImg)

        if outImg is not None:
            color = (255, 255, 255)
            if colorId == RED:
                color = (255, 0, 0)
            if colorId == GREEN:
                color = (0, 255, 0)
            if colorId == YELLOW:
                color = (255, 255, 0)
            img.draw_rectangle(qr.rect())
            x, y, w, h = qr.rect()
            img.draw_string(
                x,
                y,
                qr.payload(),
                color=color,
                scale=2
            )
            pass

        if isDebug:
            log("message:" + qr.payload())
            pass

        if sendRecognize:
            buf = ([
                       DATA_FRAME_HEADER,
                       NATIVE_IDENTIFIER,
                       0x92,
                       0x01,
                       0x02,  # 同步数据
                       i,
                       colorId,
                       len(qr.payload().encode())
                   ]
                   + list(qr.payload().encode())
                   + [
                       0x00,
                       DATA_FRAME_TAIL
                   ])

            send(buf)


def identifyQrCodeColor(img):
    red = [
        (49, 0, 5, 37, -14, 30)
    ]
    green = [
        (14, 35, -76, -6, -86, 24),
        (7, 48, -64, -6, -75, 29),
        (21, 58, -39, -7, -67, 35),
        (19, 58, -72, -12, -44, 52)
    ]
    # yellow = []

    R = len(img.find_blobs(red, x_stride=10, y_stride=10, area_threshold=20, pixels_threshold=5))
    G = len(img.find_blobs(green, x_stride=10, y_stride=10, area_threshold=20, pixels_threshold=5))
    # Y = 0

    if R > G:
        return RED  # 红色
    elif G > R:
        return GREEN  # 绿色
    else:
        return YELLOW  # 黄色
    pass


# endregion


# region 红绿灯
TL_RED_RADIUS = [
    (37, 66, 45, 90, -17, 49)
]
TL_GREEN_RADIUS = [
    (53, 90, -78, -18, -18, 42)
]
TL_YELLOW_RADIUS = [
    (77, 100, -40, 127, 26, 127)
]


def trafficLightRecognize(img, outImg=None, sendRecognize=True):
    global openTrack
    openTrack = False

    if isDebug:
        log("trafficLightRecognize...")
        pass

    rBlobs = img.find_blobs(TL_RED_RADIUS, x_stride=20, y_stride=20, area_threshold=20, pixels_threshold=5)
    gBlobs = img.find_blobs(TL_GREEN_RADIUS, x_stride=20, y_stride=20, area_threshold=20, pixels_threshold=5)
    yBlobs = img.find_blobs(TL_YELLOW_RADIUS, x_stride=20, y_stride=20, area_threshold=20, pixels_threshold=5)

    R = 0
    G = 0
    Y = 0

    for blob in rBlobs:
        R += blob.area()
    for blob in gBlobs:
        G += blob.area()
    for blob in yBlobs:
        Y += blob.area()

    if outImg is not None:
        for blob in rBlobs:
            outImg.draw_rectangle(
                blob.rect(),
                color=colorToRbg(RED)
            )
        for blob in gBlobs:
            outImg.draw_rectangle(
                blob.rect(),
                color=colorToRbg(GREEN)
            )
        for blob in yBlobs:
            outImg.draw_rectangle(
                blob.rect(),
                color=colorToRbg(YELLOW)
            )

    color = NONE

    if R > G and R > Y:
        color = RED
    if G > R and G > Y:
        color = GREEN
    if Y > R and Y > G:
        color = YELLOW

    if isDebug:
        log("r:" + str(R) + " g:" + str(G) + "y:" + str(Y) + " recognize:" + colorToString(color))
        pass

    if sendRecognize:
        buf = bytearray([
            DATA_FRAME_HEADER,
            NATIVE_IDENTIFIER,
            0x92,
            0x03,
            color,
            0x00,
            0x00,
            DATA_FRAME_TAIL
        ])

        send(buf)


# endregion

# region PING
def pinged():
    global openTrack
    openTrack = False

    buf = [
        DATA_FRAME_HEADER,
        NATIVE_IDENTIFIER,
        0x92,
        0xFE,
        0x00,
        0x00,
        0x00,
        DATA_FRAME_TAIL
    ]
    send(buf)


# endregion

# region LOG
LOG_MAX_LINES = 13  # 最大显示行数
FONT_SIZE = 16  # 推荐12/16/24
LINE_HEIGHT = FONT_SIZE + 2  # 行高

log_lines = []


def log(msg):
    log_lines.append(msg)
    print(msg)
    # 保持最多 LOG_MAX_LINES 行
    if len(log_lines) > LOG_MAX_LINES:
        log_lines.pop(0)


def drawLogs(img):
    y = 0
    for log_line in log_lines:
        img.draw_string(
            5,
            y,
            log_line
        )
        y += LINE_HEIGHT
        pass


# endregion

# def logicalThread():
#    while True:
#        uartReadBack()
#        if trackSend:
#            sendTrack()
#        time.sleep(0.05)
#        pass


primitiveImg = None

if __name__ == '__main__':
    init()
    log("initEnd")

    setCameraSteeringGearAngle(-55)

    while True:
        try:
            gc.collect()

            img = sensor.snapshot().lens_corr(strength=1.5, zoom=1.0)
            primitiveImg = img.copy()

            uartReadBack(None)

            # colorExtractionLoop(primitiveImg, img)

            # trafficLightRecognize(primitiveImg, img, False)

            if openTrack:
                track(primitiveImg, img, True)

            drawLogs(img)

            lcd.display(img)
        except Exception as err:
            log("unknown err:" + str(err))
