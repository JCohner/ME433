// functions to operate the ST7735 on the PIC32
// adapted from https://github.com/sumotoy/TFT_ST7735
// and https://github.com/adafruit/Adafruit-ST7735-Library

// pin connections:
// VCC - 3.3V
// GND - GND
// CS - B7
// RESET - 3.3V
// A0 - B15
// SDA - A1
// SCK - B14
// LED - 3.3V

// B8 is turned into SDI1 but is not used or connected to anything

#include <xc.h>
#include "ST7735.h"
#define X_WIDTH 64
void SPI1_init() {
  SDI1Rbits.SDI1R = 0b0100; // B8 is SDI1
  RPA1Rbits.RPA1R = 0b0011; // A1 is SDO1
  TRISBbits.TRISB7 = 0; // SS is B7
  LATBbits.LATB7 = 1; // SS starts high

  // A0 / DAT pin
  ANSELBbits.ANSB15 = 0;
  TRISBbits.TRISB15 = 0;
  LATBbits.LATB15 = 0;

  SPI1CON = 0; // turn off the spi module and reset it
  SPI1BUF; // clear the rx buffer by reading from it
  SPI1BRG = 0; // baud rate to 12 MHz [SPI1BRG = (48000000/(2*desired))-1]
  SPI1STATbits.SPIROV = 0; // clear the overflow bit
  SPI1CONbits.CKE = 1; // data changes when clock goes from hi to lo (since CKP is 0)
  SPI1CONbits.MSTEN = 1; // master operation
  SPI1CONbits.ON = 1; // turn on spi1
}

unsigned char spi_io(unsigned char o) {
  SPI1BUF = o;
  while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
    ;
  }
  return SPI1BUF;
}

void LCD_command(unsigned char com) {
    LATBbits.LATB15 = 0; // DAT
    LATBbits.LATB7 = 0; // CS
    spi_io(com);
    LATBbits.LATB7 = 1; // CS
}

void LCD_data(unsigned char dat) {
    LATBbits.LATB15 = 1; // DAT
    LATBbits.LATB7 = 0; // CS
    spi_io(dat);
    LATBbits.LATB7 = 1; // CS
}

void LCD_data16(unsigned short dat) {
    LATBbits.LATB15 = 1; // DAT
    LATBbits.LATB7 = 0; // CS
    spi_io(dat>>8);
    spi_io(dat);
    LATBbits.LATB7 = 1; // CS
}

