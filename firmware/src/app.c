/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It 
    implements the logic of the application's state machine and it may call 
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/
// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "app.h"
#include "stdio.h"
#include "time.h"
#include "string.h"
#include "libgeohash-master/geohash.h"

#define ON 1
#define OFF 0   


uint8_t   sourceBuffer[4096],targetBuffer[4096];
// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************
/* Array in the program memory with starting address PROGRAM_FLASH_BASE_ADDRESS_VALUE to write the data */
const uint32_t dataFlash[DEVICE_PAGE_SIZE_DIVIDED_BY_4] __attribute__(( address(PROGRAM_FLASH_BASE_ADDRESS_VALUE))) =  {0};

unsigned char system_in_sleep=0;   
unsigned int  i=0,minutes=0,horas=0, anyo;  
unsigned long int seconds=0;
float latitud, longitud;
char *datos_gps,*ptr,*ptr2;
char *datos_lat, *datos_lon;    
//int result;
unsigned int blink_rate;
unsigned long DATOS_FLASH[DEVICE_ROW_SIZE_DIVIDED_BY_4]; 
char parado=1,parpadeo_rumbo=0,guardar_en_flash=0, primera_escritura_flash,ajustar_reloj,parpadeo; 
char DEVICE_CCID[25],DEVICE_IMEI[20],DEVICE_IMEI_4THDIGITS[20],DATOS_LAT[20],DATOS_LON[20],DATOS_VEL[20],DATOS_RUM[20],DATOS_HORAS[20],DATOS_DIST[20],FECHAYHORA[19],AUX[4],AUX_NPANT[3]; //version en string de los datos tracker
double rumbo_float=0,vel_gps,/*distancia_gps */parcial_guardar_datos=0,vel_rueda,distancia_rueda,aux_calc_pulsos=0;
char trama_tx[200],aux_tx[10];
char trama_tx2[200];
double distancia_gps /*__attribute__ ((persistent))*/;
double vel_max __attribute__ ((persistent));   
char mostrar_en_display __attribute__ ((persistent));


//variables para estadisticas de kms
double kms_trip __attribute__ ((persistent));  //parcial(lo que se muestra en el display truncado a XX.xx) y es lo que recalo con los botones + y - ;

//Estadisticas bluetooth del Odocap
double parcial_viaje_1=0;               //estadisticas de kms para guardar en eeprom y visualizar mediante bluetooth en un futuro
double vmax_1 = 0;
double v_media_1 = 0;
double tv1= 0;

double parcial_viaje_2=0;               //estadisticas de kms para guardar en eeprom y visualizar mediante bluetooth en un futuro
double vmax_2=0;
double v_media_2=0;
double tv2=0;

double kms_aceite=0;
unsigned long int fecha_aceite;

double kms_total;

char  nombre_propietario[40];  //con un maximo de 30 caracteres

char primera_vez_bt=1;
RESET_REASON reasonType;

extern char trama_rx[BUFFER_RX_SIZE],existe_captador;
extern unsigned int nbyte,veces_hab_bt;
extern char poner_a_dormir;

//mensajes comandos AT para manejar modulo de comunicaciones
 const char *AT = "AT\r\n";
 const char *ECHO_OFF = "ATE0\r\n";
 const char *AT_CREG = "AT+CREG?\r\n";
 const char *AT_FACTORY = "AT&F0\r\n";
 //const char *AT_FACTORY = "AT+GMR\r\n";
 const char *GPS_ON = "AT+CGNSPWR=1\r\n";
 const char *GPS_OFF = "AT+CGNSPWR=0\r\n";
 const char *ANT_GPS_ON = "AT+CGPIO=0,39,1,1\r\n";
 const char *ANT_GPS_OFF = "AT+CGPIO=0,39,1,0\r\n";
 const char *GPS_GET_POS = "AT+CGNSINF\r\n";
 const char *GPS_REPORT = "AT+CGNSURC=1\r\n"; ////int frequency (0-255), 0=0FF;1=1s;2=2s....ETC UNTIL 255
 const char *GPS_REPORT_OFF = "AT+CGNSURC=0\r\n"; ////int frequency (0-255), 0=0FF;1=1s;2=2s....ETC UNTIL 255
 const char *AT_OFF = "AT+CPOWD=1\r\n";
//gprs
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
//bt
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
 //const char *BAUDRATE_SEL = "AT+IPR=9600\r\n"; 
 const char *BAUDRATE_SEL = "AT+IPR=115200\r\n"; 
 //POST
//const char *URL_ST_TRACKER_GRAFANA2 = "AT+HTTPPARA=\"URL\",\"http://misana-iot.es:1880/api/v2/?token=crjw75yS9gnBsj26uQWEqm9v1vqmMKQ6&id=865067021287761\"\n\r";
//GET
 const char *URL_ST_TRACKER_GRAFANA2 = "AT+HTTPPARA=\"URL\",\"http://misana-iot.es:1880/api/v2/?token=crjw75yS9gnBsj26uQWEqm9v1vqmMKQ6&id=865067028127128&payload=";
 
 
  const char *URL_LOCATEC = "AT+HTTPPARA=\"URL\",\"https://www.locatec.es/proyectos/manezylozano/ws/getData.php?";
  
  
  const char *URL_TEST = "AT+HTTPPARA=\"URL\",\"https://httpbin.org/get\"\r\n";
  
  // aqui defino las etiquetas de las variables que voy a enviar por bluetooth
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
 //****************************************************************************
 
  
  
  const char *SEPARATOR= "&";
  const char *URL_TERMINATOR ="\"\r\n";
  const char *SSL_ENABLE="AT+HTTPSSL=1\r\n";

//[{%22fields%22:{"latitude":{{lat}},"longitude": {{long}}},"tags":{},"timestamp":null}]

 const char *URL_DATA_DOWNLOAD = "AT+HTTPDATA=1500,1000\r\n";
const char *URL_CONTENT_TYPE = "AT+HTTPPARA=\"CONTENT\",application/json\r\n";
const char *URL_PARAM_TERMINATOR ="}\"\r\n";
//POST
 /*const char *TRAMA_INI = "[";
 const char *TRAMA_LAT = "{\"fields\": {\"latitude\":";
 const char *TRAMA_LON =",\"longitude\":";
 const char *TRAMA_TIMESTAMP ="},\"tags\": {},\"timestamp\":";
 const char *TRAMA_NEXT = "},";
 const char *TRAMA_END = "}]";*/
//GET
 const char *TRAMA_INI = "[";
 //const char *TRAMA_GEO = "{%22fields%22:{%22device%22:1},%22tags%22:{%22geohash%22:%22";
 const char *TRAMA_GEO = "{%22fields%22:{%22latitude%22:";
 const char *TRAMA_GEO2 = ",%22longitude%22:";
 const char *TRAMA_GEO3 = "},%22tags%22:{},%22timestamp%22:null";
 
 const char *TRAMA_TIMESTAMP ="%22},%22timestamp%22:";
 const char *TRAMA_NEXT = "},";
 const char *TRAMA_END = "}]\"\r\n";
 
 const char *hola_mundo = "Hola Mundo\r\n";
 
 //**********ETIQUETAS TEST*****************************************************
 const char *TEST_EEPROM_OK = "test eeprom ok\r\n";
 const char *TEST_EEPROM_NOK = "test eeprom fallido\r\n";
 const char *TEST_UART_OK = "test uart ok\r\n";
 const char *TEST_UART_NOK = "test uart fallido\r\n";
 const char *TEST_PULRUEDA_OK = "test pulsos rueda ok\r\n";
 const char *TEST_PULRUEDA_NOK = "test pulsos rueda fallido\r\n";
 const char *TEST_GPS_OK = "test gps ok\r\n";
 const char *TEST_GPS_NOK = "test gps fallido\r\n";
 const char *TEST_GPRS_OK = "test gprs ok\r\n";
 const char *TEST_GPRS_NOK = "test gprs fallido\r\n";
 
 const char *INICIO_TEST = "INICIO TEST\r\n";
 const char *TEST_TODO_OK = "TEST COMPLETO\r\n";
 const char *TEST_FALLIDO = "TEST FALLIDO\r\n";
 
  
  int test_ok=1;
 
 /*

 const char *LOG = "LOGO";
 const char *PRINC = "PRINCIPAL"; 
 const char *MEN = "MENU";
 const char *CALC = "MENU_CALCULO";
 const char *AGEN = "MENU_AGENDA"; 
 const char *AJ_FABR = "MENU_AJUSTES_FABRICA";
 const char *AJ_USU = "MENU_AJUSTES_USUARIO";
 const char *ESTADIS = "ESTADISTICAS"; 
 const char *INSTAL = "MENU_INSTALACION";
 const char *CALCDRA = "MENU_CALCULADORA";
 const char *AVE = "AVERIAS"; 
 const char *INF = "INFO";
 const char *LLE_DEP = "MENU_LLENAR_DEPOSITO";
 const char *MED = "MENU_MEDIDA"; 
 const char *TECL = "MENU_TECLADO";
 const char *PN = "MENU_CODIGO_PIN";
 const char *RES = "MENU_RESUMEN"; 
 */
  //lista de etiquetas para identificar en que pantalla estamos
 // respetar orden pues es lo que se utiliza para guardar nro_pantalla y el nro_parametros vs pantalla
 char etiquetas_pant_array[19][21]={"","LOGO","PRINCIPAL","MENU","MENU_CALCULO","MENU_AGENDA","MENU_AJUSTES_FABRICA","MENU_AJUSTES_USUARIO","ESTADISTICAS","MENU_INSTALACION","MENU_CALCULADORA","AVERIAS","INFO","MENU_LLENAR_DEPOSITO","MENU_MEDIDA","MENU_TECLADO","MENU_CODIGO_PIN","MENU_RESUMEN"};
 const short nro_para_vs_pant[19]= {0,1,21,1,1,8,11,3,8,5,8,3,1,1,3,1,1,5}; //array de constantes que chivan al gps cuantos parametros extraer de cada tipo de mensaje
                                                                   //la pantalla teclado tiene el estado enable de las 4 ev en un mismo parametro ej: 1010  2ev on 2ev off  

 int timeout_count=0,timeout_parada=0, intentos_comunicacion_fallida=0;
 char reset_count = 0;
 char flag_envio_web=0;
 int envio_bt_escalonado=0;
 
char lat[12];
char lon[12];
char times[13];
char trama_registro[500];
char payload[350];
char trama_test[100]="1,1,20181108084203.000,39.187100,-0.507605,98.000,1.39,304.7,1,,6.2,6.2,1.0,,7,3,,,31,,";
char envio=0;
char cont=0;
char gps_valid=0;
int k=0;
struct tm tm_str;
char year[4];
char mes[2];
char dia[2];
char hora[2];
char minuto[2];
char segundo[2];
time_t date;
int ret=0;
char time_stamp[10];
char hash[15];   
double d_long=0,d_lat=0;
char real_data=1;
char next_data=0;

extern int enviar_bt;


 //*****************************************************************************
 DRV_HANDLE flash_spi_handle;
 DRV_SST25VF064C_BLOCK_COMMAND_HANDLE  commandHandle1,commandHandle2,commandHandle3;
 DRV_SST25VF064C_COMMAND_STATUS f_status;
 //*****************************************************************************
  // event handler of spi flash driver
 void APP_SST25VF020BEventHandler(DRV_SST25VF064C_BLOCK_EVENT event,
            DRV_SST25VF064C_BLOCK_COMMAND_HANDLE handle, uintptr_t context)
{
    switch(event)
    {
        case DRV_SST25VF064C_EVENT_BLOCK_COMMAND_COMPLETE:
            // This means the data was transferred.

            if (handle == commandHandle1)
            {
                // this means block operation has been completed
                strcpy(trama_tx,"Operacion Completada\r\n");
                //appData.mensaje_a_enviar2 =trama_tx;
                //while(WriteString_2()!=true); 
                //bluetooth_send(trama_tx);
            }
            break;

        case DRV_SST25VF064C_EVENT_BLOCK_COMMAND_ERROR:
            // Error handling here.
            //if (handle == commandHandle3)
            //{
                // this means block operation has failed
                //strcpy(trama_tx,"Error \r\n");
                //appData.mensaje_a_enviar2 =trama_tx;
                //while(WriteString_2()!=true); 
            //}
            break;

        default:
            break;
    }
}
 void bluetooth_send(char *string);
//******************************************************************************
//******************************************************************************
 char aux[11]={1,2,3,4,5,6,7,8,9,10};
 char aux2[17];
