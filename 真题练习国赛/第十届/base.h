void init();
unsigned char key_read();
void smg_show(unsigned char pos,dat,point);
void led_show(unsigned char pos,flag);
void uart_init();
void uart_send_byte(unsigned char dat);
void uart_send_string(unsigned char *dat);