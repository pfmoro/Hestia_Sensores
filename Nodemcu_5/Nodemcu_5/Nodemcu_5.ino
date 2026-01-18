#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include "ThingSpeak.h"
#include "MQ135_corr.h"

 ---- Configurações de Rede e ThingSpeak ----
const char* ssid   = "Sua_Rede";
const char* password = "Sua_senha";
unsigned long myChannelNumber = 1111111;
const char* myWriteAPIKey = "suaAPIkey";

// ---- Pinos ----
#define PINO_MQ135 A0
#define DHTPIN D2
#define DHTTYPE DHT11

// ---- Objetos ----
MQ135_corr gasSensor(5.0);   // MQ135 alimentado via VIN (5V)
DHT dht(DHTPIN, DHTTYPE);
WiFiClient client;
ESP8266WebServer server(80);

// ---- Variáveis Globais ----
float temperatura = 0.0;
float umidade = 0.0;
float ppm_corrigido = 0.0;
float rzero_estimado = 0.0;

int mq135_raw = 0;
int mq135_raw_corrigido = 0;

unsigned long lastTimeThingSpeak = 0;
const unsigned long timerDelay = 600000; // 10 minutos

// ---- Controle de WiFi ----
unsigned long lastWifiCheck = 0;
const unsigned long wifiCheckInterval = 10000; // 10s
bool wifiWasConnected = false;

// ---- Função de Reconexão WiFi ----
void checkWiFi() {
  if (millis() - lastWifiCheck < wifiCheckInterval) return;
  lastWifiCheck = millis();

  if (WiFi.status() != WL_CONNECTED) {
    if (wifiWasConnected) {
      Serial.println("[WiFi] Conexão perdida. Tentando reconectar...");
      wifiWasConnected = false;
    }
    WiFi.disconnect();
    WiFi.begin(ssid, password);
  } else {
    if (!wifiWasConnected) {
      wifiWasConnected = true;
      Serial.println("[WiFi] Reconectado com sucesso!");
      Serial.print("[WiFi] Endereço IP: ");
      Serial.println(WiFi.localIP());
    }
  }
}

// ---- Leitura dos Sensores ----
void readSensors() {
  umidade = dht.readHumidity();
  temperatura = dht.readTemperature();

  if (isnan(umidade) || isnan(temperatura)) {
    Serial.println("[ERRO] Falha na leitura do DHT11!");
    umidade = 0;
    temperatura = 20;
  }

  mq135_raw = analogRead(PINO_MQ135);

  // Correção do divisor de tensão (2k / 3k)
  mq135_raw_corrigido = mq135_raw * 1.5;
  if (mq135_raw_corrigido > 1023) mq135_raw_corrigido = 1023;

  ppm_corrigido = gasSensor.getCorrectedPPM(
    mq135_raw_corrigido,
    temperatura,
    umidade
  );

  rzero_estimado = gasSensor.getRZero(
    mq135_raw_corrigido,
    temperatura,
    umidade
  );

  // ---- Debug Serial ----
  Serial.println("===== LEITURA AMBIENTAL =====");
  Serial.print("Temperatura: "); Serial.print(temperatura); Serial.println(" °C");
  Serial.print("Umidade: "); Serial.print(umidade); Serial.println(" %");
  Serial.print("MQ135 Raw ADC: "); Serial.println(mq135_raw);
  Serial.print("MQ135 Raw Corrigido ADC: "); Serial.println(mq135_raw_corrigido);
  Serial.print("PPM Corrigido: "); Serial.print(ppm_corrigido); Serial.println(" ppm");
  Serial.print("RZero Estimado: ");
  Serial.print(rzero_estimado);
  Serial.println(" kΩ  [VALIDO APÓS BURN-IN ≥ 24h EM AR LIMPO]");
  Serial.println("=============================");
}

// ---- Endpoint Web ----
void handleRoot() {
  readSensors();

  StaticJsonDocument<256> doc;
  doc["temperatura"] = temperatura;
  doc["umidade"] = umidade;
  doc["mq135_raw_adc"] = mq135_raw;
  doc["mq135_raw_corrigido_adc"] = mq135_raw_corrigido;
  doc["ppm_corrigido"] = ppm_corrigido;
  doc["rzero_estimado"] = rzero_estimado;

  String json;
  serializeJson(doc, json);
  server.send(200, "application/json", json);
}

// ---- Setup ----
void setup() {
  Serial.begin(115200);
  dht.begin();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n[SISTEMA] WiFi conectado!");
  Serial.print("[SISTEMA] Endereço IP: ");
  Serial.println(WiFi.localIP());
  wifiWasConnected = true;

  ThingSpeak.begin(client);

  server.on("/", handleRoot);
  server.begin();

  Serial.println("[SISTEMA] Servidor Web e ThingSpeak iniciados.");
}

// ---- Loop ----
void loop() {
  server.handleClient();
  checkWiFi();

  if (millis() - lastTimeThingSpeak > timerDelay) {
    readSensors();

    ThingSpeak.setField(8, ppm_corrigido);

    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    if (x == 200) {
      Serial.println("[ThingSpeak] Update realizado com sucesso.");
    } else {
      Serial.println("[ERRO] ThingSpeak HTTP code " + String(x));
    }

    lastTimeThingSpeak = millis();
  }
}
