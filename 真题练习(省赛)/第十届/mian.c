#include<STC15.H>
#include<INTRINS.H>

#include<iic.h>
#include<led.h>
#include<smg.h>
#include<key.h>

unsigned char smg_buf[] = {10,10,10,10,10,10,10,10};
unsigned char smg_point[] = {0,0,0,0,0,0,0,0};
unsigned char led_buf[] = {0,0,0,0,0,0,0,0};

unsigned char key_count = 0;
unsigned char smg_count = 0;
unsigned char smg_bit_count  = 0;
unsigned int timer0_count = 0;
unsigned int ne555_f = 0;

unsigned char key_val,key_up,key_down,key_old;

bit led_mode = 0;
bit smg_value = 0;
bit smg_mode = 0;
bit dac_mode = 0;

unsigned int da = 0;

void init();
void timer0_init();
void timer0_service();
void timer1_init();
void timer1_service();

void key_proc();
void smg_proc();
void led_proc();
void other_proc();

void Delay1000ms()		//@12.000MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 46;
	j = 153;
	k = 245;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

void main()
{
    init();
    timer0_init();
    timer1_init();
    Delay1000ms();
    while(1)
    {
        key_proc();
        smg_proc();
        led_proc();
        other_proc();
    } 
}

void init()
{
    P2 = P2 & 0x1f | 0x80;
    P0 = 0xff;
    P2 &= 0x1f;

    P2 = P2 & 0x1f | 0xa0;
    P0 = 0x00;
    P2 &= 0x1f;
}
void timer0_init()
{
    AUXR &= 0x7F; 
    TMOD |= 0x05;
    TL0 = 0x00;		//设置定时初始值
	TH0 = 0x00;		//设置定时初始值
    TF0 = 0;
    TR0 = 1;
}
void timer1_init()		//1毫秒@12.000MHz
{
	AUXR |= 0x40;		//定时器时钟1T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0x20;		//设置定时初始值
	TH1 = 0xD1;		//设置定时初始值
	TF1 = 0;		//清除TF1标志
	TR1 = 1;		//定时器1开始计时

    ET1 = 1;
    EA = 1;
}
void timer1_service() interrupt 3
{
    if(++key_count == 10) key_count = 0;
    if(++smg_count == 500) smg_count = 0;
    if(++smg_bit_count == 8) smg_bit_count = 0;
    if(++timer0_count == 1000)
    {
        ne555_f = TH0 << 8 | TL0;
        timer0_count = 0;
        TL0 = 0x00;		//设置定时初始值
	    TH0 = 0x00;		//设置定时初始值
    }
    led_show(smg_bit_count,led_buf[smg_bit_count]);
    smg_show(smg_bit_count,smg_buf[smg_bit_count],smg_point[smg_bit_count]);
}
void key_proc()
{
    if(key_count) return;
    key_count = 1;

    key_val = key_read();
    key_down = key_val & (key_val ^ key_old);
    key_up = ~key_val & (key_val ^ key_old);
    key_old = key_val;

    switch(key_down)
    {
        case 4:
            smg_mode = ~smg_mode;
            break;
        case 5:
            dac_mode = ~dac_mode;
            break;
        case 6:
            led_mode = ~led_mode;
            break;
        case 7:
            smg_value = ~smg_value;
            break;
    }
}
void smg_proc()
{
    if(smg_count) return;
    smg_count = 1;

    if(~smg_value)
    {
        if(~smg_mode)
        {
            smg_buf[0] = 11;
            if(ne555_f > 9999) smg_buf[3] = ne555_f / 10000;
            else  smg_buf[3] = 10;
            if(ne555_f > 999) smg_buf[4] = ne555_f / 1000 % 10;
            else  smg_buf[4] = 10;
            if(ne555_f > 99) smg_buf[5] = ne555_f / 100 % 10;
            else  smg_buf[5] = 10;
            smg_point[5] = 0;
            if(ne555_f > 9) smg_buf[6] = ne555_f / 10 % 10;
            else  smg_buf[6] = 10;
            if(ne555_f > 0) smg_buf[7] = ne555_f % 10;
            else  smg_buf[7] = 10;
        }
        else
        {
            smg_buf[0] = 12;
            smg_buf[3] = 10;
            smg_buf[4] = 10;
            smg_buf[5] = da * 100 / 51 / 100;
            smg_point[5] = 1;
            smg_buf[6] = da * 100 / 51 / 10 % 10;
            smg_buf[7] = da * 100 / 51 % 10;  
        }
    }
    else
    {
        smg_buf[0] = 10;
        smg_buf[4] = 10;
        smg_buf[5] = 10;
        smg_point[5] = 0;
        smg_buf[6] = 10;
        smg_buf[7] = 10;
    } 
}
void led_proc()
{
    if(!led_mode)
    {
        if(!smg_mode)
        {
            led_buf[0] = 1;
            led_buf[1] = 0;
        }
        else
        {
            led_buf[0] = 0;
            led_buf[1] = 1;
        }
        if(da < 76.5) led_buf[2] = 0;
        else if(da < 127.5) led_buf[2] = 1;
        else if(da < 178.5) led_buf[2] = 0;
        else led_buf[2] = 1;

        if(ne555_f < 1000) led_buf[3] = 0;
        else if(ne555_f <5000) led_buf[3] = 1;
        else if(ne555_f < 10000) led_buf[3] = 0;
        else led_buf[3] = 1;

        if(!dac_mode) led_buf[4] = 0;
        else led_buf[4] = 1;
    }
    else 
    {
        led_buf[0] = 0;
        led_buf[1] = 0;
        led_buf[2] = 0;
        led_buf[3] = 0;
        led_buf[4] = 0;
        led_buf[5] = 0;
        led_buf[6] = 0;
        led_buf[7] = 0;
    }
}
void other_proc()
{
    

    if(!dac_mode) 
    {
        da = 102;
        da_write(102);
        
    }
    else 
    {
        da_write(da);
        da = ad_read();
    }
}