void LCD_init() {
  SPI1_init();
  int time = 0;
  LCD_command(ST7735_SWRESET);//software reset
  time = _CP0_GET_COUNT();
  while (_CP0_GET_COUNT() < time + 48000000/2/2) {}

	LCD_command(ST7735_SLPOUT);//exit sleep
  time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/200) {}

	LCD_command(ST7735_FRMCTR1);//Frame Rate Control (In normal mode/Full colors)
	LCD_data(0x01);
	LCD_data(0x2C);
	LCD_data(0x2D);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command(ST7735_FRMCTR2);//Frame Rate Control (In normal mode/Full colors)
	LCD_data(0x01);
	LCD_data(0x2C);
	LCD_data(0x2D);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command(ST7735_FRMCTR3);//Frame Rate Control (In normal mode/Full colors)
	LCD_data(0x01);
	LCD_data(0x2C);
	LCD_data(0x2D);
	LCD_data(0x01);
	LCD_data(0x2C);
	LCD_data(0x2D);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command(ST7735_INVCTR);//display inversion
	LCD_data(0x07);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command(ST7735_PWCTR1);
	LCD_data(0x0A);//4.30 - 0x0A
	LCD_data(0x02);//0x05
	LCD_data(0x84);//added auto mode
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command(ST7735_PWCTR2);
	LCD_data(0xC5);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command( ST7735_PWCTR3);
	LCD_data(0x0A);
	LCD_data(0x00);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command( ST7735_PWCTR4);
	LCD_data(0x8A);
	LCD_data(0x2A);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command( ST7735_PWCTR5);
	LCD_data(0x8A);
	LCD_data(0xEE);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command(ST7735_VMCTR1);
	LCD_data(0x0E);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command(ST7735_INVOFF);

	LCD_command(ST7735_MADCTL);
	LCD_data(0xC8);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command(ST7735_COLMOD);
	LCD_data(0x05);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command(ST7735_CASET);
	LCD_data(0x00);
	LCD_data(0x00);
	LCD_data(0x00);
	LCD_data(0x7F);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command(ST7735_RASET);
	LCD_data(0x00);
	LCD_data(0x00);
	LCD_data(0x00);
	LCD_data(0x9F);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command(ST7735_GMCTRP1);
	LCD_data(0x02);
	LCD_data(0x1C);
	LCD_data(0x07);
	LCD_data(0x12);
	LCD_data(0x37);
	LCD_data(0x32);
	LCD_data(0x29);
	LCD_data(0x2D);
	LCD_data(0x29);
	LCD_data(0x25);
	LCD_data(0x2B);
	LCD_data(0x39);
	LCD_data(0x00);
	LCD_data(0x01);
	LCD_data(0x03);
	LCD_data(0x10);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command(ST7735_GMCTRN1);
	LCD_data(0x03);
	LCD_data(0x1D);
	LCD_data(0x07);
	LCD_data(0x06);
	LCD_data(0x2E);
	LCD_data(0x2C);
	LCD_data(0x29);
	LCD_data(0x2D);
	LCD_data(0x2E);
	LCD_data(0x2E);
	LCD_data(0x37);
	LCD_data(0x3F);
	LCD_data(0x00);
	LCD_data(0x00);
	LCD_data(0x02);
	LCD_data(0x10);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command(ST7735_NORON);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/100) {}

	LCD_command(ST7735_DISPON);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/10) {}

	LCD_command(ST7735_MADCTL); // rotation
    LCD_data(MADCTL_MX | MADCTL_MY | MADCTL_RGB);
}

void LCD_drawPixel(unsigned short x, unsigned short y, unsigned short color) {
  // check boundary
  LCD_setAddr(x,y,x+1,y+1);
  LCD_data16(color);
}

void LCD_setAddr(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1) {
  LCD_command(ST7735_CASET); // Column
  LCD_data16(x0);
	LCD_data16(x1);

	LCD_command(ST7735_RASET); // Page
	LCD_data16(y0);
	LCD_data16(y1);

	LCD_command(ST7735_RAMWR); // Into RAM
}

void LCD_clearScreen(unsigned short color) {
  int i;
  LCD_setAddr(0,0,_GRAMWIDTH,_GRAMHEIGH);
	for (i = 0;i < _GRAMSIZE; i++){
		LCD_data16(color);
	}
}

void LCD_drawChar(unsigned short x0, unsigned short y0, unsigned char character, unsigned short color1, unsigned short color2 ){
    int row = (int) ((char) character - 0x20); //row number of requested char in ASCII table in .h file
    int col = 0;
    for (col = 0; col < 5; col++){
        char pixel = ASCII[row][col];
        int ii;
        for (ii = 0; ii < 8; ii++){
            if((pixel >> ii & 0b1)){
                LCD_drawPixel((x0 + col), (y0+ii), color1);
            } else {
                LCD_drawPixel((x0 + col), (y0+ii), color2);
            }
        }
    }
}

void LCD_drawString(unsigned short x0, unsigned short y0, const char message[], unsigned short color1, unsigned short color2){
    int i = 0; 
    while(message[i]){
        LCD_drawChar((x0 + (5*i)), (y0) ,message[i], color1, color2); 
        i++;
    }
}

