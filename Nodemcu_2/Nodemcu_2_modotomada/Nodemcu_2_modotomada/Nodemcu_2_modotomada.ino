#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Wire.h> // Necessário para a comunicação I2C do BMP280

// Substitua com as credenciais da sua rede Wi-Fi
const char* ssid = "Sua rede aqui";
const char* password = "Sua senha aqui";

// Pinos dos sensores
const int SOIL_MOISTURE_PIN = A0;
const int RAIN_SENSOR_PIN = D5;

// Objeto para o sensor BMP280
Adafruit_BMP280 bmp;

// Variáveis para armazenar os dados dos sensores
float temperatura_bmp;
float pressao_bmp;

ESP8266WebServer server(80);

void handleRoot() {
  // Leitura dos sensores
  int soilMoistureValue = analogRead(SOIL_MOISTURE_PIN);
  int rainSensorValue = digitalRead(RAIN_SENSOR_PIN);

  // Leitura do BMP280
  temperatura_bmp = bmp.readTemperature();
  pressao_bmp = bmp.readPressure() / 100.0F; // Converte a pressão para hPa

  // Criação do objeto JSON com a biblioteca ArduinoJson
  StaticJsonDocument<200> doc; // Aumenta o tamanho do documento para acomodar os novos dados
  doc["umidade_solo"] = soilMoistureValue;
  doc["estado_chuva"] = (rainSensorValue == HIGH) ? 0 : 1;
  doc["temperatura_bmp"] = temperatura_bmp;
  doc["pressao_bmp"] = pressao_bmp;

  String jsonString;
  serializeJson(doc, jsonString);

  server.send(200, "application/json", jsonString);
}

void setup() {
  Serial.begin(115200);
  pinMode(RAIN_SENSOR_PIN, INPUT_PULLUP); // O sensor de chuva é ativo em LOW

  // Inicializa o sensor BMP280
  if (!bmp.begin(0x76)) {
    Serial.println(F("Não foi possível encontrar um sensor BMP280 válido, verifique a fiação!"));
  } else {
    Serial.println("Sensor BMP280 encontrado e inicializado.");
  }

  // Conecta ao Wi-Fi
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());

  // Inicia o servidor web
  server.on("/", handleRoot);
  server.begin();
  Serial.println("Servidor HTTP iniciado.");
}

void loop() {
  server.handleClient();
}

