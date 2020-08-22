
#ifndef AT_COMMANDS_H
#define AT_COMMANDS_H
    //mensajes comandos AT para manejar modulo de comunicaciones
    const char *AT = "AT\r\n"; //! sent at, expectec response OK
     const char *ECHO_OFF = "ATE0\r\n";
     const char *AT_CREG = "AT+CREG?\r\n";
     const char *AT_FACTORY = "AT&F0\r\n";
     const char *BAUDRATE_SEL = "AT+IPR=115200\r\n";
     const char *AT_OFF = "AT+CPOWD=1\r\n";
     //GPS
     const char *GPS_ON = "AT+CGNSPWR=1\r\n";
     const char *GPS_OFF = "AT+CGNSPWR=0\r\n";
     const char *ANT_GPS_ON = "AT+CGPIO=0,39,1,1\r\n";
     const char *ANT_GPS_OFF = "AT+CGPIO=0,39,1,0\r\n";
     const char *GPS_GET_POS = "AT+CGNSINF\r\n";
     const char *GPS_REPORT = "AT+CGNSURC=2\r\n"; ////int frequency (0-255), 0=0FF;1=1s;2=2s....ETC UNTIL 255
     const char *GPS_REPORT_OFF = "AT+CGNSURC=0\r\n"; ////int frequency (0-255), 0=0FF;1=1s;2=2s....ETC UNTIL 255
     const char *GPS_INFORM = "AT+CGNSINF\r\n";
     //GPRS
    const char *ATTACH_GPRS = "AT+CGATT=1\r\n";
    const char *DEATTACH_GPRS = "AT+CGATT=0\r\n"; 
    const char *GPRS_ATTACHED = "AT+CGATT?\r\n";
    const char *GPRS_RESET_IP = "AT+CIPSHUT\r\n";
    const char *GPRS_CHECK_IP = "AT+CIPSTATUS\r\n";
    const char *GPRS_ONLY_ONE_SESION = "AT+CIPMUX=0\r\n";
    const char *HABILITAR_GPRS = "AT+CFUN=1\r\n";   //
    const char *DESHABILITAR_GPRS = "AT+CFUN=4\r\n";
    //DATOS APN PARA SIMYO
    const char *APN_CON_TYPE = "AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r\n";
    const char *APN = "AT+SAPBR=3,1,\"APN\",\"gprs-service.com\"\r\n";
    const char *OPEN_APN = "AT+SAPBR=1,1\r\n";
    const char *QUERY_APN = "AT+SAPBR=2,1\r\n";
    const char *CLOSE_APN = "AT+SAPBR=0,1\r\n";
    // COMANDOS PARA MANEJAR EL HTTP
    const char *GPRS_HTTP_CLOSE = "AT+HTTPTERM\r\n";
    const char *GPRS_HTTP_START = "AT+HTTPINIT\r\n";
    const char *GPRS_HTTP_ACTION = "AT+HTTPACTION=0\r\n";   //POST->1; GET->0
    const char *GPRS_HTTP_READ = "AT+HTTPREAD\r\n";
    const char *CID_SEL = "AT+HTTPPARA=\"CID\",1\r\n";
    const char *SSL_ENABLE="AT+HTTPSSL=1\r\n";
    //BLUETOOTH
     const char *BTH_ON = "AT+BTPOWER=1\r\n";       
     const char *BTH_OFF = "AT+BTPOWER=0\r\n";        
    // const char *BTH_PAIR = "AT+BTPAIR=1,1\r\n"; 
     const char *BTH_VIS_ON= "AT+BTVIS=1\r\n";
     const char *BTH_AUTO_PAIR= "AT+BTPAIRCFG=2\r\n";
     //const char *BTH_AUTO_PAIR= "AT+BTPAIRCFG=1,1234\r\n";
     const char *BTH_CH_NAME="AT+BTHOST=GPS_";
     const char *BTH_ONE_CON="AT+BTSPPCFG=0\r\n";
     const char *BTH_SSP = "AT+BTACPT=1\r\n";               
     const char *BTH_SEND = "AT+BTSPPSEND\r\n";
    // const char *BTH_MESSAGE_TERM= (char*)0x1A;

     const char *GET_IMEI = "AT+GSN\r\n"; 
     const char *GET_CCID = "AT+CCID\r\n";

