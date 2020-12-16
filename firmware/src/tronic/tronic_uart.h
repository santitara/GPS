/**
 *******************************************************************************
 * @file tronic_uart.h
 * @author slopez
 * @version 1.0.0
 * @date Creation: 28/05/2020
 * @date Last modification: 28/05/2020
 * @brief App header file. Includin dependences and comon functions and structures
 *******************************************************************************

    @addtogroup TRONIC 
    @{
    @defgroup TRONIC UART header file
    @{
    @brief
    @details

*/

#ifndef _TRONIC_UART_H    /* Guard against multiple inclusion */
#define _TRONIC_UART_H

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif
    
/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "stdbool.h" 
#include "string.h" 
#include "uart/uart_ops.h"

/* Definitions ---------------------------------------------------------------*/    
#define SIZE_DATA_TRONIC        10
/* Public enum ---------------------------------------------------------------*/

/* Public struct -------------------------------------------------------------*/

typedef struct
{
    char depo   [SIZE_DATA_TRONIC];
    char cau    [SIZE_DATA_TRONIC];
    char lud    [SIZE_DATA_TRONIC];
    char evsi   [SIZE_DATA_TRONIC];
    char evii   [SIZE_DATA_TRONIC];
    char evsd   [SIZE_DATA_TRONIC];
    char evid   [SIZE_DATA_TRONIC];
    char evsi_en[SIZE_DATA_TRONIC];
    char evii_en[SIZE_DATA_TRONIC];
    char evsd_en[SIZE_DATA_TRONIC];
    char evid_en[SIZE_DATA_TRONIC];
    char campo  [SIZE_DATA_TRONIC];
    char trat   [SIZE_DATA_TRONIC];
    char vel_trat[SIZE_DATA_TRONIC];
    char cau_cal[SIZE_DATA_TRONIC];
    char lud_tart[SIZE_DATA_TRONIC];
    char nmen   [SIZE_DATA_TRONIC];
    char modo   [SIZE_DATA_TRONIC];
    char agiro  [SIZE_DATA_TRONIC];
    char ud_med [SIZE_DATA_TRONIC];
}data_s;

typedef struct
{
    char rx_buffer[BUFF_SIZE_RX];
    uint8_t flag_rx_end; // flag to indicate that msg received is completed
    uint8_t index; //index of data byte array received
    data_s data;
}tronic_uart_t;

extern tronic_uart_t tronic_uart_v;;
/* Pubic functions ------------------------------------------------------- */
void    tronic_uart_rx_state           ( void );
    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _TRONIC_UART_H */

