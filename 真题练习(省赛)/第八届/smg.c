#include<stc15.h>
#include<absacc.h>
#include<intrins.h>
#include<smg.h>


void smg_show_bit(unsigned pos, unsigned char dat)//数码管显示一位
{
	XBYTE[0xe000] = 0xff;
	XBYTE[0xa000] = 0x01 << pos;
	XBYTE[0xe000] = dat;
	Delay1ms();
}
void smg_show_all(unsigned char dat)//数码管全显示
{
	XBYTE[0xe000] = 0xff;
	XBYTE[0xa000] = 0xff;
	XBYTE[0xe000] = dat;
}
void smg_show()//数码管动态显示
{
	unsigned char i;
	for(i = 0; i < 8; i++)
	{
		smg_show_bit(i, dspbuf[i]);
	}
	smg_show_all(0xff);
}
void Delay1ms()//@11.0592MHz,数码管动态扫描延时
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