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

//etiquetas del gps
const char *VAR_DEV ="dev=";
const char *VAR_LAT ="lat=";
const char *VAR_LON ="lng=";
const char *VAR_VEL ="vel=";
const char *VAR_TS ="ts=";
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
gps_uart_t gps_uart_v;


/*********private functions prototype***********************************************************/
void gps_uart_get_time              (char *date);
void gps_uart_get_latitude          (char *data);
void gps_uart_get_longitude         (char *data);
void gps_uart_get_speed             (char *data);
void gps_uart_prepare_data_frame    (void);
void gps_uart_process_GNSINF        (void);
void gps_uart_get_url_post          (void);
void gps_uart_check_http_res        (void);
void gps_uart_check_bt_res          (void);
void gps_uart_prepare_frame_bt      (void);
/*********private vars***************************************************************/

/**
 * @brief gps uart write. Write bytes in UART port
 * @param[in] char pointer, size of data to send 
 * @return bool var. true when finish false while not send all bytes
 */
bool gps_uart_write(char *data, uint8_t size)
{
    if(*gps_config_v.msg == '\0')
    {
        return true;
    }
    /* Write a character at a time, only if transmitter is empty */
    while (PLIB_USART_TransmitterIsEmpty(USART_ID_2))
    {
        /* Send character */
        PLIB_USART_TransmitterByteSend(USART_ID_2, *gps_config_v.msg);

        /* Increment to address of next character */
        gps_config_v.msg++;

        if(*gps_config_v.msg == '\0')
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief gps uart process response. Check data received from uart with expected
 * message
 * @param[in] uint8_t *buff. Buffer with data receivd,
 * @param[in ]const char *check_msg, msg to compare buff 
 * @return bool var. true when msg to chec is match, false when not
 */
bool gps_uart_process_response(uint8_t *buff, const char *check_msg)
{
    char* ret = 0;
    ret = strstr(buff,check_msg);
    if(ret != NULL)
    {
      return true;
    }
    else
    {
        return false;
    }
}

/**
 * @brief gps uart rx state. Function to process all data info received
 * message
 * @param[in] none
 * @param[out]none
 * @return none
 */
void gps_uart_rx_state (void)
{
    char* ret = 0;
    static count_tout = 0;
    static err_count = 0;
    //ret = strstr(buff,check_msg); 
    if(gps_uart_v.flag_rx_end)
    {
        gps_config_v.counter_tout = 0;
        gps_config_v.flag_timeout = 0;
        /*if(gps_uart_process_response(gps_uart_v.rx_buffer,HTTPACTION_RES))
        {
            if(gps_config_v.http_method == GET)
            {
                if(gps_uart_process_response(gps_uart_v.rx_buffer,HTTPACTION_OK_GET))
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
                if(gps_uart_process_response(gps_uart_v.rx_buffer,HTTPACTION_OK_POST))
                {
                    led_control_v.module_status_bit.web_state_bit = 1;
                }
                else
                {
                    led_control_v.module_status_bit.web_state_bit = 0;
                    gps_config_v.state = ASK_COVERAGE;
                }
            }
        }*/
        if (gps_uart_process_response(gps_uart_v.rx_buffer,UGNSINF0))
        {
            gps_config_v.state = gps_config_v.state_ok;
            gps_uart_v.flag_rx_end = 0;
            gps_uart_v.index=0;
            gps_uart_v.ptr = strstr(gps_uart_v.rx_buffer,UGNSINF0);
            led_control_v.module_status_bit.gps_state_bit = 0;
           /* if(gps_uart_v.ptr != NULL)
            {
                gps_uart_process_GNSINF();
            }*/
            /*check msg response of http*/
            gps_uart_check_http_res();
            gps_uart_check_bt_res();
            memset(gps_uart_v.rx_buffer,1,255);
        }
        else if (gps_uart_process_response(gps_uart_v.rx_buffer,UGNSINF1))
        {
            gps_config_v.flag_report_rx =0;
            gps_config_v.state = gps_config_v.state_ok;
            gps_uart_v.flag_rx_end = 0;
            gps_uart_v.index=0;
            led_control_v.module_status_bit.gps_state_bit = 1;

            gps_uart_v.ptr = strstr(gps_uart_v.rx_buffer,UGNSINF1);
            if(gps_uart_v.ptr != NULL)
            {
                gps_uart_process_GNSINF();
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
        else if(gps_uart_process_response(gps_uart_v.rx_buffer,"OK"))
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
        else if(gps_uart_process_response(gps_uart_v.rx_buffer,ERR))
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
        else if(gps_uart_process_response(gps_uart_v.rx_buffer,CREG_OK))
        {
            gps_config_v.state = gps_config_v.state_ok;
            gps_uart_v.flag_rx_end = 0;
            gps_uart_v.index=0;
            memset(gps_uart_v.rx_buffer,1,255);
            led_control_v.module_status_bit.gprs_state_bit = 1;
        }
        else if(gps_uart_process_response(gps_uart_v.rx_buffer,CREG_NOK))
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
void gps_uart_get_time(char *date)
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

    itoa(gps_uart_v.data.time_stamp,t,10);
    
}

/**
 * @brief gps uart get latitude. Function get latitude from data frame of gps module
 * message
 * @param[in] char *data, pontier to char of data frame
 * @param[out]none
 * @return none
 */
void gps_uart_get_latitude(char *data)
{
    strcpy(gps_uart_v.data.lat_s,data);
    gps_uart_v.data.lat = atof(data);
    
}
/**
 * @brief gps uart get longitude. Function get longitude from data frame of gps module
 * message
 * @param[in] char *data, pontier to char of data frame
 * @param[out]none
 * @return none
 */
void gps_uart_get_longitude(char *data)
{
    strcpy(gps_uart_v.data.lon_s,data);
    gps_uart_v.data.longi =atof(data);
}

/**
 * @brief gps uart get speed. Function get speed from data frame of gps module
 * message
 * @param[in] char *data, pontier to char of data frame
 * @param[out]none
 * @return none
 */
void gps_uart_get_speed(char *data)
{
	strcpy(gps_uart_v.data.speed_s,data);																				  
    gps_uart_v.data.speed = atof(data);  //version de la velocidad instantanea en coma flotante
    
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
void gps_uart_prepare_data_frame(void)
{            
    if(led_control_v.module_status_bit.gps_state_bit == 0)
    {  //si el gps no tiene fix preparo los datos 
        strcpy(gps_uart_v.data.lat_s,"0");
        strcpy(gps_uart_v.data.lon_s,"0");
        strcpy(gps_uart_v.data.speed_s,"0");
        strcpy(gps_uart_v.data.time_stamp,"null");
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
        else //POST
        {
            strcpy(gps_uart_v.data_frame_tx,TRAMA_INI_POST);
        }
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
        strcat(gps_uart_v.data_frame_tx,gps_uart_v.data.time_stamp);
    }
    else
    {
        strcat(gps_uart_v.data_frame_tx,TRAMA_LAT_POST);
        strcat(gps_uart_v.data_frame_tx,gps_uart_v.data.lat_s);

        strcat(gps_uart_v.data_frame_tx,TRAMA_LON_POST);
        strcat(gps_uart_v.data_frame_tx,gps_uart_v.data.lon_s);
        
        strcat(gps_uart_v.data_frame_tx,TRAMA_SPEED_POST);
        strcat(gps_uart_v.data_frame_tx,gps_uart_v.data.speed_s);

        strcat(gps_uart_v.data_frame_tx,TRAMA_TIMESTAMP_POST);
        strcat(gps_uart_v.data_frame_tx,gps_uart_v.data.time_stamp); 
    }
    snprintf(gps_uart_v.data_frame_tx_copy,SIZE_BUF_DATA_TX,"%s%s12.2%s1.5%s5.4%s1238545%s",TRAMA_INI_POST,TRAMA_LAT_POST,
                                                            TRAMA_LON_POST,TRAMA_SPEED_POST,TRAMA_TIMESTAMP_POST,TRAMA_END_POST);    
    if(gps_uart_v.data.msg_num == MSG_TO_SEND)
    {
        if(gps_config_v.http_method == GET)
        {
            strcat(gps_uart_v.data_frame_tx,TRAMA_END);
            gps_config_v.state = SET_HTTP_FRAME;
        }
        else //POST
        {
            strcat(gps_uart_v.data_frame_tx,TRAMA_END_POST);
            gps_config_v.state = SET_HTTP_DATA_POST;
        }
        gps_uart_v.data.msg_num = 0;
    }
    else
    {
        strcat(gps_uart_v.data_frame_tx,TRAMA_NEXT);
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
void gps_uart_process_GNSINF(void)
{
    char *ptr_data_gps;
    strncpy(gps_uart_v.data_gps,(gps_uart_v.ptr+14),56); //copio la informacion relevante del gps
    //copy data pointer
    ptr_data_gps = gps_uart_v.data_gps;
    //get timestamp
    ptr_data_gps = strtok(ptr_data_gps,",");
    gps_uart_get_time(ptr_data_gps);
    //get latitude 
    ptr_data_gps = strtok(NULL,",");
    gps_uart_get_latitude(ptr_data_gps);
    //get longitude 
    ptr_data_gps = strtok(NULL,",");
    gps_uart_get_longitude(ptr_data_gps);
    //get speed
    ptr_data_gps = strtok(NULL,",");
    ptr_data_gps = strtok(NULL,",");
    gps_uart_get_speed(ptr_data_gps);

    gps_uart_prepare_data_frame();
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
    if(gps_uart_process_response(gps_uart_v.rx_buffer,HTTPACTION_RES))
    {
        if(gps_config_v.http_method == GET)
        {
            if(gps_uart_process_response(gps_uart_v.rx_buffer,HTTPACTION_OK_GET))
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
            if(gps_uart_process_response(gps_uart_v.rx_buffer,HTTPACTION_OK_POST))
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
    if(gps_uart_process_response(gps_uart_v.rx_buffer,BTCONECTING))
        {
            //led_control_v.module_status_bit.bt_state_bit = 1;
            //gps_config_v.flag_confg_bt = 1;
            gps_config_v.state = SET_SSP_BLUETOOTH;
        }
        if(gps_uart_process_response(gps_uart_v.rx_buffer,BTCONNECT))
        {
            led_control_v.module_status_bit.bt_state_bit = 1;
            //gps_config_v.flag_confg_bt = 1;
            //gps_config_v.state = SET_SSP_BLUETOOTH;
        }
        if(gps_uart_process_response(gps_uart_v.rx_buffer,BTDISCONN))
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
    strcat(gps_uart_v.bt_frame_tx,gps_uart_v.data.time_stamp);
    strcat(gps_uart_v.bt_frame_tx,"\n"); 
}

