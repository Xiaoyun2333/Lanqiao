#ifndef __SMG_H
#define __SMG_H

void smg_show_bit(unsigned pos, unsigned char dat);//数码管显示一位
void smg_show_all(unsigned char dat);//数码管全显示
void smg_show();//数码管动态显示
void Delay1ms();//@11.0592MHz,数码管动态扫描延时

#endif