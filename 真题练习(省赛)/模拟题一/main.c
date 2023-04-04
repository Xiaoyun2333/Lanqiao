#include<STC15.H>
#include<INTRINS.H>
#include<base.h>
#include<STDIO.H>

unsigned char smg_buf[] = {10,10,10,10,10,10,10,10};
unsigned char led_buf[] = {0,0,0,0,0,0,0,0};
unsigned char point[] = {0,0,0,0,0,0,0,0};
unsigned char uart_send[20];

unsigned char key_count = 0;
unsigned char smg_count = 0;
unsigned char uart_count = 0;
unsigned char pos_count = 0;

unsigned char key_vla,key_old,key_down,key_up;

unsigned char smg_mode = 0;

unsigned char distance = 0;
unsigned char distance_set = 30;

unsigned char ms400_count = 0;
unsigned char led_flash = 0;

void key_proc();
void smg_proc();
void led_proc();
void uart_proc();

void timer0_init();
void timer0_service();

void uart_init();
void uart_service();

void main()
{
    init();
    timer0_init();
	uart_init();
    while (1)
    {
        key_proc();
        smg_proc();
        led_proc();
        uart_proc();
    }   
}

void key_proc()
{
    if(key_count) return;
    key_count = 1;

    key_vla = key_read();
    key_down = key_vla & (key_vla ^ key_old);
    key_up = ~key_vla & (key_vla ^ key_old);
    key_old = key_vla;

    switch (key_up)
    {
        case 4:
            if(++smg_mode == 2 ) smg_mode = 0;
        break;
        case 8:
            if(smg_mode == 0) distance_set = distance;
        break;
        case 9:
            sprintf(uart_send,"Distance:%dcm\r\n",distance);
            uart_send_string(uart_send);
        break;
        case 12:
            if(smg_mode == 1) 
            {
                if((distance_set <= 255) && (distance_set > 245)) distance_set = 255;
                else distance_set += 10;
            }
        break;
        case 16:
            if(smg_mode == 1)
            {
                if(distance_set < 10) distance_set = 0;
                else distance_set -= 10;
            }
        break;
    }
}
void smg_proc()
{
    if(smg_count) return;
    smg_count = 1;

    distance = wave_data();

    switch (smg_mode)
    {
        case 0:
            smg_buf[0] = 11;
            smg_buf[1] = 1;
            if((distance / 100) == 0) smg_buf[5] = 10;
            else smg_buf[5] = distance / 100;
            if((distance / 10 % 10) == 0)
            {
                if((distance / 100) == 0) smg_buf[6] = 10;
                else smg_buf[6] = 0;
            }
            else smg_buf[6] = distance / 10 % 10;
            smg_buf[7] = distance % 10;
        break;
        case 1:
            smg_buf[0] = 11;
            smg_buf[1] = 2;
            if((distance_set / 100) == 0) smg_buf[5] = 10;
            else smg_buf[5] = distance_set / 100;
            if((distance_set / 10 % 10) == 0)
            {
                if((distance_set / 100) == 0) smg_buf[6] = 10;
                else smg_buf[6] = 0;
            }
            else smg_buf[6] = distance_set / 10 % 10;
            smg_buf[7] = distance_set % 10;
        break;
    }
}
void led_proc()
{
    switch (smg_mode)
    {
    case 0:
        led_buf[0] = 1;
        led_buf[1] = 0;
        break;
    case 1:
        led_buf[1] = 1;
        led_buf[0] = 0;
        break;
    }
    if(distance > distance_set) led_flash = 1;
    else led_flash = 0;

    if(ms400_count < 200) led_buf[2] = 0;
    else led_buf[2] = 1;

}
void uart_proc()
{
    if(uart_count) return;
    uart_count = 1;
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
void uart_init()
{
    SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x01;		//串口1选择定时器2为波特率发生器
	AUXR |= 0x04;		//定时器时钟1T模式
	T2L = 0xC7;		//设置定时初始值
	T2H = 0xFE;		//设置定时初始值
	AUXR |= 0x10;		//定时器2开始计时   

    ES = 1;
    EA = 1;
}
void timer0_service() interrupt 1
{
    if(++key_count == 10) key_count = 0;
    if(++smg_count == 500) smg_count = 0;
    if(++uart_count == 200) uart_count = 0;
    if(++pos_count == 8) pos_count = 0;
    if(led_flash)
    {
        if(++ms400_count == 400) ms400_count = 0;
    }
    else ms400_count = 0;
    led_show(pos_count,led_buf[pos_count]);
    smg_show(pos_count,smg_buf[pos_count],point[pos_count]);
}
void Uart1Server() interrupt 4
{
	// if(RI == 1 && uart_recv_index < 20) //串口接收数据
	// {
	// 	uart_recv[uart_recv_index] = SBUF;
	// 	uart_recv_index++;
	// 	RI = 0;
	// }
}