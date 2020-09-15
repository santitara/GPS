#ifndef _GPS_CONFIG_H    /* Guard against multiple inclusion */
#define _GPS_CONFIG_H

/*********include headers***********************************************************/
#include "stdint.h"
/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

/*********define *****************************************************************/


/*********public enum***************************************************************/
    
typedef enum
{
	//geranal config states
    INIT_CONFIG = 0,
	POWER_ON_MODULE,
	START_AT_CONFIG,
    SET_AT_FACTORY,
    SET_BAUDRATE,
    //gps config states
    SET_GPS_ON,
    SET_ANT_GPS_ON,
    SET_ECHO_OFF,
    SET_GET_IMEI,
    //gprs config states
    SET_ENABLE_GPRS,
    SET_AT_CREG,
    SET_ATTACH_GPRS,
    SET_GPRS_RESET_IP,
    SET_APN_CON_TYPE,
    SET_APN,
    SET_OPEN_APN,
    SET_QUERY_APN,
    //http config states
    SET_GPRS_HTTP_START,
    SET_CID_SEL,
    SET_SSL_ENABLE,
    SET_CONTENT_TYPE,
    SET_HTTP_URL_POST,      
    //bluetooth config states
    SET_AUTO_PAIR_BT,
    SET_BT_ON,
    SET_VIS_ON,        
    SET_BT_ONE_CONN,  
    SET_NAME_BT,     
    //gps enable info
    SET_GPS_REPORT,
    PROCESS_REPORT_GPS_BT,
    //send data 
    SET_HTTP_FRAME,
    SET_HTTP_DATA_POST,
    SEND_HTTP_FRAME,
    SET_HTTP_READ,
    ASK_COVERAGE,
            
    IDLE,
    NEXT_CONFIG_MODULE,        
    WAIT_RESPONSE,
}gps_conf_states_e;

typedef enum
{
    GET = 0,
    POST,         
}http_method_type_e;

/*********public struct***********************************************************/

typedef struct
{
    gps_conf_states_e state;
    gps_conf_states_e state_ok;
    gps_conf_states_e state_wrong;
	const char *msg;
    const char *expect_res;
    uint8_t n_retries;
    uint8_t test_mode;
    uint8_t flag_report_rx;
    uint8_t flag_timeout;
    uint8_t counter_tout;
    uint8_t flag_get_imei;
    uint8_t flag_gps_report;
    uint8_t flag_gprs_sent;
    http_method_type_e http_method;    
}gps_config_lv;
extern gps_config_lv gps_config_v;


/*********public functions prototype***********************************************************/
void    gps_config_init_module          (void);
void    gps_config_ON_OFF_module        (void);
void    gps_config_at_general           (void);
void    gps_config_at_GPS               (void);
void    gps_config_at_GRPS              (void);
void    gps_config_at_HTTP              (void);
void    gps_config_at_BT                (void);
void    gps_config_at_GPS_reports       (void);
/*********public variables***********************************************************/
//extern gps_config_lv gps_config_v;



    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _GPS_CONFIG_H */