# Pràctica de sensorització 

|             | |
|---:         |---|
| Alumne      | Lluís Bernat Ladaria  |
| Assignatura | 11771 - Àrees d'aplicació d'IoT |
|             | MUSI - UIB  |
| Professor   | Dr. Bartomeu Alorda Ladaria |
|             | |

---

## Introducció

En aquesta pràctica de sensorització 
disposam de dos sensors que permeten 
mesurar la intensitat 
(*Root Mean Square*) del corrent 
que circula per dins el 
seu cor magnètic. 
Cada un dels sensors mesurarà 
un rang distint d'intensitats. 
Un microcontrolador Arduino 
en prendrà les mesures de forma 
periòdica. 
Aquestes lectures es transmetran 
de forma segura cap a un servidor 
MQTT que és el punt d'entrada 
a un ecosistema 
Influxdb + Grafana que ens 
permetrà explotar aquestes dades. 

## Arquitectura

Ambdós sensors estan connectats 
de forma independent 
a una placa microcontrolada 
Arduino Nano 33 IoT per mitjà d'un 
senzill circuit que acondiciona el senyal. 
Un dels sensors ja du integrada una 
resistència de càrrega de 60 Ohm 
que li permet generar 1 V a la seva 
sortida si detecta un 
corrent de 30 A (RMS). 
El segon no en du, per tant 
amb aquest darrer tenim la llibertat 
de poder-ne ajustar la seva sensibilitat, 
canviant el valor de la resistència 
de càrrega que afegirem al circuit. 
Hem optat per un valor de 430 Ohm que 
permet generar 1 V amb només 4.65 A. 
És a dir una sensibilitat molt més gran.

Aquest microcontrolador Arduino 
es connecta de forma segura 
(autenticació mútua amb mTLS) 
a un *broker* 
MQTT Mosquitto per tal de transmetre 
els nivells de corrent mesurats 
de forma periòdica i encriptada. 

Per a aquesta finalitat 
s'han desenvolupat tres entitats 
de *software* i els dos petits 
circuits electrònics d'acondicionament del 
senyal que passam a descriure breument: 

- MUSIEmonLib: 
llibreria que permet l'accés de lectura 
al sensor de corrent per tal d'estimar 
quin és el seu valor *RMS* de corrent. 
El codi de la llibreria està convenientment 
comentat i incorpora un exemple senzill 
del càlcul IRMS.

- SensorCurrent.ino:  
Aquest és el cor de la pràctica. 
És l'encarregat de fer les cridades 
apropiades a la API MUSIEmonLib, 
i també fer l'autenticació 
mútua, i encriptar el canal de 
comunicació amb el servidor MQTT.
Requereix que la placa Arduino 
disposi d'un xip AtmelECCx08 
(aquests xips ofereixen funcions d'encriptació), doncs 
la clau privada serà generada i 
custodiada per aquest xip.

- Shape.ino: 
petita utilitat que permet bolcar la forma de l'ona mostrejada cap a un servidor MQTT, 
per tal de poder visualitzar-la i poder 
efectuar controls visuals de qualitat 
del nostre montatge 
(bàsicament diagnóstic 
per mitjà de la forma de 
la ona mostrejada). 

- Circuit divisor de tensió + 
condensador de desacoblament: 
en aquest cas tenim dos circuits que 
generen una tensió d'aproximadament 
la meitat de la tensió que poden tractar 
els conversors ADC. 
Pel cas del Nano, són 
uns 1.65 Volts. 
Aquesta tensió serveix com a referència 
pel sensor de corrent, que es connecta 
per una banda a aquest punt mig i 
a la vegada a *GND* 
per mitjà d'un condensador de
10 uF i per l'altre cap a l'entrada de 
l'ADC. En el cas del sensor obert 
(sense resistència de càrrega) el circuit 
n'incorpora una (430 Ohms). 

## Especificacions 

| | |
|---:|---|
|Número de canals:| 2|
|Sensibilitat Canal 1:| 30A (IRMS) / 1V |
|Canal 2:| 4.65A (IRMS) / 1V|
|Velocitat màx. de mostreig:| 1148 mostres / s / canal |
|Temps màxim d'entrenament del filtre DC:| 5000 ms / canal|
|Format del *payload*:|influxdb|