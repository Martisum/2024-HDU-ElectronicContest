#include "main.h"
#include "math.h"
#include "geometry.h"
#include "oled.h"
#include "dac.h"

//显存
uint8_t RAM[128][8] = { 0 };
uint8_t wave[128] = { 0 };
uint8_t head = 0;

//全局波形选择变量
uint8_t global_wave_type=0; //0-不显示波形 1-正弦波 2-方波

uint16_t sine_wave_u16[POINTS];
int sine_wave[POINTS]; 
void sin_basedata(void) 
{  
    for (int i = 0; i < POINTS; i++) 
    {  
        double x = ((double)i / (POINTS - 1)) * 2 * M_PI;  // 将索引转换为0到2π之间的值  
        double sin_value = sin(x);  // 计算正弦值  
        sine_wave[i] = (int)((sin_value + 1) * SCALE + OFFSET);  // 缩放和平移正弦值到100~4000范围 
        sine_wave_u16[i] = (uint16_t)sine_wave[i];
    }  
}


uint16_t my_abs(uint16_t m,uint16_t n) {
    if (m >= n)
        return m - n;
    else
        return n - m;
}

int8_t my_limit(int8_t num) {
    if (num > 63)
        num = 63;
    else if (num < 0)
        num = 0;
    return num;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       画点函数
// @param       x               x 轴坐标设置 0-127
// @param       y               y 轴坐标设置 0-63
// @return      void
// Sample usage:                show_point(0, 0);
//在0,0点绘制一个点
//-------------------------------------------------------------------------------------------------------------------
void show_point(uint8_t x, uint8_t y) {
    /*if (x >= 128 || y >= 8 || x < 0 || y < 0) {
     return;
     }*/

    //ty决定了在哪一个位置
    uint8_t ty = y / 8, ly = y % 8;
    //bi为写入的二进制数值，可能需要倒过来
    uint8_t bi = 1 << ly;
    RAM[x][ty] = bi;
    oled_draw_point(x, ty, RAM[x][ty]);
    return;
}

void show_two_point(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
    uint8_t ty1 = y1 / 8, ly1 = y1 % 8;
    uint8_t ty2 = y2 / 8, ly2 = y2 % 8;
    uint8_t bi1 = 0, bi2 = 0;
    if (ty1 != ty2) {
        bi1 = 1 << ly1;
        bi2 = 1 << ly2;
        oled_draw_point(x1, ty1, bi1);
        oled_draw_point(x2, ty2, bi2);
    } else if (ty1 == ty2 && x1 == x2) {
        bi1 = 1 << ly1;
        bi2 = 1 << ly2;
        bi1 = bi1 | bi2;
        oled_draw_point(x1, ty1, bi1);
    }

}

//-------------------------------------------------------------------------------------------------------------------
// @brief       删除点函数
// @param       x               x 轴坐标设置 0-127
// @param       y               y 轴坐标设置 0-63
// @return      void
// Sample usage:                del_point(0, 0);
//在0,0点删除一个点
//-------------------------------------------------------------------------------------------------------------------
void del_point(uint8_t x, uint8_t y) {
    //ty决定了在哪一个位置
    uint8_t ty = y / 8, ly = y % 8;
    //bi为写入的二进制数值，可能需要倒过来
    //删除是bi=0b11111110
    uint8_t bi = 0xfe << ly;
    RAM[x][ty] &= bi;
    oled_draw_point(x, ty, RAM[x][ty]);
    return;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       显示线函数
// @param       x1               x1 轴坐标设置 0-127
// @param       y1               y1 轴坐标设置 0-63
// @param       x2               x2 轴坐标设置 0-127
// @param       y2               y2 轴坐标设置 0-127
// @return      void
// Sample usage:                show_line(0, 0, 5, 5);
//在0,0到5,5点之间绘制一个直线
//-------------------------------------------------------------------------------------------------------------------
void show_line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
    //临时交换变量，实际当前绘制点
    double k, tx, ty;    //斜率，以及点的绘制过程中的理想临时点
    double x01 = x1, x02 = x2, y01 = y1, y02 = y2;
    uint8_t ny = 0, tmp;
    //nx+=1,ny+=1;
    //nx-=1,ny-=1;
    //第一种情况直接绘制点
    if (x1 == x2 && y1 == y2) {
        show_point(x1, y1);
    } else if (y1 == y2) {
        //绘制竖线
        if (x1 < x2) {
            for (int i = x1; i <= x2; i++) {
                show_point(i, y1);
            }
            return;
        } else if (x1 > x2) {
            for (int i = x2; i <= x1; i++) {
                show_point(i, y1);
            }
            return;
        }
    } else if (x1 == x2) {
        //绘制横线
        if (y1 < y2) {
            for (int i = y1; i <= y2; i++) {
                show_point(x1, i);
            }
            return;
        } else if (y1 > y2) {
            for (int i = y2; i <= y1; i++) {
                show_point(x1, i);
            }
            return;
        }

    }
    //一般直线的绘制
    else {
        //如果两个变量的x的值的大小不一样，就必须要进行交换
        //直线的bug出现在如果斜率太小，可能导致y轴被强制推进从而显示不出
        //另一个bug是，当k小于0的时候，x的变化必须得变小，因此需要交换x的位置
        if (x02 < x01) {
            tmp = x01;
            x01 = x02;
            x02 = tmp;

            tmp = y01;
            y01 = y02;
            y02 = tmp;
        }
        //注意这里整数的除法，结果可能会导致k为0，因此需要先复制进入浮点变量
        k = (y01 - y02) / (x01 - x02);
        tx = x01, ty = y01;
        ny = y01;
        if (k > 0) {
            while(ty<y02) {
                //中间的线需要补上去
                tx=tx+1;
                if((uint8_t)(ty+k)>(uint8_t)(ty)) {
                    ty=ty+k;
                    //tx使用最新的,距离就是ny到ty的距离
                    show_line((uint8_t)tx,ny,(uint8_t)tx,(uint8_t)ty);
                    //更新上一个y
                    ny=(uint8_t)ty;
                } else {
                    //因为每一次都在原地踏步，y并没有实际变化，故仍然使用老y
                    ty=ty+k;
                    show_point((uint8_t)tx, (uint8_t)ty);
                }
            }
        } else if (k < 0) {
            while(ty>y02) {
                //中间的线需要补上去
                tx=tx+1;
                if((uint8_t)(ty+k)<(uint8_t)(ty)) {
                    ty=ty+k;
                    //tx使用最新的,距离就是ny到ty的距离
                    show_line((uint8_t)tx,ny,(uint8_t)tx,(uint8_t)ty);
                    //更新上一个y
                    ny=(uint8_t)ty;
                } else {
                    //因为每一次都在原地踏步，y并没有实际变化，故仍然使用老y
                    ty=ty+k;
                    show_point((uint8_t)tx, (uint8_t)ty);
                }
            }
        }
    }
    return;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       删除一个矩形区域的函数
// @param       x               x 轴坐标设置 0-127
// @param       y               y 轴坐标设置 0-63
// @param       height          竖直高度设置（向下）
// @param       wide            水平宽度设置设置（向右）
// @return      void
// Sample usage:                del_rectangle(0, 0, 5, 5);
//以0,0为标记点，往水平右（wide）方向和竖直下（height）方向延申
//-------------------------------------------------------------------------------------------------------------------
void del_rectangle(uint8_t x, uint8_t y, uint8_t height, uint8_t wide) {
    //首先删除不满一个字节的，包括矩形区域的上侧和下侧
    uint8_t ymax = y + height;
    //删除区域的上侧和下侧处在同一个page
    if (y / 8 == ymax / 8) {
        //这种情况只能使用for循环来删除
        for (int i = x; i <= x + wide; i++) {
            for (int j = y; j <= y + height; j++) {
                del_point(i, j);
            }
        }
    } else if (y / 8 != ymax / 8) {
        //如果上侧和下侧处在不同的page，那么中间可能有夹层，也可能没有夹层，这里做一个分类
        for (int i = x; i <= x + wide; i++) {
            //最后那个点不能删除，所以使用小于号
            for (int j = y; j < (y / 8 + 1) * 8; j++) {
                del_point(i, j);
            }
        }

        //直接使用自带库删除一个page
        for (int i = x; i <= x + wide; i++) {
            for (int j = y / 8 + 1; j <= ymax / 8 - 1; j++) {
                oled_draw_point(i, j, 0x00);
            }
        }

        //删除下方的杂物
        for (int i = x; i <= x + wide; i++) {
            for (int j = (ymax / 8) * 8; j <= ymax; j++) {
                del_point(i, j);
            }
        }
    }
    return;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       速度转换函数，调整速度上下限为0~63
// @param       temp               传入转换数值
// @return      void
// Sample usage:                speed_data_convert(322);
//-------------------------------------------------------------------------------------------------------------------
uint8_t speed_data_convert(int16_t temp) {
    uint8_t conv_temp = 0;
    temp *= 100;
    conv_temp = temp / 70;
    return my_limit(conv_temp);
}

//初始化波形数组
void wave_point_init(void) {
    for (uint8_t i = 0; i < 128; i++) {
        wave[i] = 63;
    }
    head = 0;
}

//为波形添加一个温度数值
void wave_point_add(uint8_t num) {
    if (head < 128) {
        wave[head] = num;
        head++;
    } else if (head >= 128) {
        for (uint8_t i = 0; i < 127; i++) {
            wave[i] = wave[i + 1];
        }
        wave[127] = num;
    }
}

//刷新波形
void refresh_wave(uint8_t startline, uint8_t endline) {
    startline = my_limit(startline);
    for (uint8_t i = startline; i < endline; i++) {
        //清空竖行
        for (uint8_t j = 0; j < 8; j++) {
            oled_draw_point(i, j, 0x00);
        }
    }
}
