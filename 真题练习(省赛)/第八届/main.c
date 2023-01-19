#include<stc15.h>
#include<absacc.h>
#include<intrins.h>

#include<ds1302.h>
#include<onewire.h>
//========================================
unsigned char tab[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};//0~9，"-"为0xbf
unsigned char w_addr[] = {0x80,0x82,0x84,0x86,0x88,0x8a,0x8c};//ds1302写地址
unsigned char r_addr[] = {0x81,0x83,0x85,0x87,0x89,0x8b,0x8d};//ds1302写地址
unsigned char time[] = {0x50,0x59,0x23,0x00,0x00,0x00,0x00};//初始时间23时59分50秒
unsigned char dspbuf[] = {255,255,255,255,255,255,255,255};//显示缓冲

sbit s7 = P3^0;
sbit s6 = P3^1;
sbit s5 = P3^2;
sbit s4 = P3^3;//四个独立按键映射

unsigned int temp = 0;//预制温度参数

unsigned char cs = 0x00;//现态
unsigned char ns = 0x00;//次态

unsigned int smg_flash = 0;//数码管闪烁表示
unsigned char led_flash = 0;//led闪烁表示

unsigned char led_flash_count = 0;//led闪烁次数

unsigned char set_time[] = {0, 0, 0};//时钟设置参数
unsigned char clk_time[] = {0, 0, 0};//闹钟设置参数
unsigned char clk_time_buf[] = {0, 0, 0};//闹钟参数显示缓冲

unsigned char clk_flag = 0;//闹钟提示标志
unsigned char clk_set_flag = 0;//闹钟设置标志

