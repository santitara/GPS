/*******************************************************************************
  RTCC Driver Initialization File

  File Name:
    drv_rtcc_static.c
	
  Summary:
    This file contains source code necessary to initialize the RTCC driver.

  Description:
   RTCC Driver includes initialization and other routines with small footprtint.
 *******************************************************************************/
 
//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
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
//DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Header Includes
// *****************************************************************************
// *****************************************************************************
#include "peripheral/rtcc/plib_rtcc.h"
#include "peripheral/devcon/plib_devcon.h"
#include "peripheral/int/plib_int.h"

// *****************************************************************************
// *****************************************************************************
// Section: RTCC static driver functions
// *****************************************************************************
// *****************************************************************************
void DRV_RTCC_Initialize(void)
{
    PLIB_DEVCON_SystemUnlock(DEVCON_ID_0); /* Unlock System */
 
    /* Initialize RTCC */
    PLIB_RTCC_WriteEnable(RTCC_ID_0); /* Enable writes to RTCC */
    PLIB_RTCC_Disable(RTCC_ID_0); /* Disable clock to RTCC */
    while (PLIB_RTCC_ClockRunningStatus(RTCC_ID_0)); /*Check if clock is disabled */
    PLIB_RTCC_RTCTimeSet(RTCC_ID_0, 0x00000000); /* Set RTCC time */
    PLIB_RTCC_RTCDateSet(RTCC_ID_0, 0x00010100); /* Set RTCC date */  
    PLIB_RTCC_AlarmDisable(RTCC_ID_0); /* Disable alarm */
    while (PLIB_RTCC_AlarmSyncStatusGet(RTCC_ID_0)); /* Wait for disable */
    PLIB_RTCC_AlarmTimeSet(RTCC_ID_0, 0x12105900); /* Set RTCC alarm time */
    PLIB_RTCC_AlarmDateSet(RTCC_ID_0, 0x00010100); /* Set RTCC alarm date */
    PLIB_RTCC_AlarmMaskModeSelect(RTCC_ID_0, RTCC_ALARM_EVERY_SECOND); /* Set RTCC alarm mode */

    /* Setup RTCC Interrupt */
    PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_RTCC); /* Set interrupt source */
    PLIB_INT_VectorPrioritySet(INT_ID_0, INT_VECTOR_RTCC, INT_PRIORITY_LEVEL5); /* Set interrupt priority */
    PLIB_INT_VectorSubPrioritySet(INT_ID_0, INT_VECTOR_RTCC, INT_SUBPRIORITY_LEVEL0); /* Set interrupt sub-priority */ 

}

void DRV_RTCC_Start(void)
{
    PLIB_RTCC_Enable(RTCC_ID_0);
}

void DRV_RTCC_Stop(void)
{
    PLIB_RTCC_Disable(RTCC_ID_0);
}

void DRV_RTCC_ClockOutput(void)
{
    PLIB_RTCC_ClockOutputEnable(RTCC_ID_0);
}

uint32_t DRV_RTCC_TimeGet(void)
{
   return PLIB_RTCC_RTCTimeGet(RTCC_ID_0);
}
uint32_t DRV_RTCC_HourGet(void)
{
   return PLIB_RTCC_RTCHourGet(RTCC_ID_0);
}
uint32_t DRV_RTCC_MinuteGet(void)
{
   return PLIB_RTCC_RTCMinuteGet(RTCC_ID_0);
}
uint32_t DRV_RTCC_SecondGet(void)
{
   return PLIB_RTCC_RTCSecondGet(RTCC_ID_0);
}
void DRV_RTCC_HourSet(uint32_t hour)
{
   PLIB_RTCC_RTCHourSet(RTCC_ID_0, hour);
   return;
}
void DRV_RTCC_MinuteSet(uint32_t minute)
{
    PLIB_RTCC_RTCMinuteSet(RTCC_ID_0, minute);
    return;
}
void DRV_RTCC_SecondSet(uint32_t second)
{
    PLIB_RTCC_RTCSecondSet(RTCC_ID_0, second);
    return;
}
uint32_t DRV_RTCC_DateGet(void)
{
   return PLIB_RTCC_RTCDateGet(RTCC_ID_0);
}

uint32_t DRV_RTCC_AlarmTimeGet(void)
{
   return PLIB_RTCC_AlarmTimeGet(RTCC_ID_0);
}

uint32_t DRV_RTCC_AlarmDateGet(void)
{
   return PLIB_RTCC_AlarmDateGet(RTCC_ID_0);
}

/*******************************************************************************
 End of File
*/