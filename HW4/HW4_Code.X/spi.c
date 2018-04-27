#include<xc.h>           // processor SFR definitions
#include <stdlib.h>
#include<sys/attribs.h>  // __ISR macro
#define RESOLUTION 0.0032558

// send a byte via spi and return the response
void spi_io(unsigned short o) {
  SPI1BUF = o;
  while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
    ;
  }
  return SPI1BUF;
}

// initialize spi1
void spi_init() {
    SPI1CON = 0;              // turn off the spi module and reset it
    SPI1BUF;                  // clear the rx buffer by reading from it
    ANSELBbits.ANSB14 = 0;     //turn of ansel for sck1
    SPI1BRG = 0x2;            // baud rate to 16 MHz [SPI1BRG = (80000000/(2*desired))-1]
    SPI1STATbits.SPIROV = 0;  // clear the overflow bit
    SPI1CONbits.CKE = 1;      // data changes when clock goes from hi to lo (since CKP is 0)
    SPI1CONbits.MSTEN = 1;    // master operation
    SPI1CONbits.SSEN = 0;
    SPI1CONbits.MODE16 = 1;
    
    //SDI1Rbits.SDI1R = 0b0000; //Config SDI. Input: Func -> Pin. SDI1 -> A1 (0000)
    RPB8Rbits.RPB8R = 0b0011;//Config SDO. Output: Pin -> Func. Pin 17 RB8 -> SDO1 (0011)
    //ODCBbits.ODCB9 = 1;
    TRISBbits.TRISB9 = 0;
    LATBbits.LATB9  = 1; 
    
    SPI1CONbits.ON = 1;       // turn on spi 1
}

void setVoltage(int channel, unsigned int voltage){
    LATBbits.LATB9 = 0;
    
    unsigned short config_bits;
    unsigned short voltage_bits;
    unsigned short bits;
    
    config_bits = (channel << 15) | (0b0111 << 12);
    
    voltage_bits = voltage & 0b1111111111; //gets lowest 10 bits
    voltage_bits = voltage_bits << 2;
    
    bits = config_bits + voltage_bits;
    spi_io(bits);
    
    LATBbits.LATB9 = 1;
}

