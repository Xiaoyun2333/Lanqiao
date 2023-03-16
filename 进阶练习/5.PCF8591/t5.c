#include<stc15.h>
#include<intrins.h>
#include<absacc.h>

#include<iic.h>

unsigned char tab[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff};
unsigned char dspbuf[] = {255, 255, 255, 255, 255, 255, 255, 255};
unsigned char ad1;
unsigned char ad2;


void cls();
unsigned char ad_read(unsigned char addr);
void da_wirte(unsigned char addr, unsigned dat);
void smg_show_bit(unsigned pos, unsigned dat);//数码管显示一位
void smg_show_all(unsigned dat);//数码管全部显示
void smg_show();//数码管动态显示
void buf_set();//设置数码管
void Delay1ms();

void main()
{
    cls();
    while(1)
    {
		ad1 = ad_read(0x43);//41为光敏电阻，43为电压输入
		ad2 = ad_read(0x41);//41为光敏电阻，43为电压输入
        smg_show();
        buf_set();
        da_wirte(0x43,127);
    }
}
void cls()
{
	XBYTE[0x8000] = 0xff;
    XBYTE[0xa000] = 0x00;
}
unsigned char ad_read(unsigned char addr)
{
    unsigned char temp;
    IIC_Start();
    IIC_SendByte(0x90);
    IIC_WaitAck();
    IIC_SendByte(addr);
    IIC_WaitAck();
	
    IIC_Start();
    IIC_SendByte(0x91);
    IIC_WaitAck();
    temp = IIC_RecByte();
    IIC_SendAck(1);
    IIC_Stop();
    return temp;
}

void da_wirte(unsigned char addr,unsigned dat)
{
    IIC_Start();
    IIC_SendByte(0x90);
    IIC_WaitAck();
    IIC_SendByte(addr);
    IIC_WaitAck();
    IIC_SendByte(dat);
    IIC_WaitAck();
    IIC_Stop();
}
void smg_show_bit(unsigned pos, unsigned dat)
{
    XBYTE[0xe000] = 0xff;
    XBYTE[0xc000] = 0x01 << pos;
    XBYTE[0xe000] = dat;
    Delay1ms();
}
void smg_show_all(unsigned dat)
{
    XBYTE[0xe000] = 0xff;
    XBYTE[0xc000] = 0xff;
    XBYTE[0xe000] = dat;
}
void smg_show()
{
    unsigned char i;
    for(i = 0; i < 8; i++)
    {
        smg_show_bit(i, dspbuf[i]);
        Delay1ms();
    }
    smg_show_all(0xff);
}
void Delay1ms()//@11.0592MHz
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
void buf_set()
{
	dspbuf[0] = tab[(ad2 / 100) % 10];
    dspbuf[1] = tab[(ad2 / 10) % 10];
    dspbuf[2] = tab[ad2 % 10];
    dspbuf[5] = tab[(ad1 / 100) % 10];
    dspbuf[6] = tab[(ad1 / 10) % 10];
    dspbuf[7] = tab[ad1 % 10];
}