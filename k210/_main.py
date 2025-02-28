'''
 K210 视频循迹示例
'''
import binascii
import gc

import lcd
import sensor
import time
from Maix import GPIO
from fpioa_manager import fm
from machine import PWM, UART, Timer

# Maix.utils.gc_heap_size(250 * 1024)


is_debug = True

QR_num = 0
QR_Flag = False
JTD_Flag = False
# --------------感光芯片配置  START -------------------

DISTORTION_FACTOR = 1  # 设定畸变系数
IMG_WIDTH = 240
IMG_HEIGHT = 320


# 感光芯片初始化函数
def init_sensor():
    lcd.init(freq=15000000)
    sensor.reset()
    sensor.set_vflip(1)
    sensor.set_pixformat(sensor.GRAYSCALE)
    sensor.set_framesize(sensor.QVGA)
    # sensor.set_auto_gain(False)
    # sensor.set_auto_whitebal(False, gain_db=16)
    sensor.skip_frames(time=200)
    clock = time.clock()


def init_rgb():
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
    clock = time.clock()  # 创建一个时钟来追踪 FPS（每秒拍摄帧数）


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


# --------------串口UART部分  START -------------------

# 映射串口引脚
fm.register(6, fm.fpioa.UART1_RX, force=True)
fm.register(7, fm.fpioa.UART1_TX, force=True)

# 初始化串口
uart = UART(UART.UART1, 115200, read_buf_len=4096)


def get_symbol(num):
    '''
    根据数值正负，返回数值对应的符号
    正数： ‘+’， 负数‘-’ 主要为了方便C语言解析待符号的数值。

    '''
    if num >= 0:
        return ord('+')
    else:
        return ord('-')


def data_format_wrapper(down_center, state_crossing, deflection_angle):
    '''
    根据通信协议封装循迹数据
    TODO 重新编写通信协议  与配套C解析代码

    '''
    send_data = [
        0x55,
        0x02,
        0x91,
        down_center,  # 底部色块中心是否在中点附近
        1 if state_crossing else 0,
        get_symbol(deflection_angle),  # 偏航角符号
        abs(int(deflection_angle)),  # 偏航角
        0xbb]
    global is_debug
    if is_debug:
        print(send_data)
    return bytes(send_data)


def data_format_wrappercs(down_center, state_crossing, deflection_angle, state_cak):
    '''
    根据通信协议封装循迹数据
    TODO 重新编写通信协议  与配套C解析代码

    '''
    send_data = [
        0x55,
        0x02,
        0x91,
        down_center,  # 底部色块中心是否在中点附近
        1 if state_crossing else 0,  # 是否检测到路口
        get_symbol(deflection_angle),  # 偏航角符号
        abs(int(deflection_angle)),  # 偏航角
        1 if state_cak else 0,  # 是否检测到卡
        0xbb]
    global is_debug
    if is_debug:
        print(send_data)
    return bytes(send_data)


def UsartSend(str_data):
    '''
    串口发送函数
    '''
    uart.write(str_data)


# --------------串口UART部分 END -------------------


# --------------定时器部分 START -------------------

is_need_send_data = False  # 是否需要发送数据的信号标志


def uart_time_trigger(tim):
    '''
    串口发送数据的定时器，定时器的回调函数
    '''
    global is_need_send_data, JTD_Flag, QR_Flag, QR_num
    if QR_Flag or JTD_Flag:
        QR_num += 1
    if QR_num >= 200:  # 10秒计时
        QR_num = 0
        # QR_Flag = False
        JTD_Flag = False

    is_need_send_data = True


'''
初始化定时器, 每秒执行20次
period ： 周期1000/20=50
callback： 回调函数
'''
tim1 = Timer(Timer.TIMER1, Timer.CHANNEL1, mode=Timer.MODE_PERIODIC, period=50, callback=uart_time_trigger)


# --------------定时器部分 END -------------------


# --------------舵机配置  START -------------------

def Servo(angle):
    '''
    说明：舵机控制函数
    功能：180度舵机：angle:-90至90 表示相应的角度
         360连续旋转度舵机：angle:-90至90 旋转方向和速度值。
        【duty】占空比值：0-100
    '''
    # PWM通过定时器配置，接到IO17引脚
    tim_pwm = Timer(Timer.TIMER0, Timer.CHANNEL0, mode=Timer.MODE_PWM)
    S1 = PWM(tim_pwm, freq=50, duty=0, pin=17)
    S1.duty((angle + 90) / 180 * 10 + 2.5)


