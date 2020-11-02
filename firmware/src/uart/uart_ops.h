/**
 *******************************************************************************
 * @file uart_ops.h
 * @author slopez
 * @version 1.0.0
 * @date Creation: 28/05/2020
 * @date Last modification: 28/05/2020
 * @brief App header file. Includin dependences and comon functions and structures
 *******************************************************************************

    @addtogroup UART 
    @{
    @defgroup UART OPS header file
    @{
    @brief
    @details

*/

#ifndef _UART_OPS_H    /* Guard against multiple inclusion */
#define _UART_OPS_H

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

/* Public struct -------------------------------------------------------------*/
 
/* Pubic functions ---------------------------------------------------------- */
bool    gps_uart_process_response   ( uint8_t * buff, const char *check_msg );
bool    gps_uart_write              ( const char *data, uint8_t size, uint8_t uart );

    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _GPS_UART_H */

