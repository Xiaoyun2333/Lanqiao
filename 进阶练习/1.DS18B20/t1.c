#include<stc15.h>
#include<intrins.h>
#include<onewire.h>

unsigned int temp = 850;

code unsigned tab_nodoc[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e};
code unsigned tab_doc[] = {0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0x08,0x03,0x46,0x21,0x06,0x0e};

void init_138(unsigned char value);
void cls();

void smg_show_bit(unsigned char pos, unsigned char dat);
void smg_show();
void smg_show_all(unsigned char dat);

void ds18b20_read_temp();

void Delay1ms();
void Delay(unsigned int t);

void main()
{
	cls();
	while(1)
	{
		ds18b20_read_temp();
		smg_show();
	}
}
//=========================================================
void init_138(unsigned char value)
{
	switch(value)
	{
		case 4:
			P2 = P2 & 0x1f | 0x80;
		break;
		case 5:
			P2 = P2 & 0x1f | 0xa0;
		break;
		case 6:
			P2 = P2 & 0x1f | 0xc0;
		break;
		case 7:
			P2 = P2 & 0x1f | 0xe0;
		break;
		default:
			P2 = P2 & 0x1f;
		break;
	}
}
void cls()
{
	init_138(4);
	P0 = 0xff;
	init_138(5);
	P0 = 0x00;
	init_138(0);
}
//===========================================================
void smg_show_bit(unsigned char pos, unsigned char dat)
{
	init_138(7);
	P0 = 0xff;
	init_138(6);
	P0 = 0x01 << pos;
	init_138(7);
	P0 = dat;
	init_138(0);
}
void smg_show()
{
	smg_show_bit(7, tab_nodoc[temp % 10]);
	Delay1ms();
	smg_show_bit(6, tab_doc[(temp % 100) / 10]);
	Delay1ms();
	smg_show_bit(5, tab_nodoc[temp / 100]);
	Delay1ms();
	
	smg_show_bit(4, 0xff);
	Delay1ms();
	smg_show_bit(3, 0xff);
	Delay1ms();
	smg_show_bit(2, 0xff);
	Delay1ms();
	smg_show_bit(1, 0xff);
	Delay1ms();
	smg_show_bit(0, 0xff);
	Delay1ms();
	
	smg_show_all(0xff);
	
}
void smg_show_all(unsigned char dat)
{
	init_138(6);
	P0 = 0xff;
	init_138(7);
	P0 = dat;
}
//===========================================================
void ds18b20_read_temp()
{
	unsigned char LSB,HSB;
	
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0x44);
	
	Delay(200);
	
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0xbe);
	
	LSB = Read_DS18B20();
	HSB = Read_DS18B20();
	
	temp = HSB;
	temp = (temp << 8) | LSB;
	if((temp & 0xf800) == 0x0000)
	{
		temp >>= 4;
		temp = temp * 10;
		temp = temp + (LSB & 0x0f) * 0.625;
	}
	//temp >>= 4;只显示整数
	
}
//===========================================================
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
void Delay(unsigned int t)		//@11.0592MHz
{
	while(--t)
	{
		smg_show();
	}
}
