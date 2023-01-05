#include<stc15.h>
#include<intrins.h>

void clsbuff();
void Delay500ms();
void led();

void main()
{
	clsbuff();
	while(1)
	{
		led();
	}
}

void clsbuff()
{
	P2 = (P2 & 0x1f | 0xaf);
	P0 = (P0 & 0x00);
	P2 &= 0x00;
}

void Delay500ms()		//@11.0592MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 22;
	j = 3;
	k = 227;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

void led()
{
	int i;
	P2 = (P2 & 0x1f | 0x8f);
	for(i = 0;i < 3; i++)
	{
		P0 = 0x00;
		Delay500ms();
		P0 = 0xff;
		Delay500ms();
	}
	P0 = 0xfe;
	Delay500ms();
	for(i = 0;i < 7; i++)
	{
		P0 = (P0<<1);
		Delay500ms();
	}
	
	P0 = 0x01;
	Delay500ms();
	for(i = 0;i < 7; i++)
	{
		P0 = (P0<<1) + 1;
		Delay500ms();
	}
	P2 &= 0x00;
}