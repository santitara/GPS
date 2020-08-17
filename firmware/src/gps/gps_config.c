/*********include headers***********************************************************/
#include "gps_config.h"
#include "stdio.h"
#include "gps_uart.h"
#include "AT_comands.h"
#include "app.h"
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


void gps_config_at_general (void)
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
        case SET_AT_FACTORY:
			// Set msg to send
			gps_config_v.msg = AT_FACTORY;
			//send msg
			gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg));
            //set next state
            gps_config_v.state = WAIT_RESPONSE;
            //set state ok
            gps_config_v.state_ok = SET_BAUDRATE;
            //set wrong state
            gps_config_v.state_wrong = SET_AT_FACTORY;
            //set msg expected
            gps_config_v.expect_res = OK;
		break;
        
        case SET_BAUDRATE:
			// Set msg to send
			gps_config_v.msg = BAUDRATE_SEL;
			//send msg
			gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg));
            //set next state
            gps_config_v.state = WAIT_RESPONSE;
            //set state ok
            gps_config_v.state_ok = NEXT_CONFIG_MODULE;
            //set wrong state
            gps_config_v.state_wrong = SET_BAUDRATE;
            //set msg expected
            gps_config_v.expect_res = OK;
		break;
        case NEXT_CONFIG_MODULE:
            appData.state = CONFIG_AT_GPS;
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

void gps_config_at_GPS (void)
{
    switch(gps_config_v.state)
	{
        case SET_GPS_ON:
			// Set msg to send
			gps_config_v.msg = GPS_ON;
			//send msg
			gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg));
            //set next state
            gps_config_v.state = WAIT_RESPONSE;
            //set state ok
            gps_config_v.state_ok = SET_ANT_GPS_ON;
            //set wrong state
            gps_config_v.state_wrong = SET_GPS_ON;
            //set msg expected
            gps_config_v.expect_res = OK;
		break;
        case SET_ANT_GPS_ON:
			// Set msg to send
			gps_config_v.msg = ANT_GPS_ON;
			//send msg
			gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg));
            //set next state
            gps_config_v.state = WAIT_RESPONSE;
            //set state ok
            gps_config_v.state_ok = NEXT_CONFIG_MODULE;
            //set wrong state
            gps_config_v.state_wrong = SET_ANT_GPS_ON;
            //set msg expected
            gps_config_v.expect_res = OK;
		break;
        case NEXT_CONFIG_MODULE:
            appData.state = CONFIG_AT_GPRS;
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

