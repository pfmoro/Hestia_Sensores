#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

// ---- Configurações Globais ----
const char* ssid     = "Rede";
const char* password = "Senha";

// ---- Pinos de dados ----
#define PINO_MQ135 A0   // Saída analógica do sensor MQ-135

ESP8266WebServer server(80);

int mq135_raw = 0;

void readSensors() {
  // Leitura do MQ-135
  mq135_raw = analogRead(PINO_MQ135);
  // Nota: Para converter este valor bruto para PPM, é necessário
  // uma calibração complexa, que não está incluída neste código.
  // O backend receberá o valor bruto (0-1023).
}

void handleRoot() {
  readSensors(); // Leitura antes de construir o JSON
  
  StaticJsonDocument<100> doc;
  doc["qualidade_ar_raw"] = mq135_raw;

  String jsonString;
  serializeJson(doc, jsonString);
  server.send(200, "application/json", jsonString);
}

void setup() {
  Serial.begin(115200);
  
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
  
  Serial.println("MQ-135 LIGADO. Aguarde 20 minutos para aquecimento (burn-in) para leituras precisas.");
}

void loop() {
  server.handleClient();
  // A leitura é feita no handleRoot()
}