Servo(-45)  # 默认向下0度
time.sleep(1)

# --------------舵机配置  END -------------------


# --------------直线检测部分 START -------------------


# 直线灰度图颜色阈值
# LINE_COLOR_THRESHOLD = [[6, 65]]
# LIGHT_LINE_COLOR_THRESHOLD = [(9, 88)]
# LINE_COLOR_THRESHOLD = [(0, 52, -19, 0, -1, 14)]
# LINE_COLOR_THRESHOLD =[(0, 14, -17, 40, -46, 49)]
# (0, 39, -12, 27, -18, 30)
LINE_COLOR_THRESHOLD = [(0, 31, -18, 23, -23, 27)]
# LINE_COLOR_THRESHOLD =[(24, 0, 127, -128, -128, 127)]

# LINE_COLOR_THRESHOLD = [[0,107]]

# 以实际空间坐标为基准，取样窗口
'''
********屏幕ROI区域*******
*  | ------上方----- |
*  左------中上-----右
*  侧------中下-----侧
*  | ------下方----- |
********屏幕ROI区域*******
'''
ROIS = {
    # 'left': (0, 0, 320, 50),            # 纵向取样-左侧
    # 'right': (0, 190, 320, 50),         # 纵向取样-右侧
    'left': (0, 0, 180, 50),  # 纵向取样-左侧
    'right': (0, 190, 180, 50),  # 纵向取样-右侧
    'up': (240, 0, 80, 240),  # 横向取样-上方
    'middle_up': (160, 0, 80, 240),  # 横向取样-中上
    'middle_down': (80, 0, 80, 240),  # 横向取样-中下
    'down': (0, 0, 80, 240),  # 横向取样-下方
    'car': (0, 50, 120, 140),
    'zhongjian': (80, 80, 80, 80)
}


def find_blobs_in_rois(img):
    '''
    说明：在ROIS中寻找色块，获取ROI中色块的中心区域与是否有色块的信息
    '''
    global ROIS
    global is_debug

    roi_blobs_result = {}  # 在各个ROI中寻找色块的结果记录
    for roi_direct in ROIS.keys():
        roi_blobs_result[roi_direct] = {
            'cx': 0,
            'cy': 0,
            'w': 0,
            'blob_flag': False
        }
    for roi_direct, roi in ROIS.items():
        # blobs=canvas.find_blobs(LINE_COLOR_THRESHOLD, roi=roi, merge=True)
        blobs = img.find_blobs(LINE_COLOR_THRESHOLD, roi=roi, merge=True)
        if len(blobs) != 0:
            largest_blob = max(blobs, key=lambda b: b.pixels())
            if largest_blob.area() > 1000:
                x, y, width, height = largest_blob[:4]
                roi_blobs_result[roi_direct]['cx'] = largest_blob.cy()
                roi_blobs_result[roi_direct]['cy'] = largest_blob.cx()
                roi_blobs_result[roi_direct]['w'] = largest_blob.h()
                roi_blobs_result[roi_direct]['blob_flag'] = True
                if is_debug:
                    img.draw_rectangle((x, y, width, height), color=(128))
        else:
            # blobs=canvas.find_blobs(LINE_COLOR_THRESHOLD, roi=roi, merge=True, pixels_area=10)
            continue

    return roi_blobs_result


