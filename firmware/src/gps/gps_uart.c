/**
 *******************************************************************************
 * @file gps_uart.C
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
#include "gps_uart.h"
#include "app.h"
#include "gps_common.h"
#include "gps_config.h"
#include "../uart/uart_ops.h"
#include "time.h"
#include <stdio.h>
/* Defines  ------------------------------------------------------------------*/  
#define MAX_ERRORS_CONSECUTIVES     5
#define MAX_CONSECUTIVE_ERRORS_TOUT 2
#define MAX_ERR_COVERAGE            2
/* Const vars ----------------------------------------------------------------*/    
const char *ERR = "ERROR";
const char *UGNSINF0 = "+CGNSINF: 1,0";
const char *UGNSINF1 = "+CGNSINF: 1,1";
const char *CREG_OK = "+CREG:0,1";
const char *CREG_NOK = "+CREG:0,0";
const char *HTTPACTION_RES = "+HTTPACTION";
const char *HTTPACTION_OK_GET = "+HTTPACTION: 0,200";
const char *HTTPACTION_OK_POST = "+HTTPACTION: 1,200";
const char *BTCONNECT = "+BTCONNECT";
const char *BTDISCONN = "+BTDISCONN";
const char *BTCONECTING = "+BTCONNECTING:";

const char *URL_TERMINATOR ="\"\r\n";
//URLs
const char *URL_MISANA = "AT+HTTPPARA=\"URL\",\"http://misana-iot.es:1880/api/v2/?token=crjw75yS9gnBsj26uQWEqm9v1vqmMKQ6&id=";
const char *URL_RPI = "AT+HTTPPARA=\"URL\",\"79.109.207.104:3658/metrics?imei=";
const char *URL_LOCATEC = "AT+HTTPPARA=\"URL\",\"https://www.locatec.es/proyectos/manezylozano/ws/getData.php?imei=";
//GET
const char *URL_ST_TRACKER_GRAFANA2_2 ="&payload=";
const char *TRAMA_INI = "[";
const char *TRAMA_GEO = "{%22fields%22:{%22latitude%22:";
const char *TRAMA_GEO2 = ",%22longitude%22:";
const char *TRAMA_GEO3 = ",%22speed%22:";
const char *TRAMA_GEO4 = "},%22tags%22:{},%22timestamp%22:null";

const char *TRAMA_TIMESTAMP ="},%22timestamp%22:";
const char *TRAMA_NEXT = "},";
const char *TRAMA_END = "}]\"\r\n";

//const char *URL_TEST = "AT+HTTPPARA=\"URL\",\"http://misana-iot.es:1880/api/v2/?token=crjw75yS9gnBsj26uQWEqm9v1vqmMKQ6&id=867717038251048&payload=[{%22fields%22:{%22latitude%22:39.262280,%22longitude:%22-1.913421},%22timestamp%22:1598872665},{%22fields%22:{%22latitude%22:39.262280,%22longitude%22:-1.913421},%22timestamp%22:1598872667},{%22fields%22:{%22latitude%22:39.262280,%22longitude%22:-1.913421},%22timestamp%22:1598872669}]\"\r\n";


//POST
const char *TRAMA_INI_POST = "[";
const char *TRAMA_LAT_POST = "{\"latitude\":";
const char *TRAMA_LON_POST =",\"longitude\":";
const char *TRAMA_SPEED_POST =",\"speed\":";
const char *TRAMA_TIMESTAMP_POST =",\"timestamp\":";
const char *TRAMA_END_POST = "}]";
/*********private enum***************************************************************/

/*********private struct***********************************************************/
struct tm tm_str;
gps_uart_t gps_uart_v ={
    .data.index_data = 0,
};


