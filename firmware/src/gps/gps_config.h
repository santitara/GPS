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


/*********public struct***********************************************************/


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