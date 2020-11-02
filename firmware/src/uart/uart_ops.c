/**
 *******************************************************************************
 * @file uart_ops.C
 * @author slopez
 * @version 1.0.0
 * @date Creation: 28/05/2020
 * @date Last modification: 28/05/2020
 * @brief uart_ops.c Procesing all data bytes of UART comms
 *******************************************************************************

    @addtogroup GPS UART
    @{

*/
/* Includes ------------------------------------------------------------------*/
#include "uart_ops.h"
#include "app.h"
#include "../gps/gps_common.h"
#include "../gps/gps_config.h"
#include "time.h"
#include <stdio.h>
/* Defines  ------------------------------------------------------------------*/  

/* Const vars ----------------------------------------------------------------*/    

/*********private enum*********************************************************/

/*********private struct*******************************************************/
/*********private functions prototype******************************************/
/*********private vars*********************************************************/

/**
 * @brief gps uart write. Write bytes in UART port
 * @param[in] char pointer, size of data to send 
 * @return bool var. true when finish false while not send all bytes
 */
bool uart_ops_write(const char *data, uint8_t size, uint8_t uart)
{
    if(*gps_config_v.msg == '\0')
    {
        return true;
    }
    /* Write a character at a time, only if transmitter is empty */
    while (PLIB_USART_TransmitterIsEmpty(uart))
    {
        /* Send character */
        PLIB_USART_TransmitterByteSend(uart, *gps_config_v.msg);

        /* Increment to address of next character */
        gps_config_v.msg++;

        if(*gps_config_v.msg == '\0')
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief gps uart process response. Check data received from uart with expected
 * message
 * @param[in] uint8_t *buff. Buffer with data receivd,
 * @param[in ]const char *check_msg, msg to compare buff 
 * @return bool var. true when msg to chec is match, false when not
 */
bool uart_ops_process_response(uint8_t *buff, const char *check_msg)
{
    char* ret = 0;
    ret = strstr(buff,check_msg);
    if(ret != NULL)
    {
      return true;
    }
    else
    {
        return false;
    }
}
