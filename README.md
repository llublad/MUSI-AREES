# Pràctica de sensorització

## Lectura de la intensitat eficaç del corrent elèctric amb els transductors SCT-013-000 i SCT-013-030 i la placa Arduino Nano 33 IoT 

| Alumne      | Lluís Bernat Ladaria  |
|---:         | --- |
| Assignatura | 11771 - Àrees d'aplicació d'IoT |
|             | MUSI - UIB  |
| Professor   | Dr. Bartomeu Alorda Ladaria |

---

## Introducció

En aquesta pràctica de sensorització disposam de dos transductors 
de la famíla SCT-013 que permeten mesurar la intensitat del corrent 
que circula per dins del seu cor magnètic. 
Aprofitant que l'SCT-013-000 no integra la resistència de càrrega, 
en calcularem una per tal de poder mesurar amb aquest, intensitats 
màximes de l'odre dels 10 A i així obtenir una major precisió a les 
lectures que la que ofereix el seu germà SCT-013-030, 
a canvi però de perdre rang de mesura. 

Un microcontrolador Arduino fent servir la llibreria 
[MUSIEmonLib](https://github.com/llbernat/MUSIEmonLib.git) 
que hem desenvolupat ad-hoc per aquest projecte 
en prendrà les mesures de forma periòdica. 
Aquestes lectures es transmetran de forma segura cap a un servidor 
MQTT que és el punt d'entrada a un ecosistema 
Influxdb + Grafana que ens permetrà explotar aquestes dades. 

## Arquitectura

Ambdós transductors SCT-013 estan connectats de forma independent 
a una placa microcontrolada Arduino Nano 33 IoT per mitjà d'un 
senzill circuit que acondiciona el senyal. 
El sensor SCT-013-030 ja du integrada una resistència de càrrega de 60 Ohm 
que li permet generar 1 V a la seva sortida 
si detecta un corrent de 30 A. 
Per contra el sensor SCT-030-000 no té resistència de càrrega, 
així que amb aquest darrer gaudim de la llibertat de poder-ne 
ajustar la seva sensibilitat i precisió, canviant el valor de la 
nova resistència de càrrega que li afegirem al seu circuit 
d'acondicionament del senyal. 
La nova resistència de càrrega serà de 330 Ohms. 
Aquest valor ens permetrà mesurar de forma segura 
pel controlador Arduino Nano intensitats de fins a 

El microcontrolador Arduino es connecta de forma segura 
(autenticació mútua amb mTLS) a un *broker* MQTT *Mosquitto* 
per tal de transmetre els nivells de corrent mesurats 
de forma periòdica i encriptada. 
Per a la realització del vídeo de la pràctica hem escollit un període 
de mostreig molt curt (10 segons) a fi de poder il·lustrar el funcionament 
durant el vídeo presentat. 
Evidentment pel cas general seria desitjable un període 
més llarg que podria ser entre 1 i 10 minuts depenent de la 
naturalesa del consum a mesurar i de les decisions a prendre. 

La solució presentada consta de vàries entitats de *software* i 
dels dos petits circuits electrònics d'acondicionament del 
senyal que passam a descriure breument: 

- [MUSIEmonLib](https://github.com/llbernat/MUSIEmonLib.git): 
llibreria que permet l'accés de lectura al sensor de corrent per tal d'estimar 
quin és el seu valor eficaç (*IRMS*) de corrent. 
El codi de la llibreria està convenientment comentat i incorpora un exemple senzill.
Fa ús (depen) de la llibreria [EmonLib](https://github.com/openenergymonitor/EmonLib.git).

- SensorCurrent.ino:  
aquest és el cor de la pràctica. 
És l'encarregat de fer les cridades apropiades a la API MUSIEmonLib, 
i també fer l'autenticació mútua, i encriptar el canal de 
comunicació amb el servidor MQTT.
Requereix que la placa Arduino disposi d'un xip AtmelECCx08 
(aquests xips ofereixen funcions d'encriptació), doncs 
la clau privada serà generada i custodiada per aquest xip.

- Shape.ino: 
petita utilitat que permet bolcar la forma de l'ona mostrejada cap a un servidor MQTT, 
per tal de poder visualitzar-la i poder efectuar controls visuals de qualitat 
del nostre montatge (bàsicament diagnóstic per mitjà de la forma de 
la ona mostrejada). 

- pki/: 
jerarquia de clau pública que dona suport a la part de comunicació segura mTLS

- Circuit divisor de tensió + condensador de desacoblament: 
en aquest cas tenim dos circuits que generen una tensió d'aproximadament 
la meitat de la tensió que poden tractar els conversors ADC. 
Pel cas del Nano, són uns 1.65 Volts. 
Aquesta tensió serveix com a referència pel sensor de corrent, que es connecta 
per una banda a aquest punt mig i a la vegada a *GND* 
per mitjà d'un condensador de desacoblament (10 uF) i 
per l'altre cap a l'entrada de l'ADC. 
En el cas del transductor obert SCT-013-00 el circuit 
n'incorpora una. Vegeu el càlcul a l'apartat següent. 

### Càlcul de la resistència de càrrega de l'SCT-013-000

L'objectiu és mesurar intensistat eficaces de fins a 10 A. 
Tenint en compte que el transductor té un solenoide amb 2000 voltes: 



## Especificacions 

| | |
|---:|---|
|Número de canals:| 2|
|Sensibilitat Canal 1:| 30A (IRMS) / 1V |
|Canal 2:| 4.65A (IRMS) / 1V|
|Velocitat màx. de mostreig:| 1148 mostres / s / canal |
|Temps màxim d'entrenament del filtre DC:| 5000 ms / canal|
|Format del *payload*:|influxdb|
