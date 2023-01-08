#include<stc15.h>
#include<intrins.h>

unsigned char r_sbuf;

void UartInit();//串口初始化
void service_uart();//串口中断服务
void send(unsigned char t_sbuf);//串口发送

void main()
{
	UartInit();
	
	send(0x5a);
	send(0xa5);
	while(1)
	{
		
	}
}
void UartInit()	//9600bps@11.0592MHz
{
	PCON &= 0x7F;		//波特率不倍速
	SCON = 0x50;		//8位数据,可变波特率
	AUXR &= 0xBF;		//定时器时钟12T模式
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//设置定时器模式
	TMOD |= 0x20;		//设置定时器模式
	TL1 = 0xFD;		//设置定时初始值
	TH1 = 0xFD;		//设置定时重载值
	ET1 = 0;		//禁止定时器%d中断
	TR1 = 1;		//定时器1开始计时
	
	ES = 1;
	EA = 1;
}
void service_uart() interrupt 4
{
	if(RI == 1)
	{
		RI = 0;
		r_sbuf = SBUF;
		send(r_sbuf + 1);
	}
}
void send(unsigned char t_sbuf)
{
	SBUF = t_sbuf;
	while(TI == 0);
	TI = 0;
}