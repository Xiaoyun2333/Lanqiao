#include<stc15.h>
#include<absacc.h>
#include<intrins.h>
//================================================
code unsigned char tab[] = {0xc0,0xf9,0xa4,0xb0,
														0x99,0x92,0x82,0xf8,
														0x80,0x90};//数码管0~F，"-"为0xbf
unsigned char dspbuf[] = {255,255,255,255,255,255,255,255};//数码管显示缓冲
unsigned int count_f = 0;
unsigned int dat_f = 0;
unsigned char count_t = 0;
//================================================
void init_timer();
void service_timer0();
void service_timer1();
//================================================
void set_dspbuf();
void smg_show_bit(unsigned char pos, unsigned char dat);
void smg_show();
void smg_show_all(unsigned char dat);
//================================================
void cls();//关闭led和蜂鸣器
//================================================
void Delay1ms();
//================================================
void main()
{
	cls();
	init_timer();
	while(1)
	{
		smg_show();
	}
}
//================================================
void init_timer()
{
	TH0 = 0XFF;
	TL0 = 0XFF;
	
	TH1 = (65535 - 50000 + 1) / 256;
	TL0 = (65535 - 50000 + 1) % 256;
	
	TMOD = 0X16;
	ET0 = 1;
	ET1 = 1;
	EA = 1;
	
	TR0 = 1;
	TR1 = 1;
}
void service_timer0() interrupt 1
{
	count_f++;
}
void service_timer1() interrupt 3
{
	TH1 = (65535 - 50000 + 1) / 256;
	TL0 = (65535 - 50000 + 1) % 256;
	count_t++;
	if(count_t == 20)
	{
		dat_f = count_f;
		count_f = 0;
		count_t = 0;
	}
}
//================================================
void cls()
{
	XBYTE[0x8000] = 0xff;
	XBYTE[0xa000] = 0x00;
}
//================================================
void set_dspbuf()
{
	dspbuf[0] = 0x8e;
	
	if(dat_f > 9999)
	{
		dspbuf[3] = tab[dat_f / 10000];
	}
	else{dspbuf[3] = 0xff;}
	if(dat_f > 999)
	{
		dspbuf[4] = tab[(dat_f / 1000) % 10];
	}
	else{dspbuf[4] = 0xff;}
	if(dat_f > 99)
	{
		dspbuf[5] = tab[(dat_f / 100) % 10];
	}
	else{dspbuf[5] = 0xff;}
	if(dat_f > 9)
	{
		dspbuf[6] = tab[(dat_f / 10) % 10];
	}
	else{dspbuf[6] = 0xff;}
	dspbuf[7] = tab[dat_f % 10];
}
void smg_show_bit(unsigned char pos, unsigned char dat)
{
	XBYTE[0xe000] = 0xff;
	XBYTE[0xc000] = 0x01 << pos;
	XBYTE[0xe000] = dat;
}
void smg_show()
{
	unsigned char i;
	set_dspbuf();
	for(i = 0; i < 8; i++)
	{
		smg_show_bit(i, dspbuf[i]);
		Delay1ms();
	}
	smg_show_all(0xff);
}
void smg_show_all(unsigned char dat)
{
	XBYTE[0xe000] = 0xff;
	XBYTE[0xc000] = 0xff;
	XBYTE[0xe000] = dat;
}
//================================================
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