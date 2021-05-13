/*
  
  SensorCurrent.ino 

  Pràctica 2 de l'assignatura 
  Àrees d'aplicació d'IoT 
  Alumne: Lluís Bernat Ladaria 
  Professor: Dr. Bartomeu Alorda 


  Description
  ----------- 

  Electrical intensity sensor. 
  
  We read the rms current values with two transducers.
  
  Channel A: SCT1013  30A/1V   transducer
  Channel B: SCT1013 100A/50mA transducer
  

  MQTT topics 
  -----------
  (defined at some .h file, look for it a few lines below)
  
  topic: metering/current


 */

/*
 
  Needed third parties libs

  Please install ... 
  
  WiFiNINA (>=1.8.8)
  ArduinoOTA (>=1.0.6)
  PubSubClient (>=2.8.0)
  BearSSLClient (>=1.7.0)
  ArduinoECCX08 (>=1.3.5)
  Emonlib (>=1.1.0)
  
 */

// We need WiFi
#include <WiFiNINA.h>
// Also MQTT
#include <PubSubClient.h>
// We want client and server (mutual) autentication
#include <ArduinoBearSSL.h>
// And a good quality hardware generated random numbers
// for MQTT client id
#include <ArduinoECCX08.h>
//
// Finally the current sensing lib
//
#include "EmonLib.h"

// And (optionally) will use OTA updates
// Use with caution, cause lack of security !!!
//
//#define OTA_UPDATES
//
#ifdef OTA_UPDATES
#include <ArduinoOTA.h>
#endif

// The server Certificate Authority
// This file has been (re)generated
// using pycert_bearssl tool from CA cert
#include "certs/musi-ca.h"

// Our public cert and
// AtmelECC608 slot number that
// has the private (and secret) key
#include "certs/sensor-cert.h"

/*
 
  The parameters you are looking for 
  
 */
#include "SensorCurrent.h"

/* 

  Global types
  
 */

//
// The channels to sense
//
typedef enum 
{
  CHAN1,
  CHAN2
} aChan;

//
// The error codes
//
#define MAX_SIGTXTLEN 5 // max message error 'morse' length
#define LED_FLASH 100   // time slice unit (ms)
#define LED_DOT 4       // slice units for dot
#define LED_DASH 10     // slice units for dash
#define LED_SPACE 2     // slice units intersymbol
//
typedef enum
{
  ERR_OK,
  ERR_NO_ECCX,
  ERR_ECCX_UNLOCKED,
  ERR_WIFI_HW,
  ERR_WIFI_CREDENTIALS,
  ERR_WIFI_GET_TIME,
  ERR_MQTT
} errCode;
//
// The error cadency table
//
struct morseError
{
  errCode code;
  char message[MAX_SIGTXTLEN + 1];
  uint8_t lastDelay; // number of FLASH units minus 1, to delay at last
  bool forever;      // halt processing and signal error forever ?
};

struct morseError errTab[] = {
    {ERR_OK,               ".-...", 9, false}, // once
    {ERR_NO_ECCX,          ".-.-.", 19, true}, // forever (also halt program)
    {ERR_ECCX_UNLOCKED,    ".-.--", 19, true}, // forever (also halt program)
    {ERR_WIFI_HW,          ".--..", 19, true}, // forever (also halt program)
    {ERR_WIFI_CREDENTIALS, ".--.-", 9, false}, // once
    {ERR_WIFI_GET_TIME,    ".---.", 9, false}, // once
    {ERR_MQTT,             ".-.--", 9, false}  // once
};

/*
  
  Global vars
    
 */

//
// ECCX08 Serial Number (guaranteed to be unique)
//
#define MAX_SNLEN 18
char serialNumber[MAX_SNLEN+1];
//
// MQTT parameters (defined in .h)
//
const char *mqttServer = MQTT_SERVER;
uint mqttPort = MQTT_PORT;
//
char mqttTopic[] = MQTT_TOPIC;
//
// Metering period
//
long periodMetering = PERIOD;
//
// Last milis() we have made a measurement
//
long lastInstant = 0;
//
// Instantiate de WiFi Client
//
WiFiClient wifiClient;
//
// And encapsulate it into an SSLClient
//
BearSSLClient wifiClientSSL(wifiClient, TAs, (size_t)TAs_NUM);
//
// Also instantiate MQTT client (publisher)
//
PubSubClient client(mqttServer, mqttPort, wifiClientSSL); // No callback, publisher role only
//
// Instantiate 2 sensing channels 
//
EnergyMonitor emon1;
EnergyMonitor emon2;