test_eeprom(){
    
    flash_spi_handle = DRV_SST25VF064C_Open(DRV_SST25VF064C_INDEX_0,DRV_IO_INTENT_READWRITE |DRV_IO_INTENT_NONBLOCKING);
    delay_ms(1000);
    //DRV_SST25VF064C_GeometryGet(flash_spi_handle);
    // set the event handler                            
    DRV_SST25VF064C_BlockEventHandlerSet(flash_spi_handle,APP_SST25VF020BEventHandler,1);
    
    for(i=0; i<4096; i++)
    {
        sourceBuffer[i]= i;
    }


    DRV_SST25VF064C_BlockErase(  flash_spi_handle, &commandHandle1,  0,  4); 

    delay_ms(1000);
   // 2nd block API to write 4k locations in the flash from address 0x00002500 
    DRV_SST25VF064C_BlockWrite
    (
        flash_spi_handle,
        &commandHandle1,
        &sourceBuffer[0],
        0x00001000,
        4096
    );
    delay_ms(1000);
    //3rd block API to read the 2k byte of data (most of which was written above) from flash 
    DRV_SST25VF064C_BlockRead
    (
        flash_spi_handle,
        &commandHandle1,
        targetBuffer,
        0x00001000,
        4096
    );
    delay_ms(1000);

                    
    
    
    
    /*
    //borro primer bloque
    DRV_SST25VF064C_BlockErase(  flash_spi_handle, &commandHandle1,  0,  4); 
    delay_ms(100);
    //escribo aux[] al principio del bloque
     DRV_SST25VF064C_BlockWrite
     (
         flash_spi_handle,
         &commandHandle2,
         &aux[0],
         0x00000000,
         4096
     );
     delay_ms(100);
     //leo en aux2 al principio del bloque
     DRV_SST25VF064C_BlockRead
     (
         flash_spi_handle,
         &commandHandle3,
         &aux2,
         0x00000000,
         4096
     );
     delay_ms(100);
     if(strcmp(aux,aux2)== 0){
         //paso test 
         bluetooth_send(TEST_EEPROM_OK); //envio el mensaje via bluetooth
     }
     else{
         //no paso test
         bluetooth_send(TEST_EEPROM_NOK);
         test_ok=0; //fallo en un paso del test asi que el dispositivo no pasa el test
     }
     delay_ms(200);*/
}
//******************************************************************************
test_gprs(){
    char iteraciones_aux=0;
    char *ptr_aux;
    int result=0;
    strcpy(trama_registro,URL_TEST);  //envio url
    
    appData.mensaje_a_enviar = trama_registro;
    while(true!= WriteString());  //seteo la URL
    delay_ms(100);
    appData.mensaje_a_enviar = GPRS_HTTP_ACTION;   
    while(true!= WriteString());  //ordeno peticion get
    delay_ms(1000);  //espero a que llegue o haga timeout 
    while(iteraciones_aux<10){
        ptr_aux=strstr(trama_rx,"+HTTPACTION:");
        if( ptr_aux != NULL){     
            result = atoi((ptr_aux+15));  //convierto el resultado de la llamada get
            if(result==200){
                iteraciones_aux=10; //termino bucle
            }
        } 
        delay_ms(1000);
        iteraciones_aux++;
    }
    if(result==200){  
    //paso test 
      bluetooth_send(TEST_GPRS_OK); //envio el mensaje via bluetooth
    }
    else{
      //no paso test
      bluetooth_send(TEST_GPRS_NOK);
      test_ok=0; //fallo en un paso del test asi que el dispositivo no pasa el test
    }  
}
//******************************************************************************
test_uart(){
//comprobar la integridad de un mensaje de la pantalla principal del tronic
    appData.Recibido_mensaje_potencia = false; //ignoro el ultimo mensaje;
    //espero suficiente tiempo a que llegue otro 
    delay_ms(2000);
    if(appData.Recibido_mensaje_potencia == true){  //cuando llegue un mensaje
        appData.Recibido_mensaje_potencia = false;
        appData.nro_pantalla = 0; //pongo el numero de pantalla a cero
        //1º obtener la pantalla
        //2º set del nro de parametros para esa pantalla
        //3º guardar todos los datos
        procesar_mensaje_rx_potencia();   //proceso los datos
        if(appData.nro_pantalla == 2){  //si el numero de pantalla es principal
        //paso test 
             bluetooth_send(TEST_UART_OK); //envio el mensaje via bluetooth
         }
         else{
             //no paso test
             bluetooth_send(TEST_UART_NOK);
             test_ok=0; //fallo en un paso del test asi que el dispositivo no pasa el test
         }
    }
    else{
        //no paso test
        bluetooth_send(TEST_UART_NOK);
        test_ok=0; //fallo en un paso del test asi que el dispositivo no pasa el test
    }
    delay_ms(200);
}
//******************************************************************************
test_pulsos_rueda(){
    int vel_aux=0;
    if(appData.nro_pantalla == 2){   //si paso test anterior 
        vel_aux=atoi(appData.parameter_array[2]);  //obtengo dato velocidad 
    }
  //comprobar que lleguen pulsos de rueda a la potencia tronic capturando la velocidad exacta en el mensaje de la pantalla principal
    if( vel_aux== 45){  //si el numero de pantalla es principal
    //paso test 
      bluetooth_send(TEST_PULRUEDA_OK); //envio el mensaje via bluetooth
    }
    else{
      //no paso test
      bluetooth_send(TEST_PULRUEDA_NOK);
      test_ok=0; //fallo en un paso del test asi que el dispositivo no pasa el test
    } 
    delay_ms(200);
}
//******************************************************************************
test_gps_fix(){
    char iteraciones_aux=0,flag_aux=0;
    
    while(iteraciones_aux<120){  //voy a esperar maximo 2 minutos para que pille fix
        
      //compruebo si ha llegado mensaje del gps  
      if(appData.Recibido_mensaje_gps == true){
        appData.Recibido_mensaje_gps = false; 
        // y si tengo fix
        if(strstr(trama_rx,"+UGNSINF: 1,1,") != NULL){     
            iteraciones_aux=120; //termino bucle
            flag_aux=1; 
        }
        
      }
      delay_ms(1000);  
      iteraciones_aux++;
    }
    if( flag_aux == 1){  //si el numero de pantalla es principal
    //paso test 
      bluetooth_send(TEST_GPS_OK); //envio el mensaje via bluetooth
    }
    else{
      //no paso test
      bluetooth_send(TEST_GPS_NOK);
      test_ok=0; //fallo en un paso del test asi que el dispositivo no pasa el test
    } 
    delay_ms(200);
}
//******************************************************************************
//******************************************************************************
//******************************************************************************
int aux_frec_envio = FRECUENCIA_ENVIO_DEF;
// *****************************************************************************
/* Application Data */ 
APP_DATA appData;
//******************************************************************************
void transicion_continuar_programa(){
    //continuo ejecucion del programa
    timeout_count=0;
    if(primera_vez_bt==1){
        appData.mensaje_a_enviar = BTH_AUTO_PAIR;     //ENCIENDO BLUETOOHT SI NO ESTABA YA ENCENDIDO
        appData.state =  SET_BT_PARAMETERS;
        i=2;
    }
    else{
        i=0;
        appData.state =  HABILITAR_INFORME_GPS; //sino voy a encender gps
    }  
}

void concatenador(char *cadena){
    
    if(appData.gps_state == 0){  //si el gps tiene fix preparo los datos 
        strcpy(lat,"0");
        strcpy(lon,"0");
        strcpy(DATOS_VEL,"0");
    }
    strcat(cadena,VAR_DEV);
        strcat(cadena,DEVICE_IMEI);  //IMEI COMO IDENTIFICADOR UNICO DE CADA DISPOSITIVO
        strcat(cadena,SEPARATOR);
        
        strcat(cadena,VAR_LAT);
        strcat(cadena,lat);
        strcat(cadena,SEPARATOR);

        strcat(cadena,VAR_LON);
        strcat(cadena,lon);
        strcat(cadena,SEPARATOR);

        strcat(cadena,VAR_VEL);
        strcat(cadena,DATOS_VEL);
        strcat(cadena,SEPARATOR);

        strcat(cadena,VAR_TS);
        strcat(cadena,time_stamp);
        
        //variables que se escuchan de la potencia tronic
        switch(appData.nro_pantalla){
            case LOGO:
                strcat(cadena,SEPARATOR);
                strcat(cadena,VAR_NPANT);
                itoa(AUX_NPANT,appData.nro_pantalla,10);
                strcat(cadena,AUX_NPANT);  
                strcat(cadena,SEPARATOR);
                
                strcat(cadena,VAR_NLOGO);
                strcat(cadena,appData.parameter_array[0]);

                break;
            case PRINCIPAL:
                
                // CORREGIR!!!
                strcat(cadena,SEPARATOR);
                strcat(cadena,VAR_NPANT);
                itoa(AUX_NPANT,appData.nro_pantalla,10);
                strcat(cadena,AUX_NPANT);  
                strcat(cadena,SEPARATOR);

                strcat(cadena,VAR_DEP);
                strcat(cadena,appData.parameter_array[0]);
                strcat(cadena,SEPARATOR);

                strcat(cadena,VAR_CAU);
                strcat(cadena,appData.parameter_array[1]);
                strcat(cadena,SEPARATOR);
                
                strcat(cadena,VAR_LUD);
                strcat(cadena,appData.parameter_array[3]);
                strcat(cadena,SEPARATOR);
                
                strcat(cadena,VAR_EVSI_EN); 
                strcat(cadena,appData.parameter_array[4]);  //EN LA 3ª POSICION VA VELOCIDAD REPETIDA ME LA AHORRO
                strcat(cadena,SEPARATOR);

                strcat(cadena,VAR_EVII_EN);
                strcat(cadena,appData.parameter_array[5]);  
                strcat(cadena,SEPARATOR);

                strcat(cadena,VAR_EVSD_EN);
                strcat(cadena,appData.parameter_array[6]);  
                strcat(cadena,SEPARATOR);

                strcat(cadena,VAR_EVID_EN);
                strcat(cadena,appData.parameter_array[7]);  
                strcat(cadena,SEPARATOR);

                strcat(cadena,VAR_EVSI);
                strcat(cadena,appData.parameter_array[8]);  
                strcat(cadena,SEPARATOR);

                strcat(cadena,VAR_EVII);
                strcat(cadena,appData.parameter_array[9]);  
                strcat(cadena,SEPARATOR);

                strcat(cadena,VAR_EVSD);
                strcat(cadena,appData.parameter_array[10]);  
                strcat(cadena,SEPARATOR);

                strcat(cadena,VAR_EVID);
                strcat(cadena,appData.parameter_array[11]);  
                strcat(cadena,SEPARATOR);

                strcat(cadena,VAR_CAMPO);
                strcat(cadena,appData.parameter_array[12]);  
                strcat(cadena,SEPARATOR);

                strcat(cadena,VAR_TRAT);
                strcat(cadena,appData.parameter_array[13]);  
                strcat(cadena,SEPARATOR);

                strcat(cadena,VAR_VEL_TRAT);
                strcat(cadena,appData.parameter_array[14]);  
                strcat(cadena,SEPARATOR);

                strcat(cadena,VAR_CAU_CAL);
                strcat(cadena,appData.parameter_array[15]);  
                strcat(cadena,SEPARATOR);

                strcat(cadena,VAR_LUD_TRAT);
                strcat(cadena,appData.parameter_array[16]);  
                strcat(cadena,SEPARATOR);

                strcat(cadena,VAR_NMEN);
                strcat(cadena,appData.parameter_array[17]);  
                strcat(cadena,SEPARATOR);

                strcat(cadena,VAR_MODO);
                strcat(cadena,appData.parameter_array[18]);  
                strcat(cadena,SEPARATOR);

                strcat(cadena,VAR_AGIRO);
                strcat(cadena,appData.parameter_array[19]);  
                strcat(cadena,SEPARATOR);

                strcat(cadena,VAR_UD_MED);
                strcat(cadena,appData.parameter_array[20]);  
                break;
        }
}

