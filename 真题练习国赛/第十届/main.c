#include<STC15.H>
#include<INTRINS.H>
#include<base.h>
#include<onewire.h>
#include<wave.h>
#include<iic.h>
#include<STDIO.H>

idata unsigned char smg_buf[] = {10,10,10,10,10,10,10,10};
idata unsigned char point[] = {0,0,0,0,0,0,0,0};
idata unsigned char led_buf[] = {0,0,0,0,0,0,0,0};
unsigned char uart_recv[10];
unsigned char uart_index;
unsigned char uart_send[10];

unsigned char key_val,key_up,key_down,key_old,key_val_real;
unsigned int ms1_count = 0;
bit key_12 = 0;
bit key_12_old = 0;
bit key_13 = 0;

unsigned char key_count = 0;
unsigned char smg_count = 0;
unsigned char uart_count = 0;
unsigned char pos_count = 0;

bit show_set_flag = 0;

unsigned char show_mode = 0;
unsigned char set_mode = 0;//各种模式参数

unsigned char temper_set = 30;
unsigned char distance_set = 35;
unsigned char temper_set_old = 30;
unsigned char distance_set_old = 35;
unsigned char set_count_buf[] = {0xff,0xfe}; 
unsigned int set_count = 0x00;
float temper = 0;
unsigned char distance = 0;


void key_proc();
void smg_proc();
void led_proc();
void uart_proc();
void other_proc();

void timer0_init();
void timer0_service();

