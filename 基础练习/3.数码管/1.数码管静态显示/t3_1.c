#include<stc15.h>
#include<intrins.h>

code unsigned char tab[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
0x88,0x83,0xc6,0xa1,0x86,0x8e};

void init_ls138(int key);
void cls();
void show_smg();
void delay();

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
	init_ls138(5);
	P0 = 0x00;
	init_ls138(4);
	P0 = 0xff;
}
void show_smg()
{
	unsigned char i,j;
	unsigned char temp1; 
	unsigned char temp2; 
	for(i = 0; i < 8; i++)
	{
		init_ls138(6);
		if(i == 0)
		{
			temp1 = 0x01;
			P0 = temp1;
		}
		else
		{
			temp1 = temp1 << 1;
			P0 = temp1;
		}
		init_ls138(7);
		for(j = 0; j < 10; j++)
		{
			temp2 = j;
			P0 = tab[temp2];
			delay();
		}
	}
	init_ls138(6);
	P0 = 0xff;
	init_ls138(7);
	for(i = 0; i < 16; i++)
	{
		P0 = tab[i];
		delay();
	}
}
void delay()
{
	unsigned char i, j, k;
	_nop_();
	_nop_();
	i = 9;
	j = 104;
	k = 139;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}