/**
 *******************************************************************************
 * @file app.c
 * @author slopez
 * @version 1.0.0
 * @date Creation: 28/05/2020
 * @date Last modification: 28/05/2020
 * @brief App main file, in this file process all state of program and check led
 * status 
 *******************************************************************************

    @addtogroup app
    @{

*/

/* Includes ------------------------------------------------------------------*/
#include "app.h"
#include "stdio.h"
#include "time.h"
#include "string.h"
#include "libgeohash-master/geohash.h"
#include "gps/gps_config.h"
#include "gps/gps_common.h"

/* Definitions ---------------------------------------------------------------*/
#define BLINK_1  2
#define BLINK_2  4
#define BLINK_3  6
#define BLINK_4  8
#define BLINK_5  10

/* Private variables ----------------------------------------------------------*/
DRV_HANDLE flash_spi_handle;
DRV_SST25VF064C_BLOCK_COMMAND_HANDLE  commandHandle1,commandHandle2,commandHandle3;
DRV_SST25VF064C_COMMAND_STATUS f_status;

/* Private struct ------------------------------------------------------------*/
led_control_t led_control_v = 
{
    .num_blink = 0,
    .module_status_bit = 0,        
};

/* Public struct -------------------------------------------------------------*/
APP_DATA appData;

/* Private function prototypes -----------------------------------------------*/
 void check_led_status(void);
 
/**
 * @brief APP initialize. Init config to gps module. Power on GPS module, and 
 * start timer 1
 */
void APP_Initialize ( void )
{  
    gps_config_init_module ();
    gps_config_ON_OFF_module();
    DRV_TMR1_Start();
}
/**
 * @brief Application tasks routine. This function implements the
 * application state machine.
 */

void APP_Tasks ( void )
{  
    switch(appData.state)
    {
        case CONFIG_AT_MODULE:
            delay_ms(500);
            gps_config_at_general();
        break;
        case CONFIG_AT_GPS:
            delay_ms(500);
            gps_config_at_GPS();
        break;
        case CONFIG_AT_GPRS:
            delay_ms(500);
            gps_config_at_GRPS();
        break;
        case CONFIG_AT_HTTP:
            delay_ms(500);
            gps_config_at_HTTP();
        break;
         case CONFIG_AT_BT:
            delay_ms(500);
            gps_config_at_BT();
        break;
        case CONFIG_AT_END:
        //comprobar si tengo que ir a configurar bluetooth
            
        //si no voy a habilitar trama gps
        //delay_ms(100);
        gps_config_at_GPS_reports();
        break;
        default:
            
        break;
    }
    check_led_status();
   
}

/**
 * @brief check_led_status. Check each iteration of program state of gps module
 * and set led acording status.
 * gps state = 0 y gsm = 0  -->5 parpadeos
 * gps state = 0 y gsm = 1  -->4 parpadeos
 * gsm state = 0 y gps = 1  -->3 parpadeos
 * gps state = 1 y gsm = 1  -->2 parpadeos
 * gps state = 1 , gsm = 1  y web state = 1-->1 parpadeos
 */

void check_led_status(void)
{
    if(led_control_v.module_status_bit.gprs_state_bit == 0 && led_control_v.module_status_bit.gps_state_bit == 0)
    {
        led_control_v.num_blink = BLINK_5;
    }
    else if(led_control_v.module_status_bit.gprs_state_bit == 1 && led_control_v.module_status_bit.gps_state_bit == 0)
    {
         led_control_v.num_blink = BLINK_4;
    }
    else if(led_control_v.module_status_bit.gprs_state_bit == 0 && led_control_v.module_status_bit.gps_state_bit == 1)
    {
         led_control_v.num_blink = BLINK_3;
    }
    else if(led_control_v.module_status_bit.gprs_state_bit == 1 && led_control_v.module_status_bit.gps_state_bit == 1 && led_control_v.module_status_bit.web_state_bit == 0)
    {
         led_control_v.num_blink = BLINK_2;
    }
    else if(led_control_v.module_status_bit.gprs_state_bit == 1 && led_control_v.module_status_bit.gps_state_bit == 1 && led_control_v.module_status_bit.web_state_bit == 1)
    {
         led_control_v.num_blink = BLINK_1;
    }
    if(led_control_v.module_status_bit.bt_state_bit == 1 )
    {
        // Set ON led bluetooth
        PLIB_PORTS_PinWrite (PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_8,1);
    }
    else
    {
         // Set OFF led bluetooth
        PLIB_PORTS_PinWrite (PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_8,0);
    }
        
}
/*******************************************************************************
 End of File
 */
 