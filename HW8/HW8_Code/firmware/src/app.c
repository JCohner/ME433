/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It 
    implements the logic of the application's state machine and it may call 
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "app.h"
#include "ST7735.h"
#include "i2c.h"
#include <stdio.h>
// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.
    
    Application strings and buffers are be defined outside this structure.
*/

APP_DATA appData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;

    
    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{

    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            bool appInitialized = true;
       
            // do your TRIS and LAT commands here
            TRISBbits.TRISB4 = 0; // B4 is output 
            TRISAbits.TRISA4 = 1; // A4 is input
            LATBbits.LATB4 = 1; 
            
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
            
            LCD_drawAccelBarBackground(BLACK);
            
            if (appInitialized)
            {
            
                appData.state = APP_STATE_SERVICE_TASKS;
            }
            break;
        }

        case APP_STATE_SERVICE_TASKS:
        {
            short imu_data[7];
            short accels[2];
            char data_msg[50];
            int xNewAccel = 64;
            int zNewAccel = 64;
            int xOldAccel;
            int zOldAccel;
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
            break;
        }

        /* TODO: implement your application state machine.*/
        

        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}

 

/*******************************************************************************
 End of File
 */
