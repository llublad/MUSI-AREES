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

//
// The public client cert (issuer, subject, cn and pub key)
//
#define MY_CERT        \
"-----BEGIN CERTIFICATE-----\n" \
"MIICATCCAacCFEbk1rl5gO2xhz9X1JhEEq7t5JJ2MAoGCCqGSM49BAMCMIGVMQsw\n" \
"CQYDVQQGEwJFUzEWMBQGA1UECAwNSWxsZXMgQmFsZWFyczEPMA0GA1UEBwwGU29s\n" \
"bGVyMQwwCgYDVQQKDANVSUIxDTALBgNVBAsMBE1VU0kxEDAOBgNVBAMMB211c2kt\n" \
"Y2ExLjAsBgkqhkiG9w0BCQEWH2xsdWlzLmJlcm5hdDJAZXN0dWRpYW50LnVpYi5j\n" \
"YXQwHhcNMjEwNTExMTczMjIxWhcNMjIwNTExMTczMjIxWjBwMQswCQYDVQQGEwJF\n" \
"UzEWMBQGA1UECBMNSWxsZXMgQmFsZWFyczEPMA0GA1UEBxMGU29sbGVyMQwwCgYD\n" \
"VQQKEwNVSUIxDTALBgNVBAsTBE1VU0kxGzAZBgNVBAMTEjAxMjNDMTNEQTI4M0FG\n" \
"QTVFRTBZMBMGByqGSM49AgEGCCqGSM49AwEHA0IABBKBZt1IcdTSotiioiDnckLh\n" \
"uumZ3MZIDmudxxKeI1ZYMoG6d7d8emyj4ZQgm3wCVUw6laRRZN/y6EwLel0jz28w\n" \
"CgYIKoZIzj0EAwIDSAAwRQIgBSBTVvPUf+BEPQKVFsT3Ol2hQ2lM+Oh4j0An2MFR\n" \
"vRcCIQD+lF5hk5JHWlI8sbx9xzeuzM44PBUUXHufKpYyvDrSBA==\n" \
"-----END CERTIFICATE-----\n"

//
// ECC608 slot with the corresponding private key 
//
#define MY_KEY_SLOT    0
