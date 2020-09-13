/*********include headers***********************************************************/
#include "gps_uart.h"
#include "app.h"
#include "gps_common.h"
#include "time.h"

const char *ERR = "ERROR";
const char *UGNSINF0 = "+CGNSINF: 1,0";
const char *UGNSINF1 = "+CGNSINF: 1,1";
const char *CREG_OK = "+CREG:0,1";
const char *CREG_NOK = "+CREG:0,0";
const char *HTTPACTION_RES = "+HTTPACTION";
const char *HTTPACTION_OK_GET = "+HTTPACTION: 0,200";
const char *HTTPACTION_OK_POST = "+HTTPACTION: 1,200";
//#include "string.h"

//#include "stdio.h"


/*********private enum***************************************************************/

/*********private struct***********************************************************/
struct tm tm_str;
gps_data_lv gps_data_v = 
{
    .msg_num = 0,
};
gps_uart_t gps_uart_v;
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
//POST
const char *URL_ST_TRACKER_GRAFANA_POST = "AT+HTTPPARA=\"URL\",\"http://misana-iot.es:1880/api/v2/?token=crjw75yS9gnBsj26uQWEqm9v1vqmMKQ6&id=865067021287761\"\n\r";
//const char *URL_ST_TRACKER_GRAFANA_POST = "AT+HTTPPARA=\"URL\",\"https://httpbin.org/post\"\n\r";
//GET
const char *URL_ST_TRACKER_GRAFANA2_1 = "AT+HTTPPARA=\"URL\",\"http://misana-iot.es:1880/api/v2/?token=crjw75yS9gnBsj26uQWEqm9v1vqmMKQ6&id=";
const char *URL_ST_TRACKER_GRAFANA2_2 ="&payload=";
const char *URL_LOCATEC = "AT+HTTPPARA=\"URL\",\"https://www.locatec.es/proyectos/manezylozano/ws/getData.php?";
const char *TRAMA_INI = "[";
const char *TRAMA_GEO = "{%22fields%22:{%22latitude%22:";
const char *TRAMA_GEO2 = ",%22longitude%22:";
const char *TRAMA_GEO3 = "},%22tags%22:{},%22timestamp%22:null";

const char *TRAMA_TIMESTAMP ="},%22timestamp%22:";
const char *TRAMA_NEXT = "},";
const char *TRAMA_END = "}]\"\r\n";
 
// const char *URL_ST_TRACKER_GRAFANA2 = "AT+HTTPPARA=\"URL\",\"http://misana-iot.es:1880/api/v2/?token=crjw75yS9gnBsj26uQWEqm9v1vqmMKQ6&id=865067028187128&payload=[{%22fields%22:{%22latitude%22:39.178832,%22longitude%22:-0.241623},%22tags%22:{},%22timestamp%22:123456789";
 
const char *URL_TEST = "AT+HTTPPARA=\"URL\",\"http://misana-iot.es:1880/api/v2/?token=crjw75yS9gnBsj26uQWEqm9v1vqmMKQ6&id=867717038251048&payload=[{%22fields%22:{%22latitude%22:39.262280,%22longitude:%22-1.913421},%22timestamp%22:1598872665},{%22fields%22:{%22latitude%22:39.262280,%22longitude%22:-1.913421},%22timestamp%22:1598872667},{%22fields%22:{%22latitude%22:39.262280,%22longitude%22:-1.913421},%22timestamp%22:1598872669}]\"\r\n"; 


//POST
 const char *TRAMA_INI_POST = "[";
 //const char *TRAMA_LAT = "{\"fields\": {\"latitude\":";
 //const char *TRAMA_LON =",\"longitude\":";
 //const char *TRAMA_TIMESTAMP ="},\"tags\": {},\"timestamp\":";
 //const char *TRAMA_NEXT = "},";
 const char *TRAMA_END_POST = "}]";
/*********private functions prototype***********************************************************/
void gps_uart_get_time(char *date);
void gps_uart_get_latitude(char *data);
void gps_uart_get_longitude(char *data);
void gps_uart_get_speed(char *data);
void gps_uart_prepare_data_frame(void);
void gps_uart_process_GNSINF(void);
/*********private vars***************************************************************/

bool gps_uart_write(char *data, uint8_t size)
{
    uint8_t a = 0;
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
            a = 1;
            return true;
        }
    }
    return false;
}



uint8_t gps_uart_process_response(uint8_t *buff, const char *check_msg)
{
    char* ret = 0;
    ret = strstr(buff,check_msg);
    if(ret != NULL)
    {
      return 1;
    }
    else
    {
        return 0;
    }
}

