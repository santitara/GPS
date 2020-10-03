/**
 *******************************************************************************
 * @file app.h
 * @author slopez
 * @version 1.0.0
 * @date Creation: 28/05/2020
 * @date Last modification: 28/05/2020
 * @brief App header file. Includin dependences and comon functions and structures
 *******************************************************************************

    @addtogroup APP 
    @{
    @defgroup APP header file
    @{
    @brief
    @details

*/

#ifndef _APP_H
#define _APP_H
/* Includes ------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif
/* Includes --------------------------------------------------------------------------------------*/    
#include "system_config.h"
#include "system_definitions.h"
#include "delay.h"
   
/* Public definitions ----------------------------------------------------------------------------*/   
#define LOGO            1   
#define PRINCIPAL       2 
#define MENU            3
#define CALCULO         4
#define AGENDA          5
#define AJ_FABRICA      6
#define AJ_USUARIO      7
#define ESTADISTICAS    8
#define INSTALACION     9
#define CALCULADORA     10
#define AVERIAS         11
#define INFO            12
#define LLENAR_DEP      13
#define MEDIDAS         14
#define TECLADO         15
#define PIN             16
#define RESUMEN         17

/* Public enum ---------------------------------------------------------------*/
typedef enum
{             
    CONFIG_AT_MODULE = 0,
    CONFIG_AT_GPS,
    CONFIG_AT_GPRS,
    CONFIG_AT_HTTP,
    CONFIG_AT_BT,
    CONFIG_AT_END,   
} app_states_e;

/* Public struct -----------------------------------------------------------*/
typedef struct 
{
	uint8_t gps_state_bit: 			1;
	uint8_t gprs_state_bit:			1;
	uint8_t bt_state_bit:			1;
    uint8_t web_state_bit:			1;
}module_status_bit_t;

typedef struct
{
    uint8_t num_blink;
    module_status_bit_t module_status_bit;   
}led_control_t;

extern led_control_t led_control_v;

typedef struct
{
	app_states_e state;
} APP_DATA;

/* Pubic functions ------------------------------------------------------- */
void APP_Initialize             ( void );
void APP_Tasks                  ( void );
bool PutCharacter               ( const char character);
bool WriteString                ( void );

extern APP_DATA appData;

#ifdef __cplusplus
}
#endif

#endif /* _APP_H */

/*******************************************************************************
 End of File
 */

