# maregramme
Le programme est composé de deux éléments distincts, chacun avec son code

- ESP8266 : est connecté à internet et donne la dernière heure de maree haute. La librairie wifi utilisée est instable (se connecte ou pas). Sert à calculer une valeur x qui est envoyée à colorduino et correspond au temps passé depuis la dernière pleine mer.
-> tourne sur un nodemcu lolin

- colorduino : prend en entrée la dernière heure de maree et affiche le graphique sur la matrice de LEDS
-> tourne sur un arduino colorduino branché à l'esp8266 en mode esclave SPI