#endif

 //const char *BAUDRATE_SEL = "AT+IPR=9600\r\n"; 

 
 //mensajes comandos AT para manejar modulo de comunicaciones
 /*
 extern const char *AT = "AT\r\n"; //! sent at, expectec response OK
 extern const char *ECHO_OFF = "ATE0\r\n";
 extern const char *AT_CREG = "AT+CREG?\r\n";
 extern const char *AT_FACTORY = "AT&F0\r\n";
 extern const char *BAUDRATE_SEL = "AT+IPR=115200\r\n";
 extern const char *AT_OFF = "AT+CPOWD=1\r\n";
 //GPS
 extern const char *GPS_ON = "AT+CGNSPWR=1\r\n";
 extern const char *GPS_OFF = "AT+CGNSPWR=0\r\n";
 extern const char *ANT_GPS_ON = "AT+CGPIO=0,39,1,1\r\n";
extern const char *ANT_GPS_OFF = "AT+CGPIO=0,39,1,0\r\n";
extern const char *GPS_GET_POS = "AT+CGNSINF\r\n";
extern const char *GPS_REPORT = "AT+CGNSURC=1\r\n"; ////int frequency (0-255), 0=0FF;1=1s;2=2s....ETC UNTIL 255
extern const char *GPS_REPORT_OFF = "AT+CGNSURC=0\r\n"; ////int frequency (0-255), 0=0FF;1=1s;2=2s....ETC UNTIL 255
//GPRS
extern const char *ATTACH_GPRS = "AT+CGATT=1\r\n";
extern const char *DEATTACH_GPRS = "AT+CGATT=0\r\n"; 
extern const char *GPRS_ATTACHED = "AT+CGATT?\r\n";
extern const char *GPRS_RESET_IP = "AT+CIPSHUT\r\n";
extern const char *GPRS_CHECK_IP = "AT+CIPSTATUS\r\n";
extern const char *GPRS_ONLY_ONE_SESION = "AT+CIPMUX=0\r\n";
extern const char *HABILITAR_GPRS = "AT+CFUN=1\r\n";   //
extern const char *DESHABILITAR_GPRS = "AT+CFUN=4\r\n";
//DATOS APN PARA SIMYO
extern const char *APN_CON_TYPE = "AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r\n";
extern const char *APN = "AT+SAPBR=3,1,\"APN\",\"gprs-service.com\"\r\n";
extern const char *OPEN_APN = "AT+SAPBR=1,1\r\n";
extern const char *QUERY_APN = "AT+SAPBR=2,1\r\n";
extern const char *CLOSE_APN = "AT+SAPBR=0,1\r\n";
// COMANDOS PARA MANEJAR EL HTTP
extern const char *GPRS_HTTP_CLOSE = "AT+HTTPTERM\r\n";
externconst char *GPRS_HTTP_START = "AT+HTTPINIT\r\n";
extern const  char *GPRS_HTTP_ACTION = "AT+HTTPACTION=0\r\n";   //POST->1; GET->0
extern const char *GPRS_HTTP_READ = "AT+HTTPREAD\r\n";
extern const char *CID_SEL = "AT+HTTPPARA=\"CID\",1\r\n";
//BLUETOOTH
extern const char *BTH_ON = "AT+BTPOWER=1\r\n";       
extern const char *BTH_OFF = "AT+BTPOWER=0\r\n";        
// const char *BTH_PAIR = "AT+BTPAIR=1,1\r\n"; 
extern const char *BTH_VIS_ON= "AT+BTVIS=1\r\n";
extern const char *BTH_AUTO_PAIR= "AT+BTPAIRCFG=2\r\n";
 //const char *BTH_AUTO_PAIR= "AT+BTPAIRCFG=1,1234\r\n";
extern const char *BTH_CH_NAME="AT+BTHOST=GPS_";
extern const char *BTH_ONE_CON="AT+BTSPPCFG=0\r\n";
extern const char *BTH_SSP = "AT+BTACPT=1\r\n";               
extern const char *BTH_SEND = "AT+BTSPPSEND\r\n";
// const char *BTH_MESSAGE_TERM= (char*)0x1A;

extern const char *GET_IMEI = "AT+GSN\r\n"; 
extern const char *GET_CCID = "AT+CCID\r\n"; 
 //co
  */