unsigned char t0_count = 0;
unsigned char t0_h = 0;
unsigned char t0_m = 0;
unsigned char t0_s = 0;//T0计数
/*状态表
0x00 时钟状态
0x10 时钟设置_时	0x11 时钟设置_分 0x12 时钟设置_秒
0x20 闹钟设置_时	0x21 闹钟设置_分 0x22 闹钟设置_秒
0x30 闹钟提示状态
温度显示状态为按下s4，不做编码
*/
//==============初始化======================
void cls();//关闭蜂鸣器、led等设备
//==============定时器0======================
void Timer0Init();//T0初始化50毫秒@11.0592MHz
void timer0_service();//T0中断服务函数
//==============数码管======================
void smg_show_bit(unsigned pos, unsigned char dat);//数码管显示一位
void smg_show_all(unsigned char dat);//数码管全显示
void smg_show();//数码管动态显示
void buf_set_time();//时间显示设置
void buf_set_temp();//温度显示设置
void buf_set_clk();//闹钟显示设置
void Delay1ms();//@11.0592MHz,数码管动态扫描延时
void smg_show_flash(unsigned char pos);//数码管闪烁
//==============电子钟======================
void ds1302_init();//ds1302初始化
void read_time();//读取时间
void time_set(unsigned char pos);//时间设置
void clock_set(unsigned char pos);//闹钟设置
//==============温传感======================
void rd_temperature();//温度转换
void Delay(unsigned int t);//温度转换延迟
//==============led提示=====================
void clk_check();//检查闹钟
void led_flashing();//led闪烁
void led_stop();//led停止闪烁
//==============状态机======================
void statemachine();//状态机
//==============主函数======================
void main()
{
	cls();//关闭蜂鸣器、led等设备
	rd_temperature();//温度转换
	ds1302_init();//ds1302初始化
	buf_set_time();//时间显示设置
	while(1)
	{
		statemachine();//状态机
	}
}
//==============初始化======================
void cls()//关闭蜂鸣器、led等设备
{
	XBYTE[0x8000] = 0xff;
	XBYTE[0xa000] = 0x00;
}
//==============定时器0======================
void Timer0Init()//T0初始化50毫秒@11.0592MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x00;		//设置定时初始值
	TH0 = 0x4C;		//设置定时初始值
	TF0 = 0;		//清除TF0标志
	ET0 = 1;
	EA = 1;
	t0_h = clk_time[0];
	t0_m = clk_time[1];
	t0_s = clk_time[2];
	if(((t0_h == 0) & (t0_m == 0) & (t0_s == 0)) == 1)
	{
		clk_set_flag = 0;
		TR0 = 0;
	}
	else
	{
		clk_set_flag = 1;
		TR0 = 1;
	}
}
void timer0_service() interrupt 1//T0中断服务函数
{
	t0_count++;
	if(t0_count == 20)
	{
		t0_s--;
		t0_count = 0;
		if(t0_s == 255)
		{
			t0_m--;
			t0_s = 59;
			if(t0_m == 255)
			{
				t0_h--;
				t0_m = 59;
				if(t0_h == 255)
				{
					t0_h = 0;
					TR0 = 0;
				}
			}
		}
	}
}
//==============数码管======================
void smg_show_bit(unsigned pos, unsigned char dat)//数码管显示一位
{
	XBYTE[0xe000] = 0xff;
	XBYTE[0xc000] = 0x01 << pos;
	XBYTE[0xe000] = dat;
	Delay1ms();
}
void smg_show_all(unsigned char dat)//数码管全显示
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
	dspbuf[0] = tab[time[2] / 16];
	dspbuf[1] = tab[time[2] % 16];
	dspbuf[2] = 0xbf;
	dspbuf[3] = tab[time[1] / 16];
	dspbuf[4] = tab[time[1] % 16];
	dspbuf[5] = 0xbf;
	dspbuf[6] = tab[time[0] / 16];
	dspbuf[7] = tab[time[0] % 16];
}
void buf_set_temp()//温度显示设置
{
	dspbuf[0] = 255;
	dspbuf[1] = 255;
	dspbuf[2] = 255;
	dspbuf[3] = 255;
	dspbuf[4] = 255;
	dspbuf[5] = tab[temp / 10];
	dspbuf[6] = tab[temp % 10];
	dspbuf[7] = 0xc6;
}
void buf_set_clk()//闹钟显示设置
{
	dspbuf[0] = tab[clk_time_buf[2] / 16];
	dspbuf[1] = tab[clk_time_buf[2] % 16];
	dspbuf[2] = 0xbf;
	dspbuf[3] = tab[clk_time_buf[1] / 16];
	dspbuf[4] = tab[clk_time_buf[1] % 16];
	dspbuf[5] = 0xbf;
	dspbuf[6] = tab[clk_time_buf[0] / 16];
	dspbuf[7] = tab[clk_time_buf[0] % 16];
}
void Delay1ms()//@11.0592MHz,数码管动态扫描延时
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
void smg_show_flash(unsigned char pos)
{
	switch(pos)
	{
		case 0:
			if(smg_flash < 100)
			{
				smg_show_bit(0, 0xff);
				smg_show_bit(1, 0xff);
				smg_flash++;
			}
			else if(smg_flash < 200)
			{
				smg_show_bit(0, dspbuf[0]);
				smg_show_bit(1, dspbuf[1]);
				smg_flash++;
				if(smg_flash == 200){smg_flash = 0;}
			}
			smg_show_bit(2, dspbuf[2]);
			smg_show_bit(3, dspbuf[3]);
			smg_show_bit(4, dspbuf[4]);
			smg_show_bit(5, dspbuf[5]);
			smg_show_bit(6, dspbuf[6]);
			smg_show_bit(7, dspbuf[7]);
		break;
		case 1:
			smg_show_bit(0, dspbuf[0]);
			smg_show_bit(1, dspbuf[1]);
			smg_show_bit(2, dspbuf[2]);
			if(smg_flash < 100)
			{
				smg_show_bit(3, 0xff);
				smg_show_bit(4, 0xff);
				smg_flash++;
			}
			else if(smg_flash < 200)
			{
				smg_show_bit(3, dspbuf[3]);
				smg_show_bit(4, dspbuf[4]);
				smg_flash++;
				if(smg_flash == 200){smg_flash = 0;}
			}
			smg_show_bit(5, dspbuf[5]);
			smg_show_bit(6, dspbuf[6]);
			smg_show_bit(7, dspbuf[7]);
		break;
		case 2:
			smg_show_bit(0, dspbuf[0]);
			smg_show_bit(1, dspbuf[1]);
			smg_show_bit(2, dspbuf[2]);
			smg_show_bit(3, dspbuf[3]);
			smg_show_bit(4, dspbuf[4]);
			smg_show_bit(5, dspbuf[5]);
			if(smg_flash < 100)
			{
				smg_show_bit(6, 0xff);
				smg_show_bit(7, 0xff);
				smg_flash++;
			}
			else if(smg_flash < 200)
			{
				smg_show_bit(6, dspbuf[6]);
				smg_show_bit(7, dspbuf[7]);
				smg_flash++;
				if(smg_flash == 200){smg_flash = 0;}
			}			
		break;
	}
}
//==============电子钟======================
void ds1302_init()//时钟初始化
{
	char i;
	Write_Ds1302_Byte(0x8e,0x00);
	for(i = 0; i < 7; i++)
	{
		Write_Ds1302_Byte(w_addr[i],time[i]);     
	}
	Write_Ds1302_Byte(0x8e,0x80);
}
void read_time()//时钟读取
{
	char i;
	for(i = 0; i < 7; i++)
	{
		time[i] =  Read_Ds1302_Byte(r_addr[i]);
	}
}
void time_set(unsigned char pos)//时间设置
{
	set_time[0] = time[2] /16 * 10 + time[2] % 16;
	set_time[1] = time[1] /16 * 10 + time[1] % 16;
	set_time[2] = time[0] /16 * 10 + time[0] % 16;
	switch(pos)
	{
		case 0:
			if(s5 == 0){smg_show();if(s5 == 0){while(s5 == 0){smg_show();}if(set_time[0] < 23){set_time[0]++;}else{set_time[0] = 0;}}}
			if(s4 == 0){smg_show();if(s4 == 0){while(s4 == 0){smg_show();}if(set_time[0] > 0){set_time[0]--;}else{set_time[0] = 23;}}}
		break;
		case 1:
			if(s5 == 0){smg_show();if(s5 == 0){while(s5 == 0){smg_show();}if(set_time[1] < 59){set_time[1]++;}else{set_time[1] = 0;}}}
			if(s4 == 0){smg_show();if(s4 == 0){while(s4 == 0){smg_show();}if(set_time[1] > 0){set_time[1]--;}else{set_time[1] = 59;}}}
		break;
		case 2:
			if(s5 == 0){smg_show();if(s5 == 0){while(s5 == 0){smg_show();}if(set_time[2] < 59){set_time[2]++;}else{set_time[2] = 0;}}}
			if(s4 == 0){smg_show();if(s4 == 0){while(s4 == 0){smg_show();}if(set_time[2] > 0){set_time[2]--;}else{set_time[2] = 59;}}}
		break;
	}
	time[2] = set_time[0] / 10 *16 + set_time[0] % 10;
	time[1] = set_time[1] / 10 *16 + set_time[1] % 10;
	time[0] = set_time[2] / 10 *16 + set_time[2] % 10;
}
void clock_set(unsigned char pos)//闹钟设置
{
	switch(pos)
	{
		case 0:
			if(s5 == 0){smg_show();if(s5 == 0){while(s5 == 0){smg_show();}if(clk_time[0] < 23){clk_time[0]++;}else{clk_time[0] = 0;}}}
			if(s4 == 0){smg_show();if(s4 == 0){while(s4 == 0){smg_show();}if(clk_time[0] > 0){clk_time[0]--;}else{clk_time[0] = 23;}}}
		break;
		case 1:
			if(s5 == 0){smg_show();if(s5 == 0){while(s5 == 0){smg_show();}if(clk_time[1] < 59){clk_time[1]++;}else{clk_time[1] = 0;}}}
			if(s4 == 0){smg_show();if(s4 == 0){while(s4 == 0){smg_show();}if(clk_time[1] > 0){clk_time[1]--;}else{clk_time[1] = 59;}}}
		break;
		case 2:
			if(s5 == 0){smg_show();if(s5 == 0){while(s5 == 0){smg_show();}if(clk_time[2] < 59){clk_time[2]++;}else{clk_time[2] = 0;}}}
			if(s4 == 0){smg_show();if(s4 == 0){while(s4 == 0){smg_show();}if(clk_time[2] > 0){clk_time[2]--;}else{clk_time[2] = 59;}}}
		break;
	}
	clk_time_buf[2] = clk_time[0] / 10 *16 + clk_time[0] % 10;
	clk_time_buf[1] = clk_time[1] / 10 *16 + clk_time[1] % 10;
	clk_time_buf[0] = clk_time[2] / 10 *16 + clk_time[2] % 10;
}
//==============温传感======================
void rd_temperature()//温度转换
{
	unsigned char LSB,MSB;
	
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0x44);
	
	Delay(100);
	
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0xbe);
	
	LSB = Read_DS18B20();
	MSB = Read_DS18B20();
	
	temp = MSB;
	temp = (temp << 8) | LSB;
	temp >>= 4;
}
void Delay(unsigned int t)//温度转换延迟
{
	while(--t)
	{
		smg_show();
	}
}
//==============led提示=====================
void clk_check()//检查闹钟
{
	if(((time[0] == 0) & (time[1] == 0) & (time[2] == 0)) == 1){clk_flag = 1;}
	if(((clk_set_flag == 1) & (t0_h == 0) & (t0_m == 0) & (t0_s == 0)) == 1){clk_flag = 1;}
}
void led_flashing()//led闪烁
{
	if(clk_flag == 1)
	{
		if(led_flash < 7)
		{
			XBYTE[0x8000] = 0xfe;
			led_flash++;
		}
		else if(led_flash < 14)
		{
			XBYTE[0x8000] = 0xff;
			led_flash++;
			if(led_flash == 14)
			{
				led_flash = 0;
				led_flash_count++;
			}
		}
		if(led_flash_count == 25)
		{
			clk_flag = 0;
			led_flash_count = 0;
			led_flash = 0;
			clk_set_flag = 0;
			ns = 0x00;
		}
	}
}
void led_stop()//led停止闪烁
{
	if((s4 & s5 & s6 & s7) == 0)
	{
		if((s4 & s5 & s6 & s7) == 0)
		{
			while((s4 & s5 & s6 & s7) == 0)
			{
				smg_show();
			}
			clk_flag = 0; 
			clk_set_flag = 0;
			led_flash = 0; 
			XBYTE[0x8000] = 0xff;
			ns = 0x00;
		}
	}
}
//========================================
void statemachine()//状态机
{
	cs = ns;
	switch(cs)
	{
		case 0x00:
			if(s7 == 0){smg_show(); if(s7 == 0){while(s7 == 0){smg_show();}buf_set_time(); ns = 0x10;}}
			if(s6 == 0){smg_show(); if(s6 == 0){while(s6 == 0){smg_show();}buf_set_time(); ns = 0x20;}}
			if(s4 == 0){smg_show(); if(s4 == 0){while(s4 == 0){buf_set_temp();smg_show();rd_temperature();}buf_set_time(); ns = 0x00;}}
			if(clk_flag == 1){ns = 0x30;}
			else{buf_set_time();}
		break;
		case 0x10:
			if(s7 == 0){smg_show(); if(s7 == 0){while(s7 == 0){smg_show();} ns = 0x11;}}
			if(clk_flag == 1){ns = 0x30;}
			else{buf_set_time();}
		break;
		case 0x11:
			if(s7 == 0){smg_show(); if(s7 == 0){while(s7 == 0){smg_show();} ns = 0x12;}}
			if(clk_flag == 1){ns = 0x30;}
			else{buf_set_time();}
		break;
		case 0x12:
			if(s7 == 0){smg_show(); if(s7 == 0){while(s7 == 0){smg_show();} ns = 0x00;}}
			if(clk_flag == 1){ns = 0x30;}
			else{buf_set_time();ds1302_init();}
		break;
		case 0x20:
			if(s6 == 0){smg_show(); if(s6 == 0){while(s6 == 0){smg_show();} ns = 0x21;}}
			if(clk_flag == 1){ns = 0x30;}
			else{TR0 = 0;clk_set_flag = 1;buf_set_time();}
		break;
		case 0x21:
			if(s6 == 0){smg_show(); if(s6 == 0){while(s6 == 0){smg_show();} ns = 0x22;}}
			if(clk_flag == 1){ns = 0x30;}
			else{buf_set_time();}
		break;
		case 0x22:
			if(s6 == 0){smg_show(); if(s6 == 0){while(s6 == 0){smg_show();} ns = 0x00;}}
			if(clk_flag == 1){ns = 0x30;}
			else{buf_set_time();Timer0Init();}
		break;
		case 0x30:
			if((s4 & s5 & s6 & s7) == 0){smg_show(); if((s4 & s5 & s6 & s7) == 0){while((s4 & s5 & s6 & s7) == 0){smg_show();} ns = 0x00;}}
			if(clk_flag == 1){ns = 0x30;}
			else{buf_set_time();}
		break;
	}
	if(cs == 0x00){clk_check();buf_set_time();smg_show();read_time();}
	else if(cs == 0x10){smg_show_flash(0);time_set(0);}
	else if(cs == 0x11){smg_show_flash(1);time_set(1);}
	else if(cs == 0x12){smg_show_flash(2);time_set(2);}
	else if(cs == 0x20){buf_set_clk();smg_show_flash(0);clock_set(0);read_time();}
	else if(cs == 0x21){buf_set_clk();smg_show_flash(1);clock_set(1);read_time();}
	else if(cs == 0x22){buf_set_clk();smg_show_flash(2);clock_set(2);read_time();}
	else if(cs == 0x30){buf_set_time();smg_show();read_time();led_flashing();led_stop();read_time();}
}