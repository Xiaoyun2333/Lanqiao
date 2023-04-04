void init();
unsigned char key_read();
void led_show(unsigned char pos,flag);
void smg_show(unsigned char pos,dat,point);
void uart_send_byte(unsigned char dat);
void uart_send_string(unsigned char *dat);
void wave_init();
unsigned char wave_data();
void Delay12us();