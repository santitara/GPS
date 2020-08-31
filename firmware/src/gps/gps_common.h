/*********include headers***********************************************************/
#include "stdint.h"
/*********define *****************************************************************/
#define BLINK_2  4
#define BLINK_3  6
#define BLINK_4  8
#define BLINK_5  10
#define SIZE_CIRC_BUFF 100

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
    SEND_HTTP_FRAME,
    
    IDLE,
    NEXT_CONFIG_MODULE,        
    WAIT_RESPONSE,
}general_conf_states_e;

/*********public struct***********************************************************/
typedef struct 
{
	uint8_t gps_state_bit: 			1;
	uint8_t gprs_state_bit:			1;
	uint8_t bt_state_bit:			1;
}module_status_bit_t;

typedef struct
{
	general_conf_states_e state;
    general_conf_states_e state_ok;
    general_conf_states_e state_wrong;
	const char *msg;
	uint8_t  msg_receive;
	uint8_t gps_enable;
	module_status_bit_t module_status_bit;
    uint8_t test_mode;
    const char *expect_res;
    uint8_t n_retries;
    uint8_t gsm_state;
    uint8_t gps_state;
    uint8_t num_blink;
    //uint8_t flag_report;
    uint8_t flag_report_rx;
    uint8_t flag_timeout;
    uint8_t tout;
    uint8_t flag_get_imei;
    uint8_t flag_gps_report;
    
}gps_config_lv;

typedef struct
{
   char time_stamp[10];
   double lat;
   double longi;
   double speed;
   uint8_t data_frame_tx[700];
   char lat_s[12];
   char lon_s[12];
   char speed_s[12];
   uint8_t msg_num;
   char imei[20];
}gps_data_lv;

// The hidden definition of our circular buffer structure
typedef struct{
	uint8_t head;
	uint8_t tail;
	uint8_t max; //of the buffer
}gps_buff_c_ptr_lv;

extern gps_buff_c_ptr_lv gps_buff_c_ptr_v;
extern gps_data_lv   gps_data_v;
extern gps_config_lv gps_config_v; 
/*********public functions prototype***********************************************************/

/*********public variables***********************************************************/
//extern gps_config_lv gps_config_v;