void LCD_drawBar(unsigned short x0, unsigned short y0, unsigned int progress, unsigned short color1, unsigned short color2){
    int ii;
    int jj;
    for (ii = 0; ii < progress; ii++){
        for (jj = 0; jj < 8; jj++){
            LCD_drawPixel((x0 + ii) , (y0 + jj), color1);
        } 
    }
    for (ii = progress; ii < 100; ii++){
      for (jj = 0; jj < 8; jj++){
            LCD_drawPixel((x0 + ii) , (y0 + jj), color2);
        }   
    }
}

void LCD_drawAccelBarHoriz(int progress, unsigned short color1, unsigned short color2){// x0: x origin, y0: y origin, 
    int ii;
    int jj;
    int x0 = 64;
    int y0 = 76;
    if (progress < 64){ //negative progress
        for (ii = 64; ii >= progress; ii--){
            for (jj = 0; jj < 8; jj++){
                LCD_drawPixel((ii) , (y0 + jj), color1);
            } 
        }
        for (ii = progress; ii >= 0; ii--){
            for (jj = 0; jj < 8; jj++){
                LCD_drawPixel((ii) , (y0 + jj), color2);
            } 
        }
        for (ii = 65; ii < 128; ii++){
            for (jj = 0; jj < 8; jj++){
                LCD_drawPixel((ii) , (y0 + jj), color2);
            }   
        }
    } else {
        for (ii = 64; ii <= progress; ii++){
            for (jj = 0; jj < 8; jj++){
                LCD_drawPixel((ii) , (y0 + jj), color1);
            } 
        }
        for (ii = progress; ii <= 128; ii++){
            for (jj = 0; jj < 8; jj++){
                LCD_drawPixel((ii) , (y0 + jj), color2);
            } 
        }
        for (ii = 64; ii > 0; ii--){
            for (jj = 0; jj < 8; jj++){
                LCD_drawPixel((ii) , (y0 + jj), color2);
            }   
        }
    }
}

void LCD_drawAccelBarVert(int progress, unsigned short color1, unsigned short color2){
    int ii;
    int jj;
    int x0 = 60;
    int y0 = 76;
    if (progress < 80){ //negative progress
        for (ii = 80; ii >= progress; ii--){
            for (jj = 0; jj < 8; jj++){
                LCD_drawPixel((x0 + jj) , (ii), color1);
            } 
        }
        for (ii = progress; ii >= 0; ii--){
            for (jj = 0; jj < 8; jj++){
                LCD_drawPixel((x0 + jj) , (ii), color2);
            } 
        }
        for (ii = 81; ii < 160; ii++){
            for (jj = 0; jj < 8; jj++){
                LCD_drawPixel((x0 + jj) , (ii), color2);
            }   
        }
    } else {
        for (ii = 80; ii <= progress; ii++){
            for (jj = 0; jj < 8; jj++){
                LCD_drawPixel((x0 + jj) , ii, color1);
            } 
        }
        for (ii = progress; ii <= 160; ii++){
            for (jj = 0; jj < 8; jj++){
                LCD_drawPixel((x0 + jj) ,ii, color2);
            } 
        }
        for (ii = 80; ii > 0; ii--){
            for (jj = 0; jj < 8; jj++){
                LCD_drawPixel((x0 + jj) , ii, color2);
            }   
        }
    }
}

void LCD_drawAccelBarBackground(unsigned short color1){// x0: x origin, y0: y origin, 
    int ii;
    int jj;
    for (ii = 0; ii < 129; ++ii){
        for (jj = 0; jj < 8; jj++){
                LCD_drawPixel(ii, (76 + jj), color1);
            } 
    }
    for (ii = 0; ii < 161; ++ii){
        for (jj = 0; jj < 8; jj++){
                LCD_drawPixel((60 + jj), (ii), color1);
            } 
    }
}

