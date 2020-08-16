/*******************************************************************************
 System Interrupts File

  File Name:
    system_interrupt.c

  Summary:
    Raw ISR definitions.

  Description:
    This file contains a definitions of the raw ISRs required to support the
    interrupt sub-system.

  Summary:
    This file contains source code for the interrupt vector functions in the
    system.

  Description:
    This file contains source code for the interrupt vector functions in the
    system.  It implements the system and part specific vector "stub" functions
    from which the individual "Tasks" functions are called for any modules
    executing interrupt-driven in the MPLAB Harmony system.

  Remarks:
    This file requires access to the systemObjects global data structure that
    contains the object handles to all MPLAB Harmony module objects executing
    interrupt-driven in the system.  These handles are passed into the individual
    module "Tasks" functions to identify the instance of the module to maintain.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2011-2014 released Microchip Technology Inc.  All rights reserved.

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
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <xc.h>
#include <sys/attribs.h>
#include "app.h"
#include "system_definitions.h"

//variables de system_interrupt
char trama_rx[BUFFER_RX_SIZE], primer_pulso=1,existe_captador=0;
unsigned int nbyte=0;
char poner_a_dormir=0;
char system_in_sleep;
unsigned int cuenta_pulsos_largos_mas=0,cuenta_pulsos_largos_menos=0,timeout_3s=0,timeout_1s=0,timeout_1s_bis = 0,veces_hab_bt=0;
unsigned int pulsos_rueda=0,overflows_timer3;
double cuenta_actual;

//variables externas a system_interrupt
extern int seconds, minutes, horas; 
extern double distancia_gps,vel_rueda,distancia_rueda,vel_gps;  //perimetro de la circunferencia de la rueda
extern char parado,guardar_en_flash;

extern double vel_max;   
extern char mostrar_en_display;
extern char flag_envio_web;

extern char real_data;
//extern unsigned long DATOS_FLASH[DEVICE_ROW_SIZE_DIVIDED_BY_4]; 
extern unsigned int blink_rate;
int index_blink=0,enviar_bt=0;
int tick_scaler=0,tick_scaler_2=0,tick_scaler_3=0,secuencia_led=0,n_parpadeos=0;

extern char permision_to_send;
//extern tick_scaler_sirena;
// *****************************************************************************
// *****************************************************************************
// Section: System Interrupt Vector Functions
// *****************************************************************************
// *****************************************************************************
//                      TIMERS 
// ****************************************************************************
void __ISR(_TIMER_2_VECTOR, ipl1AUTO) _IntHandlerDrvTmrInstance1(void)                  //timer para gestionar la entrada de comandos por botones +,- y D
{
    //el tick basico de este timer es de 10 ms aprox	
    
    if(appData.modo_rutina_test==0){
        appData.tick_scaler_sirena++;
    }
    
    if( appData.tick_scaler_sirena>1000){ //a los 10 segundos de inactividad en el programa principal  
       appData.tick_scaler_sirena=1000;   
       //permision_to_send=1;
       appData.state =  SIM808_CRASH_HANDLER;  //cambio a transicion hacer juego de luces
    }
    
     //RED_LED_BLINK_RATE   
    if(appData.gsm_state == 0){
        if(tick_scaler_3<GSM_RECONECT_PERIOD){
            tick_scaler_3++;
          }
        else{
           tick_scaler_3=0;   
           appData.state =  DESHABILITAR_INFORME_GPS;  //cambio a transicion para desconectar gps y reenganchar gsm

        }
    }
    
    if(tick_scaler<RED_LED_PERIOD){
      tick_scaler++;
    }
    else{
       tick_scaler=0;   
       secuencia_led=1;
       n_parpadeos = appData.parpadeos_led_rojo;
    }
    if(secuencia_led>0){
    //hago la secuencia del led 
        
        switch(secuencia_led){
            case 1:
                PLIB_PORTS_PinWrite (PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_9,1);
                tick_scaler_2=0;
                secuencia_led++;
                break;
            case 2:
                if(tick_scaler_2<RED_LED_BLINK_RATE){   //cuento el tiempo que debe estar encendido
                    tick_scaler_2++;
                }
                else{
                   PLIB_PORTS_PinWrite (PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_9,0);  //apago el led rojo 
                   tick_scaler_2=0;     
                   n_parpadeos--;
                   if(n_parpadeos==0){
                        secuencia_led=0;  //termino la secuencia
                   }
                   else{
                       secuencia_led++; //voy al case 3
                   }
                   
                }
                break;
                case 3:
                    if(tick_scaler_2<(RED_LED_BLINK_RATE*4)){  //espero RED_LED_PERIOD con el led apagado
                        tick_scaler_2++;
                    }
                    else{
                       tick_scaler_2=0;   
                       secuencia_led=1;  //vuelvo al case 1
                    }    
                    break;
        }
    }  
    
  PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_2);
}
 /*****************************************************************************/ 
 void __ISR(_TIMER_3_VECTOR, ipl1AUTO) _IntHandlerDrvTmrInstance2(void)                 //timer para medir la velocidad mediante captador rueda
 {
     //el tick basico de este timer es de 2080 ms aprox
     //no usar este timer esta dedicado para la salida de pulsos de velocidad

    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_3);
 }
