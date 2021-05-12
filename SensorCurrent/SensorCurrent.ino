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
// The error codes
//
#define MAX_SIGTXTLEN 7 // max message error 'morse' length
#define LED_FLASH 100   // time slice unit (ms)
#define LED_DOT 2       // slice units for dot
#define LED_DASH 5      // slice units for dash
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
    {ERR_OK,               "-.-....", 9, false}, // once
    {ERR_NO_ECCX,          "-.-..-.", 19, true}, // forever (halt program)
    {ERR_ECCX_UNLOCKED,    "-.-..--", 19, true}, // forever
    {ERR_WIFI_HW,          "-.-.-..", 19, true}, // forever
    {ERR_WIFI_CREDENTIALS, "-.-.-.-", 9, false}, // once
    {ERR_WIFI_GET_TIME,    "-.-.--.", 9, false}, // once
    {ERR_MQTT,             "-.-..--", 9, false}  // once
};

/*
  
  Global vars
    
 */

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
            delay(LED_FLASH);
            break;
          case '.': //dot
            digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
            delay(LED_DOT * LED_FLASH);
            digitalWrite(LED_BUILTIN, LOW); // turn the LED off
            delay(LED_FLASH);
            break;
        }
        j++;
      }
      delay(errTab[i].lastDelay * LED_FLASH);
    } while (errTab[i].forever); // if fatal error, signal forever
  }
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
    delay(3000);
  }

  return isConnected;
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
  // Get current time callback
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
    Serial.println("The ECC508/ECC608 is not locked!");
    signal_error(ERR_ECCX_UNLOCKED);
  }
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

//
// Allow OTA updates
//
// start the WiFi OTA library with internal (flash) based storage
//
#ifdef OTA_UPDATES
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
  Serial.println(" seconds from 1/1/1970");

  setup_mTLS();
}

/*

 Main loop
 
 */

void loop()
{
  //
  // local working vars
  //
  bool isConnected = false;
  //
  char payLoad[MAX_PAYLEN];

//
// check for WiFi OTA updates
//
#ifdef OTA_UPDATES
  ArduinoOTA.poll();
#endif

  //
  // Verify and mantain MQTT connection,
  // otherwise reconnect
  //
  if (!client.connected())
    isConnected = reconnect();
  else
    client.loop(); // mantain MQTT connection

  long now = millis();
  //
  // if its metering time and have connection,
  // then sample and send
  //
  if ((now - lastInstant >= periodMetering) and isConnected)
  {
    lastInstant = now;

    Serial.print("Sampling ... ");
    digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)

    String lecture1 = String(ECCX08.random(1000));

    lecture1.toCharArray(payLoad, MAX_PAYLEN);

    Serial.print("publishing ... ");
    client.publish(mqttTopic, payLoad);
    //
    // force encryption and transmission,
    // cause SSL lib optimization
    //
    client.flush();

    digitalWrite(LED_BUILTIN, LOW); // turn the LED off by making the voltage LOW
    Serial.println("published");
  }
}
