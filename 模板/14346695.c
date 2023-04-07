#include<STC15.H>
#include<base.h>
#include<ds1302.h>
#include<iic.h>
#include<STDIO.H>
#include<onewire.h>
#include<uart.h>
#include<INTRINS.H>

unsigned char smg_buf[] = {10,10,10,10,10,10,10,10};
unsigned char led_buf[] = {0,0,0,0,0,0,0,0};
unsigned char dot[] = {0,0,0,0,0,0,0,0};
unsigned char uart_recv[10];
unsigned char uart_index = 0;
unsigned char uart_send[10];

unsigned char key_val,key_down,key_up,key_old,key_val_real;
static unsigned char key_up_time = 0;
static unsigned char key_down_time = 0;

unsigned char key_count = 0;
unsigned char smg_count = 0;
unsigned char uart_count = 0;
unsigned char pos_count = 0;
unsigned char ms1_count = 0;
unsigned int ne555_count = 0;

unsigned int ne555_f = 0;

// float temper = 0;

void key_proc();
void smg_proc();
void led_proc();
void uart_proc();
void other_proc();

void timer0_init();
void timer0_service();

void uart_service();

void main()
{
    init();
    // temper_read();
    // Delay750us();
    timer0_init();
    uart_init();
    while(1)
    {
        key_proc();
        smg_proc();
        led_proc();
        uart_proc();
        other_proc();
    }   
}
void timer0_init()
{
    AUXR |= 0x80;			//定时器时钟1T模式
	TMOD &= 0xF0;			//设置定时器模式
	TL0 = 0x20;				//设置定时初始值
	TH0 = 0xD1;				//设置定时初始值
	TF0 = 0;				//清除TF0标志
	TR0 = 1;				//定时器0开始计时

    ET0 = 1;
    EA = 1;
}
// void timer0_init()
// {
//     AUXR |= 0x80;			//定时器时钟1T模式
// 	TMOD &= 0xF0;
//     TMOD |= 0x05;

//     TL0 = 0x00;				
// 	TH0 = 0x00;				
// 	TF0 = 0;				
// 	TR0 = 1;
// }//ne555模式
void timer0_service() interrupt 1
{
    if(++key_count == 10) key_count = 0;
    if(++smg_count == 500) smg_count = 0;
    if(++uart_count == 200) uart_count = 0;
    if(++pos_count == 8) pos_count = 0;

    ms1_count++;

    // if(++ne555_count == 1000)
    // {
    //     TR0 = 0;
    //     ne555_f = TH0 << 8 | TL0;
    //     ne555_count = 0;
    //     TR0 = 1;
    // }

    smg_show(pos_count,smg_buf[pos_count],dot[pos_count]);
    led_show(pos_count,led_buf[pos_count]);
}
void uart_service() interrupt 4
{
    if((RI == 1) && (uart_index < 10))
    {
        uart_recv[uart_index] = SBUF;
        uart_index++;
        RI = 0;
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
    if(!key_val) key_val_real = 0;

    if(key_down) key_down_time = ms1_count;
    if(key_up_time) key_up_time = ms1_count;
}
void smg_proc()
{
    if(smg_count) return;
    smg_count = 1;
}
void led_proc()
{

}
void uart_proc()
{
    if(uart_count) return;
    uart_count = 1;

    if(uart_index)
    {
        uart_index = 0;
    }
}
void other_proc()
{

}
