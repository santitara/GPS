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

 //****************************************************************************
 DRV_HANDLE flash_spi_handle;
 DRV_SST25VF064C_BLOCK_COMMAND_HANDLE  commandHandle1,commandHandle2,commandHandle3;
 DRV_SST25VF064C_COMMAND_STATUS f_status;
 
 
/* Application Data */ 
APP_DATA app_data_v;
//*****************************************************************************

void APP_Initialize ( void )
{  
    gps_config_init_module ();
    gps_config_ON_OFF_module(); 
}
/**********************************************************
 * Application tasks routine. This function implements the
 * application state machine.
 ***********************************************************/
void APP_Tasks ( void )
{
    switch(app_data_v.state)
    {
        case CONFIG_AT_MODULE:
            gps_config_at_module();
        break;
        case CONFIG_AT_GPRS:
        break;
        case CONFIG_AT_GPS:
            
        break;
                    
        default:
            
        break;
    }
   
}


/*******************************************************************************
 End of File
 */
 