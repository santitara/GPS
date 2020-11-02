/**
 *******************************************************************************
 * @file gps_uart.h
 * @author slopez
 * @version 1.0.0
 * @date Creation: 28/05/2020
 * @date Last modification: 28/05/2020
 * @brief App header file. Includin dependences and comon functions and structures
 *******************************************************************************

    @addtogroup GPS 
    @{
    @defgroup GPS UART header file
    @{
    @brief
    @details

*/

#ifndef _GPS_UART_H    /* Guard against multiple inclusion */
#define _GPS_UART_H

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif
    
/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "stdbool.h" 
#include "string.h" 

/* Definitions ---------------------------------------------------------------*/    
#define BUFF_SIZE_RX        255
#define MSG_TO_SEND         15
#define SIZE_BUF_DATA_TX    1800
#define BUFF_SIZE_BT        255
 
/* Public enum ---------------------------------------------------------------*/

/* Public struct ---------------------------------------------------------------*/
    
typedef struct
{
   //uint8_t data_frame_tx[SIZE_BUF_DATA_TX]; //<!
   //uint8_t url_post_tx[SIZE_BUF_POST_TX]; 
   unsigned long time_stamp[MSG_TO_SEND];
   char time_stamp_s[10];
   double lat[MSG_TO_SEND];
   double longi[MSG_TO_SEND];
   double speed[MSG_TO_SEND];
   char lat_s[12];
   char lon_s[12];
   char speed_s[12];
   uint8_t msg_num; //! number of msg in post data frame
   char imei[20];
   uint8_t index_data; //! index of arrays lat, longi, speed to store data each msg
}gps_data_lv;

typedef struct
{
    char rx_buffer[BUFF_SIZE_RX]; //! uart buffer
    char data_frame_tx[SIZE_BUF_DATA_TX];
    char data_frame_tx_copy[SIZE_BUF_DATA_TX];//<!
    char bt_frame_tx[BUFF_SIZE_BT];//<!
    char rx_buffer_tronic[BUFF_SIZE_RX];
    uint8_t flag_rx_end_tronic; // flag to indicate that msg received is completed
    uint8_t flag_rx_end; // flag to indicate that msg received is completed
    uint8_t index; //index of data byte array received
    uint8_t index_tronic; //index of data byte array received
    char *ptr; //! pointer to process data recived
    gps_data_lv data;
}gps_uart_t;

extern gps_uart_t gps_uart_v;;
 
/* Pubic functions ------------------------------------------------------- */
bool    gps_uart_process_response   ( uint8_t * buff, const char *check_msg );
void    gps_uart_rx_state           ( void );
void    gps_uart_prepare_url_post   ( void );
bool    gps_uart_write              ( const char *data, uint8_t size, uint8_t uart );

    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _GPS_UART_H */

