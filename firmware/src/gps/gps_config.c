/*********include headers***********************************************************/
#include "gps_config.h"
#include "stdio.h"
#include "gps_uart.h"
#include "AT_comands.h"
#include "gps_common.h"
#include "app.h"
/*********define *****************************************************************/


const char *OK = "OK";
//const char *UGNSINF0 = "+CGNSINF: 1,0";
//const char *UGNSINF1 = "+CGNSINF: 1,1";
/*********private enum***************************************************************/

/*********private functions prototype***********************************************************/
uint8_t gps_config_test_mode (void);
/*********public functions prototype***********************************************************/
void gps_config_init_module (void);
void gps_config_ON_OFF_module(void);

/*********private variables***********************************************************/
gps_config_lv gps_config_v =
{
	.state = START_AT_CONFIG,
	.gps_enable = 0,
	.module_status_bit = 0,
	.msg_receive = 0,
    .n_retries = 0,
    .gsm_state = 0,
    .gps_state = 0,
    .flag_get_imei = 0,
    .flag_gps_report = 0,

};

gps_buff_c_ptr_lv gps_buff_c_ptr_v = 
{
    .head = 0,
    .tail = 0,
    .max = 0,
};

void gps_config_init_module (void)
{
	// clear buff rx and tx
	memset(gps_uart_v.rx_buffer,1,BUFF_SIZE_RX);
	memset(gps_uart_v.tx_buffer,1,BUFF_SIZE_RX);
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
    bool ret= false;
	switch(gps_config_v.state)
	{
		case START_AT_CONFIG:
			// Set msg to send
			gps_config_v.msg = AT;
			//send msg
            //while(gps_uart_write()!=true);
			while(gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg)) != true);
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
			//gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg));
            while(gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg)) != true);
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
			//gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg));
            while(gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg)) != true);
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
            gps_config_v.state = SET_GPS_ON;
        break;
		case WAIT_RESPONSE:
            gps_uart_rx_state ();
            
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
			//gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg));
             while(gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg)) != true);
            //set next state
            gps_config_v.state = WAIT_RESPONSE;
            //set state ok
            gps_config_v.state_ok = SET_ECHO_OFF;
            //set wrong state
            gps_config_v.state_wrong = SET_GPS_ON;
            //set msg expected
            gps_config_v.expect_res = OK;
		break;
        /*case SET_ANT_GPS_ON:
			// Set msg to send
			gps_config_v.msg = ANT_GPS_ON;
			//send msg
			//gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg));
             while(gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg)) != true);
            //set next state
            gps_config_v.state = WAIT_RESPONSE;
            //set state ok
            gps_config_v.state_ok = SET_GET_IMEI;
            //set wrong state
            gps_config_v.state_wrong = SET_ANT_GPS_ON;
            //set msg expected
            gps_config_v.expect_res = OK;
		break;*/
        case SET_ECHO_OFF:
			// Set msg to send
			gps_config_v.msg = ECHO_OFF;
			//send msg
			//gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg));
             while(gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg)) != true);
            //set next state
            gps_config_v.state = WAIT_RESPONSE;
            //set state ok
            gps_config_v.state_ok = SET_GET_IMEI;
            //set wrong state
            gps_config_v.state_wrong = SET_ECHO_OFF;
            //set msg expected
            gps_config_v.expect_res = OK;
		break;
        case SET_GET_IMEI:
			// Set msg to send
			gps_config_v.msg = GET_IMEI;
            gps_config_v.flag_get_imei = 1;
			//send msg
			//gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg));
             while(gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg)) != true);
            //set next state
            gps_config_v.state = WAIT_RESPONSE;
            //set state ok
            gps_config_v.state_ok = NEXT_CONFIG_MODULE;
            //set wrong state
            gps_config_v.state_wrong = SET_GET_IMEI;
            //set msg expected
            gps_config_v.expect_res = OK;
		break;
        case NEXT_CONFIG_MODULE:
            appData.state = CONFIG_AT_GPRS;
            gps_config_v.state = SET_ENABLE_GPRS;
        break;
        case WAIT_RESPONSE:
            gps_uart_rx_state ();
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
			//gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg));
             while(gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg)) != true);
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
			//gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg));
             while(gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg)) != true);
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
			//gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg));
            while(gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg)) != true);
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
			//gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg));
            while(gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg)) != true);
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
			//gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg));
            while(gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg)) != true);
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
			//gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg));
            while(gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg)) != true);
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
			//gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg));
            while(gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg)) != true);
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
			//gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg));
            while(gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg)) != true);
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
            gps_config_v.state = SET_GPRS_HTTP_START;
        break; 
        
        case WAIT_RESPONSE:

            gps_uart_rx_state ();
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
			//gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg));
            while(gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg)) != true);
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
			//gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg));
            while(gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg)) != true);
            //set next state
            gps_config_v.state = WAIT_RESPONSE;
            //set state ok
            gps_config_v.state_ok = SET_CONTENT_TYPE;
            //set wrong state
            gps_config_v.state_wrong = SET_CID_SEL;
            //set msg expected
            gps_config_v.expect_res = OK;
		break;
        case SET_SSL_ENABLE:
			// Set msg to send
			gps_config_v.msg = SSL_ENABLE;
			//send msg
			//gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg));
            while(gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg)) != true);
            //set next state
            gps_config_v.state = WAIT_RESPONSE;
            //set state ok
            gps_config_v.state_ok = SET_CONTENT_TYPE;
            //set wrong state
            gps_config_v.state_wrong = SET_CID_SEL;
            //set msg expected
            gps_config_v.expect_res = OK;
		break;
        case SET_CONTENT_TYPE:
			// Set msg to send
			gps_config_v.msg = URL_CONTENT_TYPE;
			//send msg
			//gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg));
            while(gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg)) != true);
            //set next state
            gps_config_v.state = WAIT_RESPONSE;
            //set state ok
            gps_config_v.state_ok = NEXT_CONFIG_MODULE;
            //set wrong state
            gps_config_v.state_wrong = SET_CONTENT_TYPE;
            //set msg expected
            gps_config_v.expect_res = OK;
		break;
         case NEXT_CONFIG_MODULE:
            appData.state = CONFIG_AT_BT;
            gps_config_v.state = SET_AUTO_PAIR_BT;
            gps_config_v.gsm_state = 1;
            //set led off
            PLIB_PORTS_PinWrite (PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_9,0);
            // Clear the WDT timer  
            //SYS_WDT_TimerClear ();
            //PLIB_PORTS_PinWrite (PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_9,1);
            DRV_TMR1_Start();  //enciendo el timer 2 que se encarga de manejar el led rojo de la placa
            
        break; 
        
        case WAIT_RESPONSE:
            gps_uart_rx_state ();
		break;
        default:
            
        break;
        
        
    }

}