/*********private functions prototype***********************************************************/
void gps_uart_get_time              (char *date, uint8_t index);
void gps_uart_get_latitude          (char *data, uint8_t index);
void gps_uart_get_longitude         (char *data, uint8_t index);
void gps_uart_get_speed             (char *data, uint8_t index);
void gps_uart_prepare_data_frame    (uint8_t index);
void gps_uart_process_GNSINF        (uint8_t index);
void gps_uart_get_url_post          (void);
void gps_uart_check_http_res        (void);
void gps_uart_check_bt_res          (void);
void gps_uart_prepare_frame_bt      (void);
/*********private vars***************************************************************/

/**
 * @brief gps uart rx state. Function to process all data info received
 * message
 * @param[in] none
 * @param[out]none
 * @return none
 */
void gps_uart_rx_state (void)
{
    static count_tout = 0;
    static err_count = 0;
    //ret = strstr(buff,check_msg); 
    if(gps_uart_v.flag_rx_end)
    {
        gps_config_v.counter_tout = 0;
        gps_config_v.flag_timeout = 0;
        
        if (uart_ops_process_response(gps_uart_v.rx_buffer,UGNSINF0))
        {
            gps_config_v.state = gps_config_v.state_ok;
            gps_uart_v.flag_rx_end = 0;
            gps_uart_v.index=0;
            gps_uart_v.ptr = strstr(gps_uart_v.rx_buffer,UGNSINF0);
            led_control_v.module_status_bit.gps_state_bit = 0;
            /*if(gps_uart_v.ptr != NULL)
            {
                gps_uart_process_GNSINF(gps_uart_v.data.index_data);
                if(gps_uart_v.data.index_data >= MSG_TO_SEND)
                {
                    //reset index
                    gps_uart_v.data.index_data = 0; 
                }
                else
                {
                    gps_uart_v.data.index_data++;
                }
            }*/
            /*check msg response of http*/
            gps_uart_check_http_res();
            gps_uart_check_bt_res();
            memset(gps_uart_v.rx_buffer,1,255);
        }
        else if (uart_ops_process_response(gps_uart_v.rx_buffer,UGNSINF1))
        {
            gps_config_v.flag_report_rx =0;
            gps_config_v.state = gps_config_v.state_ok;
            gps_uart_v.flag_rx_end = 0;
            gps_uart_v.index=0;
            led_control_v.module_status_bit.gps_state_bit = 1;

            gps_uart_v.ptr = strstr(gps_uart_v.rx_buffer,UGNSINF1);
            if(gps_uart_v.ptr != NULL)
            {
                gps_uart_process_GNSINF(gps_uart_v.data.index_data);
                if(gps_uart_v.data.index_data >= MSG_TO_SEND)
                {
                    //reset index
                    gps_uart_v.data.index_data = 0; 
                }
                else
                {
                    gps_uart_v.data.index_data++;
                }
                if(led_control_v.module_status_bit.bt_state_bit)
                {
                    gps_uart_prepare_frame_bt();
                }
            }
            /*check msg response of http*/
            gps_uart_check_http_res();
            gps_uart_check_bt_res();
            memset(gps_uart_v.rx_buffer,1,255);
        }
        else if(uart_ops_process_response(gps_uart_v.rx_buffer,"OK"))
        {
            //get imei data and store
            if(gps_config_v.flag_get_imei == 1)
            {
                strncpy(gps_uart_v.data.imei,&gps_uart_v.rx_buffer[2],15);
                gps_config_v.flag_get_imei = 0;
            }
            /*check if state is one of send packet to confirm that packet*/
            if(gps_config_v.state >=  SET_HTTP_FRAME && gps_config_v.state <= SEND_HTTP_FRAME)
            {
                //Set gprs status
                led_control_v.module_status_bit.gprs_state_bit = 1;
            }
            gps_config_v.state = gps_config_v.state_ok;
            gps_uart_v.flag_rx_end = 0;
            gps_uart_v.index=0;
            memset(gps_uart_v.rx_buffer,1,255);
        }
        else if(uart_ops_process_response(gps_uart_v.rx_buffer,ERR))
        {
            if(gps_config_v.flag_gprs_sent)
            {
                //gps_config_v.state = ASK_COVERAGE;
                led_control_v.module_status_bit.web_state_bit = 0;
            }
            if(gps_config_v.prev_state == SET_GPS_REPORT)
            {
                led_control_v.module_status_bit.gps_state_bit = 0;
            }
            if(gps_config_v.prev_state >=  SET_HTTP_FRAME && gps_config_v.prev_state <= SEND_HTTP_FRAME)
            {
                led_control_v.module_status_bit.gprs_state_bit = 0;
            }
            err_count++;
            gps_config_v.state = gps_config_v.state_wrong;
            gps_uart_v.flag_rx_end = 0;
            gps_uart_v.index=0;
            memset(gps_uart_v.rx_buffer,1,BUFF_SIZE_RX);
            if(err_count > MAX_ERRORS_CONSECUTIVES && gps_config_v.state >= SET_GPS_REPORT)
            {
                err_count = 0;
                gps_config_v.state = IDLE;
                memset(gps_uart_v.rx_buffer,1,BUFF_SIZE_RX);
                memset(gps_uart_v.data_frame_tx,0,SIZE_BUF_DATA_TX);
                gps_uart_v.flag_rx_end = 0;
                gps_uart_v.index=0;
                gps_config_v.flag_gps_report = 0;
            }
            else if(err_count >MAX_ERRORS_CONSECUTIVES)
            {
                if(gps_config_v.state != START_AT_CONFIG)
                {
                    gps_config_v.state = gps_config_v.state - 1;
                }
                err_count = 0;
            }
        }
        else if(uart_ops_process_response(gps_uart_v.rx_buffer,CREG_OK))
        {
            gps_config_v.state = gps_config_v.state_ok;
            gps_uart_v.flag_rx_end = 0;
            gps_uart_v.index=0;
            memset(gps_uart_v.rx_buffer,1,255);
            led_control_v.module_status_bit.gprs_state_bit = 1;
        }
        else if(uart_ops_process_response(gps_uart_v.rx_buffer,CREG_NOK))
        {
            gps_config_v.state = gps_config_v.state_ok;
            gps_uart_v.flag_rx_end = 0;
            gps_uart_v.index=0;
            memset(gps_uart_v.rx_buffer,1,255);
            led_control_v.module_status_bit.gprs_state_bit = 0;
        }
        else
        {
            gps_config_v.state = gps_config_v.state_wrong;
            gps_uart_v.flag_rx_end = 0;
            gps_uart_v.index=0;
            memset(gps_uart_v.rx_buffer,1,255);
        }
        
    } 
    else
    {
        if(gps_config_v.flag_timeout == 1)
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
        }
    }

}

