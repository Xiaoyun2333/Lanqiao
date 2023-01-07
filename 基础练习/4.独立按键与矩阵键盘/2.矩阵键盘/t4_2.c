#include<stc15.h>
#include<intrins.h>

sbit r0 = P3^0;
sbit r1 = P3^1;
sbit r2 = P3^2;
sbit r3 = P3^3;

sbit c0 = P4^4;
sbit c1 = P4^2;
sbit c2 = P3^5;
sbit c3 = P3^4;

unsigned char key_num = 0;

code unsigned char tab[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e};

void init_138(int key);
void cls();
void Delay5ms();//·À¶¶¶¯ÑÓ³Ù
void dsp_smg(unsigned char value);
void scan_key_matrix();

void main()
{
	cls();
	init_138(6);
	P0 = 0x01;
	init_138(7);
	while(1)
	{
		scan_key_matrix();
		dsp_smg(key_num);
	}
}

void init_138(int key)
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
	}
}
void cls()
{
	init_138(4);
	P0 = 0xff;
	init_138(5);
	P0 = 0x00;
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
void dsp_smg(unsigned char value)
{
	P0 = tab[value];
}
void scan_key_matrix()
{
	r0 = 0;
	r1 = r2 = r3 = 1;
	c0 = c1 = c2 = c3 = 1;
	if(c0 == 0){Delay5ms();if(c0 == 0){while(c0 == 0);Delay5ms();key_num = 0;}}
	if(c1 == 0){Delay5ms();if(c1 == 0){while(c1 == 0);Delay5ms();key_num = 1;}}
	if(c2 == 0){Delay5ms();if(c2 == 0){while(c2 == 0);Delay5ms();key_num = 2;}}
	if(c3 == 0){Delay5ms();if(c3 == 0){while(c3 == 0);Delay5ms();key_num = 3;}}
	
	r1 = 0;
	r0 = r2 = r3 = 1;
	if(c0 == 0){Delay5ms();if(c0 == 0){while(c0 == 0);Delay5ms();key_num = 4;}}
	if(c1 == 0){Delay5ms();if(c1 == 0){while(c1 == 0);Delay5ms();key_num = 5;}}
	if(c2 == 0){Delay5ms();if(c2 == 0){while(c2 == 0);Delay5ms();key_num = 6;}}
	if(c3 == 0){Delay5ms();if(c3 == 0){while(c3 == 0);Delay5ms();key_num = 7;}}
	
	r2 = 0;
	r1 = r0 = r3 = 1;
	if(c0 == 0){Delay5ms();if(c0 == 0){while(c0 == 0);Delay5ms();key_num = 8;}}
	if(c1 == 0){Delay5ms();if(c1 == 0){while(c1 == 0);Delay5ms();key_num = 9;}}
	if(c2 == 0){Delay5ms();if(c2 == 0){while(c2 == 0);Delay5ms();key_num = 10;}}
	if(c3 == 0){Delay5ms();if(c3 == 0){while(c3 == 0);Delay5ms();key_num = 11;}}
	
	r3 = 0;
	r1 = r0 = r2 = 1;
	if(c0 == 0){Delay5ms();if(c0 == 0){while(c0 == 0);Delay5ms();key_num = 12;}}
	if(c1 == 0){Delay5ms();if(c1 == 0){while(c1 == 0);Delay5ms();key_num = 13;}}
	if(c2 == 0){Delay5ms();if(c2 == 0){while(c2 == 0);Delay5ms();key_num = 14;}}
	if(c3 == 0){Delay5ms();if(c3 == 0){while(c3 == 0);Delay5ms();key_num = 15;}}
}