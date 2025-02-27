import binascii
import gc

import lcd
import sensor
import time
from Maix import GPIO
from fpioa_manager import fm
from machine import PWM, UART, Timer

isDebug = True


# ---------------------INIT-----------------------
def init():
    initGpio()
    initUart()
    initTime()
    initPwm()
    initCameraHighRes()
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


def initTime():
    global uartTime
    global pwmTime
    uartTime = Timer(Timer.TIMER1, Timer.CHANNEL1, mode=Timer.MODE_PERIODIC, period=50, callback=uartReadBack)
    uartTime.start()
    pwmTime = Timer(Timer.TIMER0, Timer.CHANNEL0, mode=Timer.MODE_PWM)
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
    buf = uart.read()

    if not buf:
        return
    global _uart_buffer
    _uart_buffer += buf

    processed_idx = 0

    # 循环处理所有可能帧
    while len(_uart_buffer) >= 8:
        # 查找帧头
        head_idx = _uart_buffer.find(0x55, processed_idx)
        if head_idx == -1 or (len(_uart_buffer) - head_idx < 8):
            break

        # 验证帧尾和长度
        if _uart_buffer[head_idx + 7] == 0xBB:
            frame_data = _uart_buffer[head_idx: head_idx]
            if isDebug:
                log("a full frame is received：" + str(frame_data))
            handleInstruction(frame_data)
            processed_idx = head_idx + 8
        else:
            processed_idx = head_idx + 1  # 继续向后查找

    # 保留未处理数据
    _uart_buffer = _uart_buffer[processed_idx:]
    pass


def handleInstruction(buf):
    if buf[1] != 0xEF:
        return
    if buf[2] != 0x01:
        byte = buf[3]
        angle = (byte & 0x7F) - (byte & 0x80)
        angle = angle if angle < -80 else angle if angle > 30 else angle
        setCameraSteeringGearAngle(angle)
    if buf[2] != 0x02:
        pass


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

# ---------------------LOG-----------------------

LOG_MAX_LINES = 20  # 最大显示行数
FONT_SIZE = 16  # 推荐12/16/24
LINE_HEIGHT = FONT_SIZE + 2  # 行高
TEXT_COLOR = (255, 0, 0)  # RGB红色

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

if __name__ == '__main__':
    init()
    log("initEnd")
    setCameraSteeringGearAngle(-80)
    while True:
        gc.collect()
        img = sensor.snapshot().lens_corr(strength=1.5, zoom=1.0)
        drawLogs(img)
        lcd.display(img)
    pass
