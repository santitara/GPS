/*********include headers***********************************************************/
//#include "AT_comands.h"
#include "stdint.h"
#include "app.h"
/*********define *****************************************************************/


/*********public enum***************************************************************/
typedef enum
{
	INIT_CONFIG = 0,
	POWER_ON_MODULE,
	START_AT_CONFIG,
    SET_AT_FACTORY,
    SET_BAUDRATE,
    WAIT_RESPONSE,
    
}gps_mod_states_e;

/*********public struct***********************************************************/
typedef struct 
{
	uint8_t gps_state_bit: 			1;
	uint8_t gprs_state_bit:			1;
	uint8_t bt_state_bit:			1;
}module_status_bit_t;

typedef struct
{
	gps_mod_states_e state;
    gps_mod_states_e state_ok;
    gps_mod_states_e state_wrong;
	const char *msg;
	uint8_t  msg_receive;
	uint8_t gps_enable;
	module_status_bit_t module_status_bit;
    uint8_t test_mode;
    const char *expect_res;
}gps_config_lv;

/*********public functions prototype***********************************************************/
void gps_config_init_module (void);
void gps_config_ON_OFF_module(void);

/*********public variables***********************************************************/
//extern gps_config_lv gps_config_v;



