#include<stc15.h>
#include<intrins.h>

void clsbuzz();
void led1();
void led2();
void relay();
void buzz();
void delay();

void main()
{
	clsbuzz();
	while(1)
	{
		led1();
		relay();
		led2();
		buzz();
	}
}

void clsbuzz()
{
	P2 = (P2 & 0x1f) | 0xaf;
	P0 = 0x00;
	P2 &= 0x1f;
}
void led1()
{
	int i;
	P2 = (P2 & 0x1f) | 0x8f;
	for(i = 0; i < 3; i++)
	{
		P0 = 0x00;
		delay();
		P0 = 0xff;
		delay();
	}
	P2 &= 0x1f;
}
void relay()
{
	P2 = (P2 & 0x1f) | 0xaf;
	P0 = 0x10;
	delay();
	delay();
	delay();
	delay();
	P0 = 0x00;
	P2 &= 0x1f;
}
void led2()
{
	int i;
	P2 = (P2 & 0x1f) | 0x8f;
	P0 = 0xfe;
	delay();
	for(i = 0; i < 7 ; i++)
	{
		P0 = P0 << 1;
		delay();
	}
	P2 &= 0x1f;
}
void buzz()
{
	P2 = (P2 & 0x1f) | 0xaf;
	P0 = 0x40;
	delay();
	delay();
	delay();
	delay();
	P0 = 0x00;
	P2 = (P2 & 0x1f) | 0x8f;
	P0 = 0xff;
	delay();
	P2 &= 0x1f;
}
void delay()
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
