#include<stc15.h>
#include<intrins.h>

sbit s5 = P3^2;
sbit s4 = P3^3;

unsigned char min = 0;//分计数
unsigned char sec = 0;//秒计数
unsigned char msec50 = 0;//50毫秒计数
unsigned char cs = 0x00;
unsigned char ns = 0x00;

code unsigned char tab[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf};//0~9、-
unsigned char dspbuf[] = {255,255,255,255,255,255,255,255};//显示缓冲

void init_138(unsigned char value);//138选择通道
void cls();//关闭蜂鸣器、LED
void show_smg();//数码管显示
void Delay1ms();//动态显示
void Delay5ms();//按键防抖动
void set_dspbuf();//设置数码管显示
void smachine_1();//第一段状态机
void smachine_2();//第二段状态机
void smachine_3();//第三段状态机
void init_timer0();//t0初始化
void cls_timer0();//关闭t0
void service_timer0();//t0中断服务函数

void main()
{
	cls();
	while(1)
	{
		smachine_1();
		smachine_2();
		smachine_3();
		show_smg();
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
void show_smg()
{	
	unsigned char i;
	set_dspbuf();
	for(i = 0; i < 8; i++)
	{
		init_138(6);
		P0 = 0x01 << i;
		init_138(7);
		P0 = dspbuf[i];
		Delay1ms();
	}
}
void Delay1ms()		//@11.0592MHz
{
	unsigned char i, j;
	_nop_();
	_nop_();
	_nop_();
	i = 11;
	j = 190;
	do
	{
		while (--j);
	} while (--i);
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
void set_dspbuf()
{
	dspbuf[0] = tab[min / 10];
	dspbuf[1] = tab[min % 10];
	dspbuf[2] = tab[10];
	dspbuf[3] = tab[sec / 10];
	dspbuf[4] = tab[sec % 10];
	dspbuf[5] = tab[10];
	dspbuf[6] = tab[msec50 / 10];
	dspbuf[7] = tab[msec50 % 10];
}
void smachine_1()
{
	if(s4 == 0){Delay5ms();if(s4 == 0){Delay5ms();ns = 0x00;}}
	cs = ns;
}
void smachine_2()
{
	switch(ns)
	{
		case 0x00:
			if(s5 == 0){Delay5ms();if(s5 == 0){Delay5ms();ns = 0x01;}}
		break;
		case 0x01:
			if(s5 == 0){Delay5ms();if(s5 == 0){Delay5ms();ns = 0x10;}}
			break;
		case 0x10:
			if(s5 == 0){Delay5ms();if(s5 == 0){Delay5ms();ns = 0x01;}}
		break;
		default:
			ns = 0;
		break;
	}
}
void smachine_3()
{
	if(cs == 0x00)
	{
		min = 0;
		sec = 0;
		msec50 = 0;
		cls_timer0();
	}
	else if(cs == 0x01)
	{
		init_timer0();
	}
	else if(cs == 0x10)
	{
		cls_timer0();
	}
}
void init_timer0()
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0xB0;		//设置定时初始值
	TH0 = 0x3C;;		//设置定时初始值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	
	ET0 = 1;
	EA = 1;
}
void cls_timer0()
{
	TR0 = 0;
	ET0 = 0;
	EA = 0;
}
void service_timer0() interrupt 1
{
	msec50++;
	if(msec50 == 20)
	{
		sec++;
		msec50 = 0;
		if(sec == 60)
		{
			min++;
			sec = 0;
			if(min == 60)
			{
				min = 0;
			}
		}
	}
}
