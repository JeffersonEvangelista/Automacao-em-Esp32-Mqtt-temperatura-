# Automacao em Esp32-Mqtt- temperatura e umidade e aviso de incendio
Este projeto é uma estação meteorológica baseada no ESP32 que monitora a temperatura e a umidade usando um sensor DHT22, exibe essas informações em um display LCD I2C, controla um LED indicador e publica os dados em um broker MQTT. Além disso, o projeto sincroniza a hora local usando um servidor NTP.

ESP32 MQTT Weather Station
Este projeto é uma estação meteorológica baseada no ESP32 que monitora a temperatura e a umidade usando um sensor DHT22, exibe essas informações em um display LCD I2C, controla um LED indicador e publica os dados em um broker MQTT. Além disso, o projeto sincroniza a hora local usando um servidor NTP.

Funcionalidades

Leitura de Sensores: Usa o sensor DHT22 para medir a temperatura e a umidade.
Exibição em LCD: Mostra os valores de temperatura e umidade em um display LCD I2C.
Controle de LED: Acende um LED se a temperatura estiver fora dos limites definidos (acima de 50°C ou abaixo de 10°C).
Publicação MQTT: Envia os dados de temperatura e umidade para um broker MQTT (public.mqtthq.com) no tópico mqttHQ-client-test.
Sincronização de Hora: Sincroniza a hora local usando o servidor NTP pool.ntp.org e exibe a hora atual no LCD.

Hardware Necessário

ESP32
Sensor DHT22
Display LCD I2C (16x2)
LED 5mm
Resistores (10kΩ para pull-up no DHT22 e 220Ω para o LED)
Jumpers e Protoboard
Bibliotecas Utilizadas
DHTesp
LiquidCrystal_I2C
WiFi
PubSubClient

Conexões de Hardware

DHT22: Pino de dados ao GPIO 15 do ESP32 com um resistor pull-up de 10kΩ.
LCD I2C: SDA ao GPIO 21 e SCL ao GPIO 22 do ESP32.
LED: Ânodo do LED ao GPIO 2 do ESP32 com um resistor de 220Ω e cátodo ao GND.

Configuração

Clone o repositório e abra o projeto na sua IDE do Arduino.
Certifique-se de ter todas as bibliotecas necessárias instaladas.
Conecte o hardware conforme as especificações acima.
Faça upload do código para o seu ESP32.
Acesse o console serial para ver os logs de depuração.

Como Usar

O ESP32 se conectará à rede Wi-Fi especificada e sincronizará a hora local.
Os dados de temperatura e umidade serão lidos a cada 10 segundos e exibidos no LCD.
Se a temperatura estiver fora dos limites definidos, o LED será aceso.
Os dados de temperatura e umidade serão publicados no broker MQTT especificado.
Licença
Este projeto é licenciado sob a MIT License.

![deft]()