void gps_uart_rx_state (void)
{
   
    char* ret = 0; 
    //ret = strstr(buff,check_msg); 
    if(gps_uart_v.flag_rx_end)
    {
        gps_config_v.tout = 0;
        gps_config_v.flag_timeout = 0;
        if(gps_uart_process_response(gps_uart_v.rx_buffer,HTTPACTION_RES))
        {
            if(gps_config_v.http_method == GET)
            {
                if(gps_uart_process_response(gps_uart_v.rx_buffer,HTTPACTION_OK_GET))
                {
                    gps_config_v.module_status_bit.web_state_bit = 1;
                }
                else
                {
                    gps_config_v.module_status_bit.web_state_bit = 0;
                }
            }
            else    //POST
            {
                if(gps_uart_process_response(gps_uart_v.rx_buffer,HTTPACTION_OK_POST))
                {
                    gps_config_v.module_status_bit.web_state_bit = 1;
                }
                else
                {
                    gps_config_v.module_status_bit.web_state_bit = 0;
                }
            }
        }
        if (gps_uart_process_response(gps_uart_v.rx_buffer,UGNSINF0))
        {
            gps_config_v.state = gps_config_v.state_ok;
            gps_uart_v.flag_rx_end = 0;
            gps_uart_v.index=0;
            gps_uart_v.ptr = strstr(gps_uart_v.rx_buffer,UGNSINF0);
            gps_config_v.module_status_bit.gps_state_bit = 0;
            if(gps_uart_v.ptr != NULL)
            {
                gps_uart_process_GNSINF();
            }
            memset(gps_uart_v.rx_buffer,1,255);
        }
        else if (gps_uart_process_response(gps_uart_v.rx_buffer,UGNSINF1))
        {
            gps_config_v.flag_report_rx =0;
            gps_config_v.state = gps_config_v.state_ok;
            gps_uart_v.flag_rx_end = 0;
            gps_uart_v.index=0;
            gps_config_v.module_status_bit.gps_state_bit = 1;

            gps_uart_v.ptr = strstr(gps_uart_v.rx_buffer,UGNSINF1);
            if(gps_uart_v.ptr != NULL)
            {
                gps_uart_process_GNSINF();
            }
            memset(gps_uart_v.rx_buffer,1,255);
        }
        else if(gps_uart_process_response(gps_uart_v.rx_buffer,"OK"))
        {
            //get imei data and store
            if(gps_config_v.flag_get_imei == 1)
            {
                strncpy(gps_data_v.imei,&gps_uart_v.rx_buffer[2],15);
                gps_config_v.flag_get_imei = 0;
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
                gps_config_v.module_status_bit.web_state_bit = 0;
            }
            //else
            //{
                gps_config_v.state = gps_config_v.state_wrong;
            //}
            
            gps_uart_v.flag_rx_end = 0;
            gps_uart_v.index=0;
            memset(gps_uart_v.rx_buffer,1,255);
        }
        else if(gps_uart_process_response(gps_uart_v.rx_buffer,CREG_OK))
        {
            gps_config_v.state = gps_config_v.state_ok;
            gps_uart_v.flag_rx_end = 0;
            gps_uart_v.index=0;
            memset(gps_uart_v.rx_buffer,1,255);
        }
        else if(gps_uart_process_response(gps_uart_v.rx_buffer,CREG_NOK))
        {
            gps_config_v.state = gps_config_v.state_ok;
            gps_uart_v.flag_rx_end = 0;
            gps_uart_v.index=0;
            memset(gps_uart_v.rx_buffer,1,255);
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
            gps_config_v.state = gps_config_v.state_wrong;
            gps_config_v.tout = 0;
            gps_config_v.flag_timeout = 0;
        }
    }

}

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

    itoa(gps_data_v.time_stamp,t,10);
}

void gps_uart_get_latitude(char *data)
{
    strcpy(gps_data_v.lat_s,data);
    gps_data_v.lat = atof(data);
    
}

void gps_uart_get_longitude(char *data)
{
    strcpy(gps_data_v.lon_s,data);
    gps_data_v.longi =atof(data);
}

