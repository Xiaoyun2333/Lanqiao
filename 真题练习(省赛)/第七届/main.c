#include<stc15.h>
#include<absacc.h>
#include<intrins.h>

#include<onewire.h>
//---------------------------------------------------------------------------
unsigned char tab[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};//0~9，"-"为0xbf
unsigned char dspbuf[] = {255,255,255,255,255,255,255,255};//显示缓冲

sbit s7 = P3^0;
sbit s6 = P3^1;
sbit s5 = P3^2;
sbit s4 = P3^3;//按键
sbit pwm = P3^4;//pwm输出

bit pwm_s = 0;//输出状态，0停止输出
unsigned char pwm_value = 2;//睡眠风20，自然风为30，常风为70

unsigned int temp = 25;//温度参数

unsigned char s = 0;//工作时间
unsigned char count_pwm = 0;//pwm计数
unsigned char count_timer0 = 0;//定时器计数
unsigned char count_10ms = 0;//10毫秒计数

unsigned char cs = 0x00;
unsigned char ns = 0x00;
bit ts = 0;//温度显示状态，0为关闭，1为显示
/*
0x_0睡眠风状态  0x_1自然风状态  0x_2常风状态 
0x0_剩于时间为0 0x1_剩于时间为1 0x2_剩于时间为2
*/
//-----------------------------初始化-----------------------------
void cls();//关闭蜂鸣器、led等设备
//-----------------------------定时器-----------------------------
void Timer0Init();//定时器0 16位自动重载 100us中断
void timer0_service();//定时器0 中断服务函数
//-----------------------------数码管-----------------------------
void smg_show_bit(unsigned pos, unsigned dat);//数码管显示一位
void smg_show_all(unsigned dat);//数码管全部显示
void smg_show();//数码管动态显示
void buf_set_time();//时间显示设置
void buf_set_mode(unsigned mode);//模式显示设置
void buf_set_temp();//温度显示设置
void Delay1ms();//动态扫描1ms延时
//-----------------------------温传感-----------------------------
void rd_temperature(void);//传递温度
void delay(unsigned int i);//传递延时
//-----------------------------状态机-----------------------------
void statemachine();//状态机
//----------------------------------------------------------------
void main()
{
	cls();//关闭蜂鸣器、led等设备
	pwm = 0;
	rd_temperature();
	Timer0Init();
	XBYTE[0x8000] = 0xfe;
	while(1)
	{
		statemachine();//状态机
	}
}
//-------------------------------初始化----------------------------
void cls()
{
	XBYTE[0x8000] = 0xff;
	XBYTE[0xa000] = 0x00;
}
//-------------------------------定时器-----------------------------
void Timer0Init()//定时器0 16位自动重载
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0xAE;		//设置定时初始值
	TH0 = 0xFB;		//设置定时初始值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	
	ET0 = 1;
	EA = 1;
}	
void timer0_service() interrupt 1 //定时器0 中断服务函数
{
	count_timer0++;
	if(s == 0){pwm_s = 0; count_timer0 = 0;}
	else
	{
		pwm_s = 1;
		count_pwm++;
		if(count_timer0 == 100){count_10ms++; count_timer0 = 0;}
		if(count_10ms == 100){s--; count_10ms = 0;}
		if(count_pwm < pwm_value){pwm = 1;}
		else if(count_pwm < 10){pwm = 0;}
		else if(count_pwm == 10){count_pwm = 0;}
	}
	
}
//-------------------------------数码管----------------------------
void smg_show_bit(unsigned pos, unsigned dat)//数码管显示一位
{
	XBYTE[0xe000] = 0xff;
	XBYTE[0xc000] = 0x01 << pos;
	XBYTE[0xe000] = dat;
	Delay1ms();
}
void smg_show_all(unsigned dat)//数码管全部显示
{
	XBYTE[0xe000] = 0xff;
	XBYTE[0xc000] = 0xff;
	XBYTE[0xe000] = dat;
}
void smg_show()//数码管动态显示
{
	unsigned char i;
	for(i = 0; i < 8; i++)
	{
		smg_show_bit(i, dspbuf[i]);
		Delay1ms();
	}
	smg_show_all(0xff);
}
void buf_set_time()//时间显示设置
{
	dspbuf[4] = 0xff;
	dspbuf[5] = tab[s/ 100];
	dspbuf[6] = tab[(s / 10) % 10];
	dspbuf[7] = tab[s % 10];
}
void buf_set_temp()//温度显示设置
{
	dspbuf[4] = 0xff;
	dspbuf[5] = tab[temp / 10];
	dspbuf[6] = tab[temp % 10];
	dspbuf[7] = 0xc6;
}
void buf_set_mode(unsigned mode)//模式显示设置
{
	dspbuf[0] = 0xbf;
	dspbuf[1] = tab[mode];
	dspbuf[2] = 0xbf;
	dspbuf[3] = 0xff;
}
void Delay1ms()//@11.0592MHz
{
	unsigned char i, j;

	_nop_();
	_nop_();
	_nop_();
	i = 11;
	j = 190;
	do
	{
		while (--j);
	} while (--i);
}
//-------------------------------温传感-------------------------------
void rd_temperature(void)//传递温度
{
	unsigned char lsb,msb;
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0x44);
	
	delay(10);
	
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0xbe);
	
	lsb = Read_DS18B20();
	msb = Read_DS18B20();
	
	temp = msb;
	temp = (temp << 8) | lsb;
	temp >>= 4;
}
void delay(unsigned int i)//传递延时
{
	while(--i)
	{
		smg_show();
	}
}
//-------------------------------状态机-------------------------------
void statemachine()//状态机
{
	cs = ns;
	switch((cs & 0x0f))
	{
		case 0x00:
			if(s4 == 0){smg_show();if(s4 == 0){while(s4 == 0){smg_show();};} ns = cs + 1; pwm_value = 3; XBYTE[0x8000] = 0xfd;}
			if(s7 == 0){smg_show();if(s7 == 0){while(s7 == 0){smg_show();};} ts = ~ts;}
		break;
		case 0x01:
			if(s4 == 0){smg_show();if(s4 == 0){while(s4 == 0){smg_show();};} ns = cs + 1; pwm_value = 7; XBYTE[0x8000] = 0xfb;}
			if(s7 == 0){smg_show();if(s7 == 0){while(s7 == 0){smg_show();};} ts = ~ts;} 
		break;
		case 0x02:
			if(s4 == 0){smg_show();if(s4 == 0){while(s4 == 0){smg_show();};} ns = cs - 2; pwm_value = 2; XBYTE[0x8000] = 0xfe;}
			if(s7 == 0){smg_show();if(s7 == 0){while(s7 == 0){smg_show();};} ts = ~ts;}
		break;	
	}
	switch((cs & 0xf0))
	{
		case 0x00:
			if(s5 == 0){smg_show();if(s5 == 0){while(s5 == 0){smg_show();};} ns = cs + 16; s = 60;}
			if(s6 == 0){smg_show();if(s6 == 0){while(s6 == 0){smg_show();};} ns = cs & 0x0f; s = 0; pwm = 0;}
		break;
		case 0x10:
			if(s5 == 0){smg_show();if(s5 == 0){while(s5 == 0){smg_show();};} ns = cs + 16; s = 120;}
			if(s6 == 0){smg_show();if(s6 == 0){while(s6 == 0){smg_show();};} ns = cs & 0x0f; s = 0; pwm = 0;}
		break;
		case 0x20:
			if(s5 == 0){smg_show();if(s5 == 0){while(s5 == 0){smg_show();};} ns = cs - 32; s = 0; pwm = 0;}
			if(s6 == 0){smg_show();if(s6 == 0){while(s6 == 0){smg_show();};} ns = cs & 0x0f; s = 0; pwm = 0;}
		break;	
	}
	if(ts == 1){buf_set_mode(4);buf_set_temp();rd_temperature();}
	else if((cs & 0x0f) == 0x00){buf_set_mode(1);buf_set_time();}
	else if((cs & 0x0f) == 0x01){buf_set_mode(2);buf_set_time();}
	else if((cs & 0x0f) == 0x02){buf_set_mode(3);buf_set_time();}
	smg_show();;
}
//