void preparar_payload_envio(){

    strcpy(payload,"\0"); //"Vacio la trama payload
    //if(appData.gps_state){  //si el gps tiene fix preparo los datos 
        //variables que da el gps
        
   //concatenador(payload);
        
    if(appData.gps_state == 0){  //si el gps no tiene fix preparo los datos 
        strcpy(lat,"0");
        strcpy(lon,"0");
        strcpy(DATOS_VEL,"0");
        strcpy(time_stamp,"0000000000");
    }
    
        
        strcat(payload,VAR_DEV);
        strcat(payload,DEVICE_IMEI);  //IMEI COMO IDENTIFICADOR UNICO DE CADA DISPOSITIVO
        strcat(payload,SEPARATOR);
        
        strcat(payload,VAR_LAT);
        strcat(payload,lat);
        strcat(payload,SEPARATOR);

        strcat(payload,VAR_LON);
        strcat(payload,lon);
        strcat(payload,SEPARATOR);

        strcat(payload,VAR_VEL);
        strcat(payload,DATOS_VEL);
        strcat(payload,SEPARATOR);

        strcat(payload,VAR_TS);
        strcat(payload,time_stamp);
        
        //variables que se escuchan de la potencia tronic
        switch(appData.nro_pantalla){
            case LOGO:
                strcat(payload,SEPARATOR);
                strcat(payload,VAR_NPANT);
                itoa(AUX_NPANT,appData.nro_pantalla,10);
                strcat(payload,AUX_NPANT);  
                strcat(payload,SEPARATOR);
                
                strcat(payload,VAR_NLOGO);
                strcat(payload,appData.parameter_array[0]);

                break;
            case PRINCIPAL:
                
                // CORREGIR!!!
                strcat(payload,SEPARATOR);
                strcat(payload,VAR_NPANT);
                itoa(AUX_NPANT,appData.nro_pantalla,10);
                strcat(payload,AUX_NPANT);  
                strcat(payload,SEPARATOR);

                strcat(payload,VAR_DEP);
                strcat(payload,appData.parameter_array[0]);
                strcat(payload,SEPARATOR);

                strcat(payload,VAR_CAU);
                strcat(payload,appData.parameter_array[1]);
                strcat(payload,SEPARATOR);
                
                strcat(payload,VAR_LUD);
                strcat(payload,appData.parameter_array[3]);
                strcat(payload,SEPARATOR);
                
                strcat(payload,VAR_EVSI_EN); 
                strcat(payload,appData.parameter_array[4]);  //EN LA 3ª POSICION VA VELOCIDAD REPETIDA ME LA AHORRO
                strcat(payload,SEPARATOR);

                strcat(payload,VAR_EVII_EN);
                strcat(payload,appData.parameter_array[5]);  
                strcat(payload,SEPARATOR);

                strcat(payload,VAR_EVSD_EN);
                strcat(payload,appData.parameter_array[6]);  
                strcat(payload,SEPARATOR);

                strcat(payload,VAR_EVID_EN);
                strcat(payload,appData.parameter_array[7]);  
                strcat(payload,SEPARATOR);

                strcat(payload,VAR_EVSI);
                strcat(payload,appData.parameter_array[8]);  
                strcat(payload,SEPARATOR);

                strcat(payload,VAR_EVII);
                strcat(payload,appData.parameter_array[9]);  
                strcat(payload,SEPARATOR);

                strcat(payload,VAR_EVSD);
                strcat(payload,appData.parameter_array[10]);  
                strcat(payload,SEPARATOR);

                strcat(payload,VAR_EVID);
                strcat(payload,appData.parameter_array[11]);  
                strcat(payload,SEPARATOR);

                strcat(payload,VAR_CAMPO);
                strcat(payload,appData.parameter_array[12]);  
                strcat(payload,SEPARATOR);

                strcat(payload,VAR_TRAT);
                strcat(payload,appData.parameter_array[13]);  
                strcat(payload,SEPARATOR);

                strcat(payload,VAR_VEL_TRAT);
                strcat(payload,appData.parameter_array[14]);  
                strcat(payload,SEPARATOR);

                strcat(payload,VAR_CAU_CAL);
                strcat(payload,appData.parameter_array[15]);  
                strcat(payload,SEPARATOR);

                strcat(payload,VAR_LUD_TRAT);
                strcat(payload,appData.parameter_array[16]);  
                strcat(payload,SEPARATOR);

                strcat(payload,VAR_NMEN);
                strcat(payload,appData.parameter_array[17]);  
                strcat(payload,SEPARATOR);

                strcat(payload,VAR_MODO);
                strcat(payload,appData.parameter_array[18]);  
                strcat(payload,SEPARATOR);

                strcat(payload,VAR_AGIRO);
                strcat(payload,appData.parameter_array[19]);  
                strcat(payload,SEPARATOR);

                strcat(payload,VAR_UD_MED);
                strcat(payload,appData.parameter_array[20]);  
                break;
        }
    //}
}
//******************************************************************************

void preparar_trama_update_tracker(void)  //escribe en trama_tx la URL para enviar al server
{          
       
    //trama_registro[0]='\0';
    //strcat(trama_registro,URL_LOCATEC);
    strcpy(trama_registro,URL_LOCATEC);
    //PAYLOAD EXAMPLE:
    //36.323434&lng=0.123424&vel=2.08&fecha=23042019&hora=1632
    //concatenador(*trama_registro);
    
    if(appData.gps_state == 0){  //si el gps no tiene fix preparo los datos 
        strcpy(lat,"0");
        strcpy(lon,"0");
        strcpy(DATOS_VEL,"0");
        strcpy(time_stamp,"0000000000");
    }
        //variables que da el gps
        strcat(trama_registro,VAR_DEV);
        strcat(trama_registro,DEVICE_IMEI);  //IMEI COMO IDENTIFICADOR UNICO DE CADA DISPOSITIVO
        strcat(trama_registro,SEPARATOR);
        
        strcat(trama_registro,VAR_LAT);
        strcat(trama_registro,lat);
        strcat(trama_registro,SEPARATOR);

        strcat(trama_registro,VAR_LON);
        strcat(trama_registro,lon);
        strcat(trama_registro,SEPARATOR);

        strcat(trama_registro,VAR_VEL);
        strcat(trama_registro,DATOS_VEL);
        strcat(trama_registro,SEPARATOR);

        strcat(trama_registro,VAR_TS);
        strcat(trama_registro,time_stamp);
        
        //variables que se escuchan de la potencia tronic
        switch(appData.nro_pantalla){
            case LOGO:
                strcat(trama_registro,SEPARATOR);
                strcat(trama_registro,VAR_NPANT);
                itoa(AUX_NPANT,appData.nro_pantalla,10);
                strcat(trama_registro,AUX_NPANT);  
                strcat(trama_registro,SEPARATOR);
                
                strcat(trama_registro,VAR_NLOGO);
                strcat(trama_registro,appData.parameter_array[0]);

                break;
            case PRINCIPAL:
                
                // CORREGIR!!!
                strcat(trama_registro,SEPARATOR);
                strcat(trama_registro,VAR_NPANT);
                itoa(AUX_NPANT,appData.nro_pantalla,10);
                strcat(trama_registro,AUX_NPANT);  
                strcat(trama_registro,SEPARATOR);

                strcat(trama_registro,VAR_DEP);
                strcat(trama_registro,appData.parameter_array[0]);
                strcat(trama_registro,SEPARATOR);

                strcat(trama_registro,VAR_CAU);
                strcat(trama_registro,appData.parameter_array[1]);
                strcat(trama_registro,SEPARATOR);
                
                strcat(trama_registro,VAR_LUD);
                strcat(trama_registro,appData.parameter_array[3]);
                strcat(trama_registro,SEPARATOR);
                
                strcat(trama_registro,VAR_EVSI_EN); 
                strcat(trama_registro,appData.parameter_array[4]);  //EN LA 3ª POSICION VA VELOCIDAD REPETIDA ME LA AHORRO
                strcat(trama_registro,SEPARATOR);

                strcat(trama_registro,VAR_EVII_EN);
                strcat(trama_registro,appData.parameter_array[5]);  
                strcat(trama_registro,SEPARATOR);

                strcat(trama_registro,VAR_EVSD_EN);
                strcat(trama_registro,appData.parameter_array[6]);  
                strcat(trama_registro,SEPARATOR);

                strcat(trama_registro,VAR_EVID_EN);
                strcat(trama_registro,appData.parameter_array[7]);  
                strcat(trama_registro,SEPARATOR);

                strcat(trama_registro,VAR_EVSI);
                strcat(trama_registro,appData.parameter_array[8]);  
                strcat(trama_registro,SEPARATOR);

                strcat(trama_registro,VAR_EVII);
                strcat(trama_registro,appData.parameter_array[9]);  
                strcat(trama_registro,SEPARATOR);

                strcat(trama_registro,VAR_EVSD);
                strcat(trama_registro,appData.parameter_array[10]);  
                strcat(trama_registro,SEPARATOR);

                strcat(trama_registro,VAR_EVID);
                strcat(trama_registro,appData.parameter_array[11]);  
                strcat(trama_registro,SEPARATOR);

                strcat(trama_registro,VAR_CAMPO);
                strcat(trama_registro,appData.parameter_array[12]);  
                strcat(trama_registro,SEPARATOR);

                strcat(trama_registro,VAR_TRAT);
                strcat(trama_registro,appData.parameter_array[13]);  
                strcat(trama_registro,SEPARATOR);

                strcat(trama_registro,VAR_VEL_TRAT);
                strcat(trama_registro,appData.parameter_array[14]);  
                strcat(trama_registro,SEPARATOR);

                strcat(trama_registro,VAR_CAU_CAL);
                strcat(trama_registro,appData.parameter_array[15]);  
                strcat(trama_registro,SEPARATOR);

                strcat(trama_registro,VAR_LUD_TRAT);
                strcat(trama_registro,appData.parameter_array[16]);  
                strcat(trama_registro,SEPARATOR);

                strcat(trama_registro,VAR_NMEN);
                strcat(trama_registro,appData.parameter_array[17]);  
                strcat(trama_registro,SEPARATOR);

                strcat(trama_registro,VAR_MODO);
                strcat(trama_registro,appData.parameter_array[18]);  
                strcat(trama_registro,SEPARATOR);

                strcat(trama_registro,VAR_AGIRO);
                strcat(trama_registro,appData.parameter_array[19]);  
                strcat(trama_registro,SEPARATOR);

                strcat(trama_registro,VAR_UD_MED);
                strcat(trama_registro,appData.parameter_array[20]);  
                break;
        }
        strcat(trama_registro,URL_TERMINATOR);  //cierro el comando
//    }
    
}


//******************************************************************************
//Encender modulo SIMCom
void encender_modulo(void)  
{
   PLIB_PORTS_PinWrite (PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_13, 1);//secuencia para arrancar el modulo mediante PWKEY
   delay_ms(1500);
   PLIB_PORTS_PinWrite (PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_13, 0);
}
//******************************************************************************
//Apagar modulo SIMCom
//******************************************************************************
void apagar_modulo(void)  
{
    appData.mensaje_a_enviar = AT_OFF;
     while(false == WriteString());
     
     return;
}
//******************************************************************************
char *ptr_cmd;
void buffer_rx_flush(void)   //funcion para limpiar todos los datos del buffer
{    
    int elementos;
    PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_USART_2_RECEIVE);                                        
    nbyte=0;
    for(elementos=0;elementos<BUFFER_RX_SIZE;elementos++){
        trama_rx[elementos]='\0';
       // trama_tx2[elementos]='\0';
    } 
    PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_USART_2_RECEIVE);
}
void cmd_flush(){
    *ptr_cmd=' ';
    *(ptr_cmd+1)=' ';
    *(ptr_cmd+2)=' ';//destrozo el comando en el buffer rx asi solo lo reconozco una vez
}

void buffer_tx2_flush(void)   //funcion para limpiar todos los datos del buffer
{    
    int elementos=0;
    PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_USART_2_RECEIVE);                                        
    nbyte=0;
    for(elementos=0;elementos<sizeof(trama_registro)/*BUFFER_RX_SIZE*/;elementos++){
        trama_registro[elementos]='\0';
        
    } 
    PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_USART_2_RECEIVE);
}
//******************************************************************************
// *****************************************************************************
void leer_datos_flash(void){
    
    for(j=0;j<32;j++){
        DATOS_FLASH[j] = PLIB_NVM_FlashRead ( NVM_ID_0,(PROGRAM_FLASH_BASE_ADDRESS_VALUE+j*4)); //la direccion debe estar alineada con la de comienzo de un registro de 32 bit
    }
}
// *****************************************************************************
/*******************************************************************************
  Function:
    bool WriteString (void)

  Summary:
    Writes a string to the Sim808 module
 * Returns True when string is completely sent
*/
bool WriteString(void)
{
    if(*appData.mensaje_a_enviar == '\0')
    {
        return true;
    }

    /* Write a character at a time, only if transmitter is empty */
    while (PLIB_USART_TransmitterIsEmpty(USART_ID_2))
    {
        /* Send character */
        PLIB_USART_TransmitterByteSend(USART_ID_2, *appData.mensaje_a_enviar);

        /* Increment to address of next character */
        appData.mensaje_a_enviar++;

        if(*appData.mensaje_a_enviar == '\0')
        {
            return true;
        }
    }
    return false;
}
//*****************************************************************************
bool WriteString_2(void)
{
    if(*appData.mensaje_a_enviar2 == '\0')
    {
        return true;
    }

    /* Write a character at a time, only if transmitter is empty */
    while (PLIB_USART_TransmitterIsEmpty(USART_ID_1))
    {
        /* Send character */
        PLIB_USART_TransmitterByteSend(USART_ID_1, *appData.mensaje_a_enviar2);

        /* Increment to address of next character */
        appData.mensaje_a_enviar2++;

        if(*appData.mensaje_a_enviar2 == '\0')
        {
            return true;
        }
    }
    return false;
}

/*******************************************************************************
  Function:
    bool PutCharacter (const char character)

  Summary:
    Sends a character to the console
*/
/*
bool PutCharacter(const char character)
{
    // Check if buffer is empty for a new transmission 
    if(PLIB_USART_TransmitterIsEmpty(USART_ID_2))
    {
        // Send character 
        PLIB_USART_TransmitterByteSend(USART_ID_2, character);
        return true;
    }
    else
        return false;
}
// *****************************************************************************
// *****************************************************************************
unsigned long long make_64(unsigned long low, unsigned long high){
    unsigned long long data64;

    data64 = (unsigned long long) high << 32 | low;
    return(data64);
}
// *****************************************************************************
// *****************************************************************************
void int64ToChar(char mesg[], unsigned long long mumber,unsigned char size) {
    int i=0, j=0;
    unsigned long long num = mumber,divider=1;
    for(i = 0 ; i <(size-1); i++){ 
        divider = divider*10;    //set up divider according to the number digits size
    }
    for(i = (size-1); i >-1; i--){ 
        mesg[((size-1)-i)]=(num/(divider))+ 0x30;
        num = (unsigned long long)(num % divider);
        divider = divider/10;
    }
    mesg[size]= '\0';
}
*/

