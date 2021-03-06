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
   
    
#define BUFFER_RX_SIZE  750  //tama�o del buffer de recepcion de datos UART
#define BUFFER_REMAIN_LIMIT  130   
    
#define K_A_RUMBO  600// para que el dato de velocidad sea v�lido la diferencia de rumbo con respecto al anterior tiene que ser K_A_RUMBO/velocidad 
    
    
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
    
    SIM808_OFF_COM,
    SIM808_OFF_CHECK_RESPONSE,  // undefined communication state with gsm module
    SIM808_TURN_ON,
            
    SIM808_CONFIG,
    GET_DEVICE_IMEI,
            
    SET_BT_PARAMETERS,    //BLUETOOTH STARTUP
    BTH_WAIT_COMMAND,      //BLUETOOTH ACTIVE, WAITING COMMANDS
    DISABLE_BT,
    SIM808_CRASH_HANDLER, 
    
    SIM808_READY,      // idle state 
         
    PROCESAR_DATOS_RX,   //para procesar los datos de gps
            
    REFRESCAR_DISPLAY,
    
    INICIAR_CONEXION_GPRS,        
    OBTENER_HORA_SERVIDOR,      //para enviar informacion por gprs
    ENVIAR_TRAMA_SERVER,
    TERMINAR_CONEXION_GPRS, 
            
    ESPERAR_ACCION_GPS_BT,

    HABILITAR_INFORME_GPS,
    INICIALIZAR_TRAMA_HTTP,
    DESHABILITAR_INFORME_GPS,

    GRABAR_DATOS_EN_FLASH,

    COMPROBAR_ESTADO_REGISTRO_DISPOSITIVO,

    ASIGNAR_SIM_DISPOSITIVO,

    REGISTRAR_DISPOSITIVO,  
            
    UART_REINITIALIZE,
            
    RUTINA_TEST
            
} APP_STATES;


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

	APP_STATES state;

	bool Recibido_mensaje,Recibido_mensaje_BT,Recibido_mensaje_gps,Recibido_mensaje_potencia;
    
    const char *mensaje_a_enviar;
    const char *mensaje_a_enviar2;
      
   //variables recuerdan estado potencia tronic
    unsigned short nro_pantalla;  //indica numericamente la pantalla en que se encuentra el equipo
    char parameter_array[22][11]; // lista de parametros asociados a la pantalla actual PRINCIPAL tiene el n� max de parametros 21
    unsigned short nro_parametros;

    unsigned short parpadeos_led_rojo;
    /*
    - Indicaciones del led rojo en placa:
	- parpadeo 5 veces consecutivas si esta sin GPS ni GPRS
	- parpadeo 4 veces si tiene GPRS y falta GPS, 
	- parpadeo 3 veces si tiene GPS pero le falta cobertura GPRS 
    - parpadeo 2 veces si tiene GPS y GPRS
	- parpadeo 1 vez si tiene GPS y GPRS y ademas establece conexi�n con la web  estado OK divinisimo de la muerte
	- apagado si el equipo no tiene tension
    - fijo si el equipo tiene tension pero no esta listo para funcionar
    */
    
    
    
     /*
    bool mas_corto;  //flag de accion pulsacion corta de las 3 teclas mas, menos y mas+menos
    bool mas_largo; //flag de accion pulsacion larga de las 3 teclas mas, menos y mas+menos
    bool mas_medio; // para la temporizacion de arranque del cambio rapido de distancia
    
    bool mas_pulsada; //flag de estado pulsada teclas 
    bool mas_pulsada_largo;
    
    unsigned int cuentas_tecla_menos;
    bool menos_corto;
    bool menos_largo;
    bool menos_medio; // para la temporizacion de arranque del cambio rapido de distancia
        
    bool menos_pulsada;
    bool menos_pulsada_largo;
    
    unsigned int cuentas_tecla_d;
    bool d_corto;
    bool d_medio;
    bool d_largo;
    
    bool d_pulsada;
    bool d_pulsada_medio;
    
    //variable estado actual display
    
    //char mostrar_en_display;
    char mostrar_en_display_anterior;
    
    // Lista de parametros del dispositivo (variables RAM temporales)
    
    char GPS_Captador_Rueda;            //P00  GPS o Captador Rueda
    char precision_distancia;           //P01  Precision: 00.00 � 000.0
    char seg_gps_a_brujula;             //P02  Segundos para pasar de GPS a br�jula
    char funcion_crono;                 //P03  Cronometro en Horas, Minutos, reloj Horas, Reloj Minutos
    char decimas_pulso_breve;           //P04  D�cimas de pulso breve
    char decimas_pulso_mantenido;       //P05  D�cimas de pulso mantenido
    unsigned int seg_visualizacion_temp;//P06  Segundos visualizaci�n temporal
    unsigned long int  seg_apagado_auto;//P07  Segundos para apagado autom�tico
    unsigned int seg_pulso_borrado;     //P08  Segundos duraci�n pulso borrado
    unsigned int  cent_rueda;           //P09  Cent�metros longitud rueda
    char km_millas;                     //P10  Kil�metros o Millas
    unsigned int pulgadas_rueda;        //P11  Pulgadas longitud Rueda
    char desv_utc;                      //P12  Desviaci�n horaria a partir del UTC.
    char Retroiluminacion;              //P13  Retroiluminaci�n, ON, OFF, Auto (activa en movimiento)
    char dec_seg_prec;                  //P14  Decimas para elegir precision distancia 1-10
    char umbral_vel_cap_gps;            //P15  Kmh min para que captador tome control sobre velocidad y distancia
    
    */
    
    
    char modo_test;
    char datos_gps_disponible;
    // Lista de estadisticas del dispositivo (variables RAM temporales)
    //double vel_max;   
    char modo_rutina_test,cuenta_hab_bt;
    
    char gps_state,bt_state,gsm_state;
    
    unsigned short frecuencia_envio;
    
    unsigned int tick_scaler_sirena;
 
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

