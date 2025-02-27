import binascii
import gc

import lcd
import sensor
import time
from Maix import GPIO
from fpioa_manager import fm
from machine import PWM, UART, Timer

def init_camera_high_res():
    '''
    初始化感光芯片为高清分辨率
    '''

    # 定义全局变量
    lcd.init(freq=15000000)  # 初始化LCD0
    sensor.reset()  # 复位和初始化摄像头
    sensor.set_vflip(1)  # 将摄像头设置成后置方式（所见即所得）
    sensor.set_pixformat(sensor.RGB565)  # 设置像素格式为彩色 RGB565
    sensor.set_framesize(sensor.QVGA)  # 设置帧大小为 VGA (640x480)
    # 如果您的摄像头支持更大的分辨率，请选择适合的分辨率设置
    # sensor.set_windowing((IMG_WIDTH, IMG_HEIGHT))  # 如果需要窗口化，请启用这行代码
    # sensor.set_windowing((0, 0, 640, 480))
    # 设置窗口化以获取图像的右半部分
    # sensor.set_windowing((IMG_WIDTH, 0, IMG_WIDTH, IMG_HEIGHT))
    # sensor.set_auto_gain(False)             # 关闭自动增益
    # sensor.set_auto_whitebal(False) # 关闭自动白平衡，并设置白平衡的RGB增益
    sensor.skip_frames(time=2000)  # 等待设置生效
    # clock变量在这里被创建，但除非在后面的代码中使用它来测量FPS，否则不是必需的
    clock = time.clock()

    # 现在IMG_WIDTH和IMG_HEIGHT可以在其他地方使用了

    print("Camera initialized")


init_camera_high_res()

while True:
    gc.collect()
    img = sensor.snapshot().lens_corr(strength=1.5, zoom=1.0)
    lcd.display(img)