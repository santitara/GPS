/**
 *******************************************************************************
 * @file tronic_uart.C
 * @author slopez
 * @version 1.0.0
 * @date Creation: 28/05/2020
 * @date Last modification: 28/05/2020
 * @brief gps uart.c Procesing all data bytes of UART comms
 *******************************************************************************

    @addtogroup GPS UART
    @{

*/
/* Includes ------------------------------------------------------------------*/
#include "tronic_uart.h"
#include "app.h"
#include "../gps/gps_common.h"
#include "../gps/gps_config.h"
#include "../uart/uart_ops.h"
#include "time.h"
#include <stdio.h>
/* Defines  ------------------------------------------------------------------*/  
#define MAX_ERRORS_CONSECUTIVES     5
#define MAX_CONSECUTIVE_ERRORS_TOUT 2
#define MAX_ERR_COVERAGE            2
/* Const vars ----------------------------------------------------------------*/    
const char *TRONIC = "TRONIC";
//etiquetas de la potencia tronic
const char *VAR_NPANT="n_pant=";
//pantalla logo
const char *VAR_NLOGO ="n_logo=";
 //pantalla principal
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
const char *SEPARATOR= "&";


/*********private enum***************************************************************/

/*********private struct***********************************************************/
tronic_uart_t tronic_uart_v;
/*********private functions prototype***********************************************************/

/*********private vars***************************************************************/

/**
 * @brief tronic uart rx state. Function to process all data info received
 * message
 * @param[in] none
 * @param[out]none
 * @return none
 */
void tronic_uart_rx_state (void)
{
    static count_tout = 0;
    static err_count = 0;
    //ret = strstr(buff,check_msg); 
    if(tronic_uart_v.flag_rx_end)
    {
        //gps_config_v.counter_tout = 0;
        //gps_config_v.flag_timeout = 0;
        
        if (uart_ops_process_response(tronic_uart_v.rx_buffer,TRONIC))
        {
            gps_config_v.state = gps_config_v.state_ok;
            tronic_uart_v.flag_rx_end = 0;
            tronic_uart_v.index=0;
            
            tronic_uart_get_info(tronic_uart_v.rx_buffer);
            
        }
        else if (uart_ops_process_response(tronic_uart_v.rx_buffer,TRONIC))
        {
           
        }
        else
        {
          
        }   
    } 
    else
    {
        /*if(tronic_uart_v.flag_timeout == 1)
        {
            if(gps_config_v.flag_response_moduele_ok == false)
            {
                count_tout++;
                if(count_tout > 5)
                {
                    count_tout = 0;
                    gps_config_ON_OFF_module();
                }
            }
            gps_config_v.state = gps_config_v.state_wrong;
            gps_config_v.counter_tout = 0;
            gps_config_v.flag_timeout = 0;
            gps_config_v.consecutive_errors_tout++;
            if(gps_config_v.consecutive_errors_tout>=MAX_CONSECUTIVE_ERRORS_TOUT)
            {
                if(gps_config_v.state != START_AT_CONFIG)
                {
                    gps_config_v.state = gps_config_v.state - 1;
                }
                gps_config_v.consecutive_errors_tout = 0;;
            }
        }*/
    }
  
}


void tronic_uart_get_info(char *data)
{
    char *p_buff;
    char buff_aux[BUFF_SIZE_RX];
    
    strncpy(buff_aux,data,BUFF_SIZE_RX);
    //copy data pointer
    p_buff = buff_aux;
    //first is header TRONIC and discart
    p_buff = strtok(p_buff,",");
    //get 
    p_buff = strtok(NULL,",");
    
    /*
     //get timestamp
    ptr_data_gps = strtok(ptr_data_gps,",");
    gps_uart_get_time(ptr_data_gps,index);
    //get latitude 
    ptr_data_gps = strtok(NULL,",");
    gps_uart_get_latitude(ptr_data_gps,index);
    //get longitude 
    ptr_data_gps = strtok(NULL,",");
    gps_uart_get_longitude(ptr_data_gps,index);
    //get speed
    ptr_data_gps = strtok(NULL,",");
    ptr_data_gps = strtok(NULL,",");
    gps_uart_get_speed(ptr_data_gps,index);*/
    
}

void tronic