def state_deflection_angle(roi_blobs_result):
    '''
    说明：偏转状态值返回
    '''
    # ROI区域权重值
    ROIS_WEIGHT = [1, 1, 1, 1]
    # ROIS_WEIGHT = [1, 0, 0, 1]
    state_crossing = False
    state_cak = False
    deflection_angle = 0
    down_center = 0
    center_num = 0

    # if not roi_blobs_result['middle_down']['w']> 40 or roi_blobs_result['middle_up']['w']>20 or roi_blobs_result['down']['w']>60:
    # print("有卡路口")
    # state_crossing = True

    # 偏转值计算，ROI中心区域X值
    centroid_sum = roi_blobs_result['up']['cx'] * ROIS_WEIGHT[0] + roi_blobs_result['middle_up']['cx'] * ROIS_WEIGHT[1] \
                   + roi_blobs_result['middle_down']['cx'] * ROIS_WEIGHT[2] + roi_blobs_result['down']['cx'] * ROIS_WEIGHT[3]
    if roi_blobs_result['up']['blob_flag']:
        center_num += ROIS_WEIGHT[0]
    if roi_blobs_result['middle_up']['blob_flag']:
        center_num += ROIS_WEIGHT[1]
    if roi_blobs_result['middle_down']['blob_flag']:
        center_num += ROIS_WEIGHT[2]
    if roi_blobs_result['down']['blob_flag']:
        center_num += ROIS_WEIGHT[3]
    center_pos = centroid_sum / (ROIS_WEIGHT[0] + ROIS_WEIGHT[1] + ROIS_WEIGHT[2] + ROIS_WEIGHT[3])
    deflection_angle = (IMG_WIDTH / 2) - center_pos

    if roi_blobs_result['left']['cy'] <= ((IMG_HEIGHT / 3)) or roi_blobs_result['right']['cy'] <= ((IMG_HEIGHT / 3)):
        # 当最下方ROI区域的黑线宽度大于120像素（检测到路口）
        if roi_blobs_result['down']['w'] > 120:
            print(roi_blobs_result['down']['w'])
            print("路口")
            state_crossing = True

    # if roi_blobs_result['car']['w'] < 40:
    # print(roi_blobs_result['car']['w'])
    # print("有卡")
    # state_cak = True

    if roi_blobs_result['middle_up']['w'] > 40:
        if roi_blobs_result['left']['w'] > 30 or roi_blobs_result['right']['w'] > 30:
            if roi_blobs_result['zhongjian']['w'] < 50:
                print(roi_blobs_result['zhongjian']['w'])
                print("有卡")
                state_cak = True

    # if roi_blobs_result['up']['w'] > 40 or roi_blobs_result['middle_up']['w'] > 40:
    # if roi_blobs_result['left']['cy'] <= ((IMG_HEIGHT/3)) or roi_blobs_result['right']['cy'] <= ((IMG_HEIGHT/3)):
    # if roi_blobs_result['car']['w'] < 60:
    # print(roi_blobs_result['car']['w'])
    # print("有卡")
    # state_cak = True

    return down_center, state_crossing, deflection_angle, state_cak


def state_deflection_anglea(roi_blobs_result):
    '''
    说明：偏转状态值返回
    '''
    # ROI区域权重值
    ROIS_WEIGHT = [1, 1, 1, 1]
    # ROIS_WEIGHT = [1, 0, 0, 1]
    state_crossinga = False
    deflection_angle = 0
    down_center = 0
    center_num = 0

    # 偏转值计算，ROI中心区域X值
    centroid_sum = roi_blobs_result['up']['cx'] * ROIS_WEIGHT[0] + roi_blobs_result['middle_up']['cx'] * ROIS_WEIGHT[1] \
                   + roi_blobs_result['middle_down']['cx'] * ROIS_WEIGHT[2] + roi_blobs_result['down']['cx'] * ROIS_WEIGHT[3]
    if roi_blobs_result['up']['blob_flag']:
        center_num += ROIS_WEIGHT[0]
    if roi_blobs_result['middle_up']['blob_flag']:
        center_num += ROIS_WEIGHT[1]
    if roi_blobs_result['middle_down']['blob_flag']:
        center_num += ROIS_WEIGHT[2]
    if roi_blobs_result['down']['blob_flag']:
        center_num += ROIS_WEIGHT[3]
    center_pos = centroid_sum / (ROIS_WEIGHT[0] + ROIS_WEIGHT[1] + ROIS_WEIGHT[2] + ROIS_WEIGHT[3])
    deflection_angle = (IMG_WIDTH / 2) - center_pos

    ## 判断两侧ROI区域检测到黑色线
    # if roi_blobs_result['left']['blob_flag']  and roi_blobs_result['right']['blob_flag']:
    ## 判断两侧ROI区域检测到黑色线处于图像下方1/3处
    # if roi_blobs_result['left']['cy'] <= ((IMG_HEIGHT/3))  or  roi_blobs_result['right']['cy'] <= ((IMG_HEIGHT/3)):
    ## 当最下方ROI区域的黑线宽度大于140像素（检测到路口）
    # if roi_blobs_result['down']['w'] > 120:
    # print("路口")
    # state_crossing = True

    # 判断 中间ROI区域检测到黑色线
    if roi_blobs_result['middle_up']['blob_flag'] and roi_blobs_result['middle_down']['blob_flag']:
        if roi_blobs_result['car']['w'] > 0:
            print("中间")
            state_crossinga = True
    return down_center, state_crossinga, deflection_angle


