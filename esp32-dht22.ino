#include "DHTesp.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define I2C_ADDR  0x27
#define LCD_COLUMNS 20
#define LCD_LINES  4
#define DHT_PIN    15
#define DHT_TYPE   DHTesp::DHT22

// Configurações Wi-Fi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Configurações MQTT
const char* mqtt_server = "public.mqtthq.com";
const int mqtt_port = 1883;
const char* mqtt_topic = "mqttHQ-client-test2";

// Configurações NTP
#define NTP_SERVER     "pool.ntp.org"
#define UTC_OFFSET     0
#define UTC_OFFSET_DST 0

WiFiClient espClient;
PubSubClient client(espClient);
DHTesp dhtSensor;
LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_LINES);

const int ledPin = 2; // Pino do LED
const int buzzerPin = 13; // Pino do buzzer piezoelétrico (D13)
bool ledState = false;
bool isDisplayingMessage = true; // Variável para controlar se a mensagem está sendo exibida

// Configurações do PWM
const int freq = 2000; // Frequência de 2kHz
const int ledChannel = 0;
const int resolution = 8; // Resolução de 8 bits

void setup() {
  Serial.begin(115200);
  setupDHTSensor();
  setupLCD();
  pinMode(ledPin, OUTPUT); // Define o pino do LED como saída
  digitalWrite(ledPin, LOW); // Assegura que o LED está apagado inicialmente

  // Configura o canal do PWM para o buzzer
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(buzzerPin, ledChannel);

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (isDisplayingMessage) {
    printMessageOnLCD("Projeto em ESP32", "");
    digitalWrite(ledPin, LOW); // Garante que o LED esteja apagado
    ledcWrite(ledChannel, 0); // Garante que o buzzer esteja desligado
    publishMessage("Projeto em ESP32", ""); // Envia a mensagem para o MQTT
    delay(5000);
    isDisplayingMessage = false;
  } else {
    TempAndHumidity data = dhtSensor.getTempAndHumidity();
    if (isValidData(data)) {
      displayData(data);
      updateLED(data.temperature); // Atualiza o LED com base na temperatura
      updateBuzzer(data.temperature); // Atualiza o buzzer com base na temperatura
      publishData(data); // Publica os dados no broker MQTT
      delay(10000); // Espera 10 segundos antes de alternar de volta para a mensagem inicial
    }
    isDisplayingMessage = true;
  }
  
  printLocalTime();
  delay(250);
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando-se a ");
  Serial.println(ssid);

  lcd.setCursor(0, 0);
  lcd.print("Connecting to ");
  lcd.setCursor(0, 1);
  lcd.print("WiFi");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    spinner();
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.println("Online");
  lcd.setCursor(0, 1);
  lcd.println("Updating time...");

  configTime(UTC_OFFSET, UTC_OFFSET_DST, NTP_SERVER);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando se conectar ao MQTT... ");
    lcd.setCursor(0, 0);
    lcd.print("Conectando no");
    lcd.setCursor(0, 1);
    lcd.print("MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("conectado");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("MQTT Conectado");
      publishMessage("Status MQTT", "Conectado"); // Envia status de conexão MQTT para o MQTT
      delay(2000); // Tempo para mostrar a mensagem no LCD
    } else {
      Serial.print("falhou, rc=");
      Serial.print(client.state());
      Serial.println(" tenta novamente em 5 segundos");
      delay(5000);
    }
  }
}

void setupDHTSensor() {
  dhtSensor.setup(DHT_PIN, DHT_TYPE);
}

void setupLCD() {
  lcd.init();
  lcd.backlight();
}

void printMessageOnLCD(String line1, String line2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  if (line2.length() <= LCD_COLUMNS) {
    lcd.print(line2);
  } else {
    lcd.print(line2.substring(0, LCD_COLUMNS));
  }
}

void displayData(TempAndHumidity data) {
  lcd.clear();
  if (data.temperature > 60.0) {
    lcd.setCursor(0, 0);
    lcd.print("Perigo de incendio");
    publishMessage(mqtt_topic, "Perigo de incendio"); // Envia a mensagem para o MQTT
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(data.temperature, 1);
    lcd.print("C");

    lcd.setCursor(0, 1);
    lcd.print("Umid.: ");
    lcd.print(data.humidity, 1);
    lcd.print("%");

    delay(5000); // Exibe temperatura e umidade por 5 segundos

    lcd.clear();
    lcd.setCursor(0, 0); // Começa no início da primeira linha
    lcd.print("Vai, Corinthians!"); // Exibe a mensagem final
    publishMessage("Mensagem MQTT", "Vai, Corinthians!"); // Envia a mensagem para o MQTT
  }
}

void updateLED(float temperature) {
  if (temperature > 50.0) {
    ledState = true;
    Serial.println("Temperatura alta! LED aceso.");
    digitalWrite(ledPin, HIGH); // Acende o LED
  } else if (temperature < 10.0) {
    ledState = true;
    Serial.println("Temperatura baixa! LED aceso.");
    digitalWrite(ledPin, HIGH); // Acende o LED
  } else {
    ledState = false;
    Serial.println("Temperatura normal. LED apagado.");
    digitalWrite(ledPin, LOW); // Apaga o LED
  }
}

void updateBuzzer(float temperature) {
  if (temperature > 60.0) {
    Serial.println("Temperatura crítica! Sirene ligada.");
    playSiren(); // Toca o som de sirene
  } else {
    Serial.println("Temperatura não crítica. Buzzer desligado.");
    ledcWriteTone(ledChannel, 0); // Desliga o buzzer
  }
}

void playSiren() {
  for (int i = 0; i < 4; i++) { // Loop para criar o efeito da sirene
    ledcWriteTone(ledChannel, 1000); // Frequência alta
    delay(500); // Meio segundo
    ledcWriteTone(ledChannel, 800); // Frequência baixa
    delay(500); // Meio segundo
  }
}

void publishData(TempAndHumidity data) {
  String payload = "Temperatura: " + String(data.temperature, 1) + "C, Umidade: " + String(data.humidity, 1) + "%";
  Serial.print("Publicando mensagem: ");
  Serial.println(payload);
  client.publish(mqtt_topic, payload.c_str());
}

void publishMessage(const char* topic, const char* message) {
  Serial.print("Publicando mensagem MQTT: ");
  Serial.println(message);
  client.publish(topic, message);
}

bool isValidData(TempAndHumidity data) {
  if (isnan(data.temperature) || isnan(data.humidity)) {
    Serial.println("Erro: Dados inválidos do sensor DHT!");
    return false;
  }
  return true;
}

void spinner() {
  static int8_t counter = 0;
  const char* glyphs = "\xa1\xa5\xdb";
  lcd.setCursor(15, 1);
  lcd.print(glyphs[counter++]);
  if (counter == strlen(glyphs)) {
    counter = 0;
  }
}

void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    lcd.setCursor(0, 1);
    lcd.println("Erro de conexão");
    return;
  }

  lcd.setCursor(8, 0);
  lcd.println(&timeinfo, "%H:%M:%S");

  lcd.setCursor(0, 1);
  lcd.println(&timeinfo, "%d/%m/%Y   %Z");
}






