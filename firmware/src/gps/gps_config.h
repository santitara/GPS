/*********include headers***********************************************************/
//#include "AT_comands.h"
#include "stdint.h"
#include "app.h"
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
    //gprs config states
    SET_ENABLE_GPRS,
    SET_AT_CREG,
    SET_ATTACH_GPRS,
    SET_GPRS_RESET_IP,
    SET_APN_CON_TYPE,
    SET_APN,
    SET_OPEN_APN,
    SET_QUERY_APN,
    //httpa config states
    SET_GPRS_HTTP_START,
    SET_CID_SEL,
    SET_SSL_ENABLE, 
            
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
}gps_config_lv;

/*********public functions prototype***********************************************************/
void    gps_config_init_module          (void);
void    gps_config_ON_OFF_module        (void);
void    gps_config_at_general           (void);
void    gps_config_at_GPS               (void);
void    gps_config_at_GRPS              (void);
void    gps_config_at_HTTP              (void);

/*********public variables***********************************************************/
//extern gps_config_lv gps_config_v;