//*******************************************************************************
/*
  Function:
    uint32_t virtualToPhysical (uint32_t address)

  Summary:
    Converts a virtual memory address to a physical one
*/
uint32_t virtualToPhysical(uint32_t address)
{
   return (address & 0x1FFFFFFF);
}

//*******************************************************************************
/*
  Function:
    void NVMpageErase (uint32_t address)

  Summary:
    Erases a page in flash memory (16KB)
*/
void NVMpageErase(uint32_t address)
{
   // Base address of page to be erased
   PLIB_NVM_FlashAddressToModify(NVM_ID_0, virtualToPhysical(address));

   // Disable flash write/erase operations
   PLIB_NVM_MemoryModifyInhibit(NVM_ID_0);

   // Select page erase function & enable flash write/erase operations
   PLIB_NVM_MemoryOperationSelect(NVM_ID_0, PAGE_ERASE_OPERATION);

   // Allow memory modifications
   PLIB_NVM_MemoryModifyEnable(NVM_ID_0);

   // Write the unlock key sequence
   PLIB_NVM_FlashWriteKeySequence(NVM_ID_0, 0x0);
   PLIB_NVM_FlashWriteKeySequence(NVM_ID_0, 0xAA996655);
   PLIB_NVM_FlashWriteKeySequence(NVM_ID_0, 0x556699AA);

   // Start the operation
   PLIB_NVM_FlashWriteStart(NVM_ID_0);

      // Wait until the operation has completed
   while (!PLIB_NVM_FlashWriteCycleHasCompleted(NVM_ID_0));

   // Disable flash write/erase operations
   PLIB_NVM_MemoryModifyInhibit(NVM_ID_0);
   
}

//*******************************************************************************
/*
  Function:
    void NVMwriteRow(uint32_t address, uint32_t dataAddress)

  Summary:
    Writes a row in flash memory (2KB)
*/
/*****************************************************************************/
void NVMwriteRow(uint32_t destAddr, uint32_t srcAddr)
{
   // Base address of row to be written to (destination)
   PLIB_NVM_FlashAddressToModify(NVM_ID_0, virtualToPhysical(destAddr));
   // Data buffer address (source)
   PLIB_NVM_DataBlockSourceAddress(NVM_ID_0, virtualToPhysical(srcAddr));

   // Disable flash write/erase operations
   PLIB_NVM_MemoryModifyInhibit(NVM_ID_0);

   // Select row write function & enable flash write/erase operations
   PLIB_NVM_MemoryOperationSelect(NVM_ID_0, ROW_PROGRAM_OPERATION);

   // Allow memory modifications
    PLIB_NVM_MemoryModifyEnable(NVM_ID_0);

   // Write the unlock key sequence
   PLIB_NVM_FlashWriteKeySequence(NVM_ID_0, 0xAA996655);
   PLIB_NVM_FlashWriteKeySequence(NVM_ID_0, 0x556699AA);

   // Start the operation
   PLIB_NVM_FlashWriteStart(NVM_ID_0);

}
// *****************************************************************************
  unsigned int hex_to_dec(uint32_t num){
    unsigned int high, low;
    high = (num >> 4) & 0x0F;
    low = num & 0x0F;
    return (high*10+low);
 }
 // *****************************************************************************
 unsigned int dec_to_hex(unsigned int num){
    unsigned int low, high,res;
    high = num / 10;
    low = num % 10;
    res =((high<<4)&0xF0) | low;
    return res;
 }
 // *****************************************************************************
 void system_sleep(void){ 
    //apagar sim808
     // Clear the WDT timer  
    SYS_WDT_TimerClear ();
     //Disable WDT in order to not wake up uC during sleep
    SYS_WDT_Disable();
    
    SYS_INT_SourceDisable(_TIMER_2_VECTOR); //inhabilito entrada por botones
     
    appData.mensaje_a_enviar = ANT_GPS_OFF; //apago alimentacion de antena gps  
    while(false == WriteString());
    
    apagar_modulo(); 
 
    PLIB_PORTS_PinWrite (PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_12, 1);//apago backlight
    
    
    //inicializacion de registros para sleep
    PLIB_DEVCON_SystemUnlock(DEVCON_ID_0);
    PLIB_OSC_OnWaitActionSet(OSC_ID_0, OSC_ON_WAIT_SLEEP);
    PLIB_DEVCON_SystemLock(DEVCON_ID_0);
    
    /*deshabilito todas las interrupciones*/
    
    SYS_INT_SourceDisable(_TIMER_4_VECTOR); //desactivo interrupcion timer display lcd
    SYS_INT_SourceDisable(_UART_2_VECTOR); //desactivo interrupcion UART
    SYS_INT_SourceDisable(_RTCC_VECTOR); //desactivo interrupcion RTCC
    SYS_INT_SourceDisable(_TIMER_3_VECTOR); //desactivo interrupcion timer captador rueda
    
    PLIB_USART_Disable(USART_ID_2);//desactivo la uart
    DRV_TMR1_Stop(); //Desactivo la entrada por botones
    /*
    DRV_TMR0_Stop();
    
    DRV_TMR2_Stop();
    DRV_RTCC_Stop();
    DRV_I2C0_DeInitialize();
    */    
    system_in_sleep=1;
    veces_hab_bt=0;
    
    delay_ms(2000); //espera para que no se produzca encendido accidental
    
    PLIB_PORTS_PinWrite (PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_2, 1);//apago convertidor 5 V
    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_EXTERNAL_4);
    SYS_INT_SourceEnable(INT_SOURCE_EXTERNAL_4);//activar acelerometro
    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_EXTERNAL_1);  // para activar la interrupcion del boton +
    SYS_INT_SourceEnable(INT_SOURCE_EXTERNAL_1);
    
    __asm__ volatile("WAIT");
    __asm__ volatile("nop");
 } 
 // *****************************************************************************
 /*
 void system_wake_up(void){ 
     
    //PLIB_PORTS_PinWrite (PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_2, 0);//enciendo convertidor 5 V
																					
 
    PLIB_PORTS_PinWrite (PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_12, 0);//enciendo backlight
    
        //si esta pulsado el boton D al arrancar entramos en modo bluetooth
    //if((PLIB_PORTS_PinGet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_15)==0)&&(PLIB_PORTS_PinGet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_14)==0)){
    //si bluetooth esta desactivado no entrar
    DRV_TMR1_Start(); 
    //mostrar_en_display = MODO_BLE;
   // appData.modo_bt = false;
    //if(appData.modo_test==BT_FIRM_ENABLED){
        appData.cuenta_hab_bt=true; //inicio cuenta boton D pulsado para activar bt
    //}
    //else{
    //    appData.cuenta_hab_bt=false;
    //}
    //escribir_logo_lcd(5);  //para escribir el logo GZ al despertar
    
    SYS_INT_SourceEnable(_TIMER_2_VECTOR); //activo interrupcion timer botones
    SYS_INT_SourceEnable(_TIMER_4_VECTOR); //activo interrupcion timer display lcd
    
    SYS_INT_SourceEnable(_RTCC_VECTOR); //activo interrupcion RTCC
    SYS_INT_SourceEnable(_TIMER_3_VECTOR); //activo interrupcion timer captador rueda
    
    DRV_USART0_Initialize();// vuelvo a inicializar la UART
    SYS_INT_VectorPrioritySet(INT_VECTOR_UART2, INT_PRIORITY_LEVEL2);
    SYS_INT_VectorSubprioritySet(INT_VECTOR_UART2, INT_SUBPRIORITY_LEVEL0);
    SYS_INT_SourceEnable(_UART_2_VECTOR); //activo interrupcion UART
    PLIB_USART_Enable(USART_ID_2);
	  
    SYS_INT_SourceDisable(INT_SOURCE_EXTERNAL_4); //desactivo interrupcion acelerometro
    SYS_INT_SourceDisable(INT_SOURCE_EXTERNAL_1); //desactivo interrupcion boton + 
     
    appData.datos_gps_disponible = 0;

    encender_modulo();
    delay_ms(1500);
    appData.state =  SIM808_OFF_COM;    
	
    appData.mensaje_a_enviar = AT;
    
    system_in_sleep=0;
                   
    delay_ms(500);
    
 }
  */
 /*
 //*****************************************************************************
 void load_to_flash_array(){
   DATOS_FLASH[1] = appData.GPS_Captador_Rueda;        
   DATOS_FLASH[2] = appData.precision_distancia;      
   DATOS_FLASH[3] = appData.seg_gps_a_brujula;         
   DATOS_FLASH[4] = appData.funcion_crono;             
   DATOS_FLASH[5] = appData.decimas_pulso_breve;       
   DATOS_FLASH[6] = appData.decimas_pulso_mantenido;   
   DATOS_FLASH[7] = appData.seg_visualizacion_temp;    
   DATOS_FLASH[8] = appData.seg_apagado_auto;          
   DATOS_FLASH[9] = appData.seg_pulso_borrado;         
   DATOS_FLASH[10] = appData.cent_rueda;                
   DATOS_FLASH[11] = appData.km_millas;                 
   DATOS_FLASH[12] = appData.pulgadas_rueda;            
   DATOS_FLASH[13] = appData.desv_utc; 
   DATOS_FLASH[14] = appData.Retroiluminacion;
   DATOS_FLASH[15] = appData.dec_seg_prec;              
   DATOS_FLASH[16] = appData.umbral_vel_cap_gps;
 }
 
 void load_to_flash_array2(){
   DATOS_FLASH[29] = parcial_viaje_1;
   DATOS_FLASH[28] = parcial_viaje_2;
   DATOS_FLASH[27] = kms_aceite;
   DATOS_FLASH[26] = kms_total; 
   DATOS_FLASH[25] = fecha_aceite;
 }
 
 void write_default_parameters(int bt_enable){
   clear_lcd();
   appData.GPS_Captador_Rueda=P00;        
   appData.precision_distancia=P01;      
   appData.seg_gps_a_brujula=P02;         
   appData.funcion_crono=P03;             
   appData.decimas_pulso_breve=P04;       
   appData.decimas_pulso_mantenido=P05;   
   appData.seg_visualizacion_temp=P06;    
   appData.seg_apagado_auto=P07;          
   appData.seg_pulso_borrado=P08;         
   appData.cent_rueda=P09;                
   appData.km_millas=P10;                 
   appData.pulgadas_rueda=P11;            
   appData.desv_utc=P12; 
   appData.Retroiluminacion =P13;
   appData.dec_seg_prec=P14;              
   appData.umbral_vel_cap_gps=P15;
   

   load_to_flash_array(); //escribir parametros en datos flash
   //escribir estadisticas en flash;
   
   if(bt_enable==1){
        appData.bt_firm_enable=BT_FIRM_ENABLED;    
   }
   else if(bt_enable==0){
        appData.bt_firm_enable=0;
   }
   else{
       appData.bt_firm_enable=PLIB_NVM_FlashRead(NVM_ID_0,PROGRAM_FLASH_BASE_ADDRESS_VALUE+30*4); 
   }
     
   DATOS_FLASH[30] = appData.bt_firm_enable;
   //DATOS_FLASH[18] = distancia_recorrida;
   DATOS_FLASH[31] = CONFIG_DATA_AVAILABLE_2;  //escribo marca al final  
   
   appData.state = GRABAR_DATOS_EN_FLASH; // VOY A GUARDAR LOS PARAMETROS EN FLASH
   primera_escritura_flash=1;
 }
 //*****************************************************************************
 void load_flash_data(void){
       appData.GPS_Captador_Rueda=PLIB_NVM_FlashRead(NVM_ID_0,PROGRAM_FLASH_BASE_ADDRESS_VALUE+1*4);        
       appData.precision_distancia=PLIB_NVM_FlashRead(NVM_ID_0,PROGRAM_FLASH_BASE_ADDRESS_VALUE+2*4);      
       appData.seg_gps_a_brujula=PLIB_NVM_FlashRead(NVM_ID_0,PROGRAM_FLASH_BASE_ADDRESS_VALUE+3*4);         
       appData.funcion_crono=PLIB_NVM_FlashRead(NVM_ID_0,PROGRAM_FLASH_BASE_ADDRESS_VALUE+4*4);             
       appData.decimas_pulso_breve=PLIB_NVM_FlashRead(NVM_ID_0,PROGRAM_FLASH_BASE_ADDRESS_VALUE+5*4);       
       appData.decimas_pulso_mantenido=PLIB_NVM_FlashRead(NVM_ID_0,PROGRAM_FLASH_BASE_ADDRESS_VALUE+6*4);   
       appData.seg_visualizacion_temp=PLIB_NVM_FlashRead(NVM_ID_0,PROGRAM_FLASH_BASE_ADDRESS_VALUE+7*4);    
       appData.seg_apagado_auto=PLIB_NVM_FlashRead(NVM_ID_0,PROGRAM_FLASH_BASE_ADDRESS_VALUE+8*4);          
       appData.seg_pulso_borrado=PLIB_NVM_FlashRead(NVM_ID_0,PROGRAM_FLASH_BASE_ADDRESS_VALUE+9*4);         
       appData.cent_rueda=PLIB_NVM_FlashRead(NVM_ID_0,PROGRAM_FLASH_BASE_ADDRESS_VALUE+10*4);                
       appData.km_millas=PLIB_NVM_FlashRead(NVM_ID_0,PROGRAM_FLASH_BASE_ADDRESS_VALUE+11*4);                 
       appData.pulgadas_rueda=PLIB_NVM_FlashRead(NVM_ID_0,PROGRAM_FLASH_BASE_ADDRESS_VALUE+12*4);            
       appData.desv_utc=PLIB_NVM_FlashRead(NVM_ID_0,PROGRAM_FLASH_BASE_ADDRESS_VALUE+13*4); 
       appData.Retroiluminacion =PLIB_NVM_FlashRead(NVM_ID_0,PROGRAM_FLASH_BASE_ADDRESS_VALUE+14*4);
       appData.dec_seg_prec=PLIB_NVM_FlashRead(NVM_ID_0,PROGRAM_FLASH_BASE_ADDRESS_VALUE+15*4);              
       appData.umbral_vel_cap_gps=PLIB_NVM_FlashRead(NVM_ID_0,PROGRAM_FLASH_BASE_ADDRESS_VALUE+16*4);
       
       //aqui hay que hacer load de trip 1, trip2, kmsaceite, kmstotal
       
        parcial_viaje_1 =PLIB_NVM_FlashRead(NVM_ID_0,PROGRAM_FLASH_BASE_ADDRESS_VALUE+29*4); 
        parcial_viaje_2 =PLIB_NVM_FlashRead(NVM_ID_0,PROGRAM_FLASH_BASE_ADDRESS_VALUE+28*4); 
        kms_aceite =PLIB_NVM_FlashRead(NVM_ID_0,PROGRAM_FLASH_BASE_ADDRESS_VALUE+27*4); 
        kms_total =PLIB_NVM_FlashRead(NVM_ID_0,PROGRAM_FLASH_BASE_ADDRESS_VALUE+26*4); 
        fecha_aceite=PLIB_NVM_FlashRead(NVM_ID_0,PROGRAM_FLASH_BASE_ADDRESS_VALUE+25*4); 
       
       appData.bt_firm_enable=PLIB_NVM_FlashRead(NVM_ID_0,PROGRAM_FLASH_BASE_ADDRESS_VALUE+30*4); 
       //vel_max = ((double)PLIB_NVM_FlashRead(NVM_ID_0,PROGRAM_FLASH_BASE_ADDRESS_VALUE+17*4))/10.0;
       //distancia_recorrida = PLIB_NVM_FlashRead(NVM_ID_0,PROGRAM_FLASH_BASE_ADDRESS_VALUE+18*4);  
       primera_escritura_flash=0;

       //******************************************************
       //RUTINA DE COMPROBACION DEL WDT AL ARRANCAR EL PROGRAMA
       // Clear the WDT timer 
       //SYS_WDT_TimerClear ();
        //Enable WDT
       //SYS_WDT_Enable ( false );

       if(SYS_RESET_ReasonGet() == RESET_REASON_WDT_TIMEOUT){
           //escribir_logo_lcd(4);
           //delay_ms(2000); //pruebas
           appData.state = REFRESCAR_DISPLAY;
           DRV_TMR1_Start();  //para el control de entrada de botones; lo activo cuando deberia poder manipular el aparato
           // Clear the WDT timer  
           SYS_WDT_TimerClear ();
            //Enable WDT
           SYS_WDT_Enable ( false );
       }
       else{
           vel_max = 0;
           kms_trip = 0;
           appData.state = SIM808_TURN_ON; //continuo programa
           mostrar_en_display = DISTANCIA;   //empiezo mostrando distancia
           escribir_logo_lcd(5);  //para escribir el logo GZ al arrancar equipo
       }
       //SYS_RESET_ReasonClear( RESET_REASON_ALL ); //BORRAR REGISTRO DE CAUSA DEL RESET
 
 }
 void preparo_guardado_flash(){
    i=0;
    appData.state = GRABAR_DATOS_EN_FLASH;
    leer_datos_flash(); // leo todo el array de datos en eeprom
    load_to_flash_array(); //sobreescribir valores de parametros 
    load_to_flash_array2(); //sobreescribir valores de estadisticas
    
 }
*/
 //*****************************************************************************
 char permision_to_send=0;
 int intentos_cazar_espacio=0;
 
 //char quit=0x1B;
 //rutina para mandar via bluetooth lo que haya cargado en trama_tx
 void bluetooth_send(char *string){
     char term=0x1A;
    //si estoy conectado envio mensaje
    if(appData.bt_state==1){
        
        appData.mensaje_a_enviar = BTH_SEND;
        while(WriteString()!=true); 
        delay_ms(50);
        while(permision_to_send==0){ 
            if(trama_rx[(nbyte-1)]==0x20){ 
                appData.mensaje_a_enviar = string;
                while(WriteString()!=true); 
                //delay_ms(50);
                appData.mensaje_a_enviar = &term;
                while(WriteString()!=true);   
                permision_to_send=1;    
            }
            else{
                if(intentos_cazar_espacio<9){
                    intentos_cazar_espacio++; 
                    delay_ms(50);
                }
                else{
                   appData.mensaje_a_enviar = &term;  //mando un ESC y rompo el bucle
                   while(WriteString()!=true);   
                   permision_to_send=1;
                }
                
            }
        }
        intentos_cazar_espacio=0;
        permision_to_send=0;
    }
 }
 //*****************************************************************************
 extern char buffer_rx_potencia[150];
 //rutina para procesar datos recibidos por parte de la potencia tronic
 void procesar_mensaje_rx_potencia(){
     char * ptr;
     short index=0;
     //ptr = buffer_rx_potencia;
     //1º obtener la pantalla
     //2º set del nro de parametros para esa pantalla
     ptr = strtok(buffer_rx_potencia,":"); //corto la trama por ":" 
     
     for (index=1;index<19;index++){
        if(strcmp(buffer_rx_potencia,etiquetas_pant_array[index]) == 0){  
            appData.nro_pantalla = index; //hago set del numero de pantalla
            appData.nro_parametros = nro_para_vs_pant[index];
            index=19; //cortar por lo sano el bucle for (ya he encontrado que pantalla es)
        }  
     }
     //3º guardar los datos 
    for(index=0;index<appData.nro_parametros;index++){  //recorro el mensaje nro_parametros veces y guardos los parametros en parameter_array en orden
        ptr = strtok(NULL,","); //continuo cortando el mensaje con \0 donde hay comas 
        strcpy(&appData.parameter_array[index],ptr);  //me copio los parametros de la pantalla 
    }
     //appData.parameter_array(index)='\0'; //indica que se han acabado los parametros
     

 }
 //*****************************************************************************
