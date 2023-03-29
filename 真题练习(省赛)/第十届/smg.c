#include<STC15.H>
#include<smg.h>

unsigned char tab1[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff,0x8e,0xc1};
unsigned char tab2[] = {1,2,4,8,16,32,64,128};

void smg_show(unsigned char addr,unsigned char dat,unsigned char point)
{
    P0 = 0xff;
    P2 = P2 & 0x1f | 0xe0;
    P2 &= 0x1f;

    P0 = tab2[addr];
    P2 = P2 & 0x1f | 0xc0;
    P2 &= 0x1f;

    P0 = tab1[dat];
    if(point) P0 &= 0x7f; 
    P2 = P2 & 0x1f | 0xe0;
    P2 &= 0x1f;
}