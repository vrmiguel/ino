# Atividade 01

```
Descrição: O Arduíno Mega deverá ser conectado ao sensor de distância e
obter leituras a cada 250 ms. Se a média das últimas 4 amostras for maior do
que 500 cm, então o LED do Arduíno deverá piscar com frequência 1 Hz.
No ESP 32, o sensor de toque deverá ser monitorado e lido a cada 100 ms.
Caso a media dos últimos 5 pontos indique “toque”, o ESP32 deverá enviar
uma solicitação de status do SR-HC04 ao Arduíno Mega.
Caso o Led do Arduíno Mega esteja piscando no momento da solicitação, o
Arduino Mega deve retornar um sinal “alarme” para o ESP32.
Nesse caso, o buzzer conectado ao ESP32 deverá soar o alarme
```