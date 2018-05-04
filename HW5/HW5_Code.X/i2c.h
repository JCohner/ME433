#ifndef I2C_H    /* Guard against multiple inclusion */
#define I2C_H

void i2c_init(void);
void init_expander(void);
void set_expander(char pin, char level);
char get_expander(void);


void i2c_master_start(void);              // send a START signal
void i2c_master_restart(void);            // send a RESTART signal
void i2c_master_send(unsigned char byte); // send a byte (either an address or data)
unsigned char i2c_master_recv(void);      // receive a byte of data
void i2c_master_ack(int val);             // send an ACK (0) or NACK (1)
void i2c_master_stop(void);               // send a stop


#endif /* I2C_H */

