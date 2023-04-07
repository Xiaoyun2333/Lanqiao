#include<STC15.H>
#include<wave.h>
#include<INTRINS.H>

sbit Tx = P1^0; 
sbit Rx = P1^1; 

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
    for(i = 0; i < 8; i++)
    {
        Tx = 1;
        Delay12us();
        Tx = 0;
        Delay12us();
    }
}
unsigned char wave_read()
{
    unsigned int time;
    TMOD &= 0x0F;
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