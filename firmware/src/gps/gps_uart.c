/*********include headers***********************************************************/
#include "gps_uart.h"
#include "app.h"

//#include "string.h"

//#include "stdio.h"

/*********private enum***************************************************************/

/*********private struct***********************************************************/

/*********private functions prototype***********************************************************/

gps_uart_t gps_uart_v;

bool gps_uart_write(char *data, uint8_t size)
{
    if(*data == '\0')
    {
        return true;
    }

    /* Write a character at a time, only if transmitter is empty */
    while (PLIB_USART_TransmitterIsEmpty(USART_ID_2))
    {
        /* Send character */
        PLIB_USART_TransmitterByteSend(USART_ID_2, *data);

        /* Increment to address of next character */
        data++;

        if(*data == '\0')
        {
            return true;
        }
    }
    return false;
}

uint8_t gps_uart_process_response(uint8_t *buff, const char *check_msg)
{
    uint8_t ret = 0;
    ret = strstr(buff,check_msg);
    if(ret != NULL)
    {
      return 1;
    }
    else
    {
        return 0;
    }
}