// DRV_HANDLE flash_spi_handle;
// DRV_SST25VF064C_BLOCK_COMMAND_HANDLE  commandHandle1,commandHandle2,commandHandle3;
// DRV_SST25VF064C_COMMAND_STATUS f_status;
 //*****************************************************************************

//******************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************
/*******************************************************************************
  Function:
    void APP_Initialize ( void )
 */
 unsigned int aux_salida_pulsos=0;
 double aux_cal_pulsos=0,vel_test=0;
 void initialize_user_vars(){
    buffer_rx_flush();
    appData.state = SIM808_TURN_ON;
    appData.tick_scaler_sirena=0;
    DRV_TMR0_Stop();
    DRV_TMR1_Stop();
    DRV_TMR2_Stop();  //parar salida de pulsos
    DRV_OC0_Enable(); //habilitar pwm salida pulsos para siempre 
    
    SYS_WDT_Disable ();
    PLIB_PORTS_PinWrite (PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_9,1); //poner led rojo fijo para indicar que el equipo tiene alimentacion pero no esta listo para trabajar
    PLIB_PORTS_PinWrite (PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_8,0);
    appData.state = SIM808_TURN_ON;
    appData.gps_state=0;
    appData.bt_state=0;
    appData.gsm_state=0;
    appData.frecuencia_envio = FRECUENCIA_ENVIO_DEF;
    PLIB_PORTS_PinWrite (PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_2, 0);//enciendo convertidor 5 V
    appData.mensaje_a_enviar = AT;
    appData.Recibido_mensaje = false;
    real_data=1;
    primera_vez_bt=1; //como he apagado el sello me toca reinicializar el bluetooth
 }
 
void APP_Initialize ( void )
{  
   /* 
    appData.state = SIM808_TURN_ON;
    appData.tick_scaler_sirena=0;
    
    DRV_TMR0_Stop();
    DRV_TMR2_Stop();  //parar salida de pulsos
    DRV_OC0_Enable(); //habilitar pwm salida pulsos para siempre 
    
    SYS_WDT_Disable ();
    
    appData.state = SIM808_TURN_ON;
    
    
    PLIB_PORTS_PinWrite (PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_9,1); //poner led rojo fijo para indicar que el equipo tiene alimentacion pero no esta listo para trabajar
    
    SYS_INT_SourceDisable(INT_SOURCE_USART_1_TRANSMIT); //inhabilito la interrupcion de transmision de la uart1
    
    appData.gps_state=0;
    appData.bt_state=0;
    appData.gsm_state=0;
    appData.frecuencia_envio = FRECUENCIA_ENVIO_DEF;
    PLIB_PORTS_PinWrite (PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_2, 0);//enciendo convertidor 5 V
    appData.mensaje_a_enviar = AT;
    appData.Recibido_mensaje = false;
    */
    
    initialize_user_vars();
    //evaluar si entro en modo test al dar alimentacion al equipo
    if(PLIB_PORTS_PinGet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_1) == 1){
        appData.modo_rutina_test=1;
    }
    
    //funciones para manejar la memoria flash
    /*
void DRV_SST25VF064C_BlockRead(const DRV_HANDLE hClient,DRV_SST25VF064C_BLOCK_COMMAND_HANDLE * commandHandle,uint8_t *targetBuffer,uint32_t blockStart,uint32_t nBlock)
void DRV_SST25VF064C_BlockWrite(DRV_HANDLE hClient, DRV_SST25VF064C_BLOCK_COMMAND_HANDLE * commandHandle,uint8_t *sourceBuffer, uint32_t blockStart, uint32_t nBlock)    
void DRV_SST25VF064C_BlockErase(  const DRV_HANDLE hClient,  DRV_SST25VF064C_BLOCK_COMMAND_HANDLE * commandHandle,  uint32_t blockStart,  uint32_t nBlock)
    */
    
    
    /*
    reasonType = SYS_RESET_ReasonGet();//obtener razon del reset
    //inicializacion de variables estado del programa
    appData.datos_gps_disponible = 0;
    appData.mensaje_a_enviar = AT;
    //mostrar_en_display = DISTANCIA;   //empiezo mostrando distancia
    //ajustar_reloj = 1;
    PLIB_PORTS_PinWrite (PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_2, 0);//enciendo convertidor 5 V
    PLIB_PORTS_PinWrite (PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_12, 0);//enciendo backlight
    //ENABLE RTCC MODULE
    //DRV_RTCC_Start();  // HABILITAR RTC
   // PLIB_RTCC_AlarmEnable (0); // HABILITAR ALARMAS   para usar el rtcc para el reloj y para el cronometro
   // PLIB_RTCC_AlarmRepeatCountSet ( 0, 10 );          //habilitar alarmas, chimeenable para que se repitan indefinidamente
   // PLIB_RTCC_AlarmChimeEnable ( 0 );                   //hay que cuadrar la fecha y hora de la alarma con la del rtc
    
    SYS_INT_SourceDisable(INT_SOURCE_EXTERNAL_4); //desactivo interrupcion acelerometro
    SYS_INT_SourceDisable(INT_SOURCE_EXTERNAL_1); //desactivo interrupcion BOTON +
    
    //acelerometer_init(); //inicializo acelerometro
    
    appData.Recibido_mensaje = false;
    
    //si se pulsan las dos teclas al arrancar el equipo se graban parametros por defecto
    if((PLIB_PORTS_PinGet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_15)==0)&&(PLIB_PORTS_PinGet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_14)==0)){
        if(SYS_RESET_ReasonGet() == RESET_REASON_POR){
            //escribir_logo_lcd(1);
            //delay_ms(2000);
            write_default_parameters(3);   //fuerzo valores de fabrica y mantengo estado bluetooth
            //SYS_RESET_ReasonClear( RESET_REASON_ALL ); //BORRAR REGISTRO DE CAUSA DEL RESET
            
        }
        else{
            if(PLIB_NVM_FlashRead(NVM_ID_0,PROGRAM_FLASH_BASE_ADDRESS_VALUE) == CONFIG_DATA_AVAILABLE){
                if(PLIB_NVM_FlashRead(NVM_ID_0,PROGRAM_FLASH_BASE_ADDRESS_END) == CONFIG_DATA_AVAILABLE_2){
                    //leo datos flash; 
                    load_flash_data();
                }
                //si se han corrompido se vuelven a escribir los parametros por defecto
                else{
                    //appData.bt_firm_enable=BT_FIRM_ENABLED;
                    //DATOS_FLASH[30] = appData.bt_firm_enable;
                    write_default_parameters(1);  //con firmware bt habilitado
                }
            }
            else{
                //appData.bt_firm_enable=BT_FIRM_ENABLED;
                //DATOS_FLASH[30] = appData.bt_firm_enable;
                write_default_parameters(1); //con firmware bt habilitado
            }
        }
    }
    //si no se continua con el programa normal
    else{
    //si existen parametros se cargan desde memoria
        if(PLIB_NVM_FlashRead(NVM_ID_0,PROGRAM_FLASH_BASE_ADDRESS_VALUE) == CONFIG_DATA_AVAILABLE){
            if(PLIB_NVM_FlashRead(NVM_ID_0,PROGRAM_FLASH_BASE_ADDRESS_END) == CONFIG_DATA_AVAILABLE_2){
                //leo datos flash; 
                load_flash_data();
            }
            //si se han corrompido se vuelven a escribir los parametros por defecto
            else{
                write_default_parameters(1);
            }
        }
        else{
            write_default_parameters(1);
        } 
    }
    //si detecto que pongo a 
    if(PLIB_PORTS_PinGet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_1)==1&&(SYS_RESET_ReasonGet() == RESET_REASON_POR)){
        if(PLIB_NVM_FlashRead(NVM_ID_0,PROGRAM_FLASH_BASE_ADDRESS_VALUE+30*4)==BT_FIRM_ENABLED){
            write_default_parameters(0);
            escribir_lcd(0,ON,ON,ON,ON,ON);
        }
        else{
            write_default_parameters(1);
            escribir_lcd(1,ON,ON,ON,ON,ON);
        }
        delay_ms(2000);
    }
    
    
    SYS_RESET_ReasonClear( RESET_REASON_ALL ); //BORRAR REGISTRO DE CAUSA DEL RESET
    */
}
/**********************************************************
 * Application tasks routine. This function implements the
 * application state machine.
 ***********************************************************/
