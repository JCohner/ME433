#include<xc.h>           // processor SFR definitions
#include <stdlib.h>
#include "i2c.h"
#include<sys/attribs.h>  // __ISR macro

#define WRITE_ADDRESS 0b11010100
#define READ_ADDRESS  0b11010101

// I2C Master utilities, 100 kHz, using polling rather than interrupts
// The functions must be callled in the correct order as per the I2C protocol
// Change I2C1 to the I2C channel you are using
// I2C pins need pull-up resistors, 2k-10k

void i2c_init(void) {
//Turn off primary functionality so B3 can be SCL2 and B2 SDA2     
ANSELBbits.ANSB2 = 0; //turn off analog
ANSELBbits.ANSB3 = 0; //turn off analog

I2C2BRG = 0x03A;            // I2CBRG = [1/(2*Fsck) - PGD]*Pblck - 2
                                    // look up PGD for your PIC32
I2C2CONbits.ON = 1;               // turn on the I2C1 module
}

void set_expander(char pin, char level){
    i2c_master_start();
    i2c_master_send(WRITE_ADDRESS);
    i2c_master_send(0x0A); //LAT reg
    
    unsigned char pin_info;
    
    pin_info = (level & 0x01) << pin;
    i2c_master_send(pin_info);
    i2c_master_stop();
}
unsigned char read_WHOAMI(void){
    i2c_master_start();
    
    i2c_master_send(WRITE_ADDRESS);
    i2c_master_send(0x0F);
    i2c_master_restart();
    
    i2c_master_send(READ_ADDRESS);
    unsigned char whoami_response = i2c_master_recv();
    i2c_master_stop();
    
    return whoami_response;
}

void read_IMU(short * data_array) {
    i2c_master_start();
    i2c_master_send(WRITE_ADDRESS);
    i2c_master_send(0x20);
    i2c_master_restart();
    i2c_master_send(READ_ADDRESS);
    short highByte;
    char lowByte;
    short combined;
    int ii;
    for (ii = 0; ii <= 7; ii++){
        lowByte =  i2c_master_recv();
        i2c_master_ack(0);
        highByte = i2c_master_recv();
        
        combined = (highByte << 8) | lowByte;
        data_array[ii] = combined;
        
        if (ii < 7){
            i2c_master_ack(0);
        } else if (ii == 7){
            i2c_master_ack(1);
        }
    }
    i2c_master_stop();
    return;
}

/*
void read_A(short * accels){   
   short imu_data[7];
   int ii;
   for (ii = 0; ii < 10; ++ii){
      read_IMU(imu_data);
      accels[0] += (short)((float) imu_data[4] / 10.0); //X_ACCEL
      accels[1] += (short)((float) imu_data[5] / 10.0); //Y_ACCEL
      _CP0_SET_COUNT(0);
      while (_CP0_GET_COUNT() < 1200000){;}
   }
   
   return;
}
*/

short read_Xaccel(void){
    i2c_master_start();
    i2c_master_send(WRITE_ADDRESS);
    i2c_master_send(0x28);
    i2c_master_restart();
    i2c_master_send(READ_ADDRESS);
    unsigned char x_accel_data_low = i2c_master_recv();
    i2c_master_ack(0);
    unsigned char x_accel_data_high = i2c_master_recv();
    i2c_master_ack(1);
    i2c_master_stop();
    short x_accel_data = (x_accel_data_high << 8) | x_accel_data_low;
    return x_accel_data;
}

short read_Zaccel(void){
    i2c_master_start();
    i2c_master_send(WRITE_ADDRESS);
    i2c_master_send(0x2C);
    i2c_master_restart();
    i2c_master_send(READ_ADDRESS);
    unsigned char z_accel_data_low = i2c_master_recv();
    i2c_master_ack(0);
    unsigned char z_accel_data_high = i2c_master_recv();
    i2c_master_ack(1);
    i2c_master_stop();
    short z_accel_data = (z_accel_data_high << 8) | z_accel_data_low;
    return z_accel_data;
}
void init_imu(void){
    i2c_master_start(); //S
    i2c_master_send(WRITE_ADDRESS); //OP + W
    
    i2c_master_send(0x10); //ctrl_1XL REG ADDR
    i2c_master_send(0b01010000); //data value to ctrl1
    i2c_master_send(0b01010000); //data value to ctrl2
    
    i2c_master_stop();
}
// Start a transmission on the I2C bus
void i2c_master_start(void) {
    I2C2CONbits.SEN = 1;            // send the start bit
    while(I2C2CONbits.SEN) { ; }    // wait for the start bit to be sent
}

void i2c_master_restart(void) {
    I2C2CONbits.RSEN = 1;           // send a restart
    while(I2C2CONbits.RSEN) { ; }   // wait for the restart to clear
}

void i2c_master_send(unsigned char byte) { // send a byte to slave
  I2C2TRN = byte;                   // if an address, bit 0 = 0 for write, 1 for read
  while(I2C2STATbits.TRSTAT) { ; }  // wait for the transmission to finish
  if(I2C2STATbits.ACKSTAT) {        // if this is high, slave has not acknowledged
    // ("I2C2 Master: failed to receive ACK\r\n");
  }
}

unsigned char i2c_master_recv(void) { // receive a byte from the slave
    I2C2CONbits.RCEN = 1;             // start receiving data
    while(!I2C2STATbits.RBF) { ; }    // wait to receive the data
    return I2C2RCV;                   // read and return the data
}

void i2c_master_ack(int val) {        // sends ACK = 0 (slave should send another byte)
                                      // or NACK = 1 (no more bytes requested from slave)
    I2C2CONbits.ACKDT = val;          // store ACK/NACK in ACKDT
    I2C2CONbits.ACKEN = 1;            // send ACKDT
    while(I2C2CONbits.ACKEN) { ; }    // wait for ACK/NACK to be sent
}

void i2c_master_stop(void) {          // send a STOP:
  I2C2CONbits.PEN = 1;                // comm is complete and master relinquishes bus
  while(I2C2CONbits.PEN) { ; }        // wait for STOP to complete
}