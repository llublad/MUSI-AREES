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
#define PERIOD         10*1000
//
// The sampling duration (ms)
//
// For better accuracy, 
// must be a multiple of the (sinus) cycle period
//
// At 50 Hz: 1000 ms -> 50 cycles
//            500 ms -> 25 cycles
//            280 ms -> 14 cycles
// At 50 Hz: 1000 ms -> 60 cycles
//            500 ms -> 30 cycles
//            233 ms -> 14 cycles
//
#define DURATION       500
//
// Max payload lenght 
//
#define MAX_PAYLEN     100
//
// Influxdb template
//
#define INFLUX_TEMPLATE "metering,sn=%s,magnitude=current,units=A,channel=%d value=%3.3f"
//
// Pin for channel 1 and 2 (we have 2 sensors)
//
#define PIN_CH1        PIN_A2  // A2
#define PIN_CH2        PIN_A7  // A7
//
// Calibration values for every sensor/channel
//
#define CAL_CH1        120.80
#define CAL_CH2        25.20
