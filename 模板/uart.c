#include<STC15.H>
#include<INTRINS.H>
#include<uart.h>

void uart_init()
{
    SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x01;		//串口1选择定时器2为波特率发生器
	AUXR |= 0x04;		//定时器时钟1T模式
	T2L = 0xC7;			//设置定时初始值
	T2H = 0xFE;			//设置定时初始值
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
    while(*dat != '\0')
        uart_send_byte(*dat++);
}