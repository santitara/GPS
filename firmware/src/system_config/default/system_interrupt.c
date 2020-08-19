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

#include <xc.h>
#include <sys/attribs.h>
#include "app.h"
#include "system_definitions.h"
#include "gps/gps_uart.h"

//extern tick_scaler_sirena;
// *****************************************************************************
// *****************************************************************************
// Section: System Interrupt Vector Functions
// *****************************************************************************
// *****************************************************************************
//                      TIMERS 
// ****************************************************************************
void __ISR(_TIMER_2_VECTOR, ipl1AUTO) _IntHandlerDrvTmrInstance1(void) //timer para gestionar la entrada de comandos por botones +,- y D
{
  
  PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_2);
}
 /*****************************************************************************/ 
 void __ISR(_TIMER_3_VECTOR, ipl1AUTO) _IntHandlerDrvTmrInstance2(void)                 //timer para medir la velocidad mediante captador rueda
 {
     //el tick basico de este timer es de 2080 ms aprox
     //no usar este timer esta dedicado para la salida de pulsos de velocidad

    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_3);
 }
// ****************************************************************************

void __ISR(_TIMER_4_VECTOR, ipl1AUTO) _IntHandlerDrvTmrInstance0(void)                  //timer para manejar la escritura de datos en el LCD de 4 digitos
{
    //el tick basico de este timer es 1.25 us 
    
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_4);
}
// ****************************************************************************
//                      UART'S
// ****************************************************************************
 void __ISR(_UART_2_VECTOR, ipl2AUTO) _IntHandlerDrvUsartInstance0(void)                //recibe y genera alertas por UART del modulo de comunicaciones
{
    static uint8_t index = 0;
    if(PLIB_INT_SourceFlagGet(INT_ID_0, INT_SOURCE_USART_2_RECEIVE))
    {
        /* Make sure receive buffer has data availible */
        if (PLIB_USART_ReceiverDataIsAvailable(USART_ID_2))
        {
            /* Get the data from the buffer */
            gps_uart_v.rx_buffer[gps_uart_v.index] = PLIB_USART_ReceiverByteReceive(USART_ID_2);
            if(strstr(gps_uart_v.rx_buffer,"\n")!=NULL)
            {
                gps_uart_v.flag_rx_end = 1;
                //index = 0;
                
            }
           /* else if(strstr(gps_uart_v.rx_buffer,"ERROR")!=NULL)
            {
                gps_uart_v.flag_rx_end = 1;
                index = 0;
            }
            else if(strstr(gps_uart_v.rx_buffer,"+UGNSINF: 1,0")!=NULL)
            {
                gps_uart_v.flag_rx_end = 1;
                index = 0;
            }
            else if(strstr(gps_uart_v.rx_buffer,"+UGNSINF: 1,1")!=NULL)
            {
                gps_uart_v.flag_rx_end = 1;
                index = 0;
            }*/
            else
            {
                gps_uart_v.index++;
            }
            
        }
        PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_2_RECEIVE);
    }
    else if (PLIB_INT_SourceFlagGet(INT_ID_0,INT_SOURCE_USART_2_TRANSMIT))
    {
        PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_USART_2_TRANSMIT);
    }
    
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_USART_2_ERROR);
    
}

 /*****************************************************************************/      
void __ISR(_UART_1_VECTOR, ipl1AUTO) _IntHandlerDrvUsartInstance1(void)
{
    DRV_USART_TasksTransmit(sysObj.drvUsart1);
    DRV_USART_TasksReceive(sysObj.drvUsart1);
    DRV_USART_TasksError(sysObj.drvUsart1);
}            
// ****************************************************************************
//                     SPI
// ****************************************************************************
void __ISR(_SPI_2_VECTOR, ipl1AUTO) _IntHandlerSPIInstance0(void)
{
    DRV_SPI_Tasks(sysObj.spiObjectIdx0);
}
    /*******************************************************************************
 End of File
*/

