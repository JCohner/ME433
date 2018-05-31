#include<xc.h>           // processor SFR definitions
#include<math.h>
#include<stdio.h>
#include "ST7735.H"
#include "I2C.H"
#include<sys/attribs.h>  // __ISR macro

// DEVCFG0
#pragma config DEBUG = OFF // no debugging
#pragma config JTAGEN = OFF // no jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // no write protect
#pragma config BWP = OFF // no boot write protect
#pragma config CP = OFF // no code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // turn off secondary oscillator
#pragma config IESO = OFF // no switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // disable secondary osc
#pragma config FPBDIV = DIV_1 // divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // do not enable clock switch
#pragma config WDTPS = PS1 // use slowest wdt if this doesnt work use PS1048576
#pragma config WINDIS = OFF // wdt no window mode
#pragma config FWDTEN = OFF // wdt disabled
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz crystal
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz
#pragma config UPLLIDIV = DIV_2 // divider for the 8MHz input clock, then multiplied by 12 to get 48MHz for USB
#pragma config UPLLEN = ON // USB clock on

// DEVCFG3
#pragma config USERID = 0b0011011010011101 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations
#pragma config FUSBIDIO = OFF // USB pins controlled by USB module
#pragma config FVBUSONIO = OFF // USB BUSON controlled by USB module

int counter = 0;
    
void __ISR(_TIMER_2_VECTOR, IPL5SOFT) Controller(void) {  // _TIMER_2_VECTOR = 8
    LATBbits.LATB4 = !LATBbits.LATB4; 
    IFS0bits.T2IF = 0;
}

int main() {

    __builtin_disable_interrupts();

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;

    // do your TRIS and LAT commands here
    TRISBbits.TRISB4 = 0; // B4 is output 
    TRISAbits.TRISA4 = 1; // A4 is input
    LATBbits.LATB4 = 1; 
    
    __builtin_enable_interrupts();

    /*Configure interrupt timer, Timer2*/
  __builtin_disable_interrupts(); // disable interrupts while configuring controller interrupt
    PR2 = 37499;                  //             set period register
    TMR2 = 0;                       //             initialize count to 0
    T2CONbits.TCKPS = 0b111;            //             set prescaler to 256
    T2CONbits.ON = 1;               //             turn on Timer1
    IPC2bits.T2IP = 5;              // INT step 4: priority
    IPC2bits.T2IS = 0;              //             subpriority
    IFS0bits.T2IF = 0;              // INT step 5: clear interrupt flag
    IEC0bits.T2IE = 1;              // INT step 6: enable interrupt
  __builtin_enable_interrupts();  // INT step 7: enable interrupts at CPU
    
  
    LCD_init();
    LCD_clearScreen(WHITE);
    i2c_init();
    init_imu();
    short imu_data[7];
    short accels[2];
    char data_msg[50];
    int xNewAccel = 64;
    int zNewAccel = 64;
    int xOldAccel;
    int zOldAccel;
    LCD_drawAccelBarBackground(BLACK);
    
    while(1) {
        int ii;
        
        xOldAccel = xNewAccel;
        zOldAccel = zNewAccel;
        
        xNewAccel = 64 + ((((float) read_Xaccel()) / 16384.0) * 64) ;
        zNewAccel = 80 + ((((float) read_Zaccel()) / 16384.0) * (-80));
        
        //xNewAccel = ((((float) imu_data[4]) / 32768.0) * 100); //normalize to 64 //possibly scale up to 128 because will never experience 2gs
        //yNewAccel = ((((float) imu_data[5]) / 32768.0) * 100); // normalize to 80 //possibly scale up to 160 because will never experience 2gs
        //LCD_drawAccelBarUpdate(xNewAccel, zNewAccel, xOldAccel, zOldAccel, CYAN, BLACK);
       
        sprintf(data_msg,"ax: %+5.5hd", xNewAccel);
        LCD_drawString(5,5, data_msg, CYAN, BLACK);
        sprintf(data_msg,"az: %+5.5hd", zNewAccel);
        LCD_drawString(5,13, data_msg, BLACK, WHITE);
        
        LCD_drawAccelBarHoriz(xNewAccel,MAGENTA, BLACK);
        LCD_drawAccelBarVert(zNewAccel,MAGENTA,BLACK);
        
        _CP0_SET_COUNT(0);
        while (_CP0_GET_COUNT() < 1200000){;}
        
    }
}