/**
 * @brief gps uart get time. Funtion to convert time of gps module obtained in 
 * timestampo to send to web server
 * message
 * @param[in] char *date, date received from gps module in format AAAAMMDDHHMMSS
 * @param[out]none
 * @return none
 */
void gps_uart_get_time(char *date, uint8_t index)
{
    char date2[20]={0};
    strncpy(date2,date,4);
    strcat(date2+4,"-");
    strncpy(date2+5,date+4,2);
    strcat(date2+6,"-");
    strncpy(date2+8,date+6,2);
    strcat(date2+10,"-");
    strncpy(date2+11,date+8,2);
    strcat(date2+12,"-");
    strncpy(date2+14,date+10,2);
    strcat(date2+15,"-");

    strncpy(date2+17,date+12,2);
    date2[4] = date2[7] = date2[10] = date2[13] = date2[16]='\0';

    tm_str.tm_year = atoi(&date2[0]) - 1900;
    tm_str.tm_mon = atoi(&date2[5]) - 1;
    tm_str.tm_mday = atoi(&date2[8]);
    tm_str.tm_hour=atoi(&date2[11]);
    tm_str.tm_min=atoi(&date2[14]);
    tm_str.tm_sec=atoi(&date2[17]);
    time_t t = mktime( &tm_str );
    gps_uart_v.data.time_stamp[index] = t;
    itoa(gps_uart_v.data.time_stamp_s,t,10);
    
}

