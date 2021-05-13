#ifndef _SENSOR-CERT_H_
#define _SENSOR-CERT_H_

//
// The public client certificate 
// includes text and pem certificate versions
//

/*

$ openssl x509 -in sensor_ec.crt -noout -text

Certificate:
    Data:
        Version: 1 (0x0)
        Serial Number:
            46:e4:d6:b9:79:80:ed:b1:87:3f:57:d4:98:44:12:ae:ed:e4:92:76
        Signature Algorithm: ecdsa-with-SHA256
        Issuer: C = ES, ST = Illes Balears, L = Soller, O = UIB, OU = MUSI, CN = musi-ca, emailAddress = lluis.bernat2@estudiant.uib.cat
        Validity
            Not Before: May 11 17:32:21 2021 GMT
            Not After : May 11 17:32:21 2022 GMT
        Subject: C = ES, ST = Illes Balears, L = Soller, O = UIB, OU = MUSI, CN = 0123C13DA283AFA5EE
        Subject Public Key Info:
            Public Key Algorithm: id-ecPublicKey
                Public-Key: (256 bit)
                pub:
                    04:12:81:66:dd:48:71:d4:d2:a2:d8:a2:a2:20:e7:
                    72:42:e1:ba:e9:99:dc:c6:48:0e:6b:9d:c7:12:9e:
                    23:56:58:32:81:ba:77:b7:7c:7a:6c:a3:e1:94:20:
                    9b:7c:02:55:4c:3a:95:a4:51:64:df:f2:e8:4c:0b:
                    7a:5d:23:cf:6f
                ASN1 OID: prime256v1
                NIST CURVE: P-256
    Signature Algorithm: ecdsa-with-SHA256
         30:45:02:20:05:20:53:56:f3:d4:7f:e0:44:3d:02:95:16:c4:
         f7:3a:5d:a1:43:69:4c:f8:e8:78:8f:40:27:d8:c1:51:bd:17:
         02:21:00:fe:94:5e:61:93:92:47:5a:52:3c:b1:bc:7d:c7:37:
         ae:cc:ce:38:3c:15:14:5c:7b:9f:2a:96:32:bc:3a:d2:04
*/

//
// PEM codified cert
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

#endif /* ifndef _SENSOR-CERT_H_ */
