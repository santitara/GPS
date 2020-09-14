#ifndef _GPS_UART_H    /* Guard against multiple inclusion */
#define _GPS_UART_H
/*********include headers***********************************************************/
#include "stdint.h"
/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif
    

#define BUFF_SIZE_TX    255
#define BUFF_SIZE_RX    255
#define MSG_TO_SEND     15
/*********pubic enum***************************************************************/


/*********public struct***********************************************************/
typedef struct
{
    char rx_buffer[BUFF_SIZE_RX];
	char tx_buffer[BUFF_SIZE_TX];
    uint8_t flag_rx_end;
    uint8_t index;
    char data_gps[BUFF_SIZE_RX];
    char *ptr;  
}gps_uart_t;

extern gps_uart_t gps_uart_v;;




/*********public functions prototype***********************************************************/

uint8_t gps_uart_process_response(uint8_t * buff, const char *check_msg);
void gps_uart_rx_state (void);
void gps_uart_prepare_url_post(void);

    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _GPS_UART_H */