/**
 * @brief gps uart get latitude. Function get latitude from data frame of gps module
 * message
 * @param[in] char *data, pontier to char of data frame
 * @param[out]none
 * @return none
 */
void gps_uart_get_latitude(char *data, uint8_t index)
{
    strcpy(gps_uart_v.data.lat_s,data);
    gps_uart_v.data.lat[index] = atof(data);
    
}
/**
 * @brief gps uart get longitude. Function get longitude from data frame of gps module
 * message
 * @param[in] char *data, pontier to char of data frame
 * @param[out]none
 * @return none
 */
void gps_uart_get_longitude(char *data, uint8_t index)
{
    strcpy(gps_uart_v.data.lon_s,data);
    gps_uart_v.data.longi[index] =atof(data);
}

/**
 * @brief gps uart get speed. Function get speed from data frame of gps module
 * message
 * @param[in] char *data, pontier to char of data frame
 * @param[out]none
 * @return none
 */
void gps_uart_get_speed(char *data, uint8_t index)
{
	strcpy(gps_uart_v.data.speed_s,data);																				  
    gps_uart_v.data.speed[index] = atof(data);  //version de la velocidad instantanea en coma flotante
    
    /*if(gps_data_v.speed<0.4)
    {
        //DRV_OC0_Disable();  //apago la salida de pulsos
        DRV_TMR2_Stop(); 
    }
    else
    {
        if(gps_data_v.speed>25) //saturo a 25kmh los pulsos
        { 
            DRV_OC_Set_Freq(1115);
            //DRV_OC0_Enable();
            DRV_TMR2_Start();
        }
        else*/
        /*{
            aux_cal_pulsos = K_PR_V/vel_gps;
            if(aux_cal_pulsos>65535)
            {
                aux_salida_pulsos=65535;
            }
            else
            {
                aux_salida_pulsos = (int) aux_cal_pulsos;
            }
            //DRV_OC0_Enable();
            DRV_TMR2_Start();
            DRV_OC_Set_Freq(aux_salida_pulsos);
          
        }
    }*/
}

/**
 * @brief gps uart prepare data frame. Function to implement data frame to send 
 * to web server.
 * @param[in] none
 * @param[out]none
 * @return none
 */
