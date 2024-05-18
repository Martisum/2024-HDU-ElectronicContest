import sensor, image, time
from pyb import UART

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time = 2000)
sensor.set_auto_gain(False)  # must be turned off for color tracking
sensor.set_auto_whitebal(False)  # must be turned off for color tracking

clock = time.clock()
ball=(91, 100, -9, 10, -5, 14)

uart = UART(3, 115200) #初始化串口

#摄像头图像变量
isBlobFlag=0
img_width=0
img_height=0
img_center_x=0
img_center_y=0
cx=0
cy=0

while(True):
    clock.tick()

    img = sensor.snapshot().lens_corr(strength = 1.6, zoom = 1.0)

    #获取相对应的图像参数
    img_width=img.width()
    img_height=img.height()

    all_blobs=img.find_blobs([ball],pixels_threshold=250, area_threshold=250, merge=False)
#    print(all_blobs)
    if len(all_blobs)!=0:
        isBlobFlag=1
        for blob in all_blobs:
            b_width=blob.w()
            b_Height=blob.h()
            wh_proportion=b_width/b_Height
            if wh_proportion>0.8 and wh_proportion<1.2:
                cx=blob.cx()
                cy=blob.cy()
                img.draw_rectangle(blob.rect())
                img.draw_cross(cx, cy,color=(127,255,212),thickness=1)
                #绘制箭头（颜色为绿色）
                #通过这个程序，可以明显发现坐标原点在图像左上角
                img.draw_arrow(img_width//2,img_height//2,blob.cx(),blob.cy(),color=(127,255,212),thickness=2)
                #print(f"cx:{cx},cy:{cy},b_width:{b_width},b_Height:{b_Height},wh_pro:{wh_proportion}")
                #最后使用的时候一定要删掉\n，这个只是为了输出方便观察！
                #uart_send_dis=f"dist:{b2_dis}"
                uart_send_string=f"[{str(cx)},{str(cy)}]"
                print(uart_send_string)
                #uart.write(uart_send_dis)
                uart.write(uart_send_string)
            del(b_width)
            del(b_Height)
    else:
        ball_spd=0
        isBlobFlag=0

