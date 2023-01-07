#include<stc15.h>
#include<intrins.h>

sbit L1 = P0^0;
sbit L8 = P0^7;

void init_138(unsigned char value);
void cls();
void working();
void Delay100ms();
void init_it0();
void service_it0();

void main()
{
	init_it0();
	cls();
	while(1)
	{
		working();
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
void Delay100ms()		//@11.0592MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 5;
	j = 52;
	k = 195;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}
void working()
{
	init_138(4);
	L1 = 0;
	Delay100ms();
	L1 = 1;
	Delay100ms();
}
void init_it0()
{
	EA = 1;
	IT0 = 1;
	EX0 = 1;
}
void service_it0() interrupt 0
{
	unsigned char i;
	init_138(4);
	L1 = 1;
	L8 = 0;
	for(i = 0; i < 10; i++)
	{
		Delay100ms();
	}
	L8 = 1;
}