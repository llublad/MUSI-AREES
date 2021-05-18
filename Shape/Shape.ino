/*
 * Send to MQTT some ADC readings
 * 
 * This is an utility to view a graph of the 
 * sensed current with a graph-capable MQTT client
 * MQTT-Explorer is recomended (https://mqtt-explorer.com/)
 * 
 * I used it to verify the quality of the voltage divider circuit
 * and the quality of the decoupling capacitor 
 * (observing the sinus shape)
 * 
 * This utility sends paylod in clear text to 
 * avoid encryption overcharge
 */

#include <WiFiNINA.h>
#include <PubSubClient.h>

#include "wifi/wifi-secrets.h" // SSID/PASS

#define MQTT_SERVER "192.168.100.33"
#define MQTT_PORT 1883
#define MQTT_USER "sensor"
#define MQTT_PASS "uib-soller"
#define INPUT1 PIN_A2
#define INPUT2 PIN_A7
#define MAX_TOPLEN 40
#define TOPIC_TEMPLATE "sensor/rawch%d"
#define MAX_PAYLEN 20
#define PAYLOAD_TEMPLATE "%d"


WiFiClient wifiClient;
PubSubClient client(MQTT_SERVER, MQTT_PORT, wifiClient);

bool isConnected; 


void setup_wifi()
{
  //
  // Take care of WiFi setup
  //

  // Create and initialize local needed vars
  //
  char ssid[] = WIFI_SSID;
  char pass[] = WIFI_PASS;
  int status = WL_IDLE_STATUS;

  delay(1000); // wait a little bit to assure readiness

  // check for the presence of the shield/hardware
  //
  if (WiFi.status() == WL_NO_SHIELD)
  {
    Serial.println("WiFi shield not present or card w/o WiFi");
    while (true) delay(1000);
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
  pinMode(INPUT1, INPUT);
  pinMode(INPUT2, INPUT);
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

void setup()
{
  setup_wifi();

  printWifiStatus();
  
  isConnected = client.connect("arduinoCalibrate", MQTT_USER, MQTT_PASS);
  if (isConnected) 
  {
    Serial.println("Established MQTT connection");
  }
  else
  {
    Serial.print("Error in MQTT connection. State = ");
    Serial.println(client.state());
    while (true) delay(1000);
  }
  
}


void pub_value(int8_t ch, int16_t val)
{
  char topic[MAX_TOPLEN];
  char payLoad[MAX_PAYLEN];

  sprintf(topic, TOPIC_TEMPLATE, ch);

  sprintf(payLoad, PAYLOAD_TEMPLATE, val);

  client.publish(topic, payLoad);
  client.flush();
}

bool reconnect()
{
  bool isConnected;

  Serial.print("Attempting MQTT connection...");
  // Create a random client ID
  String clientId = "ArduinoClient-";
  clientId += String(random(0xffff), HEX);
  // Attempt to connect
  isConnected = client.connect(clientId.c_str());
  if (isConnected)
  {
    Serial.println("connected");
  }
  else
  {
    Serial.print("failed, rc=");
    Serial.println(client.state());
    Serial.println("Will try again in a few seconds");
    delay(1000);  // retry in a half metering period
  }

  return isConnected;
}


void loop()
{
  int16_t read[2];
  
  isConnected = client.loop(); // keeps alive and tell us if reconnection is needed
  if (!isConnected) isConnected = reconnect();

  read[0] = analogRead(INPUT1);
  pub_value(1, read[0]);
  
  read[1] = analogRead(INPUT2);
  pub_value(2, read[1]);

  delay(1);
}