# --------------直线与路口检测部分 END -------------------


def QR_Check():
    """
    检查是否有二维码
    """
    global QR_Flag, uart

    if QR_Flag:
        qr_results = img.find_qrcodes()  # 寻找二维码
        if qr_results:
            send_qr_code_count(len(qr_results))  # 发送识别到的二维码数量
            print(len(qr_results))
            sorted_qr_results = sorted(qr_results, key=lambda qr: (qr.rect()[1], qr.rect()[0]))
            for qr_code in sorted_qr_results:
                handle_qr_code(qr_code)
                sensor.skip_frames(time=1000)
            # tim1.stop()         # 停止定时器
            # QR_Flag = False


def send_qr_code_count(count):
    """
    通过串口发送识别到的二维码数量
    """
    data_packet = bytes([0x55, 0x02, 0x92, 0x00, count, 0x00, 0x00, 0xBB])
    uart.write(data_packet)


def send_qr_code_payload(payload, result):
    """
    通过串口发送二维码信息
    """
    start_byte = 0x55
    command_byte = 0x92
    length_byte = len(payload)
    end_byte = 0xBB

    data_packet = bytes([start_byte, 0x02, command_byte, 0x01, length_byte, result]) + payload.encode() + bytes([end_byte])

    try:
        uart.write(data_packet)
    except Exception as e:
        print("Error sending data:", e)


def handle_qr_code(qr_code):
    """
    处理二维码信息
    """
    qr_x, qr_y, qr_w, qr_h = qr_code.rect()
    roi = (qr_x, qr_y, qr_w, qr_h)

    rect = qr_code.rect()
    payload = qr_code.payload()
    img.draw_rectangle(rect, color=(255, 0, 0), thickness=2)
    img.draw_string(rect[0], rect[1] - 20, payload, color=(0, 128, 0), scale=2)

    print(payload)

    img_roi = img.copy(roi=roi)
    # 处理颜色判断
    result = QR_RWM(img_roi)

    send_qr_code_payload(payload, result)


def QR_RWM(img_roi):
    """
    二维码识别和颜色判断
    """
    red = [(49, 0, 5, 37, -14, 30)]
    green = [(14, 35, -76, -6, -86, 24), (7, 48, -64, -6, -75, 29), (21, 58, -39, -7, -67, 35), (19, 58, -72, -12, -44, 52)]
    yellow = []
    R = 0
    G = 0
    Y = 0

    print("进入颜色判断")
    for b in img_roi.find_blobs(red, x_stride=10, y_stride=10, area_threshold=20, pixels_threshold=5):
        img_roi.draw_rectangle(b[0:4])
        img_roi.draw_cross(b[5], b[6])
        R += 1
    for b in img_roi.find_blobs(green, x_stride=10, y_stride=10, area_threshold=20, pixels_threshold=5):
        img_roi.draw_rectangle(b[0:4])
        img_roi.draw_cross(b[5], b[6])
        G += 1
    for b in img_roi.find_blobs(yellow, x_stride=10, y_stride=10, area_threshold=20, pixels_threshold=5):
        img_roi.draw_rectangle(b[0:4])
        img_roi.draw_cross(b[5], b[6])
        Y += 1
        print("判断")
    if R > G:
        print("红色")
        return 0x55
    elif G > R:
        print("绿色")
        return 0x66
    else:
        print("黄色")
        return 0x77


# --------------二维码颜色识别部分 START -------------------


