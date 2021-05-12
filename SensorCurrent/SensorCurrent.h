/*

  Some parameters that can be tuned 

 */


//
// The MQTT server 
//
#define MQTT_SERVER    "musi-broker.ladaria.eu" // really 192.168.100.33
#define MQTT_PORT      8883 
//
// The topic to publish on
//
#define MQTT_TOPIC     "metering/current" 
//
// The sampling period (ms)
//
#define PERIOD         15000
//
// Max payload lenght 
//
#define MAX_PAYLEN     50



/*

  The secrets are here, please don't tell anyone ... ;-)

 */


#define OTA_PASS       "otapass" 

#define WIFI_SSID      "CASBASCOS3TPL" 
#define WIFI_PASS      "miradordemontepinar3tpl" 
