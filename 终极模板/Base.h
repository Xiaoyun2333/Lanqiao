#ifndef _BASE_H
#define _BASE_H

unsigned char Key_Read();
void Smg_Bit(unsigned char pos,dat,dot);
void Led_Bit(unsigned char pos,flag);
void Relay(unsigned char flag);
void Beep(unsigned char flag);
unsigned char Dis_Read();
#endif