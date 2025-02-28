import binascii
import gc

import lcd
import sensor
import time
from Maix import GPIO
from fpioa_manager import fm
from machine import PWM, UART, Timer

isDebug = True

# ---------------------配置-----------------------

DATA_FRAME_HEADER = 0x55
DATA_FRAME_TAIL = 0xBB

NATIVE_IDENTIFIER = 0x02

CAMERA_ANGLE_ID = 0x03

TRACKING_ID = 0x02

QR_SERVICE_ID = 0x03
QR_COUNT_SERVICE_ID = 0x01
QR_MESSAGE_SERVICE_ID = 0x02

TRAFFIC_LIGHT_SERVICE_ID = 0x04

PING_SERVICE_ID = 0xFE

NONE = 0x00
WHITE = 0x01
BLACK = 0x02
RED = 0x03
GREEN = 0x04
YELLOW = 0x05


# ---------------------配置_END-----------------------

# ---------------------INIT-----------------------
def init():
    initGpio()
    initUart()
    initTime()
    initPwm()
    initCameraHighRes()
    initColorExtraction()
    initTrack()
    pass


# ---------------------INIT_END-----------------------


# ---------------------GPIO-----------------------

LED_B = None


def initGpio():
    fm.register(12, fm.fpioa.GPIO0)
    global LED_B
    LED_B = GPIO(GPIO.GPIO0, GPIO.OUT)
    pass


# ---------------------GPIO_END-----------------------


# ---------------------TIME-----------------------

uartTime = None
pwmTime = None
trackTime = None


def initTime():
    global uartTime
    global pwmTime
    global trackTime
    uartTime = Timer(Timer.TIMER1, Timer.CHANNEL1, mode=Timer.MODE_PERIODIC, period=50, callback=uartReadBack)
    uartTime.start()
    pwmTime = Timer(Timer.TIMER0, Timer.CHANNEL0, mode=Timer.MODE_PWM)
    trackTime = Timer(Timer.TIMER2, Timer.CHANNEL2, mode=Timer.MODE_PERIODIC, period=50, callback=trackLoop)
    pass


# ---------------------TIME_END-----------------------

# ---------------------PWM-----------------------
steeringEngine = None


def initPwm():
    global steeringEngine
    steeringEngine = PWM(pwmTime, freq=50, duty=0, pin=17)
    pass


# ---------------------PWM_END-----------------------

# ---------------------UART-----------------------

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
            frame_data = _uart_buffer[head_idx: head_idx + 7]
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
    #if buf[len(buf) - 2] != judgment(buf):
    #    if isDebug:
    #        log("judgment fail, s:" + str(buf[len(buf) - 2]) + " t:" + str(judgment(buf)))
    #    return
    if buf[2] == 0x91:
        if buf[3] == 0x01:
            setTrack(True)
        if buf[3] == 0x02:
            setTrack(False)
        if buf[3] == 0x03:
            byte = buf[4]
            angle = (byte ^ 0x80) - 0x80
            angle = angle if angle < -80 else angle if angle > 30 else angle
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


# ---------------------UART_END-----------------------

# ---------------------CAMERA-----------------------
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
    sensor.skip_frames(time=2000)  # 等待设置生效
    print("Camera initialized")


# ---------------------CAMERA_END-----------------------


# ---------------------舵机控制-----------------------

def setCameraSteeringGearAngle(angle):
    '''
    设置摄像头舵机角度
    @param angle 角度 舵机角度，范围-80至+40，0度垂直于车身
    '''
    if isDebug:
        log("setCameraSteeringGearAngle:" + str(angle))
    steeringEngine.duty((angle + 90) / 180 * 10 + 2.5)
    pass


# ---------------------舵机控制_END-----------------------

# ---------------------寻迹-----------------------

trackFlag = False

# ROI区域权重值
ROIS_WEIGHT = [1, 1, 1, 1]

DISTORTION_FACTOR = 1  # 设定畸变系数
IMG_WIDTH = 240
IMG_HEIGHT = 320


def initTrack():
    pass


def trackLoop(e):
    hasIntersection = False
    hasCard = False
    center_num = 0

    # 偏转值计算，ROI中心区域X值
    centroid_sum = (
            up.cx * ROIS_WEIGHT[0]
            + middle_up.cx * ROIS_WEIGHT[1]
            + middle_down.cx * ROIS_WEIGHT[2]
            + down.cx * ROIS_WEIGHT[3]
    )

    if up.blob_flag:
        center_num += ROIS_WEIGHT[0]
    if middle_up.blob_flag:
        center_num += ROIS_WEIGHT[1]
    if middle_down.blob_flag:
        center_num += ROIS_WEIGHT[2]
    if down.blob_flag:
        center_num += ROIS_WEIGHT[3]
    center_pos = centroid_sum / (ROIS_WEIGHT[0] + ROIS_WEIGHT[1] + ROIS_WEIGHT[2] + ROIS_WEIGHT[3])
    deflection_angle = (IMG_WIDTH / 2) - center_pos

    if left.cy <= (IMG_HEIGHT / 3) or right.cy <= (IMG_HEIGHT / 3):
        # 当最下方ROI区域的黑线宽度大于120像素（检测到路口）
        if down.w > 120:
            hasIntersection = True

    if middle_up.w > 40:
        if left.w > 30 or right.w > 30:
            if middle.w < 50:
                hasCard = True

    # if middle_up.blob_flag and middle_down.blob_flag:
    #    if car.w > 0:
    #        print("中间")
    #        hasIntersection = True

    return hasIntersection, deflection_angle, hasCard
    pass


