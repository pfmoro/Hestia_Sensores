#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

// ---- Configurações Globais ----
const char* ssid     = "Rede";     // Substitua pelo SSID
const char* password = "Senha";    // Substitua pela senha

// ---- Controle de energia dos sensores (Ruidosos) ----
#define PWR_UMIDADE D6   // VCC do sensor de umidade (GPIO12)
#define PWR_CHUVA   D7   // VCC do sensor de chuva (GPIO13)

// ---- Pinos de dados ----
#define PINO_UMIDADE A0   // Saída analógica do sensor de umidade
#define PINO_CHUVA   D5   // Saída digital do sensor de chuva (GPIO14)

ESP8266WebServer server(80);

int umidadeSolo = 0;
int chuva = 0;

void readSensors() {
  // 1) Ler sensor de umidade capacitivo
  digitalWrite(PWR_UMIDADE, HIGH);   // Liga VCC
  // Atraso aumentado para 500ms para estabilizar o sensor capacitivo (evitando saltos)
  delay(500);                        
  umidadeSolo = analogRead(PINO_UMIDADE);
  digitalWrite(PWR_UMIDADE, LOW);    // Desliga VCC

  // 2) Ler sensor de chuva
  digitalWrite(PWR_CHUVA, HIGH);     // Liga VCC
  delay(150);                        // Estabilização rápida
  // Lê o pino digital. (0 = Chuva, 1 = Sem Chuva - ajuste se o seu módulo for o contrário)
  chuva = digitalRead(PINO_CHUVA); 
  digitalWrite(PWR_CHUVA, LOW);      // Desliga VCC
}

void handleRoot() {
  // Chamada de leitura deve ser feita antes de construir o JSON
  readSensors(); 
  
  StaticJsonDocument<100> doc;
  doc["umidade_solo"] = umidadeSolo;
  doc["estado_chuva"] = chuva;

  String jsonString;
  serializeJson(doc, jsonString);
  server.send(200, "application/json", jsonString);
}

void setup() {
  Serial.begin(115200);

  // Inicializa controle de energia
  pinMode(PWR_UMIDADE, OUTPUT);
  pinMode(PWR_CHUVA, OUTPUT);
  digitalWrite(PWR_UMIDADE, LOW); 
  digitalWrite(PWR_CHUVA, LOW);

  pinMode(PINO_CHUVA, INPUT_PULLUP); 

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
  // Não há delay() aqui, pois a leitura é feita no handleRoot()
}