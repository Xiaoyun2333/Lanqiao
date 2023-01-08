#include<stc15.h>
#include<intrins.h>

sbit L1 = P0^0;
sbit s7 = P3^0;

unsigned char cs = 0x00;
unsigned char ns = 0x00;
unsigned char pwm = 0;
unsigned char counter_t0 = 0;

void init_138(unsigned char value);
void cls();
void smachine();
void Delay5ms();
void Timer0Init();
void service_t0();

void main()
{
	cls();
	init_138(4);
	P0 = 0xff;
	Timer0Init();
	while(1)
	{
		smachine();
	}
}
void init_138(unsigned char value)
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
	init_138(4);
	P0 = 0xff;
	init_138(5);
	P0 = 0x00;
}
void smachine()
{
	cs = ns;
	switch(cs)
	{
		case 0x00:
			if(s7 == 0){Delay5ms();if(s7 == 0){while(s7 == 0);Delay5ms();ns = 0x01;}}
		break;
		case 0x01:
			if(s7 == 0){Delay5ms();if(s7 == 0){while(s7 == 0);Delay5ms();ns = 0x10;}}
		break;
		case 0x10:
			if(s7 == 0){Delay5ms();if(s7 == 0){while(s7 == 0);Delay5ms();ns = 0x11;}}
		break;
		case 0x11:
			if(s7 == 0){Delay5ms();if(s7 == 0){while(s7 == 0);Delay5ms();ns = 0x00;}}
		break;
		default:
			ns = 0x00;
		break;
	}
	if(cs == 0x00){pwm = 0;}
	else if(cs == 0x01){pwm = 10;}
	else if(cs == 0x10){pwm = 50;}
	else if(cs == 0x11){pwm = 90;}
}
void Delay5ms()		//@11.0592MHz
{
	unsigned char i, j;

	i = 54;
	j = 199;
	do
	{
		while (--j);
	} while (--i);
}
void Timer0Init()		//1毫秒@12.000MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x50;		//设置定时初始值
	TH0 = 0xFB;		//设置定时初始值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0 = 1;
	EA = 1;
	TR0 = 1;		//定时器0开始计时
	
}
void service_t0() interrupt 1
{
	counter_t0++;
	if(counter_t0 >= pwm)
	{
		L1 = 1;
	}
	else if(counter_t0 < pwm)
	{
		L1 = 0;
	}
	else if(counter_t0 < 100)
	{
		L1 = 1;
	}
	else if(counter_t0 == 100)
	{
		L1 = 0;
		counter_t0 = 0;
	}
}
