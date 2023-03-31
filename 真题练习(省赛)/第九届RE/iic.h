
static void I2C_Delay(unsigned char n);
void I2CStart(void);
void I2CStop(void);
void I2CSendByte(unsigned char byt);
unsigned char I2CReceiveByte(void);
unsigned char I2CWaitAck(void);
void I2CSendAck(unsigned char ackbit);
//void dac_write(unsigned char dat);
unsigned char adc_read();
void eeprom_write(unsigned char*EEPROM_String,unsigned char addr,unsigned char num);
unsigned char eeprom_read(unsigned char*EEPROM_String,unsigned char addr,unsigned char num);