#include<STC15.H>
#include<base.h>

unsigned char tab[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff,0xbf,0xc6};
unsigned char tab2[] = {1,2,4,8,16,32,64,128};

void init()
{
    P2 = P2 & 0x1f | 0x80;
    P0 = 0xff;
    P2 &= 0x1f;

    P2 = P2 & 0x1f | 0xa0;
    P0 = 0x00;
    P2 &= 0x1f;
}
unsigned char key_read()
{
    unsigned char temp = 0;
    if(P33 == 0) temp = 4;
    if(P32 == 0) temp = 5;
    if(P31 == 0) temp = 6;
    if(P30 == 0) temp = 7;
    return temp;
}
void led_show(unsigned char pos,flag)
{
    static unsigned char temp = 0x00;
    static unsigned char temp_old = 0xff;

    if(flag) temp |= 0x01 << pos; 
    else temp &= ~(0x01 << pos);

    if(temp != temp_old)
    {
        P0 = ~temp;
        P2 = P2 = P2 & 0x1f | 0x80;
        P2 &= 0x1f;
        temp_old = temp;
    }
}
void smg_show(unsigned char pos,dat,point)
{
    P0 = 0xff;
    P2 = P2 & 0x1f | 0xe0;
    P2 &= 0x1f;

    P0 = tab2[pos];
    P2 = P2 & 0x1f | 0xc0;
    P2 &= 0x1f;

    P0 = tab[dat];
    if(point) P0 &= 0x7f;
    P2 = P2 & 0x1f | 0xe0;
    P2 &= 0x1f;
}