void gps_uart_prepare_data_frame(uint8_t index)
{            
    if(led_control_v.module_status_bit.gps_state_bit == 0)
    {  //si el gps no tiene fix preparo los datos 
        strcpy(gps_uart_v.data.lat_s,"0");
        strcpy(gps_uart_v.data.lon_s,"0");
        strcpy(gps_uart_v.data.speed_s,"0");
        strcpy(gps_uart_v.data.time_stamp_s,"null");
    }
    
    if(gps_uart_v.data.msg_num == 0)
    {
        memset(gps_uart_v.data_frame_tx,0,SIZE_BUF_DATA_TX);
        
        if(gps_config_v.http_method == GET)
        {
            strcpy(gps_uart_v.data_frame_tx,URL_MISANA);
            strcat(gps_uart_v.data_frame_tx,gps_uart_v.data.imei);
            strcat(gps_uart_v.data_frame_tx,URL_ST_TRACKER_GRAFANA2_2);
            strcat(gps_uart_v.data_frame_tx,TRAMA_INI);
        }
       /* else //POST
        {
            strcpy(gps_uart_v.data_frame_tx,TRAMA_INI_POST);
        }*/
    }
    if(gps_config_v.http_method == GET)
    {
        strcat(gps_uart_v.data_frame_tx,TRAMA_GEO);
        strcat(gps_uart_v.data_frame_tx,gps_uart_v.data.lat_s);

        strcat(gps_uart_v.data_frame_tx,TRAMA_GEO2);
        strcat(gps_uart_v.data_frame_tx,gps_uart_v.data.lon_s);
        
        strcat(gps_uart_v.data_frame_tx,TRAMA_GEO3);
        strcat(gps_uart_v.data_frame_tx,gps_uart_v.data.speed_s);
        
        strcat(gps_uart_v.data_frame_tx,TRAMA_TIMESTAMP);
        strcat(gps_uart_v.data_frame_tx,gps_uart_v.data.time_stamp_s);
    }
   /* else
    {
        strcat(gps_uart_v.data_frame_tx,TRAMA_LAT_POST);
        strcat(gps_uart_v.data_frame_tx,gps_uart_v.data.lat_s);

        strcat(gps_uart_v.data_frame_tx,TRAMA_LON_POST);
        strcat(gps_uart_v.data_frame_tx,gps_uart_v.data.lon_s);
        
        strcat(gps_uart_v.data_frame_tx,TRAMA_SPEED_POST);
        strcat(gps_uart_v.data_frame_tx,gps_uart_v.data.speed_s);

        strcat(gps_uart_v.data_frame_tx,TRAMA_TIMESTAMP_POST);
        strcat(gps_uart_v.data_frame_tx,gps_uart_v.data.time_stamp_s); 
    }*/
    
    /*snprintf(gps_uart_v.data_frame_tx_copy,SIZE_BUF_DATA_TX,"%s%s%f%s%f%s%f%s%lu%s",
                TRAMA_INI_POST,TRAMA_LAT_POST,gps_uart_v.data.lat[index],
                TRAMA_LON_POST,gps_uart_v.data.longi[index],
                TRAMA_SPEED_POST,gps_uart_v.data.speed[index],
                TRAMA_TIMESTAMP_POST,gps_uart_v.data.time_stamp[index],TRAMA_END_POST);    
    
   
    */
    if(gps_uart_v.data.msg_num == (MSG_TO_SEND - 1))
    {
        if(gps_config_v.http_method == GET)
        {
            strcat(gps_uart_v.data_frame_tx,TRAMA_END);
            gps_config_v.state = SET_HTTP_FRAME;
        }
        else //POST
        {
            //strcat(gps_uart_v.data_frame_tx,TRAMA_END_POST);
            //gps_config_v.state = SET_HTTP_DATA_POST;
            strcpy(gps_uart_v.data_frame_tx,TRAMA_INI_POST);
            uint8_t i;
            for(i = 0; i<MSG_TO_SEND; i++)
            {
                snprintf(gps_uart_v.data_frame_tx_copy,SIZE_BUF_DATA_TX,"%s%f%s%f%s%f%s%lu",
                    TRAMA_LAT_POST,gps_uart_v.data.lat[i],
                    TRAMA_LON_POST,gps_uart_v.data.longi[i],
                    TRAMA_SPEED_POST,gps_uart_v.data.speed[i],
                    TRAMA_TIMESTAMP_POST,gps_uart_v.data.time_stamp[i]);
                strcat(gps_uart_v.data_frame_tx,gps_uart_v.data_frame_tx_copy);
                if(i < (MSG_TO_SEND-1))
                {
                    strcat(gps_uart_v.data_frame_tx,TRAMA_NEXT);
                }
            }
            strcat(gps_uart_v.data_frame_tx,TRAMA_END_POST);
        }
        gps_config_v.state = SET_HTTP_DATA_POST;
        gps_uart_v.data.msg_num = 0;
    }
    else
    {
        //strcat(gps_uart_v.data_frame_tx,TRAMA_NEXT);
        gps_uart_v.data.msg_num++;
        gps_config_v.state = IDLE;
    }
   
    
}

/**
 * @brief gps uart process GNSINF. Function to extract data from frame of gps module
 * latitude, longitude, speed, timestamp...
 * to web server.
 * @param[in] none
 * @param[out]none
 * @return none
 */