void gps_config_at_GRPS (void)
{
    switch(gps_config_v.state)
	{
        case SET_ENABLE_GPRS:
			// Set msg to send
			gps_config_v.msg = HABILITAR_GPRS;
			//send msg
			gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg));
            //set next state
            gps_config_v.state = WAIT_RESPONSE;
            //set state ok
            gps_config_v.state_ok = SET_AT_CREG;
            //set wrong state
            gps_config_v.state_wrong = SET_ENABLE_GPRS;
            //set msg expected
            gps_config_v.expect_res = OK;
		break;
        case SET_AT_CREG:
			// Set msg to send
			gps_config_v.msg = AT_CREG;
			//send msg
			gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg));
            //set next state
            gps_config_v.state = WAIT_RESPONSE;
            //set state ok
            gps_config_v.state_ok = SET_ATTACH_GPRS;
            //set wrong state
            gps_config_v.state_wrong = SET_AT_CREG;
            //set msg expected
            gps_config_v.expect_res = OK;
		break;
        case SET_ATTACH_GPRS:
            // Set msg to send
			gps_config_v.msg = ATTACH_GPRS;
			//send msg
			gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg));
            //set next state
            gps_config_v.state = WAIT_RESPONSE;
            //set state ok
            gps_config_v.state_ok = SET_GPRS_RESET_IP;
            //set wrong state
            gps_config_v.state_wrong = SET_ATTACH_GPRS;
            //set msg expected
            gps_config_v.expect_res = OK;
        break;
        case SET_GPRS_RESET_IP:
            // Set msg to send
			gps_config_v.msg = GPRS_RESET_IP;
			//send msg
			gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg));
            //set next state
            gps_config_v.state = WAIT_RESPONSE;
            //set state ok
            gps_config_v.state_ok = SET_APN_CON_TYPE;
            //set wrong state
            gps_config_v.state_wrong = SET_GPRS_RESET_IP;
            //set msg expected
            gps_config_v.expect_res = OK;
        break;
        case SET_APN_CON_TYPE:
            // Set msg to send
			gps_config_v.msg = APN_CON_TYPE;
			//send msg
			gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg));
            //set next state
            gps_config_v.state = WAIT_RESPONSE;
            //set state ok
            gps_config_v.state_ok = SET_APN;
            //set wrong state
            gps_config_v.state_wrong = SET_APN_CON_TYPE;
            //set msg expected
            gps_config_v.expect_res = OK;
        break;
        
        case SET_APN:
            // Set msg to send
			gps_config_v.msg = APN;
			//send msg
			gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg));
            //set next state
            gps_config_v.state = WAIT_RESPONSE;
            //set state ok
            gps_config_v.state_ok = SET_OPEN_APN;
            //set wrong state
            gps_config_v.state_wrong = SET_APN;
            //set msg expected
            gps_config_v.expect_res = OK;
        break;
        
        case SET_OPEN_APN:
            // Set msg to send
			gps_config_v.msg = OPEN_APN;
			//send msg
			gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg));
            //set next state
            gps_config_v.state = WAIT_RESPONSE;
            //set state ok
            gps_config_v.state_ok = SET_QUERY_APN;
            //set wrong state
            gps_config_v.state_wrong = SET_OPEN_APN;
            //set msg expected
            gps_config_v.expect_res = OK;
            break;
            
        case SET_QUERY_APN:
            // Set msg to send
			gps_config_v.msg = QUERY_APN;
			//send msg
			gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg));
            //set next state
            gps_config_v.state = WAIT_RESPONSE;
            //set state ok
            gps_config_v.state_ok = NEXT_CONFIG_MODULE;
            //set wrong state
            gps_config_v.state_wrong = SET_QUERY_APN;
            //set msg expected
            gps_config_v.expect_res = OK;
        break;
        
        case NEXT_CONFIG_MODULE:
            appData.state = CONFIG_AT_HTTP;
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

void gps_config_at_HTTP(void)
{
    switch(gps_config_v.state)
	{
        case SET_GPRS_HTTP_START:
			// Set msg to send
			gps_config_v.msg = GPRS_HTTP_START;
			//send msg
			gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg));
            //set next state
            gps_config_v.state = WAIT_RESPONSE;
            //set state ok
            gps_config_v.state_ok = SET_CID_SEL;
            //set wrong state
            gps_config_v.state_wrong = SET_GPRS_HTTP_START;
            //set msg expected
            gps_config_v.expect_res = OK;
		break;
        case SET_CID_SEL:
			// Set msg to send
			gps_config_v.msg = CID_SEL;
			//send msg
			gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg));
            //set next state
            gps_config_v.state = WAIT_RESPONSE;
            //set state ok
            gps_config_v.state_ok = SET_SSL_ENABLE;
            //set wrong state
            gps_config_v.state_wrong = SET_CID_SEL;
            //set msg expected
            gps_config_v.expect_res = OK;
		break;
         case SET_SSL_ENABLE:
			// Set msg to send
			gps_config_v.msg = SSL_ENABLE;
			//send msg
			gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg));
            //set next state
            gps_config_v.state = WAIT_RESPONSE;
            //set state ok
            gps_config_v.state_ok = NEXT_CONFIG_MODULE;
            //set wrong state
            gps_config_v.state_wrong = SET_CID_SEL;
            //set msg expected
            gps_config_v.expect_res = OK;
		break;
         case NEXT_CONFIG_MODULE:
            appData.state = CONFIG_AT_END;
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