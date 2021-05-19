# Conjunt d'instrucions executades per a construir l'infrastructura de clau pública

## PKI generation

### CA, broker and telegraf client certificates

On Raspberry pi do: 

```
cd
mkdir -p pki/ca
mkdir pki/broker
mkdir pki/client

cd ~/pki/ca

openssl ecparam -name secp256r1 -genkey -out ca_ec.key

using curve name prime256v1 instead of secp256r1

openssl req -new -x509 -key ca_ec.key -sha256 -nodes \
-out ca_ec.crt -days 3700

You are about to be asked to enter information that will be incorporated
into your certificate request.
What you are about to enter is what is called a Distinguished Name or a DN.
There are quite a few fields but you can leave some blank
For some fields there will be a default value,
If you enter '.', the field will be left blank.
-----
Country Name (2 letter code) [AU]:ES
State or Province Name (full name) [Some-State]:Illes Balears
Locality Name (eg, city) []:Soller
Organization Name (eg, company) [Internet Widgits Pty Ltd]:UIB
Organizational Unit Name (eg, section) []:MUSI
Common Name (e.g. server FQDN or YOUR name) []:musi-ca
Email Address []:lluis.bernat2@estudiant.uib.cat

cat ca_ec.crt ca_ec.key > ca_ec.pem

cd ~/pki/broker

openssl ecparam -name secp256r1 -genkey -out broker_ec.key

using curve name prime256v1 instead of secp256r1

openssl req -out broker_ec.csr -key broker_ec.key -new

You are about to be asked to enter information that will be incorporated
into your certificate request.
What you are about to enter is what is called a Distinguished Name or a DN.
There are quite a few fields but you can leave some blank
For some fields there will be a default value,
If you enter '.', the field will be left blank.
-----
Country Name (2 letter code) [AU]:ES
State or Province Name (full name) [Some-State]:Illes Balears
Locality Name (eg, city) []:Soller
Organization Name (eg, company) [Internet Widgits Pty Ltd]:UIB
Organizational Unit Name (eg, section) []:MUSI
Common Name (e.g. server FQDN or YOUR name) []:musi-broker.ladaria.eu
Email Address []:

Please enter the following 'extra' attributes
to be sent with your certificate request
A challenge password []:
An optional company name []:

openssl x509 -req -in broker_ec.csr -CA ../ca/ca_ec.crt \
-CAkey ../ca/ca_ec.key -CAcreateserial -out broker_ec.crt -days 365

Signature ok
subject=C = ES, ST = Illes Balears, L = Soller, O = UIB, OU = MUSI, CN = musi-broker.ladaria.eu
Getting CA Private Key

sudo vi /etc/mosquitto/conf.d/user.conf

allow_anonymous false
#password_file /etc/mosquitto/passwd
acl_file /etc/mosquitto/acl_list
listener 8883
cafile /etc/mosquitto/ca_certificates/ca.crt
certfile /etc/mosquitto/certs/broker.crt
keyfile /etc/mosquitto/certs/broker.key
tls_version tlsv1.2
require_certificate true
# If require_certificate is true, you may set use_identity_as_username to true
# to use the CN value from the client certificate as a username. If this is
# true, the password_file option will not be used for this listener.
#use_identity_as_username false
use_identity_as_username true

sudo vi /etc/mosquitto/acl_list

#
# Usuari certificat software
#
user telegraf
#
topic read metering/#
#
# Usuari certificat amb clau privada al xip ECC608
#
user 0123C13DA283AFA5EE
#
topic write metering/#

cd ~/pki/client

openssl ecparam -name secp256r1 -genkey -out telegraf_ec.key

using curve name prime256v1 instead of secp256r1

openssl req -out telegraf_ec.csr -key telegraf_ec.key -new

You are about to be asked to enter information that will be incorporated
into your certificate request.
What you are about to enter is what is called a Distinguished Name or a DN.
There are quite a few fields but you can leave some blank
For some fields there will be a default value,
If you enter '.', the field will be left blank.
-----
Country Name (2 letter code) [AU]:ES
State or Province Name (full name) [Some-State]:Illes Balears
Locality Name (eg, city) []:Soller
Organization Name (eg, company) [Internet Widgits Pty Ltd]:UIB
Organizational Unit Name (eg, section) []:MUSI
Common Name (e.g. server FQDN or YOUR name) []:telegraf
Email Address []:

Please enter the following 'extra' attributes
to be sent with your certificate request
A challenge password []:
An optional company name []:

openssl x509 -req -in telegraf_ec.csr -CA ../ca/ca_ec.crt \
-CAkey ../ca/ca_ec.key -CAcreateserial -out telegraf_ec.crt -days 365

Signature ok
subject=C = ES, ST = Illes Balears, L = Soller, O = UIB, OU = MUSI, CN = telegraf
Getting CA Private Key

sudo vi /etc/telegraf/telegraf.conf

[[inputs.mqtt_consumer]]
servers = ["tcp://127.0.0.1:8883"]
topics = [
"telegraf/host01/cpu",
"telegraf/+/mem",
"sensors/#",
"metering/#"
]
tls_ca = "/etc/telegraf/ca.pem"
tls_cert = "/etc/telegraf/cert.pem"
tls_key = "/etc/telegraf/key.pem"

sudo cp ~/pki/ca/ca_ec.crt /etc/mosquitto/ca_certificates/ca.crt 
sudo cp ~/pki/broker/broker_ec.crt /etc/mosquitto/certs/broker.crt 
sudo cp ~/pki/broker/broker_ec.key /etc/mosquitto/certs/broker.key 
sudo chown mosquitto /etc/mosquitto/certs/broker.key
sudo systemctl stop mosquitto
sudo systemctl start mosquitto
# check listener: 
netstat -tunlp | grep 8883 

sudo cp ~/pki/ca/ca_ec.crt /etc/telegraf/ca.pem
sudo cp ~/pki/client/telegraf_ec.crt /etc/telegraf/cert.pem
sudo cp ~/pki/client/telegraf_ec.key /etc/telegraf/key.pem
sudo chown telegraf /etc/telegraf/key.pem
sudo systemctl restart telegraf
# check 
sudo systemctl status telegraf
```

