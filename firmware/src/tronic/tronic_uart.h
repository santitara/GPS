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
 
/* Public enum ---------------------------------------------------------------*/

/* Public struct -------------------------------------------------------------*/
const char *VAR_DEP ="depo=";
const char *VAR_CAU ="caud=";
const char *VAR_LUD ="ltr_ud=";
const char *VAR_EVSI ="ev_si=";
const char *VAR_EVII ="ev_ii=";
const char *VAR_EVSD ="ev_sd=";
const char *VAR_EVID ="ev_id=";
const char *VAR_EVSI_EN ="ev_si_en=";
const char *VAR_EVII_EN ="ev_ii_en=";    //averiguar evs y poner 
const char *VAR_EVSD_EN ="ev_sd_en=";
const char *VAR_EVID_EN ="ev_id_en=";
const char *VAR_CAMPO ="campo=";
const char *VAR_TRAT ="trat=";
const char *VAR_VEL_TRAT ="vel_trat=";
const char *VAR_CAU_CAL ="cau_trat=";
const char *VAR_LUD_TRAT ="ltr_ud_trat=";
const char *VAR_NMEN ="n_men=";
const char *VAR_MODO="modo_trab=";
const char *VAR_AGIRO ="a_giro=";
const char *VAR_UD_MED ="ud_med=";

typedef struct
{
    
};
typedef struct
{
    char rx_buffer[BUFF_SIZE_RX];
    uint8_t flag_rx_end; // flag to indicate that msg received is completed
    uint8_t index; //index of data byte array received
}tronic_uart_t;

extern tronic_uart_t tronic_uart_v;;
/* Pubic functions ------------------------------------------------------- */
void    tronic_uart_rx_state           ( void );
    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _TRONIC_UART_H */

