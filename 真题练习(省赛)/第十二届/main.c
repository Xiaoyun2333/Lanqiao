#include<stc15.h>
#include<intrins.h>
#include<iic.h>
#include<onewire.h>
//********************************************************
sbit c0 = P4^4;
sbit c1 = P4^2;
sbit c2 = P3^5;
sbit c3 = P3^4;

sbit r0 = P3^0;
sbit r1 = P3^1;
sbit r2 = P3^2;
sbit r3 = P3^3;//矩阵行列

unsigned char tab1[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff,0xc6,0x8c,0x88};//c为0xc6,a为0x88,p为0x8c
unsigned char tab2[] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
unsigned char smg_buf[] = {10,10,10,10,10,10,10,10};
unsigned char smg_point[] = {0,0,0,0,0,0,0,0};
unsigned char led_buf[] = {0,0,0,0,0,0,0,0};

unsigned char key_count = 0;
unsigned char smg_count = 0;
unsigned char smg_bit_count = 0;//中断计数变量

unsigned char key_val,key_down,key_old,key_up;//记录按键

unsigned char smg_mode = 0;//数码管显示模式
unsigned char led_mod = 0;//led显示模式
bit dac_mod = 0;//dac工作模式
unsigned char temp_p = 25;//参考温度
unsigned char temp_p_old = 25;//参考温度副本

