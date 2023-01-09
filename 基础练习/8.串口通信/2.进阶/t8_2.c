#include<stc15.h>
#include<intrins.h>

unsigned char r_sbuf;//接收缓冲

void init_ls138(unsigned char value);//选择138输出
void cls();//关闭LED等设备
void UartInit();//串口初始化
void service_uart();//串口中断服务函数
void sendchar(unsigned char Char);//发送字符
void sendstring(unsigned char *p);//发送字符串
void check_r_sbuf();//检查r_sbuf

//=======================================================//
void main()
{
	cls();
	UartInit();
	sendstring("Welcome to XiaoyunOS!");
	while(1)
	{
		if(RI == 1){check_r_sbuf();}
	}
}
//=======================================================//
void init_ls138(unsigned char value)
{
	switch(value)
	{
		case 4:
			P2 = P2 & 0x1f | 0x80;
		break;
		case 5:
			P2 = P2 & 0x1f | 0xa0;
		break;
		case 6:
			P2 = P2 & 0x1f | 0xc0;
		break;
		case 7:
			P2 = P2 & 0x1f | 0xe0;
		break;
	}
}
void cls()
{
	init_ls138(5);
	P0 = 0x00;
	init_ls138(4);
	P0 = 0xff;
}
void UartInit(void)		//9600bps@12.000MHz
{
	PCON &= 0x7F;		//波特率不倍速
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x40;		//定时器时钟1T模式
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//设置定时器模式
	TMOD |= 0x20;		//设置定时器模式
	TL1 = 0xDC;		//设置定时初始值
	TH1 = 0xDC;		//设置定时重载值
	ET1 = 0;		//禁止定时器%d中断
	
	ES = 1;
	EA = 1;
	
	TR1 = 1;		//定时器1开始计时
}
void service_uart() interrupt 4
{
	if(RI == 1)
	{
		RI = 0;
		r_sbuf = SBUF;
	}
}
void sendchar(unsigned char Char)
{
	SBUF = Char;
	while(!TI);
	TI=0;
}
void sendstring(unsigned char *p)
{
	while(*p)
	{
		sendchar(*p);
		p++;
	}
}
void check_r_sbuf()
{
	if((r_sbuf & 0xf0) == 0xa0){P0 = (P0 & 0xf0) | ((~r_sbuf) & 0x0f);}
	else if((r_sbuf & 0xf0) == 0xb0){P0 = (P0 & 0x0f) | (~r_sbuf << 4);}
	else if((r_sbuf & 0xf0) == 0xc0){sendstring("XiaoyunOS is running");}
}