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
#define SIZE_BUF_POST_TX 1500
#define SIZE_BUF_DATA_TX 1500
    
/*********pubic enum***************************************************************/

/*********public struct***********************************************************/
typedef struct
{
    char rx_buffer[BUFF_SIZE_RX];
	char tx_buffer[BUFF_SIZE_TX];
    char data_gps[BUFF_SIZE_RX];
    uint8_t flag_rx_end; // flag to indicate that msg received is completed
    uint8_t index; //index of data byte array received
    char *ptr; // pointer to process data recived  
}gps_uart_t;

extern gps_uart_t gps_uart_v;;

typedef struct
{
   uint8_t data_frame_tx[SIZE_BUF_DATA_TX]; //<!
   uint8_t url_post_tx[SIZE_BUF_POST_TX]; 
   char time_stamp[10];
   double lat;
   double longi;
   double speed;
   char lat_s[12];
   char lon_s[12];
   char speed_s[12];
   uint8_t msg_num;
   char imei[20]; 
}gps_data_lv;

extern gps_data_lv   gps_data_v;
// 
/*********public functions prototype***********************************************************/

uint8_t gps_uart_process_response(uint8_t * buff, const char *check_msg);
void gps_uart_rx_state (void);
void gps_uart_prepare_url_post(void);

    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _GPS_UART_H */

