/*******************************************************************************
 System Interrupts File

  File Name:
    system_interrupt.c

  Summary:
    Raw ISR definitions.

  Description:
    This file contains a definitions of the raw ISRs required to support the
    interrupt sub-system.

  Summary:
    This file contains source code for the interrupt vector functions in the
    system.

  Description:
    This file contains source code for the interrupt vector functions in the
    system.  It implements the system and part specific vector "stub" functions
    from which the individual "Tasks" functions are called for any modules
    executing interrupt-driven in the MPLAB Harmony system.

  Remarks:
    This file requires access to the systemObjects global data structure that
    contains the object handles to all MPLAB Harmony module objects executing
    interrupt-driven in the system.  These handles are passed into the individual
    module "Tasks" functions to identify the instance of the module to maintain.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2011-2014 released Microchip Technology Inc.  All rights reserved.

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

#include "system/common/sys_common.h"
#include "app.h"
#include "system_definitions.h"

// *****************************************************************************
// *****************************************************************************
// Section: System Interrupt Vector Functions
// *****************************************************************************
// *****************************************************************************


void __ISR(_USB_1_VECTOR, ipl4AUTO) _IntHandlerUSBInstance0(void)
{
    DRV_USBFS_Tasks_ISR(sysObj.drvUSBObject);
}
int count = 0;
int counts_1;
int counts_2;
int counts_1_prev = 0;
int counts_2_prev = 0;
int s1;
int s2;
int r1 = 3;
int r2 = 3;
int e1;
int e2;
int eint1 = 0;
int eint2 = 0;
float u1;
float u2;
float kp1 = 1;
float kp2 = 1;
float ki1 = 1;
float ki2 = 1;
void __ISR(_TIMER_4_VECTOR, IPL4SOFT) Timer4ISR(void) {
  // code for PI control goes here
  /*Encoder Counts Test*/
  /*
  if (count == 0){
    OC1RS = 1200;
    OC4RS = 1200;
    count = 1;
  }
  
  if (TMR5 > 7000) {
    OC1RS = 0;
  }
  if (TMR3 > 7000) {
    OC4RS = 0;
  }
  */
    
    counts_1 = TMR5;
    counts_2 = TMR3;
    
    s1 = counts_1 - counts_1_prev;
    s2 = counts_2 - counts_2_prev;
    
    

    e1 = r1 - s1;
    e2 = r2 - s2;

    eint1 = eint1 + e1;
    eint2 = eint2 + e2;
    
    u1 = kp1 * e1 + ki1 * e1;
    u2 = kp2 * e2 + ki2 * e2;
    
    if (u1 > 100) {
        u1 = 100;
    } else if (u1 < 0){
        u1 = 0;
    } else if (u2 > 100){
        u2 = 100;
    } else if (u2 < 0) {
        u2 = 0;
    }
    
    OC1RS = u1 * 2399;
    OC4RS = u2 * 2399;
    
    counts_1_prev = counts_1;
    counts_2_prev = counts_2;
    
    IFS0bits.T4IF = 0; // clear interrupt flag, last line
}
/*******************************************************************************
 End of File
*/
