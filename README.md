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
es preten mesurar la intensitat 
del corrent elèctric que circula per dins 
l'anell magnètic de cada un dels dos 
sensors de que disposam i que estan 
configurats amb 
sensibilitats distintes. 

Ambdós sensors estan connectats 
de forma independent 
a una placa microcontrolada 
Arduino Nano 33 IoT. 

Finalment aquest microcontrolador 
es connecta de forma segura 
(autenticació mútua) a un *broker* 
MQTT Mosquitto per tal de transmetre 
els nivells de corrent mesurats 
de forma periòdica (IRMS). 

Per a aquesta finalitat 
s'han desenvolupat tres entitats 
de *software* y dos petits 
circuits electrònics d'acondicionament del 
senyal que passam a descriure breument: 

- MUSIEmonLib: 
llibreria que permet l'accés de lectura 
al sensor de corrent

- SensorCurrent.ino: 
programari que permet ésser compilat i carregat 
a una placa Arduino que disposi d'un 
xip ECCx08. 
Aquests xips ofereixen funcions d'encriptació. 

- Shape.ino: 
petita utilitat que permet bolcar la forma de l'ona mostrejada cap a un servidor MQTT, 
per tal de poder visualitzar-la i poder 
efectuar controls manuals de qualitat 
del nostre montatge. 