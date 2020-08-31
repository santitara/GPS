 /*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_Initialize" and "APP_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
//DOM-IGNORE-END

#ifndef _APP_H
#define _APP_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "system_config.h"
#include "system_definitions.h"
#include "bmc150.h"
#include "bmm050.h"
#include "delay.h"
#include <math.h>

//#include "reset_p32mx120f032b.h"

#include "peripheral/nvm/plib_nvm.h"
#include "sys/kmem.h"

#include "system/reset/src/sys_reset_local.h"


// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END 

// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************
#define TEST_COMM_ATTEMPTS 10 
#define CHECK_NETWORK_ATTEMPS 200
   
    
#define BUFFER_RX_SIZE  750  //tamaño del buffer de recepcion de datos UART
#define BUFFER_REMAIN_LIMIT  130   
    
#define K_A_RUMBO  600// para que el dato de velocidad sea válido la diferencia de rumbo con respecto al anterior tiene que ser K_A_RUMBO/velocidad 
    
    
#define FRECUENCIA_ENVIO_DEF  4
    
#define RED_LED_PERIOD   350 
#define RED_LED_BLINK_RATE   10
    
#define GSM_RECONECT_PERIOD   60000 // 10min - 600s - 60000 cuentas del timer   
    
    
 #define K_PR_V   28000.0 //para que sea un coma flotante

    
#define DATABUFF_SIZE         (sizeof(databuff) / sizeof(uint32_t))
#define NVM_FILL_DATABUF_AND_ERASE_STATE    1
#define NVM_ERASE_COMPLETION_CHECK      2
#define NVM_ERASE_ERROR_CHECK           3
#define NVM_WRITE_COMPLETION_CHECK      4
#define NVM_WRITE_ERROR_CHECK           5
#define NVM_ERROR_STATE                 6
#define NVM_SUCCESS_STATE               7


#define PROGRAM_FLASH_BASE_ADDRESS  (unsigned int) (&dataFlash)
#define PROGRAM_FLASH_BASE_ADDRESS_VALUE  (unsigned int) 0x9D007C00 //direccion de la ultima pagina de flash para guardar datos permanentes
#define PROGRAM_FLASH_BASE_ADDRESS_END  (unsigned int) 0x9D007C7C
    
#define DEVICE_ROW_SIZE_DIVIDED_BY_4   32  //row size 128 bytes para 32MX120F032B
#define DEVICE_PAGE_SIZE_DIVIDED_BY_4 256 //page size 128 bytes para 32MX120F032B
    
#define CONFIG_DATA_AVAILABLE 0xABCDEF // marca al inicio de datos configuracion en flash
    
#define CONFIG_DATA_AVAILABLE_2 0xFEDCBA // marca al final de datos configuracion en flash
    

   


//Listado pantallas VS su valor numerico asignado    
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
    
    
#define PI 3.14159265359
    
    
#define RESET_REASON_WDT_TIMEOUT 0x00000010
#define RESET_REASON_POR 0x00000083     

    
#define TIEMPO_PARADO    59 // 59->60segundos  14->15 segundos para considerar que el tracker esta parado   
#define N_MAX_REINTENTOS 10 //Veces que se intenta enviar un dato
#define TIMEOUT_COUNT_MAX 40 // 20 s tiempo que se espera para recibir respuesta del server

//Variables globales necesarias para el uso del lcd
 unsigned int trama_lcd[6];
 int j, segmentos, digitos_display;
 char fin;

// *****************************************************************************
/* Application states

  Summary:
    Application states enumeration

  Description:
    This enumeration defines the valid application states.  These states
    determine the behavior of the application at various times.
*/

typedef enum
{             
    CONFIG_AT_MODULE = 0,
    CONFIG_AT_GPRS,
    CONFIG_AT_GPS,
    CONFIG_AT_HTTP,
    CONFIG_AT_BT,
    CONFIG_AT_END,   
} app_states_e;


// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    Application strings and buffers are be defined outside this structure.
 */

typedef struct
{
	app_states_e state;
} APP_DATA;



typedef struct
{
	//SYS_MODULE_OBJ   drvObject;
	 
} APP_DRV_OBJECTS;
// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Routines
// *****************************************************************************
// *****************************************************************************
/* These routines are called by drivers when certain events occur.
*/
// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Summary:
     MPLAB Harmony application initialization routine.

  Description:
    This function initializes the Harmony application.  It places the 
    application in its initial state and prepares it to run so that its 
    APP_Tasks function can be called.

  Precondition:
    All other system initialization routines should be called before calling
    this routine (in "SYS_Initialize").

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_Initialize();
    </code>

  Remarks:
    This routine must be called from the SYS_Initialize function.
*/

void APP_Initialize ( void );


/*******************************************************************************
  Function:
    void APP_Tasks ( void )

  Summary:
    MPLAB Harmony Demo application tasks function

  Description:
    This routine is the Harmony Demo application's tasks function.  It
    defines the application's state machine and core logic.

  Precondition:
    The system and application initialization ("SYS_Initialize") should be
    called before calling this.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_Tasks();
    </code>

  Remarks:
    This routine must be called from SYS_Tasks() routine.
 */

void APP_Tasks( void );


/*******************************************************************************
  Function:
    bool PutCharacter(const char character)

  Summary:
    Function to put a character to UART

  Description:
    This function for UART transmits a character on UART.
    This function is non-blocking and will return the control back to calling
    function if the transmit buffer is not available for a new transmission. The
    function will return a "true" if the character is transmitted. Otherwise,
    the function will return "false".

  Precondition:
     UART should be enabled and TX function should be enabled before calling this
     function.

  Parameters:
    Character to be transmitted to UART.

  Returns:
    true: If the character could be transmitted.
    false: If the character could not be transmitted.

  Example:
    while(false == PutCharacter('A'));


  Remarks:
    This routine must be called from the APP_Tasks function.
*/
bool PutCharacter(const char character);

/*******************************************************************************
  Function:
    bool WriteString(void)

  Summary:
    Function to put a string to UART

  Description:
    This function transmits a string to UART. The string pointer is passed using
    a global pointer "stringPointer" define in structure "APP_DATA". 
    This function is non-blocking and will return the control back to calling
    function if the transmit buffer is not available for a new transmission. The
    function will return a "true" if the entire string is transmitted. Otherwise,
    the function will return "false".

  Precondition:
    UART should be enabled and TX function should be enabled before calling this
    function.
    The string to be transmitted should be terminated with NULL character.

  Parameters:
    Character to be transmitted to UART.

  Returns:
    true: If the function encounters a NULL character (string is completed).
    false: If the string is not yet completely transmitted.

  Example:
    const char *str = "Hello World";
    APP_DATA appData;

    appData.stringPointer = str;

    while(false == PutCharacter());
    


  Remarks:
    This routine must be called from the APP_Tasks function.
*/
bool WriteString(void);

// *****************************************************************************
// *****************************************************************************
// Section: extern declarations
// *****************************************************************************
// *****************************************************************************

extern APP_DRV_OBJECTS appDrvObject;

extern APP_DATA appData;

#endif /* _APP_H */

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

/*******************************************************************************
 End of File
 */