void gps_config_at_BT(void)
{
    switch(gps_config_v.state)
	{
        case SET_AUTO_PAIR_BT:
			// Set msg to send
			gps_config_v.msg = BTH_AUTO_PAIR;
			//send msg
            while(gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg)) != true);
            //set next state
            gps_config_v.state = WAIT_RESPONSE;
            //set state ok
            gps_config_v.state_ok = SET_BT_ON;
            //set wrong state
            gps_config_v.state_wrong = SET_AUTO_PAIR_BT;
            //set msg expected
            gps_config_v.expect_res = OK;
		break;
        case SET_BT_ON:
			// Set msg to send
			gps_config_v.msg = BTH_ON;
			//send msg
            while(gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg)) != true);
            //set next state
            gps_config_v.state = WAIT_RESPONSE;
            //set state ok
            gps_config_v.state_ok = SET_VIS_ON;
            //set wrong state
            gps_config_v.state_wrong = SET_BT_ON;
            //set msg expected
            gps_config_v.expect_res = OK;
		break;
        case SET_VIS_ON:
			// Set msg to send
			gps_config_v.msg = BTH_VIS_ON;
			//send msg
            while(gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg)) != true);
            //set next state
            gps_config_v.state = WAIT_RESPONSE;
            //set state ok
            gps_config_v.state_ok = SET_BT_ONE_CONN;
            //set wrong state
            gps_config_v.state_wrong = SET_VIS_ON;
            //set msg expected
            gps_config_v.expect_res = OK;
		break;
        case SET_BT_ONE_CONN:
			// Set msg to send
			gps_config_v.msg = BTH_ONE_CON;
			//send msg
            while(gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg)) != true);
            //set next state
            gps_config_v.state = WAIT_RESPONSE;
            //set state ok
            gps_config_v.state_ok = SET_NAME_BT;
            //set wrong state
            gps_config_v.state_wrong = SET_BT_ONE_CONN;
            //set msg expected
            gps_config_v.expect_res = OK;
		break;
        case SET_NAME_BT:
			// Set msg to send
            memset(gps_uart_v.tx_buffer,0,255);
            strcpy(gps_uart_v.tx_buffer,BTH_CH_NAME);
            strcat(gps_uart_v.tx_buffer,"1234");//sustituir por imei
            strcat(gps_uart_v.tx_buffer,"\r\n");
            gps_config_v.msg = gps_uart_v.tx_buffer;// GPS_REPORT
			//send msg
            while(gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg)) != true);
            //set next state
            gps_config_v.state = WAIT_RESPONSE;
            //set state ok
            gps_config_v.state_ok = NEXT_CONFIG_MODULE;
            //set wrong state
            gps_config_v.state_wrong = SET_NAME_BT;
            //set msg expected
            gps_config_v.expect_res = OK;
		break;
        case NEXT_CONFIG_MODULE:
            appData.state = CONFIG_AT_END;
            gps_config_v.state = SET_GPS_REPORT;
        break;
        case WAIT_RESPONSE:
            gps_uart_rx_state ();
		break;
        default:
            
        break;
    }
}
void gps_config_at_GPS_reports (void)
{
    switch(gps_config_v.state)
	{
        case SET_GPS_REPORT:
			// Set msg to send
			gps_config_v.msg =GPS_INFORM;// GPS_REPORT;
			//send msg
			while(gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg)) != true);
            //set next state
            gps_config_v.state = WAIT_RESPONSE;
            //set state ok
            gps_config_v.state_ok = SET_HTTP_FRAME;//SET_HTTP_FRAME;
            //set wrong state
            gps_config_v.state_wrong = SET_GPS_REPORT;
            //set msg expected
            gps_config_v.expect_res = "+CGNSINF";
            //set flag report
            //gps_config_v.flag_report = 1;
            
		break;
        case SET_HTTP_FRAME:
            //send msg
            //while(gps_uart_write(GPRS_HTTP_START, sizeof(gps_config_v.msg)) != true);
            //delay_ms(100);
            gps_config_v.flag_gprs_sent = 1;
            gps_config_v.msg = gps_data_v.data_frame_tx;// GPS_REPORT;
			while(gps_uart_write(gps_data_v.data_frame_tx, sizeof(gps_config_v.msg)) != true);
            //delay_ms(300);
            //set next state
            gps_config_v.state = WAIT_RESPONSE;//WAIT_RESPONSE;
            //set state ok
            gps_config_v.state_ok = SEND_HTTP_FRAME;
            //set wrong state
            gps_config_v.state_wrong = SET_GPS_REPORT;
            //set msg expected
            gps_config_v.expect_res = OK;
		break;
        case SEND_HTTP_FRAME:
            // Set msg to send
            //delay_ms(00);
            gps_config_v.msg = GPRS_HTTP_ACTION;
            //send msg
			while(gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg)) != true);
            delay_ms(500);
            
            //set next state
            gps_config_v.state = WAIT_RESPONSE;//WAIT_RESPONSE;
            //set state ok
            gps_config_v.state_ok = IDLE;
            //set wrong state
            gps_config_v.state_wrong = SET_GPS_REPORT;
            //set msg expected
            gps_config_v.expect_res = "+HTTPACTION";
		break;
        case ASK_COVERAGE:
			// Set msg to send
			gps_config_v.msg = AT_CREG;
			//send msg
             while(gps_uart_write(gps_config_v.msg, sizeof(gps_config_v.msg)) != true);
            //set next state
            gps_config_v.state = WAIT_RESPONSE;
            //set state ok
            gps_config_v.state_ok = IDLE;
            //set wrong state
            gps_config_v.state_wrong = IDLE;
            //set msg expected
            gps_config_v.expect_res = OK;
		break;
        case NEXT_CONFIG_MODULE:
            gps_config_v.state = SET_GPS_REPORT;
            //appData.state = CONFIG_AT_GPRS;
        break;
        case WAIT_RESPONSE:
			gps_uart_rx_state ();
		break;
        case IDLE:
            if(gps_config_v.flag_gps_report)
            {
                gps_config_v.flag_gprs_sent = 0;    
                gps_config_v.state = SET_GPS_REPORT;
                gps_config_v.flag_gps_report = 0;
            }
            else
            {
                //gps_config_v.state = SET_HTTP_FRAME;
            }
        break;
        default:
            
        break;
    }
}



