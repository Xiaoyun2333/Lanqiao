#include<stc15.h>
#include<intrins.h>

unsigned char state = 0x11;
unsigned char nstate = 0x11;

sbit s7 = P3^0;
sbit s6 = P3^1;
sbit s5 = P3^2;
sbit s4 = P3^3;

sbit L1 = P0^0;
sbit L2 = P0^1;
sbit L3 = P0^2;
sbit L4 = P0^3;
sbit L5 = P0^4;
sbit L6 = P0^5;

void init_ls138(int key);
void cls();
void Delay10ms();
void init_state();
void set_state();
void show_state();

void main()
{
	cls();
	init_ls138(4);
	P0 = 0xff;
	while(1)
	{
		show_state();
		set_state();
	}
}

void init_ls138(int key)
{
	switch(key)
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
		default:
			P2 &= 0x00; 
		break;
	}
}
void cls()
{
	init_ls138(5);
	P0 = 0x00;
}
void Delay10ms()		//@11.0592MHz
{
	unsigned char i, j;
	i = 108;
	j = 145;
	do
	{
		while (--j);
	} while (--i);
}
void set_state()
{
	state = nstate;
}
void show_state()
{
	switch(state)
	{
		case 0x11:
			L1 = 1;
			L2 = 1;
			if(s7 == 0)
			{
				Delay10ms();
				if(s7 == 0)
				{
					while(s7 == 0);
					nstate = 0x01;
				}
			}
			if(s6 == 0)
			{
				Delay10ms();
				if(s6 == 0)
				{
					while(s6 == 0);
					nstate = 0x10;
				}
			}
			break;
		case 0x01:
			L1 = 0;
			L2 = 1;
			if(s7 == 0)
			{
				Delay10ms();
				if(s7 == 0)
				{
					while(s7 == 0);
					nstate = 0x11;
				}
			}
			if(s5 == 0)
			{
				Delay10ms();
				if(s5 == 0)
				{
					while(s5 == 0);
					nstate = 0x01;
					L3 = ~L3;
				}
			}
			if(s4 == 0)
			{
				Delay10ms();
				if(s4 == 0)
				{
					while(s4 == 0);
					nstate = 0x01;
					L4 = ~L4;
				}
			}
			break;
		case 0x10:
			L1 = 1;
			L2 = 0;
			if(s6 == 0)
			{
				Delay10ms();
				if(s6 == 0)
				{
					while(s6 == 0);
					nstate = 0x11;
				}
			}
			if(s5 == 0)
			{
				Delay10ms();
				if(s5 == 0)
				{
					while(s5 == 0);
					nstate = 0x10;
					L5 = ~L5;
				}
			}
			if(s4 == 0)
			{
				Delay10ms();
				if(s4 == 0)
				{
					while(s4 == 0);
					nstate = 0x10;
					L6 = ~L6;
				}
			}
			break;
		default:
			L1 = 1;
			L2 = 1;
			nstate = 0x11;
			break;
	}
}