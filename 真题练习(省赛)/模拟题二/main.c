#include<STC15.H>
#include<INTRINS.H>
#include<iic.h>
#include<onewire.h>
#include<base.h>
#include<STDIO.H>

unsigned char smg_buf[] = {10,10,10,10,10,10,10,10};
unsigned char point[] = {0,0,0,0,0,0,0,0};
unsigned char led_buf[] = {0,0,0,0,0,0,0,0};
unsigned char uart_recv;
unsigned char uart_index;
unsigned char uart_send[20];

unsigned char key_val,key_old,key_up,key_down;

unsigned char key_count = 0;
unsigned char smg_count = 0;
unsigned char uart_count = 0;
unsigned char pos_count = 0;

unsigned char ms200_count = 0;

float temper = 0;
unsigned char adc = 0;

unsigned char smg_mode = 0;
bit lock_flag = 0;

void timer0_init();
void timer0_service();

void uart_service();

void key_prco();
void smg_proc();
void led_proc();
void uart_proc();
void other_proc();

void main()
{
    init();
    read_temper();
    Delay750ms();
    uart_init();
    timer0_init();
    while (1)
    {
        key_prco();
        smg_proc();
        led_proc();
        uart_proc();
        other_proc();
    }
}
void timer0_init()
{
    AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x20;		//设置定时初始值
	TH0 = 0xD1;		//设置定时初始值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;

    ET0 = 1;
    EA = 1;
}
void timer0_service() interrupt 1
{
    if(++key_count == 10) key_count = 0;
    if(++smg_count == 500) smg_count = 0;
    if(++uart_count == 200) uart_count = 0;
    if(++pos_count == 8) pos_count = 0;
    if(lock_flag)
    {
        if(++ms200_count == 200) ms200_count = 0;
    }
    else ms200_count = 0;
    smg_show(pos_count,smg_buf[pos_count],point[pos_count]);
    led_show(pos_count,led_buf[pos_count]);
}
void uart_service() interrupt 4
{
    if(RI)
    {
        if(!lock_flag)
        {
            uart_recv = SBUF;
        }
        RI = 0;
    }
}
void key_prco()
{
    if(key_count) return;
    key_count = 1;

    key_val = key_read();
    key_down = key_val & (key_val ^ key_old);
    key_up = ~key_val & (key_val ^ key_old);
    key_old = key_val;

    switch(key_up)
    {
        case 4:
            lock_flag = 1;
        break;
        case 5:
            lock_flag = 0;
        break;
        case 12:
            if(smg_mode == 0)
            {
                sprintf(uart_send,"TEMP:%.1fC\r\n",temper);
                uart_send_string(uart_send);
            }
            if(smg_mode == 1)
            {
                sprintf(uart_send,"Voltage:%.2fV\r\n",adc / 51.0);
                uart_send_string(uart_send);
            }
        break;
    }
}
void smg_proc()
{
    if(smg_count) return;
    smg_count = 1;

    temper = read_temper();
    adc = adc_read();
    switch(smg_mode)
    {
        case 0:
            smg_buf[0] = 11;
            smg_buf[1] = 1;
            smg_buf[5] = temper / 10;
            point[5] = 0;
            smg_buf[6] = (unsigned char)temper % 10 ;
            point[6] = 1;
            smg_buf[7] = (unsigned int)(temper * 10) % 10;  
        break;
        case 1:
            smg_buf[0] = 11;
            smg_buf[1] = 2;
            smg_buf[5] = adc / 51;
            point[5] = 1;
            smg_buf[6] = (unsigned int)(adc * 100 / 51) / 10 % 10;
            point[6] = 0;
            smg_buf[7] = (unsigned int)(adc * 100 / 51) % 10 % 10;
        break;
    }
}
void led_proc()
{
    if(smg_mode == 0)
    {
        led_buf[0] = 1;
        led_buf[1] = 0;
    }
    else
    {
        led_buf[0] = 0;
        led_buf[1] = 1;
    }

    if(ms200_count < 100) led_buf[2] = 0;
    else led_buf[2] = 1;
}
void uart_proc()
{
    if(uart_count) return;
    uart_count = 1;
    
    switch(uart_recv)
    {
        case 'A':
            smg_mode = 0;
        break;
        case 'B':
            smg_mode = 1;
        break;
    }

}
void other_proc()
{
    if(temper >= 28.0) relay(1);
    else relay(0);
    if((adc / 51.0) > 3.6) beep(1);
    else beep(0);
}