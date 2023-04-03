#include<STC15.H>
#include<INTRINS.H>
#include<base.h>
#include<onewire.h>
#include<ds1302.h>

unsigned char smg_buf[] = {10,10,10,10,10,10,10,10};
unsigned char point[] = {0,0,0,0,0,0,0,0};
unsigned char led_buf[] = {0,0,0,0,0,0,0,0};
unsigned char time[] = {0x23,0x59,0x50};
unsigned char time_set[] = {0x00,0x00,0x00};
unsigned char time_clk[] = {0x00,0x00,0x00};

unsigned char key_val,key_up,key_down,key_old;
unsigned char key_count = 0,smg_count = 0,pos_count = 0;

unsigned int ms1000_count = 0;
unsigned char ms200_count = 0;
unsigned char s5_count = 0;

unsigned char smg_mode = 0;
bit temper_switch = 0;
unsigned char clk_mode = 0;
bit clk_flag = 0;
bit led_flag = 0;

float temper = 0;
void key_proc();
void smg_proc();
void led_proc();

void timer0_init();
void timer0_service();

void Delay750ms();

void main()
{
    init();
    read_temper();
    Delay750ms();
    set_ds1302(time);
    read_ds1302(time_set);
    temper = read_temper();
    timer0_init();
    while(1)
    {
        key_proc();
        smg_proc();
        led_proc();
    }   
}