void LCD_drawAccelBarUpdate(int xNewAccel, int yNewAccel, int xOldAccel, int yOldAccel, unsigned short color1, unsigned short color2){
    char xState;
    char yState;
    
    if (xNewAccel > xOldAccel) {
        if (xOldAccel > 64) {
            xState = 'a';  //new accel greater than old, both greater than 0
        } else if ((xOldAccel < 64) && (xNewAccel > 64)){
            xState = 'b';  //need to 0 negative indication and just go from 0 to to newAccel
        } else if (xNewAccel < 64) {
            xState = 'c'; //both are less than 0 needs to be plotted closer to origin
        }
    } else { // (xNewAccel < xOldAccel)
        if (xNewAccel > 64) {
            xState = 'd';  //new accel greater than old, both greater than 0
        } else if ((xOldAccel > 64) && (xNewAccel < 64)){
            xState = 'e';  //need to 0 negative indication and just go from 0 to to newAccel
        } else if (xOldAccel < 64) {
            xState = 'f'; //both are less than 0 needs to be plotted closer to origin
        }
    }
    
    if (yNewAccel > yOldAccel) {
        if (yOldAccel > 0) {
            yState = 'a';  //new accel greater than old, both greater than 0
        } else if ((yOldAccel < 0) && (yNewAccel > 0)){
            yState = 'b';  //need to 0 negative indication and just go from 0 to to newAccel
        } else if (yNewAccel < 0) {
            yState = 'c'; //both are less than 0 needs to be plotted closer to origin
        }
    } else { // (yNewAccel < yOldAccel)
        if (yNewAccel > 0) {
            yState = 'd';  //new accel greater than old, both greater than 0
        } else if ((yOldAccel > 0) && (yNewAccel < 0)){
            yState = 'e';  //need to 0 negative indication and just go from 0 to to newAccel
        } else if (yOldAccel < 0) {
            yState = 'f'; //both are less than 0 needs to be plotted closer to origin
        }
    }
    
    int ii;
    int jj;
    switch (xState) {
        case 'a': //xNewAccel > xOldAccel > 64
            for (ii = xOldAccel; ii <= xNewAccel; ++ii){
                for (jj = 0; jj < 8; jj++){
                    LCD_drawPixel(ii, (76 + jj), color1);
                } 
            }
            break;
        case 'b': //xNewAccel > 64 > xOldAccel
            for (ii = xOldAccel; ii <= 64; ++ii){
                for (jj = 0; jj < 8; jj++){
                    LCD_drawPixel(ii, (76 + jj), color2);
                } 
            }
            for (ii = 64; ii <= xNewAccel; ++ii){
                for (jj = 0; jj < 8; jj++){
                    LCD_drawPixel(ii, (76 + jj), color1);
                } 
            }
            break;
        case 'c': //64 > xNewAccel > xOldAccel
            for (ii = xOldAccel; ii <= xNewAccel; ++ii){
                for (jj = 0; jj < 8; jj++){
                    LCD_drawPixel(ii, (76 + jj), color2);
                } 
            }
            break;
        case 'd': //xOldAccel > xNewAccel > 64
            for (ii = xNewAccel; ii <= xOldAccel; ++ii){
                for (jj = 0; jj < 8; jj++){
                    LCD_drawPixel(ii, (76 + jj), color2);
                } 
            }
            break;
        case 'e': //xOldAccel > 64 > xNewAccel
            for (ii = xOldAccel; ii >= 64; --ii){
                for (jj = 0; jj < 8; jj++){
                    LCD_drawPixel(ii, (76 + jj), color2);
                } 
            }
            for (ii = 64; ii >= xNewAccel; --ii){
                for (jj = 0; jj < 8; jj++){
                    LCD_drawPixel(ii, (76 + jj), color1);
                } 
            }
            break;
        case 'f': //64 > xOldAccel > xNewAccel
            for (ii = xOldAccel; ii <= xNewAccel; --ii){
                for (jj = 0; jj < 8; jj++){
                    LCD_drawPixel(ii, (76 + jj), color1);
                } 
            }
            break;
    }
    
    
    
}