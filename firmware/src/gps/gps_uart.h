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
#define SIZE_BUF_DATA_TX    1700
 
/* Public enum ---------------------------------------------------------------*/

/* Public struct ---------------------------------------------------------------*/
    
typedef struct
{
   //uint8_t data_frame_tx[SIZE_BUF_DATA_TX]; //<!
   //uint8_t url_post_tx[SIZE_BUF_POST_TX]; 
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

typedef struct
{
    char rx_buffer[BUFF_SIZE_RX];
    char data_frame_tx[SIZE_BUF_DATA_TX]; //<!
    //char url_post_tx[SIZE_BUF_POST_TX];
    char data_gps[BUFF_SIZE_RX];
    uint8_t flag_rx_end; // flag to indicate that msg received is completed
    uint8_t index; //index of data byte array received
    char *ptr; // pointer to process data recived
    gps_data_lv data;
}gps_uart_t;

extern gps_uart_t gps_uart_v;;
 
/* Pubic functions ------------------------------------------------------- */
bool gps_uart_process_response      ( uint8_t * buff, const char *check_msg );
void    gps_uart_rx_state           ( void );
void    gps_uart_prepare_url_post   ( void );

    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _GPS_UART_H */