void APP_Tasks ( void )
{
   
 switch ( appData.state )
    { 
        
        case SIM808_CRASH_HANDLER: 
           /* 
           PLIB_PORTS_PinWrite (PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_9,1);
           PLIB_PORTS_PinWrite (PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_8,0);
           delay_ms(500);
           PLIB_PORTS_PinWrite (PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_9,0);
           PLIB_PORTS_PinWrite (PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_8,1);
           delay_ms(500);
            */ 
           // if(appData.Recibido_mensaje_potencia == true){
           //     appData.Recibido_mensaje_potencia = false;
           //     procesar_mensaje_rx_potencia();
           // }
            encender_modulo();
            delay_ms(1500);
            initialize_user_vars();
            
            break;     
        
        case SIM808_OFF_COM: 
       
             if(true == WriteString())
            {
                delay_ms(500); 
                appData.state = SIM808_OFF_CHECK_RESPONSE;   
            }
            
            break; 
            
        case SIM808_OFF_CHECK_RESPONSE:   
            
            if(appData.Recibido_mensaje == true){
                appData.Recibido_mensaje = false;
                if(strstr(trama_rx,"OK") != NULL){
                    appData.state =  SIM808_CONFIG;
                    delay_ms(5000);
                    i=0;
                }  
            }
            else{
                if( i < TEST_COMM_ATTEMPTS ){
                    i++;
                    appData.mensaje_a_enviar = AT;
                    appData.state =  SIM808_OFF_COM; 
                }
                else{
                    i=0;
                    appData.state =  SIM808_TURN_ON;  
                    buffer_rx_flush();
                }
            }
           
            break; 
            
        case SIM808_TURN_ON:          
            encender_modulo();
            delay_ms(1500);   //el modulo simcomm tarda 3 segundos en ponerse operativo. 1.5 para encender por pwrkey, 1.5 para empezar a interpretar comandos via puerto serie
            appData.state =  SIM808_OFF_COM;    
            break; 
                               
        case SIM808_CONFIG:     
            //SECUENCIA DE COMANDOS PARA CONFIGURAR EL MODULO 
            switch(i){
                case 0:
                    appData.mensaje_a_enviar = AT_FACTORY;                    
                    i++;
                    break;
                case 1:
                    if(true == WriteString())
                    {
                        appData.mensaje_a_enviar = BAUDRATE_SEL;
                        i++;
                        delay_ms(500);
                      
                    }
                    break;
                case 2:
                    if(true == WriteString())
                    {
                        appData.mensaje_a_enviar = GPS_ON;
                        i++;
                        delay_ms(500);
                    }
                    break;  
                case 3:
                    if(true == WriteString())
                    {
                        appData.mensaje_a_enviar = ANT_GPS_ON;
                        i++;
                        delay_ms(500);
                    }
                    break;  
                case 4:
                    if(true == WriteString())
                    {
                        appData.mensaje_a_enviar = ECHO_OFF;
                        i++;
                        delay_ms(500);
                    }
                    break;  
                case 5:
                    if(true == WriteString())
                    {
                        i=0;    
                        appData.state =  GET_DEVICE_IMEI; //transicion a configuracion de bluetooth dispositivo
                        appData.mensaje_a_enviar = GET_IMEI;
                        delay_ms(500); //para evitar cuelgue del sim808
                    }
                    break;     
            }
            break;
            
     case INICIAR_CONEXION_GPRS:
         switch(i){   
               case 0:
                    appData.mensaje_a_enviar = HABILITAR_GPRS; //habilitar radiofrecuencia del modulo
                    i++;
                    break;
               case 1:  
                    if(true == WriteString())
                    {
                        i++;
                        delay_ms(5000);  // espero 10 segundos para salir de modo avion y conseguir cobertura de red
                    }  
                    break;                     
               case 2:
                    appData.mensaje_a_enviar = AT_CREG;   //pido estado cobertura de red
                    i++;
                    break; 
               case 3:
                    if(true == WriteString())
                    {
                        i++;
                        delay_ms(1000); //esperar 1 seg
                    }
                    break;            
               case 4:   
                    if(appData.Recibido_mensaje == true){    
                        
                        if(strstr(trama_rx,"+CREG: 0,1") != NULL){    //si tengo cobertura avanzo proceso de configuracion 
                            
                            i++;
                            appData.mensaje_a_enviar = ATTACH_GPRS;
                            delay_ms(1500);
                            appData.Recibido_mensaje = false;
                            timeout_count=0;
                        }
                        else{
                            if(timeout_count < TIMEOUT_COUNT_MAX ){    //si no tengo cobertura espero y pregunto hasta encontrar cobertura o salta timeout
                                i=2;
                                appData.mensaje_a_enviar = AT_CREG;
                                timeout_count++;
                                //delay_ms(1000);
                            }
                            else{
                                transicion_continuar_programa();
                                appData.gsm_state = 0;
                            }
                        }
                        buffer_rx_flush();   //vacio el buffer para no tener falso positivo
                        appData.Recibido_mensaje = false;
                        intentos_comunicacion_fallida=0;
                    }
                    // EN CASO DE QUE EL MODULO SIM808 SE QUEDE COLGADO Y NO DE RESPUESTA
                    else{
                        if( intentos_comunicacion_fallida < TEST_COMM_ATTEMPTS ){
                            intentos_comunicacion_fallida++;   //contar un fallo
                            appData.mensaje_a_enviar = AT_CREG;  //volver a intentar ultima accion 
                            i=3;
                        }
                        else{
                            //reiniciar el modulo de comunicaciones
                            /*
                            i=0;
                            intentos_comunicacion_fallida=0;
                            encender_modulo(); //apago el modulo(la funcion encender apaga si esta encendido y enciende si esta apagado)
                            delay_ms(3000);  // espero a que el modulo se apague 
                            appData.state =  SIM808_TURN_ON;  
                            //reestablecer sistema
                             */ 
                            
                        }
                    }
           
                    break;
               case 5:
                    if(true == WriteString())
                    {
                        i++;
                        delay_ms(3000);
                        appData.mensaje_a_enviar = GPRS_ATTACHED;
                    }
                    break;
                case 6:
                    if(true == WriteString())
                    {
                        i++; 
                        delay_ms(1500);
                    }
                    break;
               case 7:
                    if(appData.Recibido_mensaje == true){
                       if(strstr(trama_rx,"+CGATT: 1")!= NULL){
                            i++;
                            appData.mensaje_a_enviar = GPRS_RESET_IP;
                            delay_ms(500);
                            timeout_count=0;
                        }
                        else{
                            //poner aqui codigo para contar n fallos
                            if(timeout_count < TIMEOUT_COUNT_MAX ){    //si no tengo cobertura espero y pregunto hasta encontrar cobertura o salta timeout
                                i=2;
                                appData.mensaje_a_enviar = AT_CREG;
                                
                                timeout_count++;
                                //delay_ms(500);
                            }
                            else{
                                transicion_continuar_programa();
                                appData.gsm_state = 0;
                            }
                        }
                        buffer_rx_flush();   //vacio el buffer para no tener falso positivo
                        appData.Recibido_mensaje = false;
                        intentos_comunicacion_fallida=0;
                    }
                    // EN CASO DE QUE EL MODULO SIM808 SE QUEDE COLGADO Y NO DE RESPUESTA
                    else{
                        if( intentos_comunicacion_fallida < TEST_COMM_ATTEMPTS ){
                            intentos_comunicacion_fallida++;   //contar un fallo
                            appData.mensaje_a_enviar = GPRS_ATTACHED;  //volver a intentar ultima accion 
                            i=6;
                        }
                        else{
                            /*
                            transicion_continuar_programa();
                            appData.gsm_state = 0;
                             */ 
                        }
                    }
                    break;
                case 8:
                    if(true == WriteString())
                    {
                        i++;
                        appData.mensaje_a_enviar = APN_CON_TYPE;
                        delay_ms(500);
                    }
                    break;
                case 9:
                    if(true == WriteString())
                    {
                        i++;
                        appData.mensaje_a_enviar = APN;
                        delay_ms(500);
                        timeout_count=0; //reseteo el contador de fallos
                    }
                    break;
                case 10:
                    if(true == WriteString())
                    {
                        i++;
                        appData.mensaje_a_enviar = OPEN_APN;
                        delay_ms(1000);
                        
                    }
                    break;
                case 11:
                    if(true == WriteString())
                    {
                        i++;
                        appData.mensaje_a_enviar = QUERY_APN;
                        delay_ms(2000);
                        
                    }
                    break;
                case 12:
                    if(true == WriteString())
                    {
                        i++;
                        delay_ms(1000);
                    }
                    break;
                 case 13:
                    if(appData.Recibido_mensaje == true){
                        if(strstr(trama_rx,"+SAPBR: 1,1") != NULL){   
                            i=0;
                            timeout_count=0;
                            buffer_rx_flush();

                            appData.state =  INICIALIZAR_TRAMA_HTTP;

                                  
                            appData.Recibido_mensaje = false;
                        } 
                        else{
                            if(timeout_count < TIMEOUT_COUNT_MAX ){    //si no tengo cobertura espero y pregunto hasta encontrar cobertura o salta timeout
                                i=12;
                                appData.mensaje_a_enviar = QUERY_APN;
                                timeout_count++;
                                delay_ms(500); 
                            }
                            else{
                                transicion_continuar_programa();
                                appData.gsm_state = 0;
                            }    
                        }
                        buffer_rx_flush();   //vacio el buffer para no tener falso positivo
                        appData.Recibido_mensaje = false;
                        intentos_comunicacion_fallida=0;
                    }
                    // EN CASO DE QUE EL MODULO SIM808 SE QUEDE COLGADO Y NO DE RESPUESTA
                    else{
                        if( intentos_comunicacion_fallida < TEST_COMM_ATTEMPTS ){
                            intentos_comunicacion_fallida++;   //contar un fallo
                            appData.mensaje_a_enviar = OPEN_APN;  //volver a intentar ultima accion 
                            i=11;
                        }
                        else{
                            /*
                            transicion_continuar_programa();
                            appData.gsm_state = 0;
                             */ 
                        }
                    }
                    
                    break;
            }
         break;
     case INICIALIZAR_TRAMA_HTTP:
         switch(i){
             case 0:
                    if(true == WriteString())
                    {
                        appData.mensaje_a_enviar = GPRS_HTTP_START;  
                        delay_ms(2000);
                        i++; 
                         
                    }
                    break;
                 case 1:
                    if(true == WriteString())
                    {
                        appData.mensaje_a_enviar = CID_SEL;  
                        delay_ms(500);
                        i++;
                        
                    }
                    break;
                    case 2:
                    if(true == WriteString())
                    {   
                        appData.mensaje_a_enviar = SSL_ENABLE; 
                        delay_ms(500);
                        i++;
                        
                    }
                    break;
                    case 3:
                    if(true == WriteString())
                    {
                        
                        if(primera_vez_bt==1){
                            appData.mensaje_a_enviar = BTH_AUTO_PAIR;     //ENCIENDO BLUETOOHT
                            appData.state =  SET_BT_PARAMETERS;
                            i=2;
                        }
                        else{
                            i=0;
                            appData.state =  HABILITAR_INFORME_GPS; //sino voy a encender gps
                        }

                        appData.gsm_state = 1; //si paso inicializacion del gprs sin problemas pongo estado gsm=1
                    }
                    break;
         }
         break;
//rutina para obtener el IMEI DEL DISPOSITIVO
         
        case GET_DEVICE_IMEI:  
            switch(i){
               case 0:
                   if(true == WriteString())
                   {   
                       i++;
                   }
                   break;
               case 1:
                   if(appData.Recibido_mensaje == true){       //OBTENER IMEI
                       if(strstr(trama_rx,"\r\n\r\nOK\r\n") != NULL){
                           PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_USART_2_RECEIVE);

                           ptr = strstr(trama_rx,"ATE0");
                           //ptr = strtok(NULL,"GSN");    //data -> IMEI

                           PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_USART_2_RECEIVE);


                           strncpy(DEVICE_IMEI_4THDIGITS,ptr+23,4);   //COJO LOS ULTIMOS 4 NUMEROS DEL IMEI DEL DISPOSITIVO                            
                           //IMEI_INTEGER = strtoull(DEVICE_IMEI,NULL,10);   
                           strncpy(DEVICE_IMEI,ptr+12,15); //Y LA IMEI COMPLETA PARA EL ENVIO POR GPRS
                            /*
                           appData.mensaje_a_enviar = BTH_ON;     //ENCIENDO BLUETOOHT
                           appData.state =  SET_BT_PARAMETERS;
                           //DRV_TMR1_Start(); //para poder apagar el equipo con una pulsacion larga de D
                           i=2;
                           */
                           appData.state =  INICIAR_CONEXION_GPRS;
                           appData.mensaje_a_enviar = HABILITAR_GPRS;
                           i=1;
                           delay_ms(1000);
                       }
                       appData.Recibido_mensaje = false;
                   }
                   break;
            }
            break;
        case SET_BT_PARAMETERS:     // BLUETOOTH CONFIG
            switch(i){
                case 2:
                    if(true == WriteString())
                    {   
                        i++;
                        delay_ms(200);
                        appData.mensaje_a_enviar = BTH_ON;   //CONFIGURO VISIBILIDAD EN ON
                        
                    }
                    break; 
                case 3:
                    if(true == WriteString())
                    {   
                        i++;
                        appData.mensaje_a_enviar = BTH_VIS_ON;  // AUTO EMPAREJAMIENTO CON DISPOSITIVOS BLUETOOTH
                        delay_ms(2000);
                    }
                    break; 
                case 4:
                    if(true == WriteString())
                    {
                        i++;
                            appData.mensaje_a_enviar = BTH_ONE_CON;  // CONFIGURO UNA UNICA CONEXION SPP
                        delay_ms(200);       
                    }
                    break;
                case 5:
                    if(true == WriteString())
                    {  
                        strcpy(trama_tx,BTH_CH_NAME);
                        strcat(trama_tx,DEVICE_IMEI_4THDIGITS);
                        strcat(trama_tx,"\r\n");
                        appData.mensaje_a_enviar = trama_tx;  // CAMBIO EL NOMBRE POR ODOCAP_XXXX
                        delay_ms(3000);
                        i++;    
                    }
                    break;               
                case 6:
                    if(true == WriteString())
                    {   
                        i=0;
                        primera_vez_bt=0; //indico que ya he inicializado el bluetooth
                        buffer_rx_flush();
                        delay_ms(500); 
                        appData.state =  HABILITAR_INFORME_GPS;  //activo informes gps
                        
                    }
                    break; 
            }
            break;   
            
     case DESHABILITAR_INFORME_GPS:    
            appData.mensaje_a_enviar = GPS_REPORT_OFF; // envio el apagado de informes del GPS
            while(WriteString()!=true); 
            appData.mensaje_a_enviar = HABILITAR_GPRS;
            appData.state =  INICIAR_CONEXION_GPRS;  //voy a la rutina de inicializacion del 2G
            delay_ms(250);
            buffer_rx_flush();
            i=1;
            break;
            
        case HABILITAR_INFORME_GPS:
            buffer_rx_flush();
            switch(i){
             case 0:
                    i++;
                    appData.mensaje_a_enviar = GPS_REPORT;
                    while(WriteString()!=true);
                    delay_ms(500);
                    break;
                case 1:
                        //transicion a modo test o modo "normal"
                        if(appData.modo_rutina_test==1){
                            appData.state =  RUTINA_TEST;
                            //activo la salida de pulsos con consigna 4.5kmh
                            DRV_TMR2_Start();
                            DRV_OC_Set_Freq((int)(K_PR_V/4.5));
                        }
                        else{
                            appData.state =  ESPERAR_ACCION_GPS_BT;
                        }
                        appData.Recibido_mensaje_gps == false;
                        appData.Recibido_mensaje_BT = false; 
                        
                        /*
                        
                        flash_spi_handle = DRV_SST25VF064C_Open(DRV_SST25VF064C_INDEX_0,DRV_IO_INTENT_READWRITE |DRV_IO_INTENT_NONBLOCKING);
                        for(i=0; i<4096; i++)
                        {
                            sourceBuffer[i]= 2*i*i+7;
                        }
                         */ 
                        DRV_SST25VF064C_GeometryGet(flash_spi_handle);
                        // set the event handler                            
                        DRV_SST25VF064C_BlockEventHandlerSet(flash_spi_handle,APP_SST25VF020BEventHandler,1);
                                
                        i=0;
                        // Clear the WDT timer  
                        SYS_WDT_TimerClear ();
                        //PLIB_PORTS_PinWrite (PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_9,1);
                        DRV_TMR1_Start();  //enciendo el timer 2 que se encarga de manejar el led rojo de la placa
                         //Enable WDT
                        //SYS_WDT_Enable ( false );
                       
                    //}
                        
                    break;
            }
            break;

        case ESPERAR_ACCION_GPS_BT:
          
           if(appData.Recibido_mensaje_gps == true){
                
                // Clear the WDT timer  
                //SYS_WDT_TimerClear ();   //limpio el wdt cada 1 s tiene timeout de 2048 ms
                
               //if(real_data==0){
                 appData.tick_scaler_sirena=0;   //limpio la salvaguarda cada vez que llega un mensaje del gps
               //}
                
                i = 0;
                appData.Recibido_mensaje_gps = false; 
                
                if(strstr(trama_rx,"+UGNSINF: 1,1,") != NULL){     
                    ptr_cmd=strstr(trama_rx,"+UGNSINF: 1,1,");
                    strncpy(trama_test,(ptr_cmd+14),56); //copio la informacion relevante del gps
                    cmd_flush();
                    
                    appData.state = PROCESAR_DATOS_RX;
                    appData.gps_state=1; // hay fix del gps
                    //appData.datos_gps_disponible = 1;
                    real_data=1;
                   // next_data=0;
                    //appData.tick_scaler_sirena=0;   //limpio la salvaguarda 
                }
                else if(strstr(trama_rx,"+UGNSINF: 1,0,") != NULL && appData.bt_state==1){
                    
                    ptr_cmd=strstr(trama_rx,"+UGNSINF: 1,0,");
                    strncpy(trama_test,(ptr_cmd+14),56); //copio la informacion relevante del gps
                    cmd_flush();
                    
                    i=7;
                    appData.state = PROCESAR_DATOS_RX;
                    //appData.gps_state=1; // hay fix del gps
                    //appData.datos_gps_disponible = 1;
                    real_data=1;
                   // next_data=0;
                    //appData.tick_scaler_sirena=0;   //limpio la salvaguarda 
                }else{
                    //real_data=0;
                    //bloque añadido para ensayar gps + envio datos web 
                    if(real_data==0){
                     
                        //real_data = 0;
                        appData.state = PROCESAR_DATOS_RX;
                        parado=0;
                        appData.gps_state=1;
                        i = 0;
                    }    
                    else{
                        appData.gps_state=0;
                    }
                    
                     
                }  
                
           }
           //ejecuto las acciones de los comandos bluetooth y borro el buffer  
            if(appData.Recibido_mensaje_BT == true){
                    appData.Recibido_mensaje_BT = false; 
                //si se desconecta inintencionadamente pongo estado bt a desconectado
                if(strstr(trama_rx,"+BTDISCONN:") != NULL){  
                    appData.bt_state=0; //cambio estatus bt a desconectado 
                    PLIB_PORTS_PinWrite (PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_8,0);
                    ptr_cmd=strstr(trama_rx,"+BTDISCONN:"); //search&destroy 
                    cmd_flush();
                }
                //peticion de conexion
                if(strstr(trama_rx,"+BTCONNECTING:") != NULL){  
                    appData.mensaje_a_enviar = BTH_SSP;  
                    while(WriteString()!=true);
                    ptr_cmd=strstr(trama_rx,"+BTCONNECTING:"); //search&destroy 
                    cmd_flush();
                }    
                //mensaje estoy conectado
                if(strstr(trama_rx,"+BTCONNECT:") != NULL){  
                    appData.bt_state=1;    
                    PLIB_PORTS_PinWrite (PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_8,1);
                    ptr_cmd=strstr(trama_rx,"+BTCONNECT:"); //search&destroy 
                    cmd_flush();
                } 
                if(strstr(trama_rx,"BTREP=") != NULL){  
                    ptr_cmd=strstr(trama_rx,"BTREP="); //search&destroy 
                    appData.frecuencia_envio = atoi(ptr_cmd+6);  //cambio la frecuencia de envio de mensajes bluetooth y gprs
                    aux_frec_envio=appData.frecuencia_envio;
                    cmd_flush();
                }
                if(strstr(trama_rx,"INFO") != NULL){  
                    ptr_cmd=strstr(trama_rx,"INFO"); //search&destroy 
                    preparar_payload_envio();//preparo el batiburrillo de variables del tronic + gps  
                    if(appData.gps_state){  //si el gps tiene fix preparo los datos 
                        bluetooth_send(payload); //envio el mensaje via bluetooth
                    }
                    cmd_flush();
                }
                if(strstr(trama_rx,"REAL=") != NULL){  
                    ptr_cmd=strstr(trama_rx,"REAL="); //search&destroy 

                    real_data=atoi((ptr_cmd+5)); 
                    cmd_flush();
                } 
                    /*
                if(strstr(trama_rx,"PWM=") != NULL){  
                    ptr_cmd=strstr(trama_rx,"PWM="); //search&destroy 
                    
                    vel_test=atof((ptr_cmd+4)); //obtengo el PR
                    cmd_flush();
                } 
                    */
                    
                /*    
                if(strstr(trama_rx,"LED_ROJO=OFF") != NULL){  
                    PLIB_PORTS_PinWrite (PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_9,0);
                    DRV_TMR1_Stop(); //paro el timer3 
                    ptr_cmd=strstr(trama_rx,"LED_ROJO=OFF"); //search&destroy 
                    cmd_flush();
                }
                if(strstr(trama_rx,"LED_ROJO=ON") != NULL){  
                    PLIB_PORTS_PinWrite (PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_9,1);
                    ptr_cmd=strstr(trama_rx,"LED_ROJO=ON"); //search&destroy 
                    cmd_flush();
                } 
                if(strstr(trama_rx,"LED_BLINK=") != NULL){  
                    DRV_TMR1_Start();
                    ptr_cmd=strstr(trama_rx,"LED_BLINK="); //search&destroy 
                    blink_rate=atoi((ptr_cmd+10));
                    cmd_flush();
                } 
                if(strstr(trama_rx,"HIW") != NULL){  
                             
                    appData.mensaje_a_enviar2 =hola_mundo;
                    while(WriteString_2()!=true);
                    //DRV_TMR1_Start();
                    ptr_cmd=strstr(trama_rx,"HIW"); //search&destroy 
                    cmd_flush();
                }
                if(strstr(trama_rx,"PWM=") != NULL){  
                    ptr_cmd=strstr(trama_rx,"PWM="); //search&destroy 
                    DRV_OC_Set_Freq(atoi((ptr_cmd+4)));
                    cmd_flush();
                } 
                if(strstr(trama_rx,"READ") != NULL){ 
                    // 3rd block API to read the 2k byte of data (most of which was written above) from flash 
                        DRV_SST25VF064C_BlockRead
                        (
                            flash_spi_handle,
                            &commandHandle3,
                            targetBuffer,
                            0x00001000,
                            2048
                        );
                        ptr_cmd=strstr(trama_rx,"READ"); //search&destroy 
                        cmd_flush();
                }
                if(strstr(trama_rx,"WRITE") != NULL){  
                       // 2nd block API to write 4k locations in the flash from address 0x00002500 
                        DRV_SST25VF064C_BlockWrite
                        (
                            flash_spi_handle,
                            &commandHandle2,
                            &sourceBuffer[0],
                            0x00001000,
                            4096
                        );
                        ptr_cmd=strstr(trama_rx,"WRITE"); //search&destroy 
                        cmd_flush();
                }   
                if(strstr(trama_rx,"ERASE") != NULL){  

                    DRV_SST25VF064C_BlockErase(  flash_spi_handle, &commandHandle1,  0,  4); 
                    ptr_cmd=strstr(trama_rx,"ERASE"); //search&destroy 
                    cmd_flush();
                }   
                    */
                //aqui hay que ir implementando los comandos bluetooth del equipo no me deja hacer mas de 7 if else if :(
              
            }
           //rutina de recoleccion de datos escuchados de la potencia tronic
            if(appData.Recibido_mensaje_potencia == true){
                appData.Recibido_mensaje_potencia = false;
                //1º obtener la pantalla
                //2º set del nro de parametros para esa pantalla
                //3º guardar todos los datos
                procesar_mensaje_rx_potencia();
                //esta rutina de abajo manda los datos de la potencia tronic al movil via bluetooth
                /*
                strcpy(trama_tx,"\0");
                 itoa(trama_tx,appData.nro_pantalla,10);
                 strcat(trama_tx,":");
                 for(i=0;i<appData.nro_parametros;i++){
                    strcat(trama_tx,appData.parameter_array[i]);
                    strcat(trama_tx,",");
                 }
                 strcat(trama_tx,"\r\n");
                 
                 bluetooth_send(trama_tx);
                 */ 
            }    
            //*****************************************************************
           //definir comportamiento del led rojo en funcion del estado de aparato
           
           if(appData.gps_state==0){
               if(appData.gsm_state==0){
                    appData.parpadeos_led_rojo=5;  
               }
               else{
                   appData.parpadeos_led_rojo=4; 
               }
           }
           else{
               if(appData.gsm_state==0){
                    appData.parpadeos_led_rojo=3;  
               }
               else{
                   appData.parpadeos_led_rojo=2; 
               }
           }            
            break;
        case PROCESAR_DATOS_RX:
            if(real_data==0){
                strcpy(trama_test,"20181108084203.000,39.186738,-0.507105,98.000,1.39,304.7,1,,6.2,6.2,1.0,,7,3,,,31,,");   
            }
            switch(i){
                case 0:
                    datos_gps= trama_test; //empiezo al principio de trama test
                    i=2;
                    break;
                case 1:
                    //datos_gps = strtok(datos_gps,","); //salto los dos datos inutiles de fix y gps on
                    //datos_gps = strtok(NULL,",");
                    //i++;
                    break;
                case 2:
                    datos_gps = strtok(datos_gps,",");  //puntero a la fecha y hora
                    
                    strcpy(FECHAYHORA,datos_gps);
                    
                    year[0]=FECHAYHORA[0];
                    year[1]=FECHAYHORA[1];
                    year[2]=FECHAYHORA[2];
                    year[3]=FECHAYHORA[3];

                    mes[0] = FECHAYHORA[4];
                    mes[1] = FECHAYHORA[5];

                    dia[0] = FECHAYHORA[6];
                    dia[1] = FECHAYHORA[7];

                    hora[0] = FECHAYHORA[8];
                    hora[1] = FECHAYHORA[9];

                    minuto[0] = FECHAYHORA[10];
                    minuto[1] = FECHAYHORA[11];

                    segundo[0] = FECHAYHORA[12];
                    segundo[1] = FECHAYHORA[13];

                    tm_str.tm_year=atoi(year)-1900;
                    tm_str.tm_mon=atoi(mes)-1;
                    tm_str.tm_mday=atoi(dia);
                    tm_str.tm_hour=atoi(hora);
                    tm_str.tm_min=atoi(minuto);
                    tm_str.tm_sec=atoi(segundo);
                    tm_str.tm_isdst=0;  
                    ret = mktime(&tm_str);
                   // if(ret!=-1){
                        itoa(time_stamp,ret,10);
                   // }else{
                     //   time_stamp[0]="n";
                     //   time_stamp[1]= "u";
                     //   time_stamp[2]= "l";
                     //   time_stamp[3]= "l";
                    //    time_stamp[4]= "\0";
                   // }
                    i++;
                    break;
                case 3:
                    datos_gps = strtok(NULL,",");
                    strcpy(DATOS_LAT,datos_gps);   //GUARDAR EN UN STRING LATITUD          
                    strcpy(lat,datos_gps);
                    d_lat=atof(lat);
                    
                    i++;
                    break;
                case 4:
                    datos_gps = strtok(NULL,",");
                    strcpy(DATOS_LON,datos_gps);  //GUARDAR EN UN STRING LONGITUD
                    strcpy(lon,datos_gps);
                    d_long=atof(lon);
                    
                    i++;
                    break;
                case 5:
                    datos_gps = strtok(NULL,",");
                    i++;
                    break;
                case 6:
                    datos_gps = strtok(NULL,",");  
                    
                    strcpy(DATOS_VEL,datos_gps); //string con la velocidad en float																								  
                    
                    vel_gps = atof(DATOS_VEL);  //version de la velocidad instantanea en coma flotante
                    //vel_gps = vel_test;
                    /*
                    datos_gps = strtok(NULL,",");                 
                    strcpy(DATOS_RUM,datos_gps); //string con el rumbo que marca el gps en degrees
                    rumbo_float = atof(DATOS_RUM);  //rumbo en formato coma flotante para trabajar
                    */
                    
                    //corrijo el valor de la salida de pulsos en funcion de la velocidad que da el gps
                    if(vel_gps<0.4){
                        //DRV_OC0_Disable();  //apago la salida de pulsos
                        DRV_TMR2_Stop(); 
                    }
                    else{
                        if(vel_gps>25){  //saturo a 25kmh los pulsos
                            DRV_OC_Set_Freq(1115);
                            //DRV_OC0_Enable();
                            DRV_TMR2_Start();
                        }
                        else{
                            aux_cal_pulsos = K_PR_V/vel_gps;
                            if(aux_cal_pulsos>65535){
                                aux_salida_pulsos=65535;
                            }
                            else{
                                aux_salida_pulsos = (int) aux_cal_pulsos;
                            }
                            //DRV_OC0_Enable();
                            DRV_TMR2_Start();
                            DRV_OC_Set_Freq(aux_salida_pulsos);
                        }
                    }
                    
                    //envio por bluetooth y gprs cada "frecuencia_envio" segundos
                    if(appData.frecuencia_envio>0){  //si esta habilitado el envio automatico
                        aux_frec_envio--;
                        if(aux_frec_envio==0){
                            i=7;
                            aux_frec_envio= appData.frecuencia_envio;
                            //envio_bt_escalonado=2;  //para mandar el mensaje bluetooth al segundo siguiente
                        }
                        else{
                            i=10;
                        }
                    }
                    else{
                        i=10;
                    }
                   break;
                case 7:  
                    //envio automatico via bluetooth
                    //i=10;
                    
                    preparar_payload_envio();//preparo el batiburrillo de variables del tronic + gps 
                    
                    //if(appData.gps_state){  //si el gps tiene fix preparo los datos 
                        bluetooth_send(payload); //envio el mensaje via bluetooth
                        delay_ms(100);
                    //}
                    
                        preparar_trama_update_tracker(); //preparo la URL
                        appData.mensaje_a_enviar = trama_registro;
                        i++;
                    
                    /*if(appData.gsm_state==1){
                        if(appData.gps_state){  //si el gps tiene fix preparo los datos 
                            preparar_trama_update_tracker(); //preparo la URL
                            appData.mensaje_a_enviar = trama_registro;
                            i++;
                        }
                        else{
                            i=10; //sino hay gps o gsm no envio los datos por gsm
                        }
                        
                    }
                    else{
                        i=10;
                    } 
                    */
                     
                    //i=10;
                    break;
                case 8:  
                    if(true == WriteString()){
                        delay_ms(100);
                        appData.mensaje_a_enviar = GPRS_HTTP_ACTION;   
                        i++;    
                    }
                    break;
                case 9:  
                    if(true == WriteString())
                    {   
                        i++;
                        delay_ms(100); 
                    }
                    break;
                case 10:
                    i=0;
                    /*
                    if(envio_bt_escalonado==1){
                        preparar_payload_envio();//preparo el batiburrillo de variables del tronic + gps 

                        if(appData.gps_state){  //si el gps tiene fix preparo los datos 
                            bluetooth_send(payload); //envio el mensaje via bluetooth
                        } 
                        envio_bt_escalonado=0; //no mandar hasta la siguiente vez
                    }
                    if(envio_bt_escalonado>0){
                        envio_bt_escalonado--;
                    }
                     */ 
                    
                    appData.state = ESPERAR_ACCION_GPS_BT;
                break;
            }
   
            break;
        case RUTINA_TEST:
            
            //aqui hacer uno por uno los pasos del test en bucle
            //gestion conexion bluetooth
            if(appData.Recibido_mensaje_BT == true){
                appData.Recibido_mensaje_BT = false; 
                //peticion de conexion
                if(strstr(trama_rx,"+BTCONNECTING:") != NULL){  
                    appData.mensaje_a_enviar = BTH_SSP;  
                    while(WriteString()!=true);
                    ptr_cmd=strstr(trama_rx,"+BTCONNECTING:"); //search&destroy 
                    cmd_flush();
                }    
                //mensaje estoy conectado
                if(strstr(trama_rx,"+BTCONNECT:") != NULL){ 
                    if(strstr(trama_rx,"SPP") != NULL){
                        appData.bt_state=1;    
                        PLIB_PORTS_PinWrite (PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_8,1); //enciendo led azul
                        ptr_cmd=strstr(trama_rx,"+BTCONNECT:"); //search&destroy 
                        cmd_flush();
                    }
                }
            }
            if(appData.bt_state==1){
                
                bluetooth_send(INICIO_TEST);
                delay_ms(200);
                
                test_ok=1; //inicializo test_ok=1;
                //test_eeprom(); de momento lo comento que no funciona y no estoy gastando la memoria 
                test_gprs();
                test_uart();
                test_pulsos_rueda();
                test_gps_fix();
                
                if( test_ok == 1){ 
                //paso test completo
                  bluetooth_send(TEST_TODO_OK); //envio el mensaje via bluetooth
                }
                else{
                  //no paso test, alguna prueba del test falla
                  bluetooth_send(TEST_FALLIDO);
                } 
                delay_ms(5000);
            }
                break;
        /*    //los parametros del equipo los grabare seguramente en la flash externa
        case GRABAR_DATOS_EN_FLASH:

            switch(i)
            {
                case 0:

                    SYS_INT_Disable();
                    PLIB_USART_Disable(USART_ID_2);  //deshabilito la UART
                    
                    // Erase the page which consist of the row to be written 
                    NVMpageErase(PROGRAM_FLASH_BASE_ADDRESS);
                    i++;
                    break;
                    
                case 1:
                    if(PLIB_NVM_FlashWriteCycleHasCompleted(NVM_ID_0)){
                        // Disable flash write/erase operations
                        PLIB_NVM_MemoryModifyInhibit(NVM_ID_0);
                        i++;
                    }
                    break;

                case 2:
                    if(PLIB_NVM_LowVoltageIsDetected(NVM_ID_0) || PLIB_NVM_WriteOperationHasTerminated(NVM_ID_0)){
                        // Write Failed  

                        DRV_USART0_Initialize();
                        SYS_INT_Enable ();
                        i=0;
                        guardar_en_flash=0;
                        
                        if(primera_escritura_flash==1){
                            //appData.state = SIM808_TURN_ON; //continuo programa
                            //primera_escritura_flash=0;
                            SYS_RESET_SoftwareReset();
                        }
                        else{
                            appData.state = ESPERAR_ACCION_GPS_BT;   
                        }
                        
                                                                          // MEJORA! definir que hacer si hay un fallo
                    }
                    else{
                        // Erase Success 
                        // Write a row of data to PROGRAM_FLASH_BASE_ADDRESS, using databuff array as the source 
                        NVMwriteRow(PROGRAM_FLASH_BASE_ADDRESS, (uint32_t)DATOS_FLASH);   //comento para ver si se borran al menos los datos flash
                        i++;
                    }
                    break;

                case 3:
                    if(PLIB_NVM_FlashWriteCycleHasCompleted(NVM_ID_0)){
                        // Disable flash write/erase operations
                        PLIB_NVM_MemoryModifyInhibit(NVM_ID_0);
                        i++;
                    }
                    break;

                case 4:
                    if(!(PLIB_NVM_LowVoltageIsDetected(NVM_ID_0) || PLIB_NVM_WriteOperationHasTerminated(NVM_ID_0))){            
                        // Write Success 
                        
                        DRV_USART0_Initialize();
                        SYS_INT_Enable ();
                        i=0;
                        guardar_en_flash=0;

                        if(primera_escritura_flash==1){
                            //appData.state = SIM808_TURN_ON; //continuo programa
                            //primera_escritura_flash=0;
                            SYS_RESET_SoftwareReset();
                        }
                        else{
                            appData.state = ESPERAR_ACCION_GPS_BT;   
                        }
                        
                    }
                    else{
 
                        DRV_USART0_Initialize();
                        SYS_INT_Enable ();
                        i=0;
                        guardar_en_flash=0;
                                                                                    // MEJORA! definir que hacer si hay un fallo
                        if(primera_escritura_flash==1){
                            //appData.state = SIM808_TURN_ON; //continuo programa
                            //primera_escritura_flash=0;
                            SYS_RESET_SoftwareReset();
                        }
                        else{
                            appData.state = ESPERAR_ACCION_GPS_BT;   
                        }
                    }
                    break;
            }
            break; 
            */
        default:
            //APP SHOULDN'T GET HERE!!!
            //SYS_DEBUG (SYS_ERROR_FATAL,"ERROR! Invalid state\r\n");
            while(1);
    }
}


/*******************************************************************************
 End of File
 */
 