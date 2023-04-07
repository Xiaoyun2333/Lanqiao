#ifndef _IIC_H
#define _IIC_H

unsigned char AD_Read(unsigned char addr);
void DA_Write(unsigned char dat);
void EEPROM_Write(unsigned char*EEPROM_String,unsigned char addr,unsigned char num);
void EEPROM_Read(unsigned char*EEPROM_String,unsigned char addr,unsigned char num);
//unsigned char Dis_Read();
#endif