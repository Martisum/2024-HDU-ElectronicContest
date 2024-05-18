#include "main.h"
#include "math.h"
#include "geometry.h"
#include "oled.h"
#include "dac.h"

//�Դ�
uint8_t RAM[128][8] = { 0 };
uint8_t wave[128] = { 0 };
uint8_t head = 0;
uint8_t rx_buf[128];	

//ȫ�ֲ���ѡ�����
uint8_t global_wave_type=0; //0-����ʾ���� 1-���Ҳ� 2-����
uint8_t global_freq=5;

uint16_t sine_wave_u16[POINTS];
int sine_wave[POINTS]; 
uint8_t sine_oled_wave[POINTS];
void sin_basedata(void) 
{  
    for (int i = 0; i < POINTS; i++) 
    {  
        double x = ((double)i / (POINTS - 1)) * 2 * M_PI;  // ������ת��Ϊ0��2��֮���ֵ  
        double sin_value = sin(x);  // ��������ֵ  
        sine_wave[i] = (int)((sin_value + 1) * SCALE + OFFSET);  // ���ź�ƽ������ֵ��100~4000��Χ 
        sine_wave_u16[i] = (uint16_t)sine_wave[i];
        sine_oled_wave[i]=(int)((sin_value + 1) * OLED_SCALE + OLED_OFFSET);  // ���ź�ƽ������ֵ��0~63��Χ 
    }  
}

