/*********include headers***********************************************************/
#include "stdint.h"
#define BUFF_SIZE_TX    255
#define BUFF_SIZE_RX    255

/*********pubic enum***************************************************************/


/*********public struct***********************************************************/
typedef struct
{
    uint8_t rx_buffer[BUFF_SIZE_RX];
	uint8_t tx_buffer[BUFF_SIZE_TX];
    uint8_t flag_rx_end;
}gps_uart_t;

extern gps_uart_t gps_uart_v;;




/*********public functions prototype***********************************************************/

uint8_t gps_uart_process_response(uint8_t * buff, const char *check_msg);