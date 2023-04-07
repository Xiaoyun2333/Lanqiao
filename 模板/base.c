#include<STC15.H>
#include<INTRINS.H>
#include<base.h>

sbit Tx = P1^0;
sbit Rx = P1^1;

code unsigned char tab[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff};
static unsigned char temp1 = 0x00;
static unsigned char temp1_old = 0xff;

void init()
{
    P2 = P2 & 0x1f | 0x80;
    P0 = 0xff;
    P2 &= 0x1f;

    P2 = P2 & 0x1f | 0xa0;
    P0 = 0x00;
    P2 &= 0x1f;
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
        temp = temp_old;
    }
}
void relay(unsigned char flag)
{
    if(flag) temp1 |= 0x10;
    else temp1 &= ~0x10;
    if(temp1 != temp1_old)
    {
        P0 = temp1;
        P2 = P2 & 0x1f | 0xa0;
        P2 &= 0x1f;
        temp1 = temp1_old;
    }
}
void beep(unsigned char flag)
{
    if(flag) temp1 |= 0x40;
    else temp1 &= ~0x40;
    if(temp1 != temp1_old)
    {
        P0 = temp1;
        P2 = P2 & 0x1f | 0xa0;
        P2 &= 0x1f;
        temp1 = temp1_old;
    }
}
unsigned char key_read()
{
    unsigned char temp = 0;
    AUXR &= 0xfe;
    if(P33 == 0) temp = 4;
    if(P32 == 0) temp = 5;
    if(P31 == 0) temp = 6;
    if(P30 == 0) temp = 7;
    AUXR |= 0x10;
    return temp;
}
// unsigned char key_read()
// {
//     unsigned char temp = 0;
//     AUXR &= 0xfe;
//     P44 = 0;P42 = 1;P35 = 1;P34 = 1;
//     if(P33 == 0) temp = 4;
//     if(P32 == 0) temp = 5;
//     if(P31 == 0) temp = 6;
//     if(P30 == 0) temp = 7;
//     P44 = 1;P42 = 0;P35 = 1;P34 = 1;
//     if(P33 == 0) temp = 8;
//     if(P32 == 0) temp = 9;
//     if(P31 == 0) temp = 10;
//     if(P30 == 0) temp = 11;
//     P44 = 1;P42 = 1;P35 = 0;P34 = 1;
//     if(P33 == 0) temp = 12;
//     if(P32 == 0) temp = 13;
//     if(P31 == 0) temp = 14;
//     if(P30 == 0) temp = 15;
//     P44 = 1;P42 = 1;P35 = 1;P34 = 0;
//     if(P33 == 0) temp = 16;
//     if(P32 == 0) temp = 17;
//     if(P31 == 0) temp = 18;
//     if(P30 == 0) temp = 19;
//     AUXR |= 0x10;
//     return temp;
// }
void smg_show(unsigned pos,dat,flag)
{
    P2 = P2 & 0x1f | 0xe0;
    P0 = 0xff;
    P2 &= 0x1f;

    P2 = P2 & 0x1f | 0xa0;
    P0 = 0x01 << pos;
    P2 &= 0x1f;

    P2 = P2 & 0x1f | 0xe0;
    P0 = tab[dat];
    P2 &= 0x1f;
}
void Delay12us()		//@12.000MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 33;
	while (--i);
}
void wave_init()
{
    unsigned char i;
    for(i = 0;i < 8;i++)
    {
        Tx = 1;
        Delay12us();
        Tx = 0;
        Delay12us();
    }
}
unsigned char wave_read()
{
    unsigned int time = 0;
    CMOD &= 0x00;
    CH = CL = 0;
    CF = 0;
    CR = 0;
    wave_init();
    CR = 1;
    while(Rx && !CF);
    CR = 0;
    if(CF == 0)
    {
        time = CH << 8 | CL;
        Rx = 1;
        return time * 0.017;
    }
    else 
    {
        CF = 0;
        return 255;
    }
}
