#include "stc15f2k60s2.h"
#include "intrins.h"
#include "Base.h"
#include "ds1302.h"
#include "iic.h"
#include "Uart.h"
#include "onewire.h"
#include "stdio.h"
//固定模板变量
unsigned char Key_old,Key_value,Key_down,Key_up;	//按键相关变量
unsigned char Key_slow,Smg_slow,Uart_slow;		//减速变量
unsigned char Smg_buffer[] = {10,10,10,10,10,10,10,10};	//数码管缓存
unsigned char Led_buffer[] = {0,0,0,0,0,0,0,0};		//LED缓存
unsigned char Smg_dot[] = {0,0,0,0,0,0,0,0};		//数码管小数点
unsigned char Smg_pos = 0;		//数码管位置变量
unsigned char Uart_send[15];		//串口发送数据数组
idata unsigned char Uart_rec[15];		//串口接收数据存放数组
idata unsigned char Uart_index;			//串口指针变量
//自定义变量
idata float temper = 0;
idata unsigned char vol = 0;
idata unsigned char light = 0;
idata unsigned char distance = 0;
idata unsigned char time_set[3] = {23,59,50};
idata unsigned char time[3] = {0,0,0};
unsigned char count;
unsigned char mode = 0;
unsigned int freq = 0;
unsigned int t_1000ms = 0;

/*键盘处理函数*/
void Key_Proc()
{
	if(Key_slow)return;
	Key_slow = 1;
	
	Key_value = Key_Read();
	Key_down = Key_value & (Key_value ^ Key_old);
	Key_up = ~Key_value & (Key_value ^ Key_old);
	Key_old = Key_value;

	switch(Key_up)
	{
		case 4:
			count++;
		break;
	}


}
/*信息处理函数*/
void Smg_Proc()
{
	if(Smg_slow)return;
	Smg_slow = 1;
	distance = Dis_Read();
	temper = Read_temper();
	vol = AD_Read(0x01);
	light = AD_Read(0x03);
	DA_Write(vol);
	Read_Ds1302(time);
	EEPROM_Write(&count,0,1);
	Smg_buffer[0] = count / 100;
	Smg_buffer[1] = count / 10 % 10;
	Smg_buffer[2] = count % 10; 
	
}
/*其他处理函数*/
void Other_Proc()
{


}
/*串口处理函数*/
void Uart_Proc()
{
	if(Uart_slow)return;
	Uart_slow = 1;
	if(Uart_index)
	{
	
	
		Uart_index = 0;
	}


}
/*串口中断函数*/
void Uart_Service()interrupt 4
{
	if(RI == 1)
	{
		Uart_rec[Uart_index] = SBUF;
		Uart_index++;
		RI = 0;
	}
}
/*定时器0初始化*/
void Timer0Init(void)		//设置定时器0计数模式,不需要打开中断
{
	AUXR |= 0x80;			//定时器时钟1T模式
	TMOD &= 0xF0;			//设置定时器模式
	TMOD |= 0x05;
	TL0 = 0x00;				//设置定时初始值
	TH0 = 0x00;				//设置定时初始值
	TF0 = 0;				//清除TF0标志
	TR0 = 1;				//定时器0开始计时

}
/*定时器1初始化函数*/
void Timer1Init(void)		//1毫秒@12.000MHz
{
	AUXR |= 0x40;			//定时器时钟1T模式
	TMOD &= 0x0F;			//设置定时器模式
	TL1 = 0x20;				//设置定时初始值
	TH1 = 0xD1;				//设置定时初始值
	TF1 = 0;				//清除TF1标志
	TR1 = 1;				//定时器1开始计时

	
	ET1 = 1;
	EA = 1;
}
//定时器1中断服务函数
void Timer1Service()interrupt 3
{
	if(++Key_slow == 10)Key_slow = 0;
	if(++Smg_slow == 500)Smg_slow = 0;
	if(++Uart_slow == 100)Uart_slow = 0;
	if(++Smg_pos == 8)Smg_pos = 0;
	if(++t_1000ms == 1000)
	{
		TR0 = 0;
		t_1000ms = 0;
		freq = TH0 << 8 | TL0;
		TH0 = TL0 = 0x00;
		TR0 = 1;
	}
	Smg_Bit(Smg_pos,Smg_buffer[Smg_pos],Smg_dot[Smg_pos]);
	Led_Bit(Smg_pos,Led_buffer[Smg_pos]);
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
void Init_Sys()
{
	P0 = 0xff;
	P2 = P2 & 0x1f | 0x80;
	P2 = P2 & 0x1f | 0xe0;
	P2 &= 0x1f;
	
	P0 = 0x00;
	P2 = P2 & 0x1f | 0xa0;
	P2 &= 0x1f;

	Timer0Init();
	Timer1Init();
	UartInit();
	
	EEPROM_Read(&count,0,1);
	Read_temper();
	Delay750ms();
	temper = Read_temper();
	
	Set_Ds1302(time_set);
	vol = AD_Read(0x03);
	DA_Write(vol);
	distance = Dis_Read(); 
}
void main(void)
{
	Init_Sys();
	while(1)
	{
		Key_Proc();
		Smg_Proc();
		Other_Proc();
		Uart_Proc();
	}
}