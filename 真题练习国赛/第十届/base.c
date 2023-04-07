#include<STC15.H>
#include<INTRINS.H>
#include<base.h>

code unsigned char tab[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff,0xc6,0xc7,0xc8,0x8c};
code unsigned char tab2[] = {1,2,4,8,16,32,64,128};

void init()
{
    P2 = P2 & 0x1f | 0x80;
    P0 = 0xff;
    P2 &= 0x1f;

    P2 = P2 & 0x1f | 0xa0;
    P0 = 0x00;
    P2 &= 0x1f;
}
unsigned char key_read()
{
    unsigned char temp = 0;
    AUXR &= 0xef;

    P44 = 0; P42 = 1; P35 = 1; P34 = 1;
    if(P33 == 0) temp = 4;
    if(P32 == 0) temp = 5;
    if(P31 == 0) temp = 6;
    if(P30 == 0) temp = 7;

    P44 = 1; P42 = 0; P35 = 1; P34 = 1;
	if(P33 == 0) temp = 8;
	if(P32 == 0) temp = 9;
	if(P31 == 0) temp = 10;
	if(P30 == 0) temp = 11;
	
	P44 = 1; P42 = 1; P35 = 0; P34 = 1;
	if(P33 == 0) temp = 12;
	if(P32 == 0) temp = 13;
	if(P31 == 0) temp = 14;
	if(P30 == 0) temp = 15;
	
	P44 = 1; P42 = 1; P35 = 1; P34 = 0;
	if(P33 == 0) temp = 16;
	if(P32 == 0) temp = 17;
	if(P31 == 0) temp = 18;
	if(P30 == 0) temp = 19;

    P3 = 0xff;
    AUXR |= 0x10;
    return temp; 
}
void smg_show(unsigned char pos,dat,point)
{
    P2 = P2 & 0x1f | 0xe0;
    P0 = 0xff;
    P2 &= 0x1f;

    P2 = P2 & 0x1f | 0xc0;
    P0 = tab2[pos];
    P2 &= 0x1f;

    P2 = P2 & 0x1f | 0xe0;
    P0 = tab[dat];
    if(point) P0 &= 0x7f;
    P2 &= 0x1f;
}
void led_show(unsigned char pos,flag)
{
    static unsigned char temp = 0x00;
    static unsigned char temp_old = 0xff;

    if(flag) temp |= 0x01 << pos;
    else temp &= ~(0x01 << pos);

    if(temp != temp_old)
    {
        P2 = P2 & 0x1f | 0x80;
        P0 = ~temp;
        P2 &= 0x1f; 
        temp_old = temp;
    }
}
void uart_init()
{
    SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x01;		//串口1选择定时器2为波特率发生器
	AUXR |= 0x04;		//定时器时钟1T模式
	T2L = 0x8f;		//设置定时初始值
	T2H = 0xfd;		//设置定时初始值
	AUXR |= 0x10;		//定时器2开始计时

    ES = 1;
    EA = 1;
}
void uart_send_byte(unsigned char dat)
{
    SBUF = dat;
    while(TI == 0);
    TI = 0;
}
void uart_send_string(unsigned char *dat)
{
    while (*dat != '\0')
        uart_send_byte(*dat++);
}