def QR_ccc_Check():
    global QR_color_Flag
    if QR_color_Flag:
        res = img.find_qrcodes()
        print(len(res))
        # 如果发现了二维码
        if len(res) > 0:
            img1 = sensor.snapshot()
            print("开始")

            for i in range(len(res)):
                # 画框
                img1.draw_rectangle(res[i].rect())
                # 获取二维码坐标
                qr_x, qr_y, qr_w, qr_h = res[i].rect()
                # 在摄像头显示屏中显示二维码数据
                img1.draw_string(2, 2, res[i].payload(), color=(0, 128, 0), scale=2)
                print(res[i].payload())
                print("循环")

                sensor.skip_frames(time=100)
                print(qr_x, qr_y, qr_w, qr_h)
                # 定义感兴趣的区域
                roi = (qr_x, qr_y, qr_w, qr_h)
                # 复制指定区域的图像
                img_roi = img1.copy(roi=roi)
                # 处理颜色判断
                result = QR_RWM(img_roi)
                send_qr_code_count(result)

                sensor.skip_frames(time=1200)

                # 处理二维码数据
                # send_qr_code_payload(res[i].payload())


# def QR_color_Check():
# '''
# 说明：二维码函数
# '''
# global QR_color_Flag
# if QR_color_Flag:
# img1 = img;
# res = img.find_qrcodes()    # 寻找二维码

# print(len(res))
## 二维码大于0个
# if len(res) > 0:
## 转换清晰彩色摄像头
##init_ysrrwm()
## 从新获得图片
# img1 = sensor.snapshot()
# print("开始")


## 循环读取数组中的所有二维码
# for i in range(len(res)):
## 画框
# img1.draw_rectangle(res[i].rect())
## 获取二维码坐标
# qr_x, qr_y, qr_w, qr_h = res[i].rect()
## 在摄像头显示屏中显示二维码数据
# img1.draw_string(2,2, res[i].payload(), color=(0,128,0), scale=2)
# print(res[i].payload())
# print("循环")
# sensor.skip_frames(time = 100)
# print(qr_x, qr_y, qr_w, qr_h)
## 定义感兴趣的区域
# roi = (qr_x, qr_y, qr_w, qr_h)
## 复制指定区域的图像
# img_roi = img1.copy(roi=roi)
## 处理颜色判断
# result = None
# result = QR_RWM(img_roi);

## 串口发送二维码信息
# uart.write(bytes([0x55]))
# uart.write(bytes([0x88]))
# uart.write(bytes([result]))

# uart.write(bytes([0x22]))


# uart.write(bytes([len(res[i].payload())]))
# for qrdata in res[i].payload():
## print(ord(qrdata))
## if(ord(qrdata)>256):
##  uart.write(bytes(40))
## else:
# uart.write(bytes([ord(qrdata)]))
# uart.write(bytes([0xbb]))
# sensor.skip_frames(time = 1200)


# --------------二维码颜色识别部分 END -------------------


# --------------交通灯识别部分 START -------------------

signal_template = bytearray([0x55, 0x02, 0x92, 0x00, 0x00, 0x00, 0x00, 0xBB])
thresholds = (90, 100, -128, 127, -128, 127)


def QR_JTD():
    # 定义颜色阈值
    # colors = {
    # 'red': [(60, 100, 20, 127, -34, 5)23, 8), (52, 71, 21, 106, -28, 27)],
    ##'green': [(80, 100, -32, -7, -17, 1), (85, 100, -60, -3, -34, 8), (85, 100, -128, -7, -10, 27)],
    # 'green': [(80, 100, -32, -7, -17, 70), (85, 100, -60, -3, -34, 70), (85, 100, -128, -7, -10, 70)],
    # 'yellow': [(64, 76, -112, -10, -128, 127), (80, 100, -18, 6, 2, 127), (76, 100, -15, 19, 2, 30),(61, 100, 1, 127, 30, 127),(35, 100, -128, -7, -10, 70)]
    # }
    # red  =  [(60, 100, 20, 127, -34, 5),(44, 100, 20, 56, -23, 8),(52, 71, 21, 106, -28, 27),(54, 71, 30, 66, -3, 20),(93, 75, -25, 36, 30, -5),(43, 59, 58, 127, 127, -6)]
    # green = [(80, 100, -32, -7, -17, 1),(85, 96, -52, -20, -8, 47),(48, 96, -60, -7, -8, 47),(76, 100, -53, -3, -9, 17),(85, 100, -60, -3, -34, 8),(85, 100, -128, -7, -10, 27)]
    # yellow = [(83, 100, -71, 25, -4, 127),(80, 100, -18, 6, 2, 127),(76, 100, -15, 19, 2, 30),(64, 100, -20, 12, 6, 35)]

    # 定义颜色阈值
    colors = {
        'red': [(37, 66, 45, 90, -17, 49)],
        'green': [(53, 90, -78, -18, -18, 42)],
        'yellow': [(77, 100, -40, 127, 26, 127)]
    }

    # 初始化信号发送标志
    global JTD_Flag, JTD_Flag1
    JTD_Flag1 = True
    if JTD_Flag:
        # img = sensor.snapshot().binary([thresholds], invert=False, zero=True)
        if JTD_Flag1:
            signal = identify_traffic_light(colors)

        uart.write(signal)
        print(signal)


def identify_traffic_light(color_thresholds):
    R, G, Y = 0, 0, 0

    # 检测各颜色灯
    for color, thresholds in color_thresholds.items():
        for threshold in thresholds:
            for b in img.find_blobs([threshold], x_stride=20, y_stride=20, area_threshold=20, pixels_threshold=5):
                img.draw_rectangle(b[0:4])
                img.draw_cross(b[5], b[6])
                if color == 'red':
                    R += 1
                elif color == 'green':
                    G += 1
                elif color == 'yellow':
                    Y += 1

    # 根据检测结果发送相应信号
    signal = bytearray(signal_template)
    if R > G:
        print("红色")
        signal[3] = 0x05
    elif G > R:
        print("绿色")
        signal[3] = 0x06
    else:
        print("黄色")
        signal[3] = 0x07

    return signal


# --------------交通灯识别部分 END -------------------


# --------------拍照部分 START -------------------
# 按键的使用
# 定义按键控制类
class Key_Control():  # 定义按键控制类
    cnt = 0  # 按键计数值
    cs = 0  # 按键模式选择标志位
    csmax = 0  # 按键模式上限
    csflag = 0  # 按键模式切换标志位
    cinput = 0  # 按键输入值保存位
    control = 0  # 按键确认及发送控制标志位
    img_cnt = 0  # 照片计数值


key = Key_Control()

# 照片起始序号设置
key.img_cnt = 60


# 获取照片函数
def Get_Image(key):
    # 拍摄一张照片 保存到 SD卡 的 img 目录下 名字为 000 + key.img_cnt.jpg 如 0001.jpg
    if key.img_cnt < 10:
        sensor.snapshot().save("/sd/img/000" + str(key.img_cnt) + ".jpg")
    elif key.img_cnt < 100:
        sensor.snapshot().save("/sd/img/00" + str(key.img_cnt) + ".jpg")
    elif key.img_cnt < 1000:
        sensor.snapshot().save("/sd/img/0" + str(key.img_cnt) + ".jpg")
    elif key.img_cnt < 10000:
        sensor.snapshot().save("/sd/img/" + str(key.img_cnt) + ".jpg")

    key.img_cnt = key.img_cnt + 1  # 计数值自增


def LCD_Show():
    lcd.draw_string(0, 0, "img_cnt: " + str(key.img_cnt), lcd.BLUE, lcd.WHITE)
    lcd.draw_string(0, 15, "Gain   : " + str(sensor.get_gain_db()), lcd.BLUE, lcd.WHITE)
    lcd.draw_string(0, 30, "RGBGain: " + str(sensor.get_rgb_gain_db()), lcd.BLUE, lcd.WHITE)


def check_memory():
    print("Used memory:", gc.mem_alloc())
    print("Free memory:", gc.mem_free())


# ---------------------MAIN-----------------------
last_cx = 0
last_cy = 0
Flag_track = False
Zw_track = False
QR_color_Flag = False
# 将蓝灯引脚IO12配置到GPIO0，K210引脚支持任意配置
fm.register(12, fm.fpioa.GPIO0)
LED_B = GPIO(GPIO.GPIO0, GPIO.OUT)  # 构建LED对象
# 按键KEY用于清屏
fm.register(16, fm.fpioa.GPIO1, force=True)
btn_debug = GPIO(GPIO.GPIO1, GPIO.IN)
x_num = 3