void uart_service();
//---------------------------------------------------------
void main()
{
    init();
    temper_read();
    eeprom_read(0x00,set_count_buf,2);
    set_count = (set_count_buf[0] << 8) | set_count_buf[1];
    Delay750ms();
    temper = temper_read();
    uart_init();
    timer0_init();
    while(1)
    {
        key_proc();
        smg_proc();
        led_proc();
        uart_proc();
        other_proc();
    }
}
//---------------------------------------------------------
void key_proc()
{   
    static unsigned int key_up_time = 0;
    static unsigned int key_down_time = 0;
    if(key_count) return;
    key_count = 1;

    key_val = key_read();
    if(!key_val) key_val_real = 0;
    key_down = key_val & (key_val ^ key_old);
    key_up = ~key_val & (key_val ^ key_old);
    key_old = key_val;

    

    if(key_down) key_down_time = ms1_count;
    if(key_up) 
    {
        key_up_time = ms1_count;
        switch(key_up)
        {
            case 13: key_val_real = 13; break;
            case 12: key_val_real = 12; break;
            case 17: key_val_real = 17; break;
            case 16: key_val_real = 16; break;
        }
        if(key_up_time - key_down_time > 1000)
        {
           key_val_real += 16;
        }
    }
    switch(key_val_real)
    {
        case 13:
            show_set_flag = ~show_set_flag;
            if(show_set_flag) set_mode = 0;
            else 
            {
                show_mode = 0;
                if((temper_set_old != temper_set) || (distance_set != distance_set_old))
                {
                    set_count++;
                    set_count_buf[0] = (unsigned char)(set_count >> 8);   
                    set_count_buf[1] = (unsigned char)(set_count &= 0x00ff);                 
                    eeprom_write(0x00,set_count_buf,2);
                    set_count = (set_count_buf[0] << 8) | set_count_buf[1];
                    distance_set_old = distance_set;
                    temper_set_old = temper_set;
                }
            }
        break;
        case 12:
            if(!show_set_flag)
            {
                if(++show_mode == 3) show_mode = 0;
            }
            else
            {
                if(++set_mode == 2) set_mode = 0;
            }
        break;
        case 16:
            if(show_set_flag)
            {
                if(set_mode == 0)
                {
                    if(temper_set <= 1) temper_set = 0;
                    else temper_set -= 2;
                }
                if(set_mode == 1)
                {
                    if(distance_set <= 5) distance_set = 0;
                    else distance_set -= 5;
                }
            }
        break;
        case 17:
            if(show_set_flag)
            {
                if(set_mode == 0)
                {
                    if((temper_set >= 97) && (temper_set <= 101)) temper_set = 99;
                    else temper_set += 2;
                }
                if(set_mode == 1)
                {
                    if((distance_set >= 94) && (distance_set <= 104)) distance_set = 99;
                    else distance_set += 5;
                }
            }
        break;
        case 29:
            key_13 = ~key_13;
        break;
        case 28:
            key_12 = ~key_12;
        break;
    }
}
void smg_proc()
{
    if(smg_count) return;
    smg_count = 1;

    temper = temper_read();
    distance = wave_read();
    if(distance >= 99) distance = 99;

    if(!show_set_flag)
    {
        switch(show_mode)
        {
            case 0:
                smg_buf[0] = 11;
                smg_buf[3] = 10;
                smg_buf[4] = temper / 10;
                smg_buf[5] = (unsigned char)temper % 10;
                smg_buf[6] = (unsigned int)(temper * 10) % 10;
                smg_buf[7] = (unsigned int)(temper * 100) % 10;
                point[5] = 1;
            break;
            case 1:
                smg_buf[0] = 12;
                smg_buf[4] = 10;
                smg_buf[5] = 10;
                if((distance / 100) == 0) smg_buf[5] = 10;
                else smg_buf[5] = distance / 100;
                if((distance / 10 % 10) == 0)
                {
                    if((distance / 100) == 0) smg_buf[6] = 10;
                    else smg_buf[6] = 0;
                }
                else smg_buf[6] = distance / 10 % 10;
                smg_buf[7] = distance % 10;
                point[5] = 0;
            break;
            case 2:
                smg_buf[0] = 13;
                if(set_count / 10000 == 0) smg_buf[3] = 10;
                else smg_buf[3] = set_count / 10000;
                if(set_count / 1000 % 10 == 0)
                {
                    if(set_count / 10000 == 0) smg_buf[4] = 10;
                    else smg_buf[4] = 0;
                }
                else smg_buf[4] = set_count / 1000 % 10;
                if(set_count / 100 % 10 == 0)
                {
                    if(set_count / 1000 == 0) smg_buf[5] = 10;
                    else smg_buf[5] = 0;
                }
                else smg_buf[5] = set_count / 100 % 10;
                if(set_count / 10 % 10 == 0)
                {
                    if(set_count / 100 == 0) smg_buf[6] = 10;
                    else smg_buf[6] = 0;
                }
                else smg_buf[6] = set_count / 10 % 10;
                if(set_count % 10 == 0) smg_buf[7] = 0;
                else smg_buf[7] = set_count % 10;
            break;
        }
    }
    else
    {
        switch(set_mode)
        {
            case 0:
                smg_buf[0] = 14;
                smg_buf[3] = 1;
                smg_buf[4] = 10;
                smg_buf[5] = 10;
                smg_buf[6] = temper_set / 10;
                smg_buf[7] = temper_set % 10;
                point[5] = 0;
            break;
            case 1:
                smg_buf[0] = 14;
                smg_buf[3] = 2;
                smg_buf[4] = 10;
                smg_buf[5] = 10;
                smg_buf[6] = distance_set / 10;
                smg_buf[7] = distance_set % 10;
            break;
        }
    }
}
void led_proc()
{
    if(temper > temper_set) led_buf[0] = 1;
    else led_buf[0] = 0;
    if(distance < distance_set) led_buf[1] = 1;
    else led_buf[1] = 0;
    if(key_13) led_buf[2] = 1;
    else led_buf[2] = 0;
}
void uart_proc()
{
    if(uart_count) return;
    uart_count = 1;
    if(uart_index)
    {
        if((uart_recv[0] == 'S')&&(uart_recv[1] == 'T')&&(uart_recv[2] == '\r')&&(uart_recv[3] == '\n')&&(uart_index == 4))
        {
            sprintf(uart_send,"$%d,%.2f\r\n",(unsigned int)distance,temper);
            uart_send_string(uart_send);
        }
        else if((uart_recv[0] == 'P')&&(uart_recv[1] == 'A')&&(uart_recv[2] == 'R')&&(uart_recv[3] == 'A')&&(uart_recv[4] == '\r')&&(uart_recv[5] == '\n')&&(uart_index == 6))
        {
            sprintf(uart_send,"#%d,%d\r\n",(unsigned int)distance_set,(unsigned int)temper_set);
            uart_send_string(uart_send);
        }
        else
        {
            sprintf(uart_send,"ERROR\r\n");
            uart_send_string(uart_send);
        }
        uart_index = 0;
    }
    
   
}
void other_proc()
{
    if(!key_13) dac_write(21);
    else
    {
        if(distance <= distance_set) dac_write(103);
        else dac_write(205);
    }
    if(key_12 != key_12_old)
    {
        set_count_buf[0] = 0;
        set_count_buf[1] = 0;
        eeprom_write(0x00,set_count_buf,2);
        set_count = (set_count_buf[0] << 8) | set_count_buf[1];
    }
}

void timer0_init()
{
    AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x20;		//设置定时初始值
	TH0 = 0xD1;		//设置定时初始值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时

    ET0 = 1;
    EA = 1;
}
void timer0_service() interrupt 1
{
    if(++key_count == 10) key_count = 0;
    if(++smg_count == 200) smg_count = 0;
    if(++uart_count == 200) uart_count = 0;
    if(++pos_count == 8) pos_count = 0;

    smg_show(pos_count,smg_buf[pos_count],point[pos_count]);
    led_show(pos_count,led_buf[pos_count]);

    ms1_count++; 
}
void uart_service() interrupt 4
{
    if((RI == 1) && (uart_index < 20))
    {
        uart_recv[uart_index] = SBUF;
        uart_index++;
        RI = 0;
    }
}