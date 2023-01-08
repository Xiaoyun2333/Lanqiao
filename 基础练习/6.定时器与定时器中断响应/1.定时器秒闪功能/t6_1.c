#include<stc15.h>
#include<intrins.h>

sbit L1 = P0^0;
sbit L8 = P0^7;

unsigned char count_1 = 0;
unsigned char count_2 = 0;

void init_138(unsigned char value);
void cls();
void init_timer0();
void service_timer0();


void main()
{
	cls();
	init_timer0();
	while(1)
	{
		
	}
}

void init_138(unsigned char value)
{
	switch(value)
	{
		case 4:
			P2 = (P2 & 0x00) | 0x80;
		break;
		case 5:
			P2 = (P2 & 0x00) | 0xa0;
		break;
		case 6:
			P2 = (P2 & 0x00) | 0xc0;
		break;
		case 7:
			P2 = (P2 & 0x00) | 0xe0;
		break;
	}
}
void cls()
{
	init_138(5);
	P0 = 0x00;
	init_138(4);
	P0 = 0xff;
	
}
void init_timer0()
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD = 0x01;		//设置定时器模式
	TL0 = 0xB0;		//设置定时初始值
	TH0 = 0x3C;		//设置定时初始值
	ET0 = 1;
	EA = 1;
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
}
void service_timer0() interrupt 1
{
	TL0 = 0xB0;		//设置定时初始值
	TH0 = 0x3C;		//设置定时初始值
	count_1++;
	if(count_1 == 10){L1 = ~L1;count_1 = 0;count_2++;}
	if(count_2 == 10){L8 = ~L8;count_2 = 0;}
}