/*

  Funcions
  
 */

void signal_error(errCode whatError)
{
  //
  // Let the LED flash a little
  // to signal to the user the type of error
  //
  const uint n = sizeof(errTab) / sizeof(morseError);
  bool forever;
  uint i = 0;
  uint8_t j;

  while (errTab[i].code != whatError and i < (n - 1))
    i++;

  if (errTab[i].code == whatError)
  {
    do
    {
      j = 0;
      while ((j < MAX_SIGTXTLEN) and errTab[i].message[j] != 0)
      {
        switch (errTab[i].message[j]) 
        {
          case '-': // dash
            digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
            delay(LED_DASH * LED_FLASH);
            digitalWrite(LED_BUILTIN, LOW); // turn the LED off
            delay(LED_SPACE * LED_FLASH);
            break;
          case '.': //dot
            digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
            delay(LED_DOT * LED_FLASH);
            digitalWrite(LED_BUILTIN, LOW); // turn the LED off
            delay(LED_SPACE * LED_FLASH);
            break;
        }
        j++;
      }
      delay(errTab[i].lastDelay * LED_FLASH);
    } while (errTab[i].forever); // if fatal error, signal forever
  }
}

void setup_wifi()
{
  //
  // Take care of WiFi setup
  //

  // Create and initialize local needed vars
  //
  char ssid[] = WIFI_SSID; // in secrets file !
  char pass[] = WIFI_PASS; // the same !
  int status = WL_IDLE_STATUS;

  delay(1000); // wait a little bit to assure readiness

  // check for the presence of the shield/hardware
  //
  if (WiFi.status() == WL_NO_SHIELD)
  {
    Serial.println("WiFi shield not present or card w/o WiFi");
    signal_error(ERR_WIFI_HW); // signal a permanent error
  }

  // Try to connect
  //
  while (status != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network.
    status = WiFi.begin(ssid, pass);
  }
}

