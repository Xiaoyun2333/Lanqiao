#include<STC15.H>
#include<INTRINS.H>

#include<base.h>
#include<iic.h>

unsigned char smg_buf[] = {10,10,10,10,10,10,10,10};
unsigned char point[] = {0,0,0,0,0,0,0,0};
unsigned char led_buf[] = {0,0,0,0,0,0,0,0};
unsigned char time_mode[] = {4,4,4,4};

unsigned char key_count = 0;
unsigned char smg_count = 0;
unsigned char pos_count = 0;

unsigned char key_val,key_old,key_down,key_up;

unsigned char smg_mode = 0;
unsigned char led_mode = 0;
unsigned char led_light_switch = 0;

unsigned int smg_flash = 0;
unsigned char led_light = 0;
unsigned char led_pwm = 0;
unsigned char led_flash = 0;
bit led_value =0;
unsigned char da = 0;

unsigned char led_work_mode = 0;
unsigned char led_work_time = 0;
unsigned int led_time_count = 0;

void timer0_init();
void timer0_service();
void time_mode_init();

void key_proc();
void smg_proc();
void led_proc();
void other_proc();

void main()
{
    init();
    timer0_init();
    time_mode_init();
    while (1)
    {
        key_proc();
        smg_proc();
        led_proc();
        other_proc();  
    }  
}

void timer0_init()		//1毫秒@12.000MHz
{
	AUXR |= 0x7F;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x18;		//设置定时初始值
	TH0 = 0xFC;		//设置定时初始值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时

    ET0 = 1;
    EA = 1;
}
void time_mode_init()
{
    eeprom_read(time_mode,0,4);
}
void timer0_service() interrupt 1
{
    if(++key_count == 10) key_count = 0;
    if(++smg_count == 500) smg_count = 0;
    if(++pos_count == 8) pos_count = 0;
    if(++smg_flash == 800) smg_flash = 0;

    if(++led_flash == 24) led_flash = 0;

    if(led_flash <= led_pwm) led_show(pos_count,led_buf[pos_count]);
    else led_show(pos_count,0);


    if(smg_mode == 1)
    {
        if(smg_flash >= 400)
        {   
            smg_show(pos_count,smg_buf[pos_count],point[pos_count]);
        }
        else if(pos_count < 4)
        {
            smg_show(pos_count,10,point[pos_count]);
        }
        else smg_show(pos_count,smg_buf[pos_count],point[pos_count]);
    }
    else if(smg_mode == 2)
    {
        if(smg_flash >= 400)
        {   
            smg_show(pos_count,smg_buf[pos_count],point[pos_count]);
        }
        else if(pos_count >= 4)
        {
            smg_show(pos_count,10,point[pos_count]);
        }
        else smg_show(pos_count,smg_buf[pos_count],point[pos_count]);
    }
    else smg_show(pos_count,smg_buf[pos_count],point[pos_count]);

    if(led_value)
    {
        led_time_count++;
        if(led_time_count == time_mode[led_work_mode] * 100){led_time_count = 0; led_work_time++;}
    }
}
void key_proc()
{
    if(key_count) return;
    key_count = 1;

    key_val = key_read();
    key_down = key_val & (key_val ^ key_old);
    key_up = ~key_val & (key_val ^ key_old);
    key_old = key_val;

    if(smg_mode == 0 && key_down == 4) led_light_switch = 1;
	if(smg_mode == 0 && key_up == 4) led_light_switch = 0;

    switch(key_up)
    {
    case 4:
        if(smg_mode == 2)
        {
            if(--time_mode[led_mode] == 3) time_mode[led_mode] = 12;
        }
        if(smg_mode == 1) if(--led_mode == 255) led_mode = 3;
        break;
    case 5:
        if(smg_mode == 2)
        {
            if(++time_mode[led_mode] == 13) time_mode[led_mode] = 4;
        }
        if(smg_mode == 1) if(++led_mode == 4) led_mode = 0;
        break;
    case 6:
        if(++smg_mode == 3) 
        {   
            smg_mode = 0;
            eeprom_write(time_mode,0,4);
        }
        break; 
    case 7:
        led_value = ~led_value;
        break;  
    }
}
void smg_proc()
{
    if(smg_count) return;
    smg_count = 1;

    if(smg_mode == 0)
    { 
        if(led_light_switch)
        {
            smg_buf[6] = 11;
            smg_buf[7] = led_light;
        }
        else
        {
            unsigned char i;
            for (i = 0; i < 8; i++)
            {
                smg_buf[i] = 10;
            } 
        }
        
    }
    else
    {
        smg_buf[0] = 11;
        smg_buf[1] = led_mode + 1;
        smg_buf[2] = 11;
        smg_buf[3] = 10;
        smg_buf[4] = 10;
        if(time_mode[led_mode] / 10 > 0) smg_buf[4] = time_mode[led_mode] / 10;
        else smg_buf[4] = 10;
        smg_buf[5] = time_mode[led_mode] % 10;
        smg_buf[6] = 0;
        smg_buf[7] = 0;
    }
}
void led_proc()
{
    if(led_value)
    {
        unsigned char i = 0;
        switch(led_work_mode)
        {
        case 0:          
            for(i; i < 8; i++)
            {
                if(i == led_work_time) led_buf[led_work_time] = 1;
                else led_buf[i] = 0;
            }
            if(led_work_time >= 8) {led_work_mode = 1;led_work_time = 0;}
            break;
        case 1:
            for(i = 0 ; i < 8; i++)
            {
                if(i == led_work_time) led_buf[7 - i] = 1;
                else led_buf[7 - i] = 0;
            }
            if(led_work_time >= 8) {led_work_mode = 2;led_work_time = 0;}
            break;
        case 2:
            for(i = 0 ; i < 4; i++)
            {
                if(i == led_work_time) 
                {
                    led_buf[(7 + i * 9) / 10] = led_buf[(7 + i * 9) % 10] = 1;
                }
                else
                {
                    led_buf[(7 + i * 9) / 10] = led_buf[(7 + i * 9) % 10] = 0;
                }
            }
            if(led_work_time >= 4) {led_work_mode = 3;led_work_time = 0;}
            break;
        case 3:
            for(i = 0 ; i < 4; i++)
            {
                if(i == led_work_time) 
                {
                    led_buf[(34 - i * 9) / 10] = led_buf[(34 - i * 9) % 10] = 1;
                }
                else
                {
                    led_buf[(34 - i * 9) / 10] = led_buf[(34 - i * 9) % 10] = 0;
                }
            }
            if(led_work_time >= 4) {led_work_mode = 0;led_work_time = 0;}
            break;
        }
    }
}
void other_proc()
{
    da = adc_read();
    if(da < 64) {led_pwm = 6;led_light = 1;}
    else if(da < 128) {led_pwm = 12;led_light = 2;}
    else if(da < 192) {led_pwm = 18;led_light = 3;}
    else if(da <= 255){led_pwm = 24;led_light = 4;}
}