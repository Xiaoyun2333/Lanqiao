#include<stc15.h>
#include<intrins.h>
#include<absacc.h>

#include<iic.h>

unsigned char tab[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff};
unsigned char dspbuf[] = {255, 255, 255, 255, 255, 255, 255, 255};
unsigned char eeprom;

void cls();
unsigned char ad_read(unsigned char addr);
void da_wirte(unsigned char addr);
void smg_show_bit(unsigned pos, unsigned dat);//数码管显示一位
void smg_show_all(unsigned dat);//数码管全部显示
void smg_show();//数码管动态显示
void buf_set();//设置数码管
void Delay1ms();

void eeprom_write(unsigned char addr, unsigned char dat);
unsigned char eeprom_read(unsigned char addr);

void main()
{
    cls();
    eeprom = eeprom_read(0x01);
    eeprom++;
    eeprom_write(0x01,eeprom);
    buf_set();
    while(1)
    {
        smg_show();
    }
}
void cls()
{
	XBYTE[0x8000] = 0xff;
    XBYTE[0xa000] = 0x00;
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
    dspbuf[5] = tab[(eeprom/ 100) % 10];
    dspbuf[6] = tab[(eeprom / 10) % 10];
    dspbuf[7] = tab[eeprom % 10];
}

void eeprom_write(unsigned char addr, unsigned char dat)
{
    IIC_Start();
    IIC_SendByte(0xA0);
    IIC_WaitAck();
    IIC_SendByte(addr);
    IIC_WaitAck();
    IIC_SendByte(dat);
    IIC_WaitAck();
    IIC_Stop();

}
unsigned char eeprom_read(unsigned char addr)
{
    unsigned char temp;
    IIC_Start();
    IIC_SendByte(0xA0);
    IIC_WaitAck();
    IIC_SendByte(addr);
    IIC_WaitAck();

    IIC_Start();
    IIC_SendByte(0xA1);
    IIC_WaitAck();
    temp = IIC_RecByte();
    IIC_SendAck(1);
    IIC_Stop();
    return temp;

}