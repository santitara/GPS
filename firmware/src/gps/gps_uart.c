/*********include headers***********************************************************/
#include "gps_uart.h"
#include "app.h"
#include "gps_common.h"
const char *ERR = "ERROR";
const char *UGNSINF0 = "+UGNSINF: 1,0";
const char *UGNSINF1 = "+UGNSINF: 1,1";
//#include "string.h"

//#include "stdio.h"

/*********private enum***************************************************************/

/*********private struct***********************************************************/

/*********private functions prototype***********************************************************/

gps_uart_t gps_uart_v;
uint8_t a=0;
bool gps_uart_write(char *data, uint8_t size)
{
    if(*gps_config_v.msg == '\0')
    {
        return true;
    }

    /* Write a character at a time, only if transmitter is empty */
    while (PLIB_USART_TransmitterIsEmpty(USART_ID_2))
    {
        /* Send character */
        PLIB_USART_TransmitterByteSend(USART_ID_2, *gps_config_v.msg);

        /* Increment to address of next character */
        gps_config_v.msg++;

        if(*gps_config_v.msg == '\0')
        {
            a=1;
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

void gps_uart_rx_state (void)
{
    if(gps_uart_v.flag_rx_end)
    {
        if(gps_uart_process_response(gps_uart_v.rx_buffer,gps_config_v.expect_res))
        {
            gps_config_v.state = gps_config_v.state_ok;
            gps_uart_v.flag_rx_end = 0;
            gps_uart_v.index=0;
            memset(gps_uart_v.rx_buffer,0,255);
        }
        else if(gps_uart_process_response(gps_uart_v.rx_buffer,ERR))
        {
            gps_config_v.state = gps_config_v.state_ok;
            gps_uart_v.flag_rx_end = 0;
            gps_uart_v.index=0;
            memset(gps_uart_v.rx_buffer,0,255);
        }
        else if (gps_uart_process_response(gps_uart_v.rx_buffer,UGNSINF0))
        {
            gps_config_v.state = gps_config_v.state_ok;
            gps_uart_v.flag_rx_end = 0;
            gps_uart_v.index=0;
            memset(gps_uart_v.rx_buffer,0,255);
        }
        else if (gps_uart_process_response(gps_uart_v.rx_buffer,UGNSINF1))
        {
            gps_config_v.state = gps_config_v.state_ok;
            gps_uart_v.flag_rx_end = 0;
            gps_uart_v.index=0;
            memset(gps_uart_v.rx_buffer,0,255);
        }
        else
        {
            gps_config_v.state = gps_config_v.state_wrong;
            gps_uart_v.flag_rx_end = 0;
            memset(gps_uart_v.rx_buffer,0,255);
        }
    }
    else
    {
        gps_config_v.n_retries++; 
        if(gps_config_v.n_retries > 2)
        {
            gps_config_v.state = gps_config_v.state_wrong;
            gps_config_v.n_retries = 0;
            //memset(gps_uart_v.rx_buffer,0,255);
        }

    }

}