void printWifiStatus()
{
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void printResolvedName(const char *hostname)
{
  IPAddress result;
  int err = WiFi.hostByName(hostname, result);

  Serial.print("Resolving: ");
  Serial.println(hostname);
  if (err == 1)
  {
    Serial.print("  The IP address is: ");
    Serial.println(result);
  }
  else
  {
    Serial.print("  Error code: ");
    Serial.println(err);
  }
}

void setup_mTLS()
{
  //
  // Tell BearSSL where is the private key
  // and give it the public cert signed by
  // our CA
  //
  wifiClientSSL.setEccSlot(MY_KEY_SLOT, MY_CERT);
}

unsigned long getTime()
{
  //
  // Get current time, callback
  //
  // will be called from ArduinoBearSSLClass::getTime()
  //
  return WiFi.getTime();
}

void setup_eccx08()
{
  //
  // Start comm with ECCX08 chip
  //
  if (!ECCX08.begin())
  {
    Serial.println("Failed to communicate with ECC508/ECC608!");
    signal_error(ERR_NO_ECCX);
  }

  if (!ECCX08.locked())
  {
    Serial.println("The ECC508/ECC608 is not locked! See ECCX08CSR.ino example");
    signal_error(ERR_ECCX_UNLOCKED);
  }

  ECCX08.serialNumber().toCharArray(serialNumber, MAX_SNLEN);
}

bool reconnect()
{
  bool isConnected;

  Serial.print("Attempting MQTT connection...");
  // Create a random client ID
  String clientId = "ArduinoClient-";
  clientId += String(ECCX08.random(0xffff), HEX);
  // Attempt to connect
  isConnected = client.connect(clientId.c_str());
  if (isConnected)
  {
    Serial.println("connected");
  }
  else
  {
    Serial.print("failed, rc=");
    Serial.print(client.state());
    Serial.print(" / SSL Client error code=");
    Serial.println(wifiClientSSL.errorCode());
    Serial.println("Will try again in a few seconds");
    signal_error(ERR_MQTT); // signal error and return
    delay(PERIOD / 2 + 1);  // retry in a half metering period
  }

  return isConnected;
}

void setup_emon() {
  //
  // Sensor at channel 1
  //
  emon1.current(PIN_CH1, CAL_CH1); // Current: input pin, calibration.
  //
  // Sensor at channel 2
  //
  emon2.current(PIN_CH2, CAL_CH2); // Current: input pin, calibration.
}

/*

  Program initialization
 
 */

void setup()
{
  //
  // We will use de builtin led to signal errors or progress
  //
  pinMode(LED_BUILTIN, OUTPUT);
  //
  // Initialize serial
  //
  Serial.begin(9600);
  //
  // Random source
  // and test eccx08 config
  //
  setup_eccx08();
  //
  // WiFi init
  //
  setup_wifi();
  //
  // Show wifi config
  //
  printWifiStatus();
  //
  // Also display server IP for debugging purposes
  //
  printResolvedName(mqttServer);

#ifdef OTA_UPDATES
  //
  // Allow OTA updates
  //
  // start the WiFi OTA library with internal (flash) based storage
  //
  ArduinoOTA.begin(WiFi.localIP(), "Arduino", OTA_PASS, InternalStorage);
#endif

  //
  // Get current time for correct certs validation
  //
  Serial.print("Testing get current time function ");
  while (getTime() == 0)
  {
    Serial.print(".");
    signal_error(ERR_WIFI_GET_TIME);
  }

  ArduinoBearSSL.onGetTime(getTime);

  Serial.print(" ");
  Serial.print(getTime());
  Serial.println(" seconds since 1/1/1970");

  setup_mTLS();

  setup_emon();  // setup inputs and calibrations
}

void readCurrentInflux(aChan whatChan, char* pLoad, int8_t pLen){
  //
  // Read a channel 
  // and format result to influxdb format
  //
  double Irms = 0.0;
  String payLoad = "";

  switch (whatChan)
  {
  case CHAN1:
    Irms = emon1.calcIrms(IRMS_SAMPLES);
    sprintf(pLoad, INFLUX_TEMPLATE, serialNumber, 1, Irms);
    break;
  case CHAN2:
    Irms = emon2.calcIrms(IRMS_SAMPLES);
    sprintf(pLoad, INFLUX_TEMPLATE, serialNumber, 2, Irms);
    break;
  }
}

/*

 Main loop
 
 */

void loop()
{
  //
  // local working vars
  //
  long now;
  //
  bool isConnected = false;
  //
  char payLoad[MAX_PAYLEN];
  // 
  long t1, t2; // test cycle reading duration 

#ifdef OTA_UPDATES
  //
  // check for WiFi OTA updates
  //
  ArduinoOTA.poll();
#endif

  //
  // Verify and mantain MQTT connection,
  // otherwise reconnect
  //
  isConnected = client.loop(); // keeps alive and tell us if reconnection is needed
  if (!isConnected) isConnected = reconnect();

  now = millis();
  //
  // if it's metering time and have connection,
  // then sample and send
  //
  if ((now - lastInstant >= periodMetering) and isConnected)
  {
    lastInstant = now;

    digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)

    Serial.print("Sampling ch1 ");
    t1 = millis(); // to calc sampling time
    readCurrentInflux(CHAN1, payLoad, MAX_PAYLEN);
    t2 = millis(); // to calc sampling time

    Serial.print("publishing: ");
    Serial.println(payLoad);
    client.publish(mqttTopic, payLoad);

    Serial.print("Sampling ch2 ");
    readCurrentInflux(CHAN2, payLoad, MAX_PAYLEN);

    Serial.print("publishing: ");
    Serial.print(payLoad);
    client.publish(mqttTopic, payLoad);


    //
    // force encryption and transmission,
    // cause SSL lib optimization
    //
    client.flush();

    Serial.println(" published");

    Serial.print("IRMS sampling time (ms):");
    Serial.println(t2 - t1);

    digitalWrite(LED_BUILTIN, LOW); // turn the LED off by making the voltage LOW
  }
  delay(10); // do nothing for a while
}
