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
#include "gps/gps_config.h"
#include "tronic/tronic_uart.h"

#define OFF_DELAY_LED 14
//extern tick_scaler_sirena;
// *****************************************************************************
// *****************************************************************************
// Section: System Interrupt Vector Functions
// *****************************************************************************
// *****************************************************************************
//                      TIMERS 
// ****************************************************************************

void led_blink(uint8_t num_blink);
/**
 * @brief timer 1 interrupt each 10 ms
 * @param[in] none
 * @param[out]none
 * @return none
 */
void __ISR(_TIMER_2_VECTOR, ipl1AUTO) _IntHandlerDrvTmrInstance1(void) //timer para gestionar la entrada de comandos por botones +,- y D
{
    //tick of timer is aproximately 10 ms/tick
    static uint8_t ms_10 = 0;
    static uint8_t ms_100 = 0; 
    static uint8_t ms_1000 = 0;
    static uint8_t ms_blink = 0;
    static uint8_t ms_gps_report = 0;
    static uint8_t ms_bt_sent = 0;
    ms_10++;
    //increase up to 100ms
    if(ms_10 == 10)
    {
        ms_100++;
        ms_10=0;
        ms_blink++;
        ms_gps_report++;
        ms_bt_sent++;
        if(ms_gps_report >= GPS_REPORTS_FREQ)
        {
            gps_config_v.flag_gps_report = 1;
            ms_gps_report = 0;
        }
        if(ms_bt_sent >= BT_SENT_FREQ)
        {
            gps_config_v.flag_bt_sent = 1;
            ms_bt_sent = 0;
        }
        if(ms_blink == 2)
        {
            led_blink(led_control_v.num_blink);
            ms_blink = 0;
        }
        
        else if(ms_blink >2 && ms_blink <=8)
        {
            PLIB_PORTS_PinWrite (PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_9,0);
        }
        else if(ms_blink >8)
        {
            ms_blink = 0;
        }
        
    }
    //increse up to 1 sec
    if(ms_100 == 10)
    {
        ms_1000++;
        ms_100 = 0;
        gps_config_v.counter_tout++;
        if(gps_config_v.counter_tout>=GPS_COMMS_TOUT)
        {
            gps_config_v.flag_timeout = 1;
            gps_config_v.counter_tout = 0;
        }
        
    }
    //increase up to 1 min
    if(ms_1000 == 60 )
    {
        ms_1000=0;
    }
    
   
  PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_2);
}

/**
 * @brief led blinc funtion, to manage number of blinks of led status
 * @param[in] none
 * @param[out]none
 * @return none
 */
void led_blink( uint8_t num_blink)
{
    static uint8_t state_led = 1;
    static uint8_t t_on = 0;

    if(t_on<=num_blink)
    {
        PLIB_PORTS_PinWrite (PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_9,state_led);
        if(state_led)
        {
            state_led=0;
        }
        else
        {
            state_led = 1;
        }

    }
    else if(t_on>num_blink && t_on<=OFF_DELAY_LED)
    {
       PLIB_PORTS_PinWrite (PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_9,0); 
    }
    else if(t_on>OFF_DELAY_LED)
    {
        t_on = 0;
    }
    t_on++;
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
const char *OKs="OK\r\n";

/**
 * @brief UART 2   interrupt each time that receive a data byte
 * @param[in] none
 * @param[out]none
 * @return none
 */
 void __ISR(_UART_2_VECTOR, ipl2AUTO) _IntHandlerDrvUsartInstance0(void)                //recibe y genera alertas por UART del modulo de comunicaciones
{
    if(PLIB_INT_SourceFlagGet(INT_ID_0, INT_SOURCE_USART_2_RECEIVE))
    {
        /* Make sure receive buffer has data availible */
        if (PLIB_USART_ReceiverDataIsAvailable(USART_ID_2))
        {
            /* Get the data from the buffer */
            gps_uart_v.rx_buffer[gps_uart_v.index] = PLIB_USART_ReceiverByteReceive(USART_ID_2);
            if(gps_uart_v.rx_buffer[gps_uart_v.index] == '\n')
            {
                if(gps_uart_v.rx_buffer[(gps_uart_v.index)-1] == '\r')
                {
                    if(gps_uart_v.rx_buffer[(gps_uart_v.index)-2] == 'K')
                    {
                        if(gps_uart_v.rx_buffer[(gps_uart_v.index)-3] == 'O')
                        {
                            gps_uart_v.flag_rx_end = 1;
                        }
                    }
                    //error case
                    else if(gps_uart_v.rx_buffer[(gps_uart_v.index)-2] == 'R')
                    {
                        if(gps_uart_v.rx_buffer[(gps_uart_v.index)-3] == 'O')
                        {
                            if(gps_uart_v.rx_buffer[(gps_uart_v.index)-4] == 'R')
                            {
                                if(gps_uart_v.rx_buffer[(gps_uart_v.index)-5] == 'R')
                                {
                                    if(gps_uart_v.rx_buffer[(gps_uart_v.index)-6] == 'E')
                                    {
                                        gps_uart_v.flag_rx_end = 1;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            gps_uart_v.index++;                       
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
    /*DRV_USART_TasksTransmit(sysObj.drvUsart1);
    DRV_USART_TasksReceive(sysObj.drvUsart1);
    DRV_USART_TasksError(sysObj.drvUsart1);*/
    if(PLIB_INT_SourceFlagGet(INT_ID_0, INT_SOURCE_USART_1_RECEIVE))
    {
        /* Make sure receive buffer has data availible */
        if (PLIB_USART_ReceiverDataIsAvailable(USART_ID_1))
        {
            /* Get the data from the buffer */
            tronic_uart_v.rx_buffer[tronic_uart_v.index] = PLIB_USART_ReceiverByteReceive(USART_ID_1);
            if(tronic_uart_v.rx_buffer[tronic_uart_v.index] == '\n')
            {
                if(tronic_uart_v.rx_buffer[(tronic_uart_v.index)-1] == '\r')
                {
                    if(tronic_uart_v.rx_buffer[(tronic_uart_v.index)-2] == 'K')
                    {
                        if(tronic_uart_v.rx_buffer[(tronic_uart_v.index)-3] == 'O')
                        {
                            tronic_uart_v.flag_rx_end = 1;
                        }
                    }
                    //error case
                    else if(tronic_uart_v.rx_buffer[(tronic_uart_v.index)-2] == 'R')
                    {
                        if(tronic_uart_v.rx_buffer[(tronic_uart_v.index)-3] == 'O')
                        {
                            if(tronic_uart_v.rx_buffer[(tronic_uart_v.index)-4] == 'R')
                            {
                                if(tronic_uart_v.rx_buffer[(tronic_uart_v.index)-5] == 'R')
                                {
                                    if(tronic_uart_v.rx_buffer[(tronic_uart_v.index)-6] == 'E')
                                    {
                                        tronic_uart_v.flag_rx_end = 1;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            tronic_uart_v.index++;                       
        }
        PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_RECEIVE);
    }
    else if (PLIB_INT_SourceFlagGet(INT_ID_0,INT_SOURCE_USART_1_TRANSMIT))
    {
        PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_USART_1_TRANSMIT);
    }
    
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_USART_1_ERROR);
    
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

