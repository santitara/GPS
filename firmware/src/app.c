/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It 
    implements the logic of the application's state machine and it may call 
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/
// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "app.h"
#include "stdio.h"
#include "time.h"
#include "string.h"
#include "libgeohash-master/geohash.h"
#include "gps/gps_config.h"
#include "gps/gps_common.h"


//****************************************************************************
DRV_HANDLE flash_spi_handle;
DRV_SST25VF064C_BLOCK_COMMAND_HANDLE  commandHandle1,commandHandle2,commandHandle3;
DRV_SST25VF064C_COMMAND_STATUS f_status;
 
/*private function prototipe */
 void check_led_status(void);
 
/* Application Data */ 
APP_DATA appData;
//*****************************************************************************

void APP_Initialize ( void )
{  
    gps_config_init_module ();
    gps_config_ON_OFF_module();
    DRV_TMR1_Start();
}
/**********************************************************
 * Application tasks routine. This function implements the
 * application state machine.
 ***********************************************************/
void APP_Tasks ( void )
{  
    switch(appData.state)
    {
        case CONFIG_AT_MODULE:
            delay_ms(1000);
            gps_config_at_general();
        break;
        case CONFIG_AT_GPS:
            delay_ms(1000);
            gps_config_at_GPS();
        break;
        case CONFIG_AT_GPRS:
            delay_ms(1000);
            gps_config_at_GRPS();
        break;
        case CONFIG_AT_HTTP:
            delay_ms(1000);
            gps_config_at_HTTP();
        break;
         case CONFIG_AT_BT:
            delay_ms(1000);
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

void check_led_status(void)
{
/*
    gps state = 0 y gsm = 0  -->5 parpadeos
	gps state = 0 y gsm = 1  -->4 parpadeos
	gsm state = 0 y gps = 1  -->3 parpadeos
	gps state = 1 y gsm = 1  -->2 parpadeos
 *  gps state = 1 , gsm = 1  y web state = 1-->1 parpadeos
 */
    if(gps_config_v.module_status_bit.gprs_state_bit == 0 && gps_config_v.module_status_bit.gps_state_bit == 0)
    {
        gps_config_v.num_blink = BLINK_5;
    }
    else if(gps_config_v.module_status_bit.gprs_state_bit == 1 && gps_config_v.module_status_bit.gps_state_bit == 0)
    {
         gps_config_v.num_blink = BLINK_4;
    }
    else if(gps_config_v.module_status_bit.gprs_state_bit == 0 && gps_config_v.module_status_bit.gps_state_bit == 1)
    {
         gps_config_v.num_blink = BLINK_3;
    }
    else if(gps_config_v.module_status_bit.gprs_state_bit == 1 && gps_config_v.module_status_bit.gps_state_bit == 1 && gps_config_v.module_status_bit.web_state_bit == 0)
    {
         gps_config_v.num_blink = BLINK_2;
    }
    else if(gps_config_v.module_status_bit.gprs_state_bit == 1 && gps_config_v.module_status_bit.gps_state_bit == 1 && gps_config_v.module_status_bit.web_state_bit == 1)
    {
         gps_config_v.num_blink = BLINK_1;
    }
}
/*******************************************************************************
 End of File
 */
 