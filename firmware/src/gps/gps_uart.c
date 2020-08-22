/*********include headers***********************************************************/
#include "gps_uart.h"
#include "app.h"
#include "gps_common.h"
#include "time.h"

const char *ERR = "ERROR";
const char *UGNSINF0 = "+CGNSINF: 1,0";
const char *UGNSINF1 = "+CGNSINF: 1,1";
//#include "string.h"

//#include "stdio.h"


/*********private enum***************************************************************/

/*********private struct***********************************************************/
struct tm tm_str;
gps_data_lv gps_data_v;
gps_uart_t gps_uart_v;
char year[4];
char mes[2];
char dia[2];
char hora[2];
char minuto[2];
char segundo[2];
int ret=0;
char time_stamp[10];
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
//const char *URL_ST_TRACKER_GRAFANA2 = "AT+HTTPPARA=\"URL\",\"http://misana-iot.es:1880/api/v2/?token=crjw75yS9gnBsj26uQWEqm9v1vqmMKQ6&id=865067021287761\"\n\r";
//GET
const char *URL_ST_TRACKER_GRAFANA2 = "AT+HTTPPARA=\"URL\",\"http://misana-iot.es:1880/api/v2/?token=crjw75yS9gnBsj26uQWEqm9v1vqmMKQ6&id=865067028127128&payload=";
const char *URL_LOCATEC = "AT+HTTPPARA=\"URL\",\"https://www.locatec.es/proyectos/manezylozano/ws/getData.php?";
  
 
  
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
        if (gps_uart_process_response(gps_uart_v.rx_buffer,UGNSINF0))
        {
            gps_config_v.state = gps_config_v.state_ok;
            gps_uart_v.flag_rx_end = 0;
            gps_uart_v.index=0;
            gps_uart_v.ptr = strstr(gps_uart_v.rx_buffer,UGNSINF0);
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
            gps_config_v.gps_state = 1;

            gps_uart_v.ptr = strstr(gps_uart_v.rx_buffer,UGNSINF1);
            if(gps_uart_v.ptr != NULL)
            {
                gps_uart_process_GNSINF();
            }
            memset(gps_uart_v.rx_buffer,1,255);
        }
        else if(gps_uart_process_response(gps_uart_v.rx_buffer,"OK")/*gps_config_v.expect_res)*/)
        {
            gps_config_v.state = gps_config_v.state_ok;
            gps_uart_v.flag_rx_end = 0;
            gps_uart_v.index=0;
            memset(gps_uart_v.rx_buffer,1,255);
        }
        else if(gps_uart_process_response(gps_uart_v.rx_buffer,ERR))
        {
            gps_config_v.state = gps_config_v.state_ok;
            gps_uart_v.flag_rx_end = 0;
            gps_uart_v.index=0;
            memset(gps_uart_v.rx_buffer,1,255);
        }
        /*/else if (gps_uart_process_response(gps_uart_v.rx_buffer,UGNSINF0))
        {
            gps_config_v.state = gps_config_v.state_ok;
            gps_uart_v.flag_rx_end = 0;
            gps_uart_v.index=0;
            gps_uart_v.ptr = strstr(gps_uart_v.rx_buffer,UGNSINF0);
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
            gps_config_v.gps_state = 1;

            gps_uart_v.ptr = strstr(gps_uart_v.rx_buffer,UGNSINF1);
            if(gps_uart_v.ptr != NULL)
            {
                gps_uart_process_GNSINF();
            }
            memset(gps_uart_v.rx_buffer,1,255);
        }*/
        else
        {
            gps_config_v.state = gps_config_v.state_wrong;
            gps_uart_v.flag_rx_end = 0;
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
       
    //trama_registro[0]='\0';
    //strcat(trama_registro,URL_LOCATEC);
    memset(gps_data_v.data_frame_tx,0,255);
    strcpy(gps_data_v.data_frame_tx,URL_LOCATEC);
    //PAYLOAD EXAMPLE:
    //36.323434&lng=0.123424&vel=2.08&fecha=23042019&hora=1632
    //concatenador(*trama_registro);
    
    if(gps_config_v.gps_state == 0)
    {  //si el gps no tiene fix preparo los datos 
        strcpy(gps_data_v.lat_s,"0");
        strcpy(gps_data_v.lon_s,"0");
        strcpy(gps_data_v.speed_s,"0");
        strcpy(gps_data_v.time_stamp,"0000000000");
    }
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
    gps_uart_get_speed(ptr_data_gps);

    gps_uart_prepare_data_frame();
}