float temp = 0;//温度
int dac = 0;//dac数据
//********************************************************
void cls();//关闭无关设备
void tiemr0_init();//t0初始化 1ms
void timer0_service();//t0服务函数
//********************************************************
unsigned char key_scan();//按键扫描
void key_proc();//按键处理
//********************************************************
void key_proc();//按键处理
void smg_proc();//数码管处理
void led_proc();//led处理
void ex_proc();//外设处理
//********************************************************
void smg_show(unsigned char wela,unsigned char dula,unsigned char point);//数码管显示
void led_show(unsigned char addr,unsigned char enable);//led显示
//********************************************************
float onewire_read();//读出温度
void Delay750ms();//@12.000MHz
//********************************************************
void da_write(unsigned char dat);//da输出
//********************************************************
void main()
{
    cls();
    onewire_read();
    Delay750ms();
    tiemr0_init();
    while (1)
    {
        ex_proc();
        key_proc();
        smg_proc();
        led_proc();
        
    }
    
}//********************************************************
void cls()//关闭无关设备
{
    P2 = (P2 & 0x1f) | 0x80;
    P0 = 0xff;
    P2 = (P2 & 0x1f) | 0xa0;
    P0 = 0x00;
    P2 = P2 & 0x1f;
}
void tiemr0_init()//t0初始化
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
void timer0_service() interrupt 1//t0服务函数
{ 
    if(++key_count == 10) key_count = 0;
    if(++smg_count == 500) smg_count = 0;
    if(++smg_bit_count == 8) smg_bit_count = 0;
    smg_show(smg_bit_count,smg_buf[smg_bit_count],smg_point[smg_bit_count]);
    led_show(smg_bit_count,led_buf[smg_bit_count]);
}
//********************************************************
unsigned char key_scan()//按键扫描
{
    unsigned char temp = 0;
    c0 = 0; c1 = 1; c2 = 1; c3 = 1;
    if(r3 == 0)temp = 4;
    if(r2 == 0)temp = 5;
    if(r1 == 0)temp = 6;
    if(r0 == 0)temp = 7;
    c0 = 1; c1 = 0; c2 = 1; c3 = 1;
    if(r3 == 0)temp = 8;
    if(r2 == 0)temp = 9;
    if(r1 == 0)temp = 10;
    if(r0 == 0)temp = 11;
    c0 = 1; c1 = 1; c2 = 0; c3 = 1;
    if(r3 == 0)temp = 12;
    if(r2 == 0)temp = 13;
    if(r1 == 0)temp = 14;
    if(r0 == 0)temp = 15;
    c0 = 1; c1 = 1; c2 = 1; c3 = 0;
    if(r3 == 0)temp = 16;
    if(r2 == 0)temp = 17;
    if(r1 == 0)temp = 18;
    if(r0 == 0)temp = 19;
    return temp;

}
void key_proc()//按键处理
{
    if(key_count) return;
    key_count = 1;

    key_val = key_scan();
    key_down = key_val & (key_old ^ key_val);
    key_up = ~key_val & (key_old ^ key_val);
    key_old = key_val;

    switch (key_up)
    {
    case 4:
        if(++smg_mode == 3) smg_mode = 0;
        if(smg_mode == 2) temp_p = temp_p_old;
        break;
    case 5:
        dac_mod = ~dac_mod;
        break;
    case 8:
        if(smg_mode == 1){--temp_p_old;}
        break;
    case 9:
        if(smg_mode == 1){++temp_p_old;}
    }
}
void smg_proc()//数码管处理
{
    if(smg_count) return;
    smg_count = 1;

    temp = onewire_read();

    switch(smg_mode)
    {
        case 0:
            smg_buf[0] = 11;
            smg_buf[4] = temp / 10;
			smg_buf[5] = (unsigned char)temp % 10;
			smg_buf[6] = (unsigned int)(temp * 10) % 10;
			smg_buf[7] = (unsigned int)(temp * 100) % 10;
            smg_point[5] = 1;
            break;
        case 1:
            smg_buf[0] = 12;
            smg_buf[4] = 10;
            smg_buf[5] = 10;
            smg_buf[6] = temp_p_old / 10;
            smg_buf[7] = temp_p_old % 10;
            smg_point[5] = 0;
            break;
        case 2:
            dac = dac * 100 / 51;
            smg_buf[0] = 13;
            smg_buf[4] = 10;
            smg_buf[5] = dac / 100;
            smg_buf[6] = dac / 10 % 10;
            smg_buf[7] = dac % 10 % 10;
            smg_point[5] = 1;
            smg_point[6] = 0;
            break;
    }
}
void led_proc()//led处理
{
    switch (smg_mode)
    {
    case 0:
        led_buf[1] = 1;
        led_buf[2] = 0;
        led_buf[3] = 0;
        break;   
    case 1:
        led_buf[1] = 0;
        led_buf[2] = 1;
        led_buf[3] = 0;
        break;
    case 2:
        led_buf[1] = 0;
        led_buf[2] = 0;
        led_buf[3] = 1;
        break;  
    }
    if(dac_mod == 0) led_buf[0] = 1;
    else led_buf[0] = 0;
}
void ex_proc()//外设处理
{
    if(dac_mod == 0)
    {
        if(temp < temp_p)
        {
            dac = 0; 
            da_write(0);
        }
        else
        {
            dac = 255; 
            da_write(255);
        }
    }
    else
    {
        if(temp <= 20)
        {
            dac = 51;
            da_write(51);
        }
        else if(temp <= 40)
        {
            dac = 7.65 * temp - 102;
            da_write((unsigned char)dac);
        }
        else
        {
            dac = 204;
            da_write(204);
        }
    }
}
//********************************************************
void smg_show(unsigned char wela,unsigned char dula,unsigned char point)//数码管显示
{
    P0 = 0xff;
	P2 = P2 & 0x1f | 0xe0;
	P2 &= 0x1f;

    P0 = tab2[wela];
	P2 = P2 & 0x1f | 0xc0;
	P2 &= 0x1f;

    P0 = tab1[dula];
	if(point) P0 &= 0x7f;
	P2 = P2 & 0x1f | 0xe0;
	P2 &= 0x1f;
}
void led_show(unsigned char addr,unsigned char enable)
{
	static unsigned char temp = 0x00;
	static unsigned char temp_old = 0xff;
	if(enable)
		temp |= 0x01 << addr;
	else
		temp &= ~(0x01 << addr);
	if(temp != temp_old)
	{
		P0 = ~temp;
		P2 = P2 & 0x1f | 0x80;
		P2 &= 0x1f;
		temp_old = temp;
	}
}
//********************************************************
float onewire_read()//读出温度
{
    unsigned char low, high;
    int temp = 0;
    float t = 0;
    init_ds18b20();
    Write_DS18B20(0xcc);
    Write_DS18B20(0x44);

    init_ds18b20();
    Write_DS18B20(0xcc);
    Write_DS18B20(0xbe);

    low = Read_DS18B20();
    high = Read_DS18B20();

    if((high & 0xf8) == 0x00)
    {
        temp |= high;
        t = ((temp << 8) | low) * 0.0625;
    }
    return t;
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
//********************************************************
void da_write(unsigned char dat)//da输出
{
    I2CStart();
    I2CSendByte(0x90);
    I2CWaitAck();
    I2CSendByte(0x43);
    I2CWaitAck();
    I2CSendByte(dat);
    I2CWaitAck();
    I2CStop();
}
//********************************************************