void gps_uart_process_GNSINF(uint8_t index)
{
    char *ptr_data_gps;
    char data_gps[BUFF_SIZE_RX];
    strncpy(data_gps,(gps_uart_v.ptr+14),56); //copio la informacion relevante del gps
    //copy data pointer
    ptr_data_gps = data_gps;
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
    gps_uart_get_speed(ptr_data_gps,index);

    gps_uart_prepare_data_frame(index);
}

/**
 * @brief gps uart prepare url POST. Configure and set url to POST msg
 * @param[in] none
 * @param[out]none
 * @return none
 */
void gps_uart_prepare_url_post(void)
{
    //Clean buffer
    memset(gps_uart_v.data_frame_tx,0,SIZE_BUF_DATA_TX);
    //copy url to send post frame
    #if SERVER_LOCATION == MISANA
        strcpy(gps_uart_v.data_frame_tx,URL_MISANA);  
    #elif SERVER_LOCATION == LOCATEC
        strcpy(gps_uart_v.data_frame_tx,URL_LOCATEC);
    #elif SERVER_LOCATION == RASPBERRY
        strcpy(gps_uart_v.data_frame_tx,URL_RPI); 
    #endif
    strcat(gps_uart_v.data_frame_tx,gps_uart_v.data.imei);
    strcat(gps_uart_v.data_frame_tx,"\"\r\n");
}

void gps_uart_check_http_res(void)
{
    static uint8_t count_err_coverage = 0;
    if(uart_ops_process_response(gps_uart_v.rx_buffer,HTTPACTION_RES))
    {
        if(gps_config_v.http_method == GET)
        {
            if(uart_ops_process_response(gps_uart_v.rx_buffer,HTTPACTION_OK_GET))
            {
                led_control_v.module_status_bit.web_state_bit = 1;
            }
            else
            {
                led_control_v.module_status_bit.web_state_bit = 0;
            }
        }
        else    //POST
        {
            if(uart_ops_process_response(gps_uart_v.rx_buffer,HTTPACTION_OK_POST))
            {
                led_control_v.module_status_bit.web_state_bit = 1;
            }
            else
            {
                led_control_v.module_status_bit.web_state_bit = 0;
                count_err_coverage++;
                if(count_err_coverage >= MAX_ERR_COVERAGE)
                {
                    gps_config_v.state = ASK_COVERAGE;
                }
            }
        }
    }   
}

void gps_uart_check_bt_res(void)
{
    if(uart_ops_process_response(gps_uart_v.rx_buffer,BTCONECTING))
        {
            //led_control_v.module_status_bit.bt_state_bit = 1;
            //gps_config_v.flag_confg_bt = 1;
            gps_config_v.state = SET_SSP_BLUETOOTH;
        }
        if(uart_ops_process_response(gps_uart_v.rx_buffer,BTCONNECT))
        {
            led_control_v.module_status_bit.bt_state_bit = 1;
            //gps_config_v.flag_confg_bt = 1;
            //gps_config_v.state = SET_SSP_BLUETOOTH;
        }
        if(uart_ops_process_response(gps_uart_v.rx_buffer,BTDISCONN))
        {
            led_control_v.module_status_bit.bt_state_bit = 0;
        }
}

void gps_uart_prepare_frame_bt(void)
{
    //clean buffer
    memset(gps_uart_v.bt_frame_tx,0,BUFF_SIZE_BT);
    //fill buffer (lat,long,speed,timpestamp)
    strcpy(gps_uart_v.bt_frame_tx,gps_uart_v.data.lat_s);
    strcat(gps_uart_v.bt_frame_tx,",");
    strcat(gps_uart_v.bt_frame_tx,gps_uart_v.data.lon_s);
    strcat(gps_uart_v.bt_frame_tx,",");
    strcat(gps_uart_v.bt_frame_tx,gps_uart_v.data.speed_s);
    strcat(gps_uart_v.bt_frame_tx,",");
    strcat(gps_uart_v.bt_frame_tx,gps_uart_v.data.time_stamp_s);
    strcat(gps_uart_v.bt_frame_tx,"\n"); 
}

