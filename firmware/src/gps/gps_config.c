/*********include headers***********************************************************/
#include "gps_config.h"
#include "stdio.h"
#include "gps_uart.h"
#include "AT_comands.h"
/*********define *****************************************************************/

const char *OK = "OK";

/*********private enum***************************************************************/

/*********private functions prototype***********************************************************/
uint8_t gps_config_test_mode (void);
/*********public functions prototype***********************************************************/
void gps_config_init_module (void);
void gps_config_ON_OFF_module(void);

/*********private variables***********************************************************/
gps_config_lv gps_config_v =
{
	.state = INIT_CONFIG,
	.gps_enable = 0,
	.module_status_bit = 0,
	.msg_receive = 0,

};

void gps_config_init_module (void)
{
	// clear buff rx and tx
	memset(gps_uart_v.rx_buffer,0,BUFF_SIZE_RX);
	memset(gps_uart_v.tx_buffer,0,BUFF_SIZE_RX);
    /*stop interrupts*/
    DRV_TMR0_Stop();
    DRV_TMR1_Stop();
    DRV_TMR2_Stop();  //parar salida de pulsos
    DRV_OC0_Enable(); //habilitar pwm salida pulsos para siempre 
    /*deshabilitar watchdotg*/
    SYS_WDT_Disable ();
	//Enalbe led GPS to indicate power on
    PLIB_PORTS_PinWrite (PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_9,1); 
    // Set OFF led bluetooth
    PLIB_PORTS_PinWrite (PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_8,0);
    // Set ON converter 5V
    PLIB_PORTS_PinWrite (PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_2, 0);
    
    gps_config_v.test_mode = gps_config_test_mode();
    
}

//Encender modulo SIMCom
void gps_config_ON_OFF_module(void)  
{   
   PLIB_PORTS_PinWrite (PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_13, 1);//secuencia para arrancar el modulo mediante PWKEY
   delay_ms(1500);
   PLIB_PORTS_PinWrite (PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_13, 0);
}

uint8_t gps_config_test_mode (void)
{
    if(PLIB_PORTS_PinGet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_1) == 1){
        return 1;
    }
    else
    {
        return 0;
    }
    
}


void gps_config_at_module ()
{
	switch(gps_config_v.state)
	{
		case START_AT_CONFIG:
			// Set msg to send
			gps_config_v.msg = AT;
			//send msg
			gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg));
            //set next state
            gps_config_v.state = WAIT_RESPONSE;
            //set state ok
            gps_config_v.state_ok = SET_AT_FACTORY;
            //set wrong state
            gps_config_v.state_wrong = START_AT_CONFIG;
            //set msg expected
            gps_config_v.expect_res = OK;
		break;
		case WAIT_RESPONSE:
			if(gps_uart_v.flag_rx_end)
			{
				if(gps_uart_process_response(gps_uart_v.rx_buffer,gps_config_v.expect_res))
                {
                    gps_config_v.state = gps_config_v.state_ok;
                }
                else
                {
                    gps_config_v.state = gps_config_v.state_wrong;
                }
			}
            else
            {
                    
            }
		break;

		default:

		break;

	}
	
}