def setTrack(open):
    global trackFlag
    trackFlag = open
    if trackFlag:
        trackTime.start()
    else:
        trackTime.stop()
    pass


# ---------------------寻迹_END-----------------------

# ---------------------取色-----------------------

class ColorBlock:
    roi = (0, 0, 0, 0)

    cx = 0
    cy = 0
    w = 0
    blob_flag = False

    def __new__(cls, rest):
        cls.roi = rest
        return super().__new__(cls)


left = ColorBlock((0, 0, 180, 50))  # 纵向取样-左侧
right = ColorBlock((0, 190, 180, 50))  # 纵向取样-右侧
up = ColorBlock((240, 0, 80, 240))  # 横向取样-上方
middle_up = ColorBlock((160, 0, 80, 240))  # 横向取样-中上
middle_down = ColorBlock((80, 0, 80, 240))  # 横向取样-中下
down = ColorBlock((0, 0, 80, 240))  # 横向取样-下方
car = ColorBlock((0, 50, 120, 140))
middle = ColorBlock((80, 80, 80, 80))

colorBlockList = {
    left,
    right,
    up,
    middle_up,
    middle_down,
    down,
    car,
    middle
}

LINE_COLOR_THRESHOLD = [(0, 31, -18, 23, -23, 27)]


def initColorExtraction():
    pass


def colorExtractionLoop(img, outImg=None):
    '''
    说明：在ROIS中寻找色块，获取ROI中色块的中心区域与是否有色块的信息
    '''

    for colorBlock in colorBlockList:
        blobs = img.find_blobs(LINE_COLOR_THRESHOLD, roi=colorBlock.roi, merge=True)
        if len(blobs) == 0:
            continue
        largest_blob = max(blobs, key=lambda b: b.pixels())
        if largest_blob.area() < 1000:
            continue
        colorBlock.cx = largest_blob.cy()  # 传（学长）说，此处获取到的值的放的，所以反正赋值——反正它能跑
        colorBlock.cy = largest_blob.cx()
        colorBlock.w = largest_blob.h()
        colorBlock.blob_flag = True
        if outImg is not None:
            x, y, width, height = largest_blob[:4]
            outImg.draw_rectangle((x, y, width, height), color=(128))

    pass


# ---------------------去色_END-----------------------


# ---------------------二维码-----------------------

def qrRecognize(img, outImg=None):
    qrcodes = img.find_qrcodes()
    qrcodesLen = len(qrcodes)

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
               + qr.payload().encode()
               + [
                   0x00,
                   DATA_FRAME_TAIL
               ])

        send(buf)
        pass
    pass


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


# ---------------------二维码_END-----------------------


# ---------------------红绿灯-----------------------


TL_RED_RADIUS = [
    (37, 66, 45, 90, -17, 49)
]
TL_GREEN_RADIUS = [
    (53, 90, -78, -18, -18, 42)
]
TL_YELLOW_RADIUS = [
    (77, 100, -40, 127, 26, 127)
]


def trafficLightRecognize(img, outImg=None):
    R = len(img.find_blobs(TL_RED_RADIUS, x_stride=20, y_stride=20, area_threshold=20, pixels_threshold=5))
    G = len(img.find_blobs(TL_GREEN_RADIUS, x_stride=20, y_stride=20, area_threshold=20, pixels_threshold=5))
    Y = len(img.find_blobs(TL_YELLOW_RADIUS, x_stride=20, y_stride=20, area_threshold=20, pixels_threshold=5))

    color = None

    if R > G:
        color = RED
    elif G > R:
        color = GREEN
    else:
        color = YELLOW

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

    pass


# ---------------------PING-----------------------

def pinged():
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


# ---------------------PING_END-----------------------

# ---------------------红绿灯_END-----------------------


# ---------------------LOG-----------------------

LOG_MAX_LINES = 20  # 最大显示行数
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


# ---------------------LOG_END-----------------------

primitiveImg = None

if __name__ == '__main__':
    init()
    log("initEnd")
    setCameraSteeringGearAngle(-55)

    while True:
        gc.collect()

        img = sensor.snapshot().lens_corr(strength=1.5, zoom=1.0)

        primitiveImg = img.copy()

        colorExtractionLoop(primitiveImg, img)
        drawLogs(img)

        lcd.display(img)