// ****************************************************************************

void __ISR(_TIMER_4_VECTOR, ipl1AUTO) _IntHandlerDrvTmrInstance0(void)                  //timer para manejar la escritura de datos en el LCD de 4 digitos
{
    //el tick basico de este timer es 1.25 us 
    
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_4);
}
// ****************************************************************************
//                      UART'S
// ****************************************************************************
 void __ISR(_UART_2_VECTOR, ipl2AUTO) _IntHandlerDrvUsartInstance0(void)                //recibe y genera alertas por UART del modulo de comunicaciones
{

    if(PLIB_INT_SourceFlagGet(INT_ID_0, INT_SOURCE_USART_2_RECEIVE))
    {
        /* Make sure receive buffer has data availible */
        if (PLIB_USART_ReceiverDataIsAvailable(USART_ID_2))
        {
            /* Get the data from the buffer */
            trama_rx[nbyte] = PLIB_USART_ReceiverByteReceive(USART_ID_2);

                if (trama_rx[nbyte] == '\n'){
                    
                    if(trama_rx[(nbyte-1)] =='\r'){
                        
                        appData.Recibido_mensaje_BT = true;  //posibilidad de que haya llegado un comando bluetooth sino el bucle princ del programa ignora y sigue su curso
                        if((BUFFER_RX_SIZE-nbyte)<BUFFER_REMAIN_LIMIT){
                            nbyte=0;
                        }
                        if(trama_rx[(nbyte-2)] =='K'){
                            
                            if(trama_rx[(nbyte-3)] =='O'){
                                //nbyte=0;
                                appData.Recibido_mensaje = true;
                                if((BUFFER_RX_SIZE-nbyte)<BUFFER_REMAIN_LIMIT){
                                    nbyte=0;
                                }
                            }
                            else{
                                nbyte++;
                            }  
                        }
                        else if(trama_rx[(nbyte-2)] ==','){
                            
                            if(trama_rx[(nbyte-3)] ==','){
                                appData.Recibido_mensaje_gps = true;
                                if((BUFFER_RX_SIZE-nbyte)<BUFFER_REMAIN_LIMIT){
                                    nbyte=0;
                                }
                            }
                            else{
                                nbyte++;
                            }
                        }
                        else{
                            nbyte++;
                        }
                    }
                    else{
                        nbyte++;
                    }   
                }
                else{      
                    nbyte++;
                }    
            //}  
            if(nbyte>(BUFFER_RX_SIZE-1)){
                nbyte=0; //curarme en salud cuando desborde el buffer
            }
        }     
        PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_2_RECEIVE);
    }

    else if (PLIB_INT_SourceFlagGet(INT_ID_0,INT_SOURCE_USART_2_TRANSMIT))
    {
        PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_USART_2_TRANSMIT);
    }
    
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_USART_2_ERROR);
    
}

 /*****************************************************************************/      
void __ISR(_UART_1_VECTOR, ipl1AUTO) _IntHandlerDrvUsartInstance1(void)
{
    DRV_USART_TasksTransmit(sysObj.drvUsart1);
    DRV_USART_TasksReceive(sysObj.drvUsart1);
    DRV_USART_TasksError(sysObj.drvUsart1);
}            
// ****************************************************************************
//                     SPI
// ****************************************************************************
void __ISR(_SPI_2_VECTOR, ipl1AUTO) _IntHandlerSPIInstance0(void)
{
    DRV_SPI_Tasks(sysObj.spiObjectIdx0);
}
    /*******************************************************************************
 End of File
*/