uint16_t square_wave_u16[POINTS];
int square_wave[POINTS]; 
uint8_t square_oled_wave[POINTS];
void square_basedata(void) 
{  
    for (int i = 0; i < POINTS; i++) 
    {  
        int8_t square_value = 0;  // ������β�ֵ
        if(i<POINTS/2){
            square_value=-1;
        }else{
            square_value=1;
        }

        square_wave[i] = (int)((square_value + 1) * SCALE + OFFSET);  // ���ź�ƽ������ֵ��100~4000��Χ 
        square_wave_u16[i] = (uint16_t)square_wave[i];
        square_oled_wave[i]=(int)((square_value + 1) * OLED_SCALE + OLED_OFFSET);
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
// @brief       ���㺯��
// @param       x               x ���������� 0-127
// @param       y               y ���������� 0-63
// @return      void
// Sample usage:                show_point(0, 0);
//��0,0�����һ����
//-------------------------------------------------------------------------------------------------------------------
void show_point(uint8_t x, uint8_t y) {
    /*if (x >= 128 || y >= 8 || x < 0 || y < 0) {
     return;
     }*/

    //ty����������һ��λ��
    uint8_t ty = y / 8, ly = y % 8;
    //biΪд��Ķ�������ֵ��������Ҫ������
    uint8_t bi = 1 << ly;
    RAM[x][ty] = bi;
    oled_draw_point(x, ty, RAM[x][ty]);
    return;
}
//-------------------------------------------------------------------------------------------------------------------
// @brief       ���㺯��
// @param       x               x ���������� 0-127
// @param       y               y ���������� 0-63
// @return      void
// Sample usage:                show_point(0, 0);
//��0,0�����һ����
//-------------------------------------------------------------------------------------------------------------------
void my_show_point(uint8_t x, uint8_t y) {
    /*if (x >= 128 || y >= 8 || x < 0 || y < 0) {
     return;
     }*/

    //ty����������һ��λ��
    uint8_t ty = y / 8, ly = y % 8;
    //biΪд��Ķ�������ֵ��������Ҫ������
    uint8_t bi = 1 << ly;
    oled_draw_point(x, ty, bi);
    return;
}
//-------------------------------------------------------------------------------------------------------------------
// @brief       ɾ���㺯��
// @param       x               x ���������� 0-127
// @param       y               y ���������� 0-63
// @return      void
// Sample usage:                del_point(0, 0);
//��0,0��ɾ��һ����
//-------------------------------------------------------------------------------------------------------------------
void del_point(uint8_t x, uint8_t y) {
    //ty����������һ��λ��
    uint8_t ty = y / 8, ly = y % 8;
    //biΪд��Ķ�������ֵ��������Ҫ������
    //ɾ����bi=0b11111110
    uint8_t bi = 0xfe << ly;
    RAM[x][ty] &= bi;
    oled_draw_point(x, ty, RAM[x][ty]);
    return;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       ��ʾ�ߺ���
// @param       x1               x1 ���������� 0-127
// @param       y1               y1 ���������� 0-63
// @param       x2               x2 ���������� 0-127
// @param       y2               y2 ���������� 0-127
// @return      void
// Sample usage:                show_line(0, 0, 5, 5);
//��0,0��5,5��֮�����һ��ֱ��
//-------------------------------------------------------------------------------------------------------------------
void show_line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
    //��ʱ����������ʵ�ʵ�ǰ���Ƶ�
    double k, tx, ty;    //б�ʣ��Լ���Ļ��ƹ����е�������ʱ��
    double x01 = x1, x02 = x2, y01 = y1, y02 = y2;
    uint8_t ny = 0, tmp;
    //nx+=1,ny+=1;
    //nx-=1,ny-=1;
    //��һ�����ֱ�ӻ��Ƶ�
    if (x1 == x2 && y1 == y2) {
        show_point(x1, y1);
    } else if (y1 == y2) {
        //��������
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
        //���ƺ���
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
    //һ��ֱ�ߵĻ���
    else {
        //�������������x��ֵ�Ĵ�С��һ�����ͱ���Ҫ���н���
        //ֱ�ߵ�bug���������б��̫С�����ܵ���y�ᱻǿ���ƽ��Ӷ���ʾ����
        //��һ��bug�ǣ���kС��0��ʱ��x�ı仯����ñ�С�������Ҫ����x��λ��
        if (x02 < x01) {
            tmp = x01;
            x01 = x02;
            x02 = tmp;

            tmp = y01;
            y01 = y02;
            y02 = tmp;
        }
        //ע�����������ĳ�����������ܻᵼ��kΪ0�������Ҫ�ȸ��ƽ��븡�����
        k = (y01 - y02) / (x01 - x02);
        tx = x01, ty = y01;
        ny = y01;
        if (k > 0) {
            while(ty<y02) {
                //�м������Ҫ����ȥ
                tx=tx+1;
                if((uint8_t)(ty+k)>(uint8_t)(ty)) {
                    ty=ty+k;
                    //txʹ�����µ�,�������ny��ty�ľ���
                    show_line((uint8_t)tx,ny,(uint8_t)tx,(uint8_t)ty);
                    //������һ��y
                    ny=(uint8_t)ty;
                } else {
                    //��Ϊÿһ�ζ���ԭ��̤����y��û��ʵ�ʱ仯������Ȼʹ����y
                    ty=ty+k;
                    show_point((uint8_t)tx, (uint8_t)ty);
                }
            }
        } else if (k < 0) {
            while(ty>y02) {
                //�м������Ҫ����ȥ
                tx=tx+1;
                if((uint8_t)(ty+k)<(uint8_t)(ty)) {
                    ty=ty+k;
                    //txʹ�����µ�,�������ny��ty�ľ���
                    show_line((uint8_t)tx,ny,(uint8_t)tx,(uint8_t)ty);
                    //������һ��y
                    ny=(uint8_t)ty;
                } else {
                    //��Ϊÿһ�ζ���ԭ��̤����y��û��ʵ�ʱ仯������Ȼʹ����y
                    ty=ty+k;
                    show_point((uint8_t)tx, (uint8_t)ty);
                }
            }
        }
    }
    return;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       ɾ��һ����������ĺ���
// @param       x               x ���������� 0-127
// @param       y               y ���������� 0-63
// @param       height          ��ֱ�߶����ã����£�
// @param       wide            ˮƽ����������ã����ң�
// @return      void
// Sample usage:                del_rectangle(0, 0, 5, 5);
//��0,0Ϊ��ǵ㣬��ˮƽ�ң�wide���������ֱ�£�height����������
//-------------------------------------------------------------------------------------------------------------------
void del_rectangle(uint8_t x, uint8_t y, uint8_t height, uint8_t wide) {
    //����ɾ������һ���ֽڵģ���������������ϲ���²�
    uint8_t ymax = y + height;
    //ɾ��������ϲ���²ദ��ͬһ��page
    if (y / 8 == ymax / 8) {
        //�������ֻ��ʹ��forѭ����ɾ��
        for (int i = x; i <= x + wide; i++) {
            for (int j = y; j <= y + height; j++) {
                del_point(i, j);
            }
        }
    } else if (y / 8 != ymax / 8) {
        //����ϲ���²ദ�ڲ�ͬ��page����ô�м�����мв㣬Ҳ����û�мв㣬������һ������
        for (int i = x; i <= x + wide; i++) {
            //����Ǹ��㲻��ɾ��������ʹ��С�ں�
            for (int j = y; j < (y / 8 + 1) * 8; j++) {
                del_point(i, j);
            }
        }

        //ֱ��ʹ���Դ���ɾ��һ��page
        for (int i = x; i <= x + wide; i++) {
            for (int j = y / 8 + 1; j <= ymax / 8 - 1; j++) {
                oled_draw_point(i, j, 0x00);
            }
        }

        //ɾ���·�������
        for (int i = x; i <= x + wide; i++) {
            for (int j = (ymax / 8) * 8; j <= ymax; j++) {
                del_point(i, j);
            }
        }
    }
    return;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       �ٶ�ת�������������ٶ�������Ϊ0~63
// @param       temp               ����ת����ֵ
// @return      void
// Sample usage:                speed_data_convert(322);
//-------------------------------------------------------------------------------------------------------------------
uint8_t speed_data_convert(int16_t temp) {
    uint8_t conv_temp = 0;
    temp *= 100;
    conv_temp = temp / 70;
    return my_limit(conv_temp);
}

//��ʼ����������
void wave_point_init(void) {
    for (uint8_t i = 0; i < 128; i++) {
        wave[i] = 63;
    }
    head = 0;
}

//Ϊ�������һ���¶���ֵ
void wave_point_add(uint8_t num) {
    if (head < 128) {
        del_point(head, wave[head]);
        wave[head] = num;
        head++;
    } else if (head >= 128) {
        for (uint8_t i = 0; i < 127; i++) {
            del_point(i, wave[i]);
            wave[i] = wave[i + 1];
        }
        wave[127] = num;
    }
}

//ˢ�²���
void refresh_wave(uint8_t startline, uint8_t endline) {
    startline = my_limit(startline);
    for (uint8_t i = startline; i < endline; i++) {
        //�������
        for (uint8_t j = 0; j < 8; j++) {
            oled_draw_point(i, j, 0x00);
        }
        show_point(i, wave[i]);
    }
}

void draw_wave(void){
    uint8_t wave_index=0;
    for(uint8_t i=0;i<128;i++){
        wave_index++;
        if(wave_index>POINTS) wave_index=0;
        my_show_point(i,wave[i]);
    }
}

uint16_t sine[200]={2047,2111,2175,2239,2303,2367,2430,2493,2556,2618,
                    2679,2740,2800,2859,2918,2976,3033,3089,3143,3197,
                    3250,3301,3351,3400,3448,3494,3539,3582,3624,3664,
                    3703,3740,3775,3808,3840,3870,3899,3925,3950,3972,
                    3993,4012,4029,4044,4057,4068,4077,4084,4089,4092,
                    4093,4092,4089,4084,4077,4068,4057,4044,4029,4012,
                    3993,3972,3950,3925,3899,3870,3840,3808,3775,3740,
                    3703,3664,3624,3582,3539,3494,3448,3400,3351,3301,
                    3250,3197,3143,3089,3033,2976,2918,2859,2800,2740,
                    2679,2618,2556,2493,2430,2367,2303,2239,2175,2111,
                    2047,1982,1918,1854,1790,1726,1663,1600,1537,1475,
                    1414,1353,1293,1234,1175,1117,1060,1004,950,896,843,
                    792,742,693,645,599,554,511,469,429,390,353,318,285,
                    253,223,194,168,143,121,100,81,64,49,36,25,16,9,4,1,
                    0,1,4,9,16,25,36,49,64,81,100,121,143,168,194,223,253,
                    285,318,353,390,429,469,511,554,599,645,693,742,792,843,
                    896,950,1004,1060,1117,1175,1234,1293,1353,1414,1475,
                    1537,1600,1663,1726,1790,1854,1918,1982};
uint16_t square[200]={4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,
               4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,
               4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,
               4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,
               4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,
               4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,
               4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,
               4095,4095,4095,4095,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};