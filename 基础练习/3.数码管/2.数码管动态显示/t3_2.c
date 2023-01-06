#include<stc15.h>
#include<intrins.h>

unsigned char month = 1;

code unsigned char tab[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff};
unsigned char dspbuf[] = {10, 10, 10, 10, 10, 10, 10, 10};

void init_ls138(int key);
void cls();
void Delay1ms();
void Delay();
void set_dspbuf();
void show_smg();

void main()
{
	cls();
	while(1)
	{	
		show_smg();
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
	init_ls138(4);
	P0 = 0xff;
	init_ls138(5);
	P0 = 0x00;
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
void show_smg()
{
	unsigned char i;
	set_dspbuf();
	for(i = 0; i < 8; i++)
	{
		init_ls138(6);
		P0 = 0x01 << i;
		init_ls138(7);
		P0 = dspbuf[i];
		Delay1ms();
		P0 = 0x00;
	}
}
void set_dspbuf()
{
	dspbuf[0] = tab[2];
	dspbuf[1] = tab[0];
	dspbuf[2] = tab[2];
	dspbuf[3] = tab[3];
	dspbuf[4] = 0xbf;
	dspbuf[5] = 0xbf;
	dspbuf[6] = tab[0];
	dspbuf[7] = tab[1];
}


	