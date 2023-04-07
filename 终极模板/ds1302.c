#include "stc15f2k60s2.h"
#include "intrins.h"
#include "ds1302.h"

sbit SCK = P1^7;
sbit SDA = P2^3;
sbit RST = P1^3;
//
void Write_Ds1302(unsigned  char temp) 
{
	unsigned char i;
	for (i=0;i<8;i++)     	
	{ 
		SCK = 0;
		SDA = temp&0x01;
		temp>>=1; 
		SCK=1;
	}
}   

//
void Write_Ds1302_Byte( unsigned char address,unsigned char dat )     
{
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
 	RST=1; 	_nop_();  
 	Write_Ds1302(address);	
 	Write_Ds1302(dat);		
 	RST=0; 
}

//
unsigned char Read_Ds1302_Byte ( unsigned char address )
{
 	unsigned char i,temp=0x00;
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
 	RST=1;	_nop_();
 	Write_Ds1302(address);
 	for (i=0;i<8;i++) 	
 	{		
		SCK=0;
		temp>>=1;	
 		if(SDA)
 		temp|=0x80;	
 		SCK=1;
	} 
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
	SCK=1;	_nop_();
	SDA=0;	_nop_();
	SDA=1;	_nop_();
	return (temp);			
}

void Read_Ds1302(unsigned char*time)
{
	unsigned char i,temp[3];
	for(i=0;i<3;i++)
	{
		temp[i] = Read_Ds1302_Byte(0x85-2*i);
		time[i] = (temp[i] / 16 * 10) + (temp[i] % 16);
	}
	
}
void Set_Ds1302(unsigned char*time)
{
	unsigned char i,temp[3];
	Write_Ds1302_Byte(0x8e,0x00);
	for(i=0;i<3;i++)
	{
		temp[i] = (time[i] / 10 << 4) | (time[i] % 10); 
		Write_Ds1302_Byte(0x84-2*i,temp[i]);
	}
	Write_Ds1302_Byte(0x8e,0x80);

}