# init_sensor()
# init_rgb()
init_camera_high_res()
numas = 3
while True:

    if (numas == 0):
        print("开始识别")
        tim1.start()
        QR_Flag = True
        # QR_color_Flag = True
        # JTD_Flag = True
        # numas += 1
    elif (numas == 1):
        print("停止识别二维码")
        tim1.stop()  # 停止定时器
        QR_color_Flag = False
        numas += 1

    LED_B.value(0)  # 点亮LED
    # 串口数据接收处理
    data = uart.read(8)
    if data is not None:
        print(data)
        print(len(data))
        print(binascii.hexlify(data).decode('utf_8'))
        if (len(data) >= 8):
            if ((data[1] == 0x02) & (data[7] == 0xBB)):
                # 巡线与控制舵机
                if (data[2] == 0x91):
                    if (data[3] == 0x01):  # 启动识别
                        print("巡线")
                        print("开始识别")
                        Flag_track = True
                        tim1.start()
                    if (data[3] == 0x02):
                        print("巡线")
                        print("停止识别")

                        Flag_track = False
                        tim1.stop()  # 停止定时器
                    if (data[3] == 0x03):  # 舵机调整
                        angle = data[5]
                        # 判断舵机控制方向
                        if data[4] == ord('-'):
                            # 限制舵机角度，防止过大损坏舵机
                            if angle > 80:
                                angle = 80
                            angle = -angle
                        elif data[4] == ord('+'):
                            # 限制舵机角度，防止过大损坏舵机
                            if angle > 35:
                                angle = 35
                            angle = angle
                        Servo(angle)  # 控制舵机
                        time.sleep(1)  # 等待舵机角度更新
                    if (data[3] == 0x04):  # 启动识别
                        print("转弯")
                        print("开始识别")
                        Zw_track = True
                        tim1.start()
                    if (data[3] == 0x05):
                        print("转弯")
                        print("停止识别")
                        Zw_track = False
                        tim1.stop()  # 停止定时器
                # 二维码识别
                if (data[2] == 0x92):
                    # print("识别二维码")
                    # Servo(-15)  # 向下-15度
                    # time.sleep(1)
                    if (data[3] == 0x01):  # 启动识别
                        print("开始识别")
                        # tim1.start()
                        QR_Flag = True
                        # QR_color_Flag = True
                    if (data[3] == 0x02):
                        print("停止识别二维码")
                        # tim1.stop()         # 停止定时器
                        QR_Flag = False
                        # QR_color_Flag = False
                    if (data[3] == 0x03):  # 启动识别
                        print("识别交通灯")
                        tim1.start()
                        JTD_Flag = True
                    if (data[3] == 0x04):
                        print("停止识别交通灯")
                        tim1.stop()
                        JTD_Flag = False
                    if (data[3] == 0x05):  # 切换摄像头
                        init_rgb()
                    if (data[3] == 0x06):
                        init_sensor()
                    if (data[3] == 0x07):
                        init_camera_high_res()
                    if (data[3] == 0x08):
                        Get_Image(key)

    # 拍摄图片
    # check_memory()
    # img = sensor.snapshot()
    img = sensor.snapshot().lens_corr(strength=1.5, zoom=1.0)
    gc.collect()
    # 去除图像畸变
    # img.lens_corr(DISTORTION_FACTOR)
    # check_memory()
    # 二维码识别
    QR_Check()
    # QR_ccc_Check()
    # 交通灯识别
    QR_JTD()
    # 巡线
    if Flag_track:
        roi_blobs_result = find_blobs_in_rois(img)
        down_center, state_crossing, deflection_angle, state_cak = state_deflection_angle(roi_blobs_result)
        if is_need_send_data:
            UsartSend(data_format_wrappercs(down_center, state_crossing, deflection_angle, state_cak))
            # time.sleep_ms(10)
            # is_need_send_data = False
    if Zw_track:
        roi_blobs_result = find_blobs_in_rois(img)
        down_center, state_crossinga, deflection_angle = state_deflection_anglea(roi_blobs_result)
        if is_need_send_data:
            UsartSend(data_format_wrapper(down_center, state_crossinga, deflection_angle))

    ##按键KEY按下。开启或关闭调试，并退出所有任务。
    # if btn_debug.value() == 0:
    # Flag_track = not Flag_track     # 巡线任务退出
    ##QR_Flag = not QR_Flag           # 二维码任务退出
    # JTD_Flag = not JTD_Flag
    # is_debug = not is_debug         # 调试标志位取
    # 在LCD上显示
    lcd.display(img)
    # LCD_Show()
