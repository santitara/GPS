//char bt_status=0;  //bt_status
                  //0=bt_off
                  //1=bt_on
                  //2=bt_connected_to_spp_client
//*****************************************************************************
//Encender bluetooth 
//*****************************************************************************
void enable_bluetooth(void)
{
   printf("AT+BTPOWER=1\r\n");         //send enable command
}
//*****************************************************************************
//Apagar bluetooth 
//*****************************************************************************
void disable_bluetooth(void)
{
   printf("AT+BTPOWER=0\r\n");         //send disable command
}
//*****************************************************************************
//Aceptar emparejamiento dispositivo
//*****************************************************************************
void aceptar_solicitud_emparejamiento(void)
{
   printf("AT+BTPAIR=1,1\r\n");        
}
//*****************************************************************************
//Aceptar conexion SPP dispositivo cliente
//*****************************************************************************
void aceptar_conexion_spp(void)
{
   printf("AT+BTACPT=1\r\n");      
}
//*****************************************************************************


AT+BTSPPSEND
AT+BTUNPAIR=0

AT+BTSCAN=1,10

AT+BTVIS=1

AT+BTHOST=ODOCAP

AT+BTPAIRCFG=2  