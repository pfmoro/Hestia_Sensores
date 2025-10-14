#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <ArduinoJson.h>

// ---- Configurações Globais ----
const char* ssid     = "Rede";
const char* password = "Senha";
const uint8_t bmpAddress = 0x76; // Endereço I2C

Adafruit_BMP280 bmp; 
ESP8266WebServer server(80);

float temperatura_bmp = NAN;
float pressao_bmp = NAN;

void readSensors() {
  // Leitura BMP280
  temperatura_bmp = bmp.readTemperature();
  pressao_bmp = bmp.readPressure() / 100.0; // Converte para hPa
}

void handleRoot() {
  readSensors(); // Leitura antes de construir o JSON
  
  StaticJsonDocument<150> doc;
  doc["temperatura_bmp"] = temperatura_bmp;
  doc["pressao_bmp"] = pressao_bmp;

  String jsonString;
  serializeJson(doc, jsonString);
  server.send(200, "application/json", jsonString);
}

void setup() {
  Serial.begin(115200);

  // Inicializa BMP280
  Wire.begin(D2, D1); // SDA, SCL
  if (!bmp.begin(bmpAddress)) {
    Serial.println(F("BMP280 não encontrado!"));
    while (1);
  }

  // Conexão Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado");

  // Configuração do servidor
  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  server.handleClient();
}