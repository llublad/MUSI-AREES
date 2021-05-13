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
#define PERIOD         15*1000
//
// Max payload lenght 
//
#define MAX_PAYLEN     100
//
// Pin for channel 1 and 2 (we have 2 sensors)
//
#define PIN_CH1        2  // A2
#define PIN_CH2        7  // A7
//
// Calibration values for every sensor/channel
//
#define CAL_CH1        41.50
#define CAL_CH2        17.60
//
// How many samples must take?
//
// On Arduino 33 IoT Nano 1144 samples ~ 1 second = 50 cycles (at 50Hz)
//
#define IRMS_SAMPLES   1143 
//
// Influxdb template
//
#define INFLUX_TEMPLATE "metering,sn=%s,magnitude=current,units=A,channel=%d value=%3.3f"


/*

  The secrets are here, please don't tell anyone ... ;-)

 */


#define OTA_PASS       "otapass" 

#define WIFI_SSID      "CASBASCOS3TPL" 
#define WIFI_PASS      "miradordemontepinar3tpl" 
