import sensor, image, time
from pyb import UART

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time = 2000)
sensor.set_auto_gain(False)  # must be turned off for color tracking
sensor.set_auto_whitebal(False)  # must be turned off for color tracking

clock = time.clock()

# 设置颜色阈值，注意这是LAB颜色空间中的一组阈值。
# 你可能需要使用OpenMV IDE的阈值编辑器工具来更新这些值。
threshold = [((12, 91, -24, 3, -23, 29))] # 示例阈值

while(True):
    clock.tick()
    img = sensor.snapshot().lens_corr(strength = 1.6, zoom = 1.0)

    # 寻找颜色阈值内的色块。
    # 注意：不同的颜色跟踪任务可能需要调整阈值。
    for blob in img.find_blobs(threshold, pixels_threshold=200, area_threshold=200, merge=True):
        # 这些参数保证了色块的最小有效大小，避免了噪点的干扰。

        # 绘制矩形和中心的十字标记。
        img.draw_rectangle(blob.rect(), color=(255, 0, 0)) # 绘制红色矩形标记色块。
        img.draw_cross(blob.cx(), blob.cy(), color=(255, 0, 0)) # 绘制中心十字。

        # 你也可以通过 blob.code() 来获取色块的yuyv色阈值。

        # 打印色块的信息。
        print("Found Blob:", blob.code(), blob.x(), blob.y(), blob.w(), blob.h())

    time.sleep(50) # 休眠一小段时间等待下一帧。