void key_proc()
{
    if(key_count) return;
    key_count = 1;

    key_val = key_read();
    key_down = key_val & (key_old ^ key_val);
    key_up = ~key_val & (key_old ^ key_val);
    key_old = key_val;

    if(smg_mode == 0 && key_down == 4) temper_switch = 1;
	if(smg_mode == 0 && key_up == 4) temper_switch = 0;

    switch(key_up)
    {
        case 4:
        if(!led_flag)
        {
            if(smg_mode == 1)
            {   
                if(time_set[0] == 0) {time_set[0] = 0x23;return;}
                --time_set[0];
                if(((time_set[0] % 16) == 0x0f) && ((time_set[0] / 16) >= 0)) 
                {
                    time_set[0] = time_set[0] & 0xf0 | 0x09;
                }
                
            }
            if(smg_mode == 2)
            {   
                if(time_set[1] == 0) {time_set[1] = 0x59;return;}
                --time_set[1];
                if(((time_set[1] % 16) == 0x0f) && ((time_set[1] / 16) >= 0)) 
                {
                    time_set[1] = time_set[1] & 0xf0 | 0x09;
                }
                
            }
            if(smg_mode == 3)
            {   
                if(time_set[2] == 0) {time_set[2] = 0x59;return;}
                --time_set[2];
                if(((time_set[2] % 16) == 0x0f) && ((time_set[2] / 16) >= 0)) 
                {
                    time_set[2] = time_set[2] & 0xf0 | 0x09;
                }
                
            }
            if(clk_mode == 1)
            {   
                if(time_clk[0] == 0) {time_clk[0] = 0x23;return;}
                --time_clk[0];
                if(((time_clk[0] % 16) == 0x0f) && ((time_clk[0] / 16) >= 0)) 
                {
                    time_clk[0] = time_clk[0] & 0xf0 | 0x09;
                }                
            }
            if(clk_mode == 2)
            {   
                if(time_clk[1] == 0) {time_clk[1] = 0x59;return;}
                --time_clk[1];
                if(((time_clk[1] % 16) == 0x0f) && ((time_clk[1] / 16) >= 0)) 
                {
                    time_clk[1] = time_clk[1] & 0xf0 | 0x09;
                }
                
            }
            if(clk_mode == 3)
            {   
                if(time_clk[2] == 0) {time_clk[2] = 0x59;return;}
                --time_clk[2];
                if(((time_clk[2] % 16) == 0x0f) && ((time_clk[2] / 16) >= 0)) 
                {
                    time_clk[2] = time_clk[2] & 0xf0 | 0x09;
                }
                
            }
        }
        else led_flag = 0;    
            break;
        case 5:
        if(!led_flag)
        {
            if(smg_mode == 1)
            {
                time_set[0]++;
                if(((time_set[0] % 16) > 9) && ((time_set[0] / 16) < 2)) 
                {
                    time_set[0] &= 0xf0;
                    time_set[0] += 16;
                }
                else if(((time_set[0] % 16) > 3) && ((time_set[0] / 16) == 2))
                {
                    time_set[0] &= 0x00;
                }
            }
            if(smg_mode == 2)
            {
                time_set[1]++;
                if(((time_set[1] % 16) > 9) && ((time_set[1] / 16) < 5)) 
                {
                    time_set[1] &= 0xf0;
                    time_set[1] += 16;
                }
                else if(((time_set[1] % 16) > 9) && ((time_set[1] / 16) == 5))
                {
                    time_set[1] &= 0x00;
                }
            }
            if(smg_mode == 3)
            {
                time_set[2]++;
                if(((time_set[2] % 16) > 9) && ((time_set[2] / 16) < 5)) 
                {
                    time_set[2] &= 0xf0;
                    time_set[2] += 16;
                }
                else if(((time_set[2] % 16) > 9) && ((time_set[2] / 16) == 5))
                {
                    time_set[2] &= 0x00;
                }
            }
            if(clk_mode == 1)
            {
                time_clk[0]++;
                if(((time_clk[0] % 16) > 9) && ((time_clk[0] / 16) < 2)) 
                {
                    time_clk[0] &= 0xf0;
                    time_clk[0] += 16;
                }
                else if(((time_clk[0] % 16) > 3) && ((time_clk[0] / 16) == 2))
                {
                    time_clk[0] &= 0x00;
                }
            }
            if(clk_mode == 2)
            {
                time_clk[1]++;
                if(((time_clk[1] % 16) > 9) && ((time_clk[1] / 16) < 5)) 
                {
                    time_clk[1] &= 0xf0;
                    time_clk[1] += 16;
                }
                else if(((time_clk[1] % 16) > 9) && ((time_clk[1] / 16) == 5))
                {
                    time_clk[1] &= 0x00;
                }
            }
            if(clk_mode == 3)
            {
                time_clk[2]++;
                if(((time_clk[2] % 16) > 9) && ((time_clk[2] / 16) < 5)) 
                {
                    time_clk[2] &= 0xf0;
                    time_clk[2] += 16;
                }
                else if(((time_clk[2] % 16) > 9) && ((time_clk[2] / 16) == 5))
                {
                    time_clk[2] &= 0x00;
                }
            }
        }
        else led_flag = 0;   
            break;
        case 6:
        if(!led_flag)
        {
            if(smg_mode == 0)
            {
                clk_flag = 1;
                if(++clk_mode == 4)
                {
                    clk_mode = 0;
                    clk_flag = 0;
                }
            }
        }
        else led_flag = 0;  
            break;
        case 7:
        if(!led_flag)
        {
            if(++smg_mode == 4)
            {
                smg_mode = 0;
                set_ds1302(time_set);
                read_ds1302(time);
            }
        }
        else led_flag = 0; 
            break;
    }
}
void smg_proc()
{
    if(smg_count) return;
    smg_count = 1;

    read_ds1302(time);
    if(!clk_flag)
    {
    switch(smg_mode)
    {
        case 0:
        if(temper_switch)
        {
            temper = read_temper();
            smg_buf[0] = smg_buf[1] = smg_buf[2] = smg_buf[3] = smg_buf[4] = 10;
            if(temper / 10) smg_buf[5] = (unsigned char)temper / 10;
            else smg_buf[5] = 10;
            smg_buf[6] = (unsigned char)temper % 10;
            smg_buf[7] = 12;
        }
        else
        {
            smg_buf[0] = time[0] / 16;
            smg_buf[1] = time[0] % 16;
            smg_buf[2] = 11;
            smg_buf[3] = time[1] / 16;
            smg_buf[4] = time[1] % 16;
            smg_buf[5] = 11;
            smg_buf[6] = time[2] / 16;
            smg_buf[7] = time[2] % 16;

            read_ds1302(time_set);
        }
        break;
    case 1:
        if(ms1000_count < 500)
        {
            smg_buf[0] = time_set[0] / 16;
            smg_buf[1] = time_set[0] % 16; 

            smg_buf[3] = time_set[1] / 16;
            smg_buf[4] = time_set[1] % 16;
            
            smg_buf[6] = time_set[2] / 16;
            smg_buf[7] = time_set[2] % 16;
        }
        else
        {
            smg_buf[0] = 10;
            smg_buf[1] = 10; 
        }
        break;
    case 2:
        if(ms1000_count < 500)
        {
            smg_buf[0] = time_set[0] / 16;
            smg_buf[1] = time_set[0] % 16; 
            
            smg_buf[3] = time_set[1] / 16;
            smg_buf[4] = time_set[1] % 16;
            
            smg_buf[6] = time_set[2] / 16;
            smg_buf[7] = time_set[2] % 16;
        }
        else
        {
            smg_buf[3] = 10;
            smg_buf[4] = 10; 
        }
        break;
    case 3:
        if(ms1000_count < 500)
        {
            smg_buf[0] = time_set[0] / 16;
            smg_buf[1] = time_set[0] % 16; 
            
            smg_buf[3] = time_set[1] / 16;
            smg_buf[4] = time_set[1] % 16;
            
            smg_buf[6] = time_set[2] / 16;
            smg_buf[7] = time_set[2] % 16;
        }
        else
        {
            smg_buf[6] = 10;
            smg_buf[7] = 10; 
        }
        break;
    }
    }
    else
    {
    switch(clk_mode) 
       {
        case 1:
            if(ms1000_count < 500)
        {
            smg_buf[0] = time_clk[0] / 16;
            smg_buf[1] = time_clk[0] % 16; 

            smg_buf[3] = time_clk[1] / 16;
            smg_buf[4] = time_clk[1] % 16;
            
            smg_buf[6] = time_clk[2] / 16;
            smg_buf[7] = time_clk[2] % 16;
        }
        else
        {
            smg_buf[0] = 10;
            smg_buf[1] = 10; 
        }
        break;
        case 2:
            if(ms1000_count < 500)
        {
            smg_buf[0] = time_clk[0] / 16;
            smg_buf[1] = time_clk[0] % 16; 

            smg_buf[3] = time_clk[1] / 16;
            smg_buf[4] = time_clk[1] % 16;
            
            smg_buf[6] = time_clk[2] / 16;
            smg_buf[7] = time_clk[2] % 16;
        }
        else
        {
            smg_buf[3] = 10;
            smg_buf[4] = 10; 
        }
        break;
        case 3:
        if(ms1000_count < 500)
        {
            smg_buf[0] = time_clk[0] / 16;
            smg_buf[1] = time_clk[0] % 16; 

            smg_buf[3] = time_clk[1] / 16;
            smg_buf[4] = time_clk[1] % 16;
            
            smg_buf[6] = time_clk[2] / 16;
            smg_buf[7] = time_clk[2] % 16;
        }
        else
        {
            smg_buf[6] = 10;
            smg_buf[7] = 10; 
        }
        break;
       }
    }
}
void led_proc()
{
    if((time_clk[0] == time[0]) && (time_clk[1] == time[1]) && (time_clk[2] == time[2])) led_flag = 1;

    if((led_flag == 1) && (s5_count < 5))
    {
        if(ms200_count < 100) led_buf[0] = 1;
        else led_buf[0] = 0;
    }
    else
    {
        s5_count = 0;
        led_buf[0] = 0;
    }
}
void timer0_init()
{
    AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x18;		//设置定时初始值
	TH0 = 0xFC;		//设置定时初始值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时

    ET0 = 1;
    EA = 1;
}
void timer0_service() interrupt 1
{
    if(++key_count == 10) key_count = 0;
    if(++smg_count == 500) smg_count = 0;
    if(++pos_count == 8) pos_count = 0;
    if(led_flag)
    {
        if(++ms200_count == 200) ms200_count = 0;
    }
    else ms200_count = 0;
    if(++ms1000_count == 1000)
    {
        ms1000_count = 0; 
        if(led_flag) s5_count++;
        if(s5_count == 5)
        {
            s5_count = 0;
            led_flag = 0;
        }
    }
    led_show(pos_count,led_buf[pos_count]);
    smg_show(pos_count,smg_buf[pos_count],point[pos_count]);
}
void Delay750ms()		//@12.000MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 35;
	j = 51;
	k = 182;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}