void gps_uart_get_speed(char *data)
{
	strcpy(gps_data_v.speed_s,data);																				  
    gps_data_v.speed = atof(data);  //version de la velocidad instantanea en coma flotante
    
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
    
void gps_uart_prepare_data_frame(void)  //escribe en trama_tx la URL para enviar al server
{            
    
    //strcpy(gps_data_v.data_frame_tx,URL_LOCATEC);
    
    if(gps_config_v.module_status_bit.gps_state_bit == 0)
    {  //si el gps no tiene fix preparo los datos 
        strcpy(gps_data_v.lat_s,"0");
        strcpy(gps_data_v.lon_s,"0");
        strcpy(gps_data_v.speed_s,"0");
        strcpy(gps_data_v.time_stamp,"null");
    }
    //to test, predefined data frame
    /*memset(gps_data_v.data_frame_tx,0,255);
    strcpy(gps_data_v.data_frame_tx,URL_ST_TRACKER_GRAFANA2);
    strcat(gps_data_v.data_frame_tx,TRAMA_END);  
    */
    if(gps_data_v.msg_num == 0)
    {
        memset(gps_data_v.data_frame_tx,0,255);
        
        //strcpy(gps_data_v.data_frame_tx,URL_ST_TRACKER_GRAFANA_POST);
        strcpy(gps_data_v.data_frame_tx,URL_ST_TRACKER_GRAFANA2_1);
        strcat(gps_data_v.data_frame_tx,gps_data_v.imei);
        if(gps_config_v.http_method == GET)
        {
            strcat(gps_data_v.data_frame_tx,URL_ST_TRACKER_GRAFANA2_2);
            strcat(gps_data_v.data_frame_tx,TRAMA_INI);
        }
        else //POST
        {
            memset(gps_data_v.data_post_tx,0,SIZE_BUF_POST_TX);
            strcat(gps_data_v.data_frame_tx,"\"\r\n");
            strcpy(gps_data_v.data_post_tx,TRAMA_INI_POST);
        }
    }
    if(gps_config_v.http_method == GET)
    {
        strcat(gps_data_v.data_frame_tx,TRAMA_GEO);
        strcat(gps_data_v.data_frame_tx,gps_data_v.lat_s);

        strcat(gps_data_v.data_frame_tx,TRAMA_GEO2);
        strcat(gps_data_v.data_frame_tx,gps_data_v.lon_s);

        strcat(gps_data_v.data_frame_tx,TRAMA_TIMESTAMP);
        strcat(gps_data_v.data_frame_tx,gps_data_v.time_stamp);
    }
    else
    {
        strcat(gps_data_v.data_post_tx,TRAMA_GEO);
        strcat(gps_data_v.data_post_tx,gps_data_v.lat_s);

        strcat(gps_data_v.data_post_tx,TRAMA_GEO2);
        strcat(gps_data_v.data_post_tx,gps_data_v.lon_s);

        strcat(gps_data_v.data_post_tx,TRAMA_TIMESTAMP);
        strcat(gps_data_v.data_post_tx,gps_data_v.time_stamp); 
    }
        
    if(gps_data_v.msg_num == MSG_TO_SEND)
    {
        if(gps_config_v.http_method == GET)
        {
            strcat(gps_data_v.data_frame_tx,TRAMA_END);
            //memset(gps_data_v.data_frame_tx,0,255);
            //strcpy(gps_data_v.data_frame_tx,URL_TEST);
        }
        else //POST
        {
            strcat(gps_data_v.data_post_tx,TRAMA_END_POST);
        }
        gps_data_v.msg_num = 0;
    }
    else
    {
        if(gps_config_v.http_method == GET)
        {
            strcat(gps_data_v.data_frame_tx,TRAMA_NEXT);
        }
        else //POST
        {
            strcat(gps_data_v.data_post_tx,TRAMA_NEXT);
        }
        gps_data_v.msg_num++;
        gps_config_v.state = IDLE;
    }
    
            
      
        
        
   /* strcat(gps_data_v.data_frame_tx,TRAMA_INI);
    strcat(gps_data_v.data_frame_tx,TRAMA_GEO);
    strcat(gps_data_v.data_frame_tx,gps_data_v.lat_s);
    
    strcat(gps_data_v.data_frame_tx,TRAMA_GEO2);
    strcat(gps_data_v.data_frame_tx,gps_data_v.lon_s);
    
    strcat(gps_data_v.data_frame_tx,TRAMA_GEO3);
    strcat(gps_data_v.data_frame_tx,gps_data_v.time_stamp);
    
    strcat(gps_data_v.data_frame_tx,TRAMA_END);*/
    
    /*
        //variables que da el gps
        strcat(gps_data_v.data_frame_tx,VAR_DEV);
        strcat(gps_data_v.data_frame_tx,"12345678");  //IMEI COMO IDENTIFICADOR UNICO DE CADA DISPOSITIVO
        strcat(gps_data_v.data_frame_tx,SEPARATOR);
        
        strcat(gps_data_v.data_frame_tx,VAR_LAT);
        strcat(gps_data_v.data_frame_tx,gps_data_v.lat_s);
        strcat(gps_data_v.data_frame_tx,SEPARATOR);

        strcat(gps_data_v.data_frame_tx,VAR_LON);
        strcat(gps_data_v.data_frame_tx,gps_data_v.lon_s);
        strcat(gps_data_v.data_frame_tx,SEPARATOR);

        strcat(gps_data_v.data_frame_tx,VAR_VEL);
        strcat(gps_data_v.data_frame_tx,gps_data_v.speed_s);
        strcat(gps_data_v.data_frame_tx,SEPARATOR);

        strcat(gps_data_v.data_frame_tx,VAR_TS);
        strcat(gps_data_v.data_frame_tx,gps_data_v.time_stamp);
        
        strcat(gps_data_v.data_frame_tx,URL_TERMINATOR);  //cierro el comando
     * */
//    }
    
}


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
    gps_buff_c_ptr_v.head++;
}

