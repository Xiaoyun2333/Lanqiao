#include<base.h>
#include<STC15.H>
#include<INTRINS.H>

unsigned char tab[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff,0xc1};
unsigned char tab2[] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};

sbit Tx = P1^0;
sbit Rx = P1^1;

void init()
{
    P2 = P2 & 0x1f | 0x80;
    P0 = 0xff;
    P2 &= 0x1f;

    P2 = P2 & 0x1f | 0xa0;
    P0 = 0x00;
    P2 &= 0x1f;
}
unsigned char key_read()
{
    unsigned char temp = 0;
	AUXR &= 0xEF;
	P44 = 0;P42 = 1;P35 = 1;P34 = 1;
	if(P33 == 0) temp = 4;
	if(P32 == 0) temp = 5;
	if(P31 == 0) temp = 6;
	if(P30 == 0) temp = 7;

	P44 = 1;P42 = 0;P35 = 1;P34 = 1;
	if(P33 == 0) temp = 8;
	if(P32 == 0) temp = 9;
	if(P31 == 0) temp = 10;
	if(P30 == 0) temp = 11;
	
	P44 = 1;P42 = 1;P35 = 0;P34 = 1;
	if(P33 == 0) temp = 12;
	if(P32 == 0) temp = 13;
	if(P31 == 0) temp = 14;
	if(P30 == 0) temp = 15;
	
	P44 = 1;P42 = 1;P35 = 1;P34 = 0;
	if(P33 == 0) temp = 16;
	if(P32 == 0) temp = 17;
	if(P31 == 0) temp = 18;
	if(P30 == 0) temp = 19;
	
	P3 = 0xff;
	AUXR |= 0x10; 

	return temp;
}
void led_show(unsigned char pos,flag)
{
    static unsigned char temp = 0x00;
    static unsigned char temp_old = 0xff;

    if(flag) temp |= 0x01 << pos;
    else temp &= ~(0x01 << pos);

    if(temp != temp_old)
    {
        P0 = ~temp;
		P2 = P2 & 0x1f | 0x80;
		P2 &= 0x1f;
		temp_old = temp;
    }
}
void smg_show(unsigned char pos,dat,point)
{
	P2 = P2 & 0x1f | 0xe0;
	P0 = 0xff;
	P2 &= 0x1f;

	P2 = P2 & 0x1f | 0xc0;
	P0 = tab2[pos];
	P2 &= 0x1f;

	P2 = P2 & 0x1f | 0xe0;
	P0 = tab[dat];
	if(point) P0 &= 0x7f;
	P2 &= 0x1f;
}
void uart_send_byte(unsigned char dat)
{
	SBUF = dat;
	while(TI == 0);
	TI = 0;
}
void uart_send_string(unsigned char *dat)
{
	while(*dat != '\0')
		uart_send_byte(*dat++);
}
void wave_init()
{
	unsigned char i;
	for(i = 0; i < 8; i++)
	{
		Tx = 1;
		Delay12us();
		Tx = 0;
		Delay12us();
	}
}
unsigned char wave_data()
{
	unsigned int time;		
	TMOD &= 0x0f;
	TH1 = TL1 = 0;
	wave_init();
	TR1 = 1;
	while((Rx == 1) && (TF1 == 0));
	TR1 = 0;
	if(TF1 == 1)
	{
		TF1 = 0;
		TH1 = 0;
		TL1 = 0;
		return 255;
	}
	else
	{
		time = TH1 << 8 | TL1;
		TH1 = 0;
		TL1 = 0;
		return (time * 0.017);
	}
}
void Delay12us()		//@12.000MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 33;
	while (--i);
}