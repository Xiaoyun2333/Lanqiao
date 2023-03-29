#include<STC15.H>
#include<key.h>

unsigned char key_read()
{
    unsigned char temp = 0;
    if(P30 == 0) temp = 7;
    if(P31 == 0) temp = 6;
    if(P32 == 0) temp = 5;
    if(P33 == 0) temp = 4;
    return temp;
}