### Arduino 33 IoT Nano client certificate

Compile and execute ECCX08CSR.ino on board:

```
ECCX08 Serial Number = 0123C13DA283AFA5EE

The ECCX08 on your board is not locked, would you like to PERMANENTLY configure and lock it now? (y/N) [N]: y
ECCX08 locked successfully

Hi there, in order to generate a new CSR for your board, we'll need the following information ...

Country Name (2 letter code) []: ES
State or Province Name (full name) []: Illes Balears
Locality Name (eg, city) []: Soller
Organization Name (eg, company) []: UIB
Organizational Unit Name (eg, section) []: MUSI
Common Name (e.g. server FQDN or YOUR name) [0123C13DA283AFA5EE]: 0123C13DA283AFA5EE
What slot would you like to use? (0 - 4) [0]: 0
Would you like to generate a new private key? (Y/n) [Y]: Y

Here's your CSR, enjoy!

-----BEGIN CERTIFICATE REQUEST-----
MIIBKzCB0gIBADBwMQswCQYDVQQGEwJFUzEWMBQGA1UECBMNSWxsZXMgQmFsZWFyczEPMA0GA1UE
BxMGU29sbGVyMQwwCgYDVQQKEwNVSUIxDTALBgNVBAsTBE1VU0kxGzAZBgNVBAMTEjAxMjNDMTNE
QTI4M0FGQTVFRTBZMBMGByqGSM49AgEGCCqGSM49AwEHA0IABBKBZt1IcdTSotiioiDnckLhuumZ
3MZIDmudxxKeI1ZYMoG6d7d8emyj4ZQgm3wCVUw6laRRZN/y6EwLel0jz2+gADAKBggqhkjOPQQD
AgNIADBFAiApamRGd41gkYb9L/KppLkXpTtvLlKsaiDHmy/1wOgiswIhAMFyl+Ddanmxtu21vY+3
ailwiuvMc03Pzf+/8HEgFd2S
-----END CERTIFICATE REQUEST-----
```

At our PC: 

```
mkdir -p ~/pki/ca
mkdir ~/pki/client

Then copy raspberry’s ~/pki/ca files to local ~/pki/ca folder
Also create the client_ec.csr file in ~/pki/client folder with the obtained CSR content with Arduino sketch (see above).

cd ~/pki/client

openssl x509 -req -in client_ec.csr -CA ../ca/ca_ec.crt \
-CAkey ../ca/ca_ec.key -CAcreateserial -out client_ec.crt -days 365

Signature ok
subject=C = ES, ST = Illes Balears, L = Soller, O = UIB, OU = MUSI, CN = 0123C13DA283AFA5EE
Getting CA Private Key

TA (trusted Anchor) creation (usually our particular CA): 

cd ~/pki/ca

python ../../pycert_bearssl/pycert_bearssl.py convert ca_ec.crt --no-search --output musi-ca.h

Loaded certificate ca_ec.crt

cp musi-ca.h ../../MUSI-AREES/SensorCurrent